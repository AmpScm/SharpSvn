using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Text.RegularExpressions;
using System.Linq;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;

namespace Errors2Enum
{
    public class Errors2Enum : Task
    {
        [Required]
        public string WinErrorHeaderPath { get; set; }

        [Required]
        public string ErrnoHeaderPath { get; set; }

        [Required]
        public string AprErrnoHeaderPath { get; set; }

        [Required]
        public string SerfHeaderPath { get; set; }

        [Required]
        public string LibSsh2HeaderPath { get; set; }

        [Required]
        public string OutputFilePath { get; set; }

        public override bool Execute()
        {
            string winerror = WinErrorHeaderPath;
            string errno = ErrnoHeaderPath;
            string aprerrno = AprErrnoHeaderPath;
            string serfh = SerfHeaderPath;
            string libssh2h = LibSsh2HeaderPath;
            string to = OutputFilePath;

            if (!File.Exists(winerror))
            {
                Log.LogError("'{0}' does not exist", winerror);
                Environment.ExitCode = 1;
                return false;
            }

            if (!File.Exists(errno))
            {
                Log.LogError("'{0}' does not exist", errno);
                Environment.ExitCode = 1;
                return false;
            }

            if (!File.Exists(aprerrno))
            {
                Log.LogError("'{0}' does not exist", aprerrno);
                Environment.ExitCode = 1;
                return false;
            }

            if (!File.Exists(serfh))
            {
                Log.LogError("'{0}' does not exist", aprerrno);
                Environment.ExitCode = 1;
                return false;
            }

            if (!Directory.Exists(Path.GetDirectoryName(to)))
            {
                Log.LogError("Parent directory of '{0}' does not exist", to);
                Environment.ExitCode = 1;
                return false;
            }


            string verHeader = "/* " + typeof(Errors2Enum).Assembly.FullName + " */";

            using (StreamWriter r = File.CreateText(to))
            using (StreamReader header = File.OpenText(winerror))
            using (StreamReader aprheader = File.OpenText(aprerrno))
            using (StreamReader serfheader = File.OpenText(serfh))
            using (StreamReader libssh2 = File.OpenText(libssh2h))
            using (StreamReader syserrs = File.OpenText(errno))
            {
                r.WriteLine(verHeader);
                r.WriteLine("/* GENERATED CODE - Don't edit this file */");
                r.WriteLine("#pragma once");
                r.WriteLine("namespace SharpSvn {");

                r.WriteLine("/// <summary>Generated mapping from winerror.h</summary>");
                r.WriteLine("public enum class SvnWindowsErrorCode {");

                WriteWinEnumBody(header, r);

                r.WriteLine("};");
                r.WriteLine();

                r.WriteLine("/// <summary>Generated mapping from apr_errno.h</summary>");
                r.WriteLine("public enum class SvnAprErrorCode {");

                Dictionary<string, string> defined = new Dictionary<string, string>();
                defined["SERF_ERROR_RANGE"] = "";
                WriteAprEnumBody(aprheader, r, true, null, defined);
                WriteAprEnumBody(syserrs, r, false, null, defined);
                WriteAprEnumBody(serfheader, r, true, "SERF_ERROR_", defined);
                r.WriteLine("};");
                r.WriteLine("");

                r.WriteLine("/// <summary>Generated mapping from libssh2.h</summary>");
                r.WriteLine("public enum class SvnSshErrorCode {");
                WriteAprEnumBody(libssh2, r, true, "LIBSSH2_ERROR_", defined);
                r.WriteLine("};");

                r.WriteLine();
                r.WriteLine("} /* SharpSvn */");
                r.WriteLine();
            }

            return true;
        }

        private static void WriteWinEnumBody(StreamReader header, StreamWriter r)
        {
            string msgId = null, msgText = null;

            string line;
            bool textFollows = false;
            while (null != (line = header.ReadLine()))
            {
                line = line.Trim();

                if (line == "//" || line.Length == 0)
                    continue;

                if (line.StartsWith("// MessageId: "))
                {
                    msgId = line.Substring(14).Trim();
                    msgText = null;
                    textFollows = false;
                    continue;
                }
                else if (line == "// MessageText:")
                    textFollows = true;
                else if (line.StartsWith("// ") && textFollows)
                    msgText += "/" + line + Environment.NewLine;
                else if (!string.IsNullOrEmpty(msgId) && line.StartsWith("#define " + msgId))
                {
                    string value = line.Substring(8 + msgId.Length).Trim();

                    int nSS = value.IndexOf("//");
                    if (nSS >= 0)
                        value = value.Substring(0, nSS).Trim();

                    if (!string.IsNullOrEmpty(value))
                    {
                        if (!string.IsNullOrEmpty(msgText))
                        {
                            r.WriteLine("/// <summary>");
                            r.Write(msgText.Replace("&", "&amp;").Replace("<", "&lt;").Replace(">", "&gt;"));
                            r.WriteLine("/// </summary>");

                            msgText = msgText.Trim();
                            if (!msgText.Contains("\n"))
                                r.WriteLine("[System::ComponentModel::DescriptionAttribute(\"" + Escape(msgId) + ": " + Escape(msgText.Substring(3).Trim()) + "\")]");
                            else
                                r.WriteLine("[System::ComponentModel::DescriptionAttribute(\"" + Escape(msgId) + "\")]");

                            r.WriteLine("[System::Diagnostics::CodeAnalysis::SuppressMessage(\"Microsoft.Naming\", \"CA1702:CompoundWordsShouldBeCasedCorrectly\")]");
                            r.WriteLine("[System::Diagnostics::CodeAnalysis::SuppressMessage(\"Microsoft.Naming\", \"CA1707:IdentifiersShouldNotContainUnderscores\")]");
                            r.WriteLine("[System::Diagnostics::CodeAnalysis::SuppressMessage(\"Microsoft.Naming\", \"CA1709:IdentifiersShouldBeCasedCorrectly\")]");
                        }
                        else
                            r.WriteLine("[System::ComponentModel::DiscriptionAttribute(\"" + Escape(msgId) + "\")]");

                        if (Regex.Match(value, "^[0-9]+L$").Success)
                            value = value.Substring(0, value.Length - 1);

                        r.WriteLine("#ifdef " + msgId);
                        r.WriteLine("# undef " + msgId);
                        r.WriteLine("# define SVN_DEF_" + msgId);
                        r.WriteLine("#endif");

                        r.WriteLine(msgId + " = (int)" + value + ",");

                        r.WriteLine("#ifdef SVN_DEF_" + msgId);
                        r.WriteLine("# define " + msgId + " ((DWORD)::SharpSvn::SvnWindowsErrorCode::" + msgId + ")");
                        r.WriteLine("#undef SVN_DEF_" + msgId);
                        r.WriteLine("#endif");
                        r.WriteLine();
                        r.WriteLine();
                    }
                }
                else if (line.StartsWith("#if") || line.StartsWith("#el") || line.StartsWith("#end"))
                    r.WriteLine(line.Replace("_WINERROR_", "_GEN_WINERROR_"));
                else
                {
                    msgId = null;
                    msgText = null;
                    textFollows = false;
                }
            }
        }

        private static void WriteAprEnumBody(StreamReader header, StreamWriter r, bool direct, string prefix, Dictionary<string, string> defined)
        {
            string line;
            Dictionary<string, string> descs = new Dictionary<string, string>();
            while (null != (line = header.ReadLine()))
            {
                while (line.EndsWith("\\", StringComparison.Ordinal))
                    line = line.Substring(0, line.Length - 1).TrimEnd() + " " + header.ReadLine().TrimStart();

                if (line.StartsWith("#if", StringComparison.Ordinal)
                    || line.StartsWith("#el", StringComparison.Ordinal)
                    || line.StartsWith("#end", StringComparison.Ordinal))
                {
                    if (line.TrimEnd().EndsWith("_H"))
                        line = line.TrimEnd() + "_GEN";

                    r.WriteLine(line.Replace("_INC", "GEN_INC"));
                }

                if (line.StartsWith(" * APR_"))
                {
                    int descEnd = 7;

                    while (descEnd < line.Length
                    && (char.IsLetterOrDigit(line, descEnd) || line[descEnd] == '_'))
                    {
                        descEnd++;
                    }

                    string desc = line.Substring(3, descEnd - 3);
                    string text = line.Substring(descEnd + 1);

                    if (text.Length > 1 && !char.IsWhiteSpace(text, 0) && char.IsLower(text, 1))
                        text = line.Substring(3);

                    text = text.Trim();

                    if (!descs.ContainsKey(desc))
                        descs[desc] = text;
                }

                if (!line.StartsWith("#define", StringComparison.Ordinal) ||
                    !char.IsWhiteSpace(line, 7))
                    continue;

                line = line.Trim();

                int nameEnd = 8;
                while (nameEnd < line.Length
                    && (char.IsLetterOrDigit(line, nameEnd) || line[nameEnd] == '_'))
                {
                    nameEnd++;
                }

                if (nameEnd == line.Length || !char.IsWhiteSpace(line, nameEnd))
                    continue;

                string name = line.Substring(8, nameEnd - 8);
                string oname = name;

                if (!string.IsNullOrEmpty(prefix))
                {
                    if (!name.StartsWith(prefix, StringComparison.Ordinal))
                        continue;
                }
                else if (!direct)
                    name = "APR_" + name;
                else if (!name.StartsWith("APR_", StringComparison.Ordinal))
                    continue;

                if (defined.ContainsKey(name))
                    continue;

                string value = direct ? line.Substring(nameEnd + 1).Trim() : oname;

                if (defined.ContainsKey(value))
                    value = defined[value];

                defined[name] = value;

                if (direct)
                {
                    r.WriteLine("#ifdef " + name);
                    r.WriteLine("# undef " + name);
                    r.WriteLine("# define SVN_DEF_" + name);
                    r.WriteLine("#endif");

                    if (descs.ContainsKey(name))
                    {
                        r.Write("/// <summary>");
                        r.Write(descs[name].Replace("&", "&amp;").Replace("<", "&lt;").Replace(">", "&gt;"));
                        r.WriteLine("</summary>");

                        r.WriteLine("[System::ComponentModel::DescriptionAttribute(\"" + Escape(descs[name]) + "\")]");
                    }

                    r.WriteLine("[System::Diagnostics::CodeAnalysis::SuppressMessage(\"Microsoft.Naming\", \"CA1702:CompoundWordsShouldBeCasedCorrectly\")]");
                    r.WriteLine("[System::Diagnostics::CodeAnalysis::SuppressMessage(\"Microsoft.Naming\", \"CA1707:IdentifiersShouldNotContainUnderscores\")]");
                    r.WriteLine("[System::Diagnostics::CodeAnalysis::SuppressMessage(\"Microsoft.Naming\", \"CA1709:IdentifiersShouldBeCasedCorrectly\")]");

                    r.WriteLine("\t{0} = (int){1},", name, value);

                    r.WriteLine("#ifdef SVN_DEF_" + name);
                    r.WriteLine("# define " + name + " ((apr_status_t)::SharpSvn::SvnAprErrorCode::" + name + ")");
                    r.WriteLine("#undef SVN_DEF_" + name);
                    r.WriteLine("#endif");
                    r.WriteLine();
                    r.WriteLine();
                }
                else if (string.Equals(name, name.ToUpperInvariant(), StringComparison.Ordinal))
                {
                    r.WriteLine("/// <summary>System error " + oname + "</summary>");
                    r.WriteLine("[System::Diagnostics::CodeAnalysis::SuppressMessage(\"Microsoft.Naming\", \"CA1702:CompoundWordsShouldBeCasedCorrectly\")]");
                    r.WriteLine("[System::Diagnostics::CodeAnalysis::SuppressMessage(\"Microsoft.Naming\", \"CA1707:IdentifiersShouldNotContainUnderscores\")]");
                    r.WriteLine("[System::Diagnostics::CodeAnalysis::SuppressMessage(\"Microsoft.Naming\", \"CA1709:IdentifiersShouldBeCasedCorrectly\")]");
                    r.WriteLine("[System::ComponentModel::DescriptionAttribute(\"System error " + Escape(oname) + "\")]");
                    r.WriteLine("\t{0} = (int){1},", name, value);
                }
            }
        }

        private static string Escape(string p)
        {
            return p.Replace("\\", "\\\\").Replace("\t", "\\t").Replace("\r", "\\r").Replace("\n", "\\n").Replace("\"", "\\\"");
        }
    }
}
