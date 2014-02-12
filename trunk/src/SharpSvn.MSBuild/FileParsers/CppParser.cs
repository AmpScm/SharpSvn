using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;

namespace SharpSvn.MSBuild.FileParsers
{
    sealed class CppParser : BaseLanguageParser
    {
        readonly List<Regex> Filters = new List<Regex>();
        public override void WriteComment(System.IO.StreamWriter sw, string text)
        {
            sw.Write("// ");
            sw.WriteLine(text);
        }

        public override bool FilterLine(string line)
        {
            string trimmed = line.Trim();

            if (!trimmed.StartsWith("["))
                return false;

            foreach (Regex re in Filters)
            {
                if (re.Match(line).Success)
                    return true;
            }

            return false;
        }

        public override void AddAttribute(Type type)
        {
            Filters.Add(new Regex(ConstructRegex(type), RegexOptions.CultureInvariant | RegexOptions.Singleline | RegexOptions.ExplicitCapture));
        }

        private string ConstructRegex(Type type)
        {
            return @"^\s*\[\s*assembly\s*:\s*" + ConstructNameRegex(type) + @"\s*(" + ArgumentsRegex + @"\s*)?\]";
        }

        private string ConstructNameRegex(Type type)
        {
            StringBuilder sb = new StringBuilder();

            string[] parts = type.FullName.Split('.');

            sb.Append('(', parts.Length - 1);
            sb.Append(@"(::\s*)?");
            for (int i = 0; i < parts.Length - 1; i++)
            {
                sb.Append(Regex.Escape(parts[i]));
                sb.Append(@"\s*::\s*)?");
            }

            string name = parts[parts.Length - 1];

            if (name.EndsWith("Attribute"))
            {
                sb.Append(Regex.Escape(name.Substring(0, name.Length - 9)));
                sb.Append("(Attribute)?");
            }
            else
                sb.Append(Regex.Escape(name));

            return sb.ToString();
        }

        private string ArgumentsRegex
        {
            get { return @"\(([^""@)]|""([^\\""]|\\.)*"")*\)"; }
        }

        protected override void StartAttribute(StreamWriter sw, Type type)
        {
            sw.Write("[assembly: ::");
            sw.Write(type.FullName.Replace(".", "::"));
            sw.Write("(");
        }

        protected override void EndAttribute(StreamWriter sw)
        {
            sw.WriteLine(")];");
        }

        public override void WriteAttribute(StreamWriter sw, Type type, string value)
        {
            StartAttribute(sw, type);
            sw.Write("\"");
            sw.Write(value.Replace("\\", "\\\\").Replace("\"", "\\\""));
            sw.WriteLine("\")];");
        }

        public override void WriteAttribute(StreamWriter sw, Type type, bool value)
        {
            StartAttribute(sw, type);
            sw.Write(value ? "true" : "false");
            EndAttribute(sw);
        }
    }
}
