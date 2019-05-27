using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.PdbAnnotate.Framework;
using System.Diagnostics;
using System.IO;
using SharpSvn;

namespace SharpSvn.PdbAnnotate.Engine
{
    /// <summary>
    /// PdbReader class; this class should be re-implemented using only the DbgHelp api, to optimize performance
    /// and remove a dependency on the Debugger SDK
    /// </summary>
    static class PdbReader
    {
        /// <summary>
        /// Reads all sourcefiles referenced from a PDB, and adds these to the indexerstate
        /// </summary>
        /// <param name="state"></param>
        /// <param name="srcToolPath"></param>
        /// <param name="reIndexPreviouslyIndexedSymbols"></param>
        public static void ReadSourceFilesFromPdbs(IndexerState state, string srcToolPath, bool reIndexPreviouslyIndexedSymbols)
        {
            List<SymbolFile> pdbsToRemove = null;
            foreach (SymbolFile pdb in state.SymbolFiles.Values)
            {
                ProcessStartInfo psi = new ProcessStartInfo(srcToolPath);

                psi.WorkingDirectory = pdb.File.DirectoryName;

                psi.UseShellExecute = false;
                psi.RedirectStandardError = true;
                psi.RedirectStandardOutput = true;

                string output;
                string errors;

                if (!reIndexPreviouslyIndexedSymbols)
                {
                    psi.Arguments = string.Format("-c \"{0}\"", pdb.FullName);

                    using (Process p = Process.Start(psi))
                    {
                        output = p.StandardOutput.ReadToEnd();
                        errors = p.StandardError.ReadToEnd();

                        p.WaitForExit();
                    }

                    if (output.Contains("source files are indexed") ||
                        errors.Contains("source files are indexed") ||
                        output.Contains("source file is indexed") ||
                        errors.Contains("source file is indexed"))
                    {
                        // No need to change annotation; it is already indexed
                        if (pdbsToRemove == null)
                            pdbsToRemove = new List<SymbolFile>();

                        pdbsToRemove.Add(pdb);
                        continue;
                    }
                }

                psi.Arguments = string.Format("-r \"{0}\"", pdb.FullName);

                using (Process p = Process.Start(psi))
                {
                    output = p.StandardOutput.ReadToEnd();
                    errors = p.StandardError.ReadToEnd();

                    p.WaitForExit();
                }

                if (!string.IsNullOrEmpty(errors))
                {
                    throw new SourceIndexToolException("SRCTOOL", errors.Trim());
                }

                bool foundOne = false;
                foreach (string item in output.Split('\r', '\n'))
                {
                    string fileName = item.Trim();

                    if (string.IsNullOrEmpty(fileName))
                        continue; // We split on \r and \n

                    if ((fileName.IndexOf('*') >= 0) || // C++ Compiler internal file
                        ((fileName.Length > 2) && (fileName.IndexOf(':', 2) >= 0)))
                    {
                        // Some compiler internal filenames of C++ start with a * 
                        // and/or have a :123 suffix

                        continue; // Skip never existing files
                    }

                    if (!File.Exists(fileName))
                        continue;

                    SourceFile file;

                    if (!state.SourceFiles.TryGetValue(fileName, out file))
                    {
                        file = new SourceFile(fileName);
                        state.SourceFiles.Add(fileName, file);
                    }

                    pdb.AddSourceFile(file);
                    file.AddContainer(pdb);
                    foundOne = true;

                }

                if (!foundOne)
                {
                    if (pdbsToRemove == null)
                        pdbsToRemove = new List<SymbolFile>();

                    pdbsToRemove.Add(pdb);
                }
            }

            if (pdbsToRemove != null)
            {
                foreach (SymbolFile s in pdbsToRemove)
                {
                    state.SymbolFiles.Remove(s.FullName);
                }
            }
        }
    }
}
