using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace SharpSvn.MSBuild.FileParsers
{
    class FSharpParser : LanguageParser
    {
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

            foreach (AttributeRegex ar in AttrMap.Values)
            {
                if (ar.Matches(line))
                    return true;
            }

            return false;
        }

        protected override void AddAttribute(Type attributeType)
        {
            AttrMap[attributeType] = new AttributeRegex(attributeType, ConstructRegex(attributeType), RegexOptions.None);
        }

        private string ConstructRegex(Type attributeType)
        {
            return @"^\s*\[\s*\<\s*assembly\s*:\s*" + ConstructNameRegex(attributeType) + @"\s*(" + ArgumentsRegex + @"\s*)?\>\s*\]";
        }

        private string ConstructNameRegex(Type attributeType)
        {
            StringBuilder sb = new StringBuilder();
            string[] parts = attributeType.FullName.Split('.');

            sb.Append('(', parts.Length - 1);
            sb.Append(@"(global\s*\.\s*)?");
            for (int i = 0; i < parts.Length - 1; i++)
            {
                sb.Append(Regex.Escape(parts[i]));
                sb.Append(@"\s*\.\s*)?");
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
            get { return @"\(([^""@)]|""([^\\""]|\\.)*""|@""([^""]|"""")*"")*\)"; }
        }

        protected override void StartAttribute(System.IO.StreamWriter sw, Type attributeType)
        {
            sw.Write("[<assembly: global.");
            sw.Write(attributeType.FullName);
            sw.Write("(");
        }

        protected override void EndAttribute(System.IO.StreamWriter sw)
        {
            sw.WriteLine(")>]");

            // In F# attributes are always applied to something. So add 'something'
            sw.WriteLine("()");
        }

        protected override void WriteAttribute(System.IO.StreamWriter sw, Type attributeType, string value)
        {
            StartAttribute(sw, attributeType);
            sw.Write("@\"");
            sw.Write(value.Replace("\"", "\"\""));
            sw.Write('\"');
            EndAttribute(sw);
        }

        protected override void WriteAttribute(System.IO.StreamWriter sw, Type attributeType, bool value)
        {
            StartAttribute(sw, attributeType);
            sw.Write(value ? "true" : "false");
            EndAttribute(sw);
        }

        public override string CopyrightEscape(string from)
        {
            return (from ?? "").Replace("(c)", "©");
        }
    }
}
