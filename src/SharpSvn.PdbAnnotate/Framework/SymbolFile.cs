using System;
using System.Collections.Generic;
using System.Configuration;

namespace SharpSvn.PdbAnnotate.Framework
{
    /// <summary>
    /// 
    /// </summary>
    class SymbolFile : SourceFileBase, IComparable<SourceFile>, IEquatable<SourceFile>
    {
        readonly List<SourceFile> _sourceFiles = new List<SourceFile>();

        /// <summary>
        /// 
        /// </summary>
        /// <param name="filename"></param>
        public SymbolFile(string filename)
            : base(filename)
        {
        }

        /// <summary>
        /// 
        /// </summary>
        public IList<SourceFile> SourceFiles
        {
            get { return _sourceFiles; }
        }

        internal void AddSourceFile(SourceFile sourceFile)
        {
            if (sourceFile == null)
                throw new ArgumentNullException("sourceFile");

            if (!_sourceFiles.Contains(sourceFile))
                _sourceFiles.Add(sourceFile);
        }

        #region ## IComparable<SourceFile>, IEquatable<SourceFile>

        /// <summary>
        /// 
        /// </summary>
        /// <param name="other"></param>
        /// <returns></returns>
        public int CompareTo(SourceFile other)
        {
            return base.CompareTo(other);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="other"></param>
        /// <returns></returns>
        public bool Equals(SourceFile other)
        {
            return base.Equals(other);
        }
        #endregion ## IComparable<SourceFile>, IEquatable<SourceFile>
    }
}
