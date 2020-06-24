using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using SharpSvn.PdbAnnotate.Providers;

namespace SharpSvn.PdbAnnotate.Framework
{
    /// <summary>
    /// 
    /// </summary>
    class SourceFile : SourceFileBase, IComparable<SourceFile>, IEquatable<SourceFile>
    {
        readonly SortedList<string, SymbolFile> _symbolFiles = new SortedList<string, SymbolFile>(StringComparer.InvariantCultureIgnoreCase);
        SourceReference _sourceReference;
        bool _noSourceAvailable;

        /// <summary>
        /// Creates a new SourceFile object for the specified file
        /// </summary>
        /// <param name="filename"></param>
        public SourceFile(string filename)
            : base(filename)
        {
        }

        /// <summary>
        /// Gets tje list of symbolfiles containing a reference to this sourcefile
        /// </summary>
        public IList<SymbolFile> Containers
        {
            get { return _symbolFiles.Values; }
        }

        /// <summary>
        /// Gets or sets the <see cref="SourceReference"/> for this <see cref="SourceFile"/>
        /// </summary>
        public SourceReference SourceReference
        {
            get { return _sourceReference; }
            set { _sourceReference = value; }
        }

        /// <summary>
        /// Gets a value indicating whether a source-reference has been found
        /// </summary>
        /// <value><c>true</c> when a <see cref="SourceReference"/> is available or when <see cref="NoSourceAvailable"/> is set, otherwise <c>false</c></value>
        public bool IsResolved
        {
            get { return (SourceReference != null) || NoSourceAvailable; }
        }

        /// <summary>
        /// Gets or sets a boolean indicating no source is available
        /// </summary>
        /// <remarks>SourceProviders should set this property to true for files other <see cref="SourceProvider"/>s don't 
        /// need to look for, but don't have a <see cref="SourceReference"/></remarks>
        public bool NoSourceAvailable
        {
            get { return _noSourceAvailable; }
            set { _noSourceAvailable = value; }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="symbolFile"></param>
        internal void AddContainer(SymbolFile symbolFile)
        {
            if (symbolFile == null)
                throw new ArgumentNullException("symbolFile");

            if (!_symbolFiles.ContainsKey(symbolFile.FullName))
                _symbolFiles.Add(symbolFile.FullName, symbolFile);
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
