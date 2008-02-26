using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Xml;
using System.Diagnostics;

namespace Po2Resx
{
	class Program
	{
		static void Main(string[] args)
		{
			List<string> argList = new List<string>(args);

			bool force = false;
			if (argList.Count > 1 && argList[0].Equals("-f", StringComparison.OrdinalIgnoreCase))
			{
				force = true;
				argList.RemoveAt(0);
			}
			if (argList.Count <= 3 || !string.Equals(argList[1], "-to", StringComparison.OrdinalIgnoreCase))
			{
				Console.Error.WriteLine("Usage: SharpSvn-Po2Resx [-f] <prefix> -to <directory> [<files>...]");
				Environment.ExitCode = 1;
				return;
			}

			string prefix = argList[0];
			string toDir = argList[2];
			argList.RemoveRange(0, 3);

			List<FileInfo> files = new List<FileInfo>();
			foreach (string f in argList)
			{
				int star = f.IndexOfAny(new char[] { '?', '*' });

				if (star >= 0)
				{
					star = f.LastIndexOfAny(new char[] { Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar });
					DirectoryInfo dir;

					if (star > 0)
						dir = new DirectoryInfo(f.Substring(0, star));
					else
						dir = new DirectoryInfo(Environment.CurrentDirectory);

					if (dir.Exists)
						foreach (FileInfo file in dir.GetFiles(f.Substring(star + 1)))
							files.Add(file);
					else
					{
						Console.Error.WriteLine("Directory '{0}' does not exist", dir.FullName);
						Environment.ExitCode = 1;
					}
				}
				else
				{
					FileInfo file = new FileInfo(f);
					if (file.Exists)
						files.Add(file);
					else
					{
						Console.Error.WriteLine("'{0}' does not exist", f);
						Environment.ExitCode = 1;
					}
				}
			}

			toDir = Path.GetFullPath(toDir);
			if (!Directory.Exists(toDir))
				Directory.CreateDirectory(toDir);
			
			GenerateResxFiles(prefix, toDir, files, force);
		}

		private static void GenerateResxFiles(string prefix, string toDir, List<FileInfo> files, bool force)
		{
			foreach (FileInfo file in files)
			{
				FileInfo toFile = new FileInfo(Path.Combine(toDir, prefix + "." + Path.GetFileNameWithoutExtension(file.Name).Replace('_', '-').ToLowerInvariant() + ".resx"));

				if (!force && toFile.Exists && toFile.LastWriteTime > file.LastWriteTime)
					continue; // File up2date

				XmlWriterSettings xws = new XmlWriterSettings();
				xws.Indent = true;
				using (XmlWriter xw = XmlWriter.Create(toFile.FullName, xws))
				{
					xw.WriteStartDocument();
					xw.WriteStartElement("root");

					WriteHeader(xw, "resmimetype", "text/microsoft-resx");
					WriteHeader(xw, "version", "2.0");
					WriteHeader(xw, "reader", "System.Resources.ResXResourceReader, System.Windows.Forms, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089");
					WriteHeader(xw, "writer", "System.Resources.ResXResourceWriter, System.Windows.Forms, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089");
					SortedDictionary<string, Msg> preDefined = new SortedDictionary<string, Msg>(StringComparer.OrdinalIgnoreCase);

					foreach (Msg msg in PoParser.ReadMessages(file))
					{
						Msg alt;
						if (preDefined.TryGetValue(msg.Key, out alt))
						{
							if(!alt.Value.Equals(msg.Value, StringComparison.InvariantCultureIgnoreCase))
							{
								Console.Error.WriteLine("Warning: Duplicate key found with different translation in '{0}'", file.FullName);
								Console.Error.WriteLine(" 1st key:   {0}", alt.Key);
								Console.Error.WriteLine(" 1st value: {0}", alt.Value);
								Console.Error.WriteLine(" 2nd key:   {0}", msg.Key);
								Console.Error.WriteLine(" 2nd value: {0}", msg.Value);
								Console.Error.WriteLine("Ignored the 2nd entry");
							}
							continue;
						}

						preDefined.Add(msg.Key, msg);

						if(!string.IsNullOrEmpty(msg.Comment))
							xw.WriteComment(msg.Comment);

						WriteData(xw, msg.Key, msg.Value);
					}
					xw.WriteEndElement();

				}
			}
		}

		static void WriteHeader(XmlWriter xw, string key, string value)
		{			
			xw.WriteStartElement("resheader");
			xw.WriteAttributeString("name", key);
			xw.WriteElementString("value", value);
			xw.WriteEndElement();
		}

		static string XmlXmlNs = "http://www.w3.org/XML/1998/namespace";
		private static void WriteData(XmlWriter xw, string key, string value)
		{			
			xw.WriteStartElement("data");
			xw.WriteAttributeString("name", key);
			xw.WriteAttributeString("xml", "space", XmlXmlNs, "preserve");
			xw.WriteElementString("value", value);
			xw.WriteEndElement();
		}

		
	}
}
