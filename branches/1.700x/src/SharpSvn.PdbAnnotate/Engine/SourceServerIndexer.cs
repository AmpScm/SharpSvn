using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using SharpSvn.PdbAnnotate.Framework;
using System.Diagnostics;
using SharpSvn.PdbAnnotate.Providers;
using SharpSvn.PdbAnnotate.Engine;
using Microsoft.Win32;
using SharpSvn;

namespace SharpSvn.PdbAnnotate.Engine
{
    /// <summary>
    /// 
    /// </summary>
    class SourceServerIndexer
    {
        IList<string> _symbolFiles = new List<string>();
        IList<string> _sourceRoots = new List<string>();
        IList<string> _excludeSourceRoots = new List<string>();

        IList<string> _providerTypes = new List<string>(new string[]
			{
				typeof(SubversionResolver).FullName,
			});

        IList<string> _srcTypes = new List<string>(new string[] { "autodetect" });
        IDictionary<string, IndexerTypeData> _indexerData = new SortedList<string, IndexerTypeData>(StringComparer.InvariantCultureIgnoreCase);

        bool _reindexPreviouslyIndexed;

        /// <summary>
        /// Initializes a new SourceServerIndexer
        /// </summary>
        public SourceServerIndexer()
        {
        }
        /// <summary>
        /// Gets or sets a list of symbol files to index
        /// </summary>
        public IList<string> SymbolFiles
        {
            get { return _symbolFiles; }
            set
            {
                if (value != null)
                    _symbolFiles = value;
                else
                    _symbolFiles = new string[0];
            }
        }

        /// <summary>
        /// Gets or sets a list of sourcecode directories to index
        /// </summary>
        /// <remarks>If one or more sourceroots are specified, only files in and below these directories are indexed</remarks>
        public IList<string> SourceRoots
        {
            get { return _sourceRoots; }
            set
            {
                if (value != null)
                    _sourceRoots = value;
                else
                    _sourceRoots = new string[0];
            }
        }

        /// <summary>
        /// Gets or sets a list of sourcecode directories not to index
        /// </summary>
        /// <remarks>These directories allow to exclude specific directories which are included in the <see cref="SourceRoots"/></remarks>
        public IList<string> ExcludeSourceRoots
        {
            get { return _sourceRoots; }
            set
            {
                if (value != null)
                    _excludeSourceRoots = value;
                else
                    _excludeSourceRoots = new string[0];
            }
        }

        /// <summary>
        /// 
        /// </summary>
        public bool ReIndexPreviouslyIndexedSymbols
        {
            get { return _reindexPreviouslyIndexed; }
            set { _reindexPreviouslyIndexed = value; }
        }

        string _srcToolPath;
        string _pdbStrPath;

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public IndexerResult Exec()
        {
            Uri codeBase = new Uri(typeof(SourceServerIndexer).Assembly.CodeBase);
            string myDir = Path.GetDirectoryName(codeBase.LocalPath);

            _srcToolPath = SvnTools.GetNormalizedFullPath(Path.Combine(myDir, "srctool.exe"));
            _pdbStrPath = SvnTools.GetNormalizedFullPath(Path.Combine(myDir, "pdbstr.exe"));

            if (!File.Exists(_srcToolPath))
                throw new FileNotFoundException("SRCTOOL.EXE not found", _srcToolPath);
            if (!File.Exists(_srcToolPath))
                throw new FileNotFoundException("PDBSTR.EXE not found", _pdbStrPath);

            IndexerState state = new IndexerState();

            foreach (string pdbFile in SymbolFiles)
            {
                SymbolFile symbolFile = new SymbolFile(pdbFile);

                if (!symbolFile.Exists)
                    throw new FileNotFoundException(string.Format("Symbol {0} file not found", symbolFile.FullName), symbolFile.FullName);

                state.SymbolFiles.Add(symbolFile.FullName, symbolFile);
            }

            ReadSourceFilesFromPdbs(state); // Check if there are files to index for this pdb file

            PerformExclusions(state);

            ResolveFiles(state);

            WritePdbAnnotations(state);

            return CreateResultData(state);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="state"></param>
        void ReadSourceFilesFromPdbs(IndexerState state)
        {
            PdbReader.ReadSourceFilesFromPdbs(state, _srcToolPath, ReIndexPreviouslyIndexedSymbols);
        }

        void PerformExclusions(IndexerState state)
        {
            #region - Apply SourceRoots
            if (SourceRoots.Count > 0)
            {
                List<string> rootList = new List<string>();

                foreach (string root in SourceRoots)
                {
                    string nRoot = state.NormalizePath(root);

                    if (!nRoot.EndsWith("\\"))
                        nRoot += "\\";

                    rootList.Add(nRoot);
                }

                string[] roots = rootList.ToArray();
                Array.Sort<string>(roots, StringComparer.InvariantCultureIgnoreCase);

                foreach (SourceFile sf in state.SourceFiles.Values)
                {
                    string fileName = sf.FullName;

                    int n = Array.BinarySearch<string>(roots, fileName, StringComparer.InvariantCultureIgnoreCase);

                    if (n >= 0)
                        continue; // Exact match found

                    n = ~n;

                    if ((n > 0) && (n <= roots.Length))
                    {
                        if (fileName.StartsWith(roots[n - 1], StringComparison.InvariantCultureIgnoreCase))
                            continue; // Root found

                        sf.NoSourceAvailable = true;
                        continue;
                    }
                    else
                        sf.NoSourceAvailable = true;
                }
            }
            #endregion - Apply SourceRoots
            #region - Apply ExcludeSourceRoots
            if (ExcludeSourceRoots.Count > 0)
            {
                List<string> rootList = new List<string>();

                foreach (string root in ExcludeSourceRoots)
                {
                    string nRoot = state.NormalizePath(root);

                    if (!nRoot.EndsWith(Path.DirectorySeparatorChar.ToString()))
                        nRoot += Path.DirectorySeparatorChar;

                    rootList.Add(nRoot);
                }

                string[] roots = rootList.ToArray();
                Array.Sort<string>(roots, StringComparer.InvariantCultureIgnoreCase);

                foreach (SourceFile sf in state.SourceFiles.Values)
                {
                    string fileName = sf.FullName;

                    int n = Array.BinarySearch<string>(roots, fileName, StringComparer.InvariantCultureIgnoreCase);

                    if (n >= 0)
                        continue; // Exact match found

                    n = ~n;

                    if ((n > 0) && (n <= roots.Length))
                    {
                        if (fileName.StartsWith(roots[n - 1], StringComparison.InvariantCultureIgnoreCase))
                            sf.NoSourceAvailable = true;
                    }
                }
            }
            #endregion


        }

        void ResolveFiles(IndexerState state)
        {
            foreach (SourceResolver sp in state.Resolvers)
            {
                sp.ResolveFiles();
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="state"></param>
        void WritePdbAnnotations(IndexerState state)
        {
            PdbWriter.WritePdbAnnotations(state, _pdbStrPath);
        }

        IndexerResult CreateResultData(IndexerState state)
        {
            int nSources = 0;

            foreach (SourceFile sf in state.SourceFiles.Values)
            {
                if (sf.SourceReference != null)
                    nSources++;
            }

            return new IndexerResult(true, state.SymbolFiles.Count, nSources, state.SourceFiles.Count, state.Resolvers.Count);
        }
    }
}
