using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Text.RegularExpressions;

namespace Errors2Enum
{
	class Program
	{
		static void Main(string[] args)
		{
			if (args.Length != 2)
			{
				Console.Error.WriteLine("Usage: Errors2Enum winerror.h <outputfile>");
				Environment.ExitCode = 1;
				return;
			}

			string from = args[0];
			string to = args[1];

			if (File.Exists(to) && File.GetLastWriteTime(to) > File.GetLastWriteTime(from)
				&& File.GetLastWriteTime(to) > File.GetLastWriteTime(new Uri(typeof(Program).Assembly.CodeBase).LocalPath))
			{
				Console.WriteLine("Result file already available; skipping generation of " + to);
				return;
			}

			using(StreamReader header = File.OpenText(args[0]))
			using (StreamWriter r = File.CreateText(args[1]))
			{
				r.WriteLine("/* GENERATED CODE - Don't edit this file */");
				r.WriteLine("#pragma once");
				r.WriteLine("namespace SharpSvn {");

				r.WriteLine("/// <summary>Generated mapping from winerror.h</summary>");
				r.WriteLine("public enum class SvnWindowsErrorCode {");

				WriteEnumBody(header, r);

				r.WriteLine("};");
				r.WriteLine("} /* SharpSvn */");
			}
		}

		private static void WriteEnumBody(StreamReader header, StreamWriter r)
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
							if (!msgText.Contains('\n'))
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

						if (msgId.StartsWith("ERROR_"))
							msgId = "WIN" + msgId;
						else
							msgId = "WINERROR_" + msgId;

						r.WriteLine(msgId + " = " + value + ",");
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

		private static string Escape(string p)
		{
			return p.Replace("\\", "\\\\").Replace("\t", "\\t").Replace("\r", "\\r").Replace("\n", "\\n").Replace("\"", "\\\"");
		}
	}
}
