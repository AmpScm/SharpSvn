using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.PdbAnnotate;
using SharpSvn.PdbAnnotate.Framework;
using System.IO;
using System.Reflection;
using Microsoft.Win32;
using SharpSvn;
using SharpSvn.PdbAnnotate.Engine;

namespace SharpSvn.PdbAnnotate
{
    class Program
    {
        [STAThread]
        static int Main(string[] args)
        {
            bool quiet = false;
            bool nologo = false;
            SourceServerIndexer indexer = LoadIndexer(args, out quiet);

            if (!quiet && !nologo)
            {
                Console.Write(((AssemblyProductAttribute)typeof(Program).Assembly.GetCustomAttributes(typeof(AssemblyProductAttribute), false)[0]).Product);

                AssemblyName name = new AssemblyName(typeof(Program).Assembly.FullName);
                Console.WriteLine(" " + name.Version.ToString(4));
                Console.WriteLine(((AssemblyCopyrightAttribute)typeof(Program).Assembly.GetCustomAttributes(typeof(AssemblyCopyrightAttribute), false)[0]).Copyright);
                Console.WriteLine("The sourcecode of this program has been released under the Apache licence.");
                Console.WriteLine();
            }

            if (indexer == null)
            {
                ShowHelp();
                return 1;
            }

            IndexerResult result = indexer.Exec();

            if (!result.Success)
            {
                Console.Error.WriteLine(result.ErrorMessage);
                return 1;
            }

            if (!quiet)
            {
                Console.WriteLine("Added {0}/{1} references to {2} symbolfile(s) with {3} provider(s).", result.IndexedSourceFiles, result.TotalSourceFiles, result.IndexedSymbolFiles, result.ProvidersUsed);
            }

            return 0; // C
        }

        private static void ShowHelp()
        {
            Console.WriteLine(@"
{0} [<arguments>] file1.pdb [file2.pdb...]

  -?, -h[elp]          Shows this helptext
  -q[uiet]             Shows no output if annotating the pdb was successfull
  -i[nclude] <path>    Includes only files within this path (can be used
                       multiple times). Directories and files can be used.
  -f[orce]             Add annotation to previously annotated files
                       (not recommended)
  -nologo              Hide logo
  -x, -exclude <path>  Don't include files in the specified path (can be used
                       multiple times). Directories and files can be used.

Please note:
 * Selecting multiple pdb files at once is far more efficient than calling 
   SsIndex once for each file.
 * <path> references can contain ?, *, ** and *** glob parameters.
   (match 1 character, 0 or more characters, 1 or more directory levels and
   0 or more directory levels).
", new AssemblyName(typeof(Program).Assembly.FullName).Name);
        }

        private static SourceServerIndexer LoadIndexer(string[] args, out bool quiet)
        {
            SourceServerIndexer indexer = new SourceServerIndexer();

            quiet = false;
            nologo = false;
            int i;
            for (i = 0; i < args.Length; i++)
            {
                bool breakOut = false;
                string arg = args[i];
                string param = (i + 1 < args.Length) ? args[i + 1] : null;

                if (arg == "/?")
                    return null; // Special case default help mode

                if (arg.Length > 1 && arg[0] == '-')
                {
                    switch (arg.Substring(1).ToLowerInvariant())
                    {
                        case "-":
                            breakOut = true;
                            break;
                        case "?":
                        case "h":
                        case "help":
                            return null; // We show help
                        case "nologo":
                            nologo = true;
                            break;
                        case "q":
                        case "quiet":
                            quiet = true;
                            break;
                        case "f":
                        case "force":
                            indexer.ReIndexPreviouslyIndexedSymbols = true;
                            break;
                        case "i":
                        case "inc":
                        case "include":
                            if (param == null)
                                return null; // Show help and exit with errorlevel 1

                            if (param.Contains("?") || param.Contains("*"))
                            {
                                foreach (string item in ExpandGlob(param, true, true))
                                {
                                    indexer.SourceRoots.Add(Path.GetFullPath(item));
                                }
                            }
                            else
                                indexer.SourceRoots.Add(Path.GetFullPath(param));

                            i++; // Skip next argument
                            break;


                        case "x":
                        case "exc":
                        case "exclude":
                            if (param == null)
                                return null; // Show help and exit with errorlevel 1

                            if (param.Contains("?") || param.Contains("*"))
                            {
                                foreach (string item in ExpandGlob(param, true, true))
                                {
                                    indexer.ExcludeSourceRoots.Add(Path.GetFullPath(item));
                                }
                            }
                            else
                                indexer.ExcludeSourceRoots.Add(Path.GetFullPath(param));

                            i++; // Skip next argument
                            break;

                        default:
                            Console.Error.WriteLine("Unknown argument '{0}'", arg);
                            return null;
                    }

                    if (breakOut)
                    {
                        i++;
                        break;
                    }
                }
                else
                    break;
            }

            if (i >= args.Length)
                return null; // Show help

            for (; i < args.Length; i++)
            {
                string param = args[i];

                if (param.Contains("?") || param.Contains("*"))
                {
                    foreach (string item in ExpandGlob(param, false, true))
                    {
                        indexer.SymbolFiles.Add(SvnTools.GetNormalizedFullPath(item));
                    }
                }
                else
                    indexer.SymbolFiles.Add(SvnTools.GetNormalizedFullPath(param));
            }

            return indexer;
        }

        private static IEnumerable<string> ExpandGlob(string param, bool returnDirs, bool returnFiles)
        {
            param = param.Replace(Path.AltDirectorySeparatorChar, Path.DirectorySeparatorChar);

            int nFixed = param.IndexOfAny(new char[] { '*', '?' });

            if (nFixed < 0)
                return new string[] { param };

            int nSep = param.LastIndexOf(Path.DirectorySeparatorChar, nFixed);

            string searchRoot;
            string rest;
            if (nSep > 0)
            {
                searchRoot = Path.GetFullPath(param.Substring(0, nSep));
                rest = param.Substring(nSep + 1);
            }
            else
            {
                searchRoot = Path.GetFullPath(Environment.CurrentDirectory);
                rest = param;
            }

            DirectoryInfo dir = new DirectoryInfo(searchRoot);

            SortedList<string, string> items = new SortedList<string, string>(StringComparer.InvariantCultureIgnoreCase);

            RecursiveFindGlobParts(items, dir, rest.Split(Path.DirectorySeparatorChar), 0, returnDirs, returnFiles);

            return items.Keys;
        }


        private static void RecursiveFindGlobParts(SortedList<string, string> items, DirectoryInfo dir, string[] parts, int index, bool returnDirs, bool returnFiles)
        {
            if (index >= parts.Length)
                return;

            string part = parts[index];

            if (part == ".")
                RecursiveFindGlobParts(items, dir, parts, index + 1, returnDirs, returnFiles);
            else if (part == "..")
                RecursiveFindGlobParts(items, dir.Parent, parts, index + 1, returnDirs, returnFiles);

            bool isLast = (index == parts.Length - 1);
            if (!isLast || returnDirs)
            {
                bool recursive = part.Contains("**");

                if (recursive && (part == "***"))
                    RecursiveFindGlobParts(items, dir, parts, index + 1, returnDirs, returnFiles);

                if (recursive)
                    part = part.Replace("**", "*");

                SearchOption option = recursive ? SearchOption.AllDirectories : SearchOption.TopDirectoryOnly;

                foreach (DirectoryInfo subdir in dir.GetDirectories(part, option))
                {
                    if (isLast)
                        items[subdir.FullName] = subdir.FullName;

                    RecursiveFindGlobParts(items, subdir, parts, index + 1, returnDirs, returnFiles);
                }
            }

            if (isLast && returnFiles)
            {
                foreach (FileInfo fif in dir.GetFiles(part))
                {
                    items[fif.FullName] = fif.FullName;
                }
            }
        }
    }
}
