using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.PdbAnnotate.Framework;
using System.IO;
using SharpSvn.PdbAnnotate.Providers;
using System.Diagnostics;

namespace SharpSvn.PdbAnnotate.Engine
{
    static class PdbWriter
    {
        public static void WritePdbAnnotations(IndexerState state, string pdbStrPath)
        {
            foreach (SymbolFile file in state.SymbolFiles.Values)
            {
                string tmpFile = Path.GetFullPath(Path.GetTempFileName());
                try
                {
                    using (StreamWriter sw = File.CreateText(tmpFile))
                    {
                        sw.NewLine = "\n";
                        if (!WriteAnnotations(state, file, sw))
                            continue; // Temp file is deleted in finally
                    }

                    ProcessStartInfo psi = new ProcessStartInfo(pdbStrPath);
                    psi.Arguments = string.Format("-w -s:srcsrv -p:\"{0}\" -i:\"{1}\"", file.FullName, tmpFile);
                    psi.UseShellExecute = false;

                    psi.RedirectStandardError = true;
                    psi.RedirectStandardOutput = true;
                    using (Process p = Process.Start(psi))
                    {
                        p.StandardOutput.ReadToEnd();
                        p.StandardError.ReadToEnd();

                        p.WaitForExit();
                    }
                }
                finally
                {
                    File.Delete(tmpFile);
                }
            }
        }

        static bool WriteAnnotations(IndexerState state, SymbolFile file, StreamWriter sw)
        {
            SortedList<string, SourceProvider> providers = new SortedList<string, SourceProvider>();
            int itemCount = 1;

            foreach (SourceFile sf in file.SourceFiles)
            {
                if (!sf.IsResolved || sf.NoSourceAvailable)
                    continue;

                SourceReference sr = sf.SourceReference;
                SourceProvider provider = sr.SourceProvider;

                if (providers.ContainsKey(provider.Id))
                    continue;

                providers.Add(provider.Id, provider);

                if (provider.SourceEntryVariableCount > itemCount)
                    itemCount = provider.SourceEntryVariableCount;
            }

            if (providers.Count == 0)
                return false;

            sw.WriteLine("SRCSRV: ini ------------------------------------------------");
            sw.WriteLine("VERSION=1");
            sw.Write("VERCTRL=SharpSvn.PdbAnnotate");
            foreach (SourceProvider sp in providers.Values)
            {
                if (!string.IsNullOrEmpty(sp.Name))
                {
                    sw.Write('+');
                    sw.Write(sp.Name);
                }
            }
            sw.WriteLine();
            sw.WriteLine("SRCSRV: variables ------------------------------------------");
            sw.WriteLine("DATETIME=" + DateTime.Now.ToUniversalTime().ToString("u"));
            sw.WriteLine("SRCSRVTRG=%fnvar%(%VAR2%__TRG)");
            sw.WriteLine("SRCSRVCMD=%fnvar%(%VAR2%__CMD)");
            //sw.WriteLine("SRCSRVENV=PATH=%PATH%\\bSystemDrive=%SystemDrive%\\bSystemRoot=%SystemRoot%\\bProgramFiles=%ProgramFiles%\\bProgramData=%ProgramData%\\b%fnvar%(%VAR2%__ENV)");
            foreach (SourceProvider sp in providers.Values)
            {
                sp.WriteEnvironment(sw);
            }
            sw.WriteLine("SRCSRV: source files ---------------------------------------");

            // Note: the sourcefile block must be written in the order they are found by the PdbReader
            //	otherwise SrcTool skips all sourcefiles which don't exist locally and are out of order
            foreach (SourceFile sf in file.SourceFiles)
            {
                if (!sf.IsResolved || sf.NoSourceAvailable)
                    continue;

                Console.Error.WriteLine("Writing: {0}", sf.FullName);

                sw.Write(sf.FullName);
                sw.Write('*');

                SourceReference sr = sf.SourceReference;
                SourceProvider provider = sr.SourceProvider;

                sw.Write(provider.Id);
                sw.Write('*');

                string[] strings = sr.GetSourceEntries();

                if (strings != null)
                    for (int i = 0; i < itemCount; i++)
                    {
                        if (i < strings.Length)
                            sw.Write(strings[i]);

                        sw.Write('*');
                    }
                else
                {
                    for (int i = 0; i < itemCount; i++)
                        sw.Write('*');
                }

                // Note: We defined the variables upto itemCount+2 (filename, type, itemcount),
                // All variables above this index are reserved for future extensions

                sw.WriteLine();
            }

            sw.WriteLine("SRCSRV: end ------------------------------------------------");

            return true;
        }
    }
}
