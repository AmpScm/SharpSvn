using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.PdbAnnotate.Framework;
using SharpSvn.PdbAnnotate.Providers;
using System.IO;

namespace SharpSvn.PdbAnnotate.Framework
{
	/// <summary>
	/// 
	/// </summary>
	sealed class IndexerState
	{
        readonly SortedList<string, SymbolFile> _symbolFiles = new SortedList<string, SymbolFile>(StringComparer.OrdinalIgnoreCase);
        readonly SortedList<string, SourceFile> _sourceFiles = new SortedList<string, SourceFile>(StringComparer.OrdinalIgnoreCase);
        readonly List<SourceResolver> _resolvers = new List<SourceResolver>();

		/// <summary>
		/// 
		/// </summary>
		public IndexerState()
		{
            _resolvers.Add(new SubversionResolver(this));
		}

		/// <summary>
		/// 
		/// </summary>
		public SortedList<string, SymbolFile> SymbolFiles
		{
			get { return _symbolFiles; }
		}

		/// <summary>
		/// 
		/// </summary>
		public SortedList<string, SourceFile> SourceFiles
		{
			get { return _sourceFiles; }
		}

		/// <summary>
		/// 
		/// </summary>
		public List<SourceResolver> Resolvers
		{
			get { return _resolvers; }
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="path"></param>
		/// <returns></returns>
		public string NormalizePath(string path)
		{
			if (path == null)
				throw new ArgumentNullException("path");

			path = path.Replace(Path.AltDirectorySeparatorChar, Path.DirectorySeparatorChar);

			if (!Path.IsPathRooted(path) || path.Contains(".."))
				path = Path.GetFullPath(path);

			return path;
		}

		static string SafeId(string name)
		{
			StringBuilder sb = new StringBuilder();

			if (name.Length > 0)
			{
				if (!char.IsLetter(name, 0))
					sb.Append("PRV");
				else
					sb.Append(name[0]);

				for (int i = 1; i < name.Length; i++)
					if (char.IsLetterOrDigit(name, i))
						sb.Append(char.ToUpperInvariant(name[i]));
			}
			else
				return "PRV";
			
			return sb.ToString();
		}
	}
}
