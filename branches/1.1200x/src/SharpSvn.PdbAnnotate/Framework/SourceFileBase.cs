using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace SharpSvn.PdbAnnotate.Framework
{
    /// <summary>
    /// Baseclass for <see cref="SourceFile"/> and <see cref="SymbolFile"/>
    /// </summary>
    abstract class SourceFileBase : IComparable
    {
        readonly string _filename;
        FileInfo _info;

        /// <summary>
        /// Creates a new SourceFile object for the specified file
        /// </summary>
        /// <param name="filename"></param>
        protected SourceFileBase(string filename)
        {
            if (string.IsNullOrEmpty(filename))
                throw new ArgumentNullException("filename");

            _filename = filename;
        }

        /// <summary>
        /// 
        /// </summary>
        public FileInfo File
        {
            get { return _info ?? (_info = new FileInfo(_filename)); }
        }

        /// <summary>
        /// Gets the fullname of the file
        /// </summary>
        public string FullName
        {
            get { return _filename; }
        }

        /// <summary>
        /// Gets a (cached) boolean indicating whether the file exists on disk
        /// </summary>
        public bool Exists
        {
            get { return File.Exists; }
        }

        /// <summary>
        /// returns <see cref="FullName"/>
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return _filename;
        }

        #region ## Compare members (specialized by base classes; for generics)

        /// <summary>
        /// Compares two <see cref="SourceFile"/> by its <see cref="FullName"/>
        /// </summary>
        /// <param name="other"></param>
        /// <returns></returns>
        public int CompareTo(SourceFileBase other)
        {
            return string.Compare(FullName, other.FullName, StringComparison.OrdinalIgnoreCase);
        }

        /// <summary>
        /// Compares two <see cref="SourceFile"/> by its <see cref="FullName"/>
        /// </summary>
        /// <param name="other"></param>
        /// <returns></returns>
        public bool Equals(SourceFileBase other)
        {
            if (other == null)
                return false;

            return string.Equals(FullName, other.FullName, StringComparison.OrdinalIgnoreCase);
        }

        #endregion ## Compare members (specialized by base classes; for generics)

        #region ## .Net 1.X compatible compare Members

        /// <summary>
        /// Compares two <see cref="SourceFile"/> by its <see cref="FullName"/>
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        public int CompareTo(object obj)
        {
            // Use typed version
            return CompareTo(obj as SourceFileBase);
        }

        /// <summary>
        /// Compares two <see cref="SourceFile"/> by its <see cref="FullName"/>
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        public override bool Equals(object obj)
        {
            return Equals(obj as SourceFileBase);
        }
        #endregion ## .Net 1.X compatible compare Members

        /// <summary>
        /// Gets the hashcode of the <see cref="SourceFile"/>
        /// </summary>
        /// <returns></returns>
        public override int GetHashCode()
        {
            return StringComparer.OrdinalIgnoreCase.GetHashCode(FullName);
        }
    }
}
