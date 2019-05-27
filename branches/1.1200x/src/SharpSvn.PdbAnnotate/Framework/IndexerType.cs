using System;
using System.Collections.Generic;
using System.Text;

namespace SharpSvn.PdbAnnotate.Framework
{
	/// <summary>
	/// 
	/// </summary>
	class IndexerTypeData
	{
		string _path;
		string _type;
		string _info;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="path"></param>
		/// <param name="type"></param>
		/// <param name="info"></param>
		public IndexerTypeData(string path, string type, string info)
		{
			if (path == null)
				throw new ArgumentNullException("path");
			else if (type == null)
				throw new ArgumentNullException("type");

			_path = path;
			_type = type;
			_info = string.IsNullOrEmpty(info) ? "" : info;
		}

		/// <summary>
		/// The path the information applies to
		/// </summary>
		public string Path
		{
			get { return _path; }
		}

		/// <summary>
		/// The provider the information applies to
		/// </summary>
		public string Type
		{
			get { return _type; }
		}

		/// <summary>
		/// The information
		/// </summary>
		/// <remarks>Can be null, if the only provided information is the type</remarks>
		public string Info
		{
			get { return _info; }
		}
	}
}
