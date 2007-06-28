using System;
using System.Collections.Generic;
using System.Text;

namespace QQn.TurtleUtils.Tokenizer.Definitions
{
	public class Token : TokenItemBase
	{
		readonly TokenMember _member;
		readonly string _name;
		readonly IList<string> _aliases;

		public Token(TokenMember member, string name, TokenAttribute attr)
		{
			if (member == null)
				throw new ArgumentNullException("member");
			else if (name == null)
				throw new ArgumentNullException("name");

			_member = member;
			_name = name;
			_aliases = attr.Aliases;
		}

		public TokenMember Member
		{
			get { return _member; }
		}

		public string Name
		{
			get { return _name; }
		}

		/// <summary>
		/// Token allows a direct value ('-v1' vs  '-v' | '-v 12' | '-v=12')
		/// </summary>
		/// <param name="value"></param>
		/// <returns></returns>
		public bool AllowDirectValue(string value)
		{
			return false;
		}

		/// <summary>
		/// Token requires a value ('-source banana' | (if <see cref="AcceptsValue"/>) '-source=banana')
		/// </summary>
		public bool RequiresValue
		{
			get { return true; }
		}

		/// <summary>
		/// Token allows a value ('-source=banana')
		/// </summary>
		public bool AcceptsValue
		{
			get { return true; }
		}

		/// <summary>
		/// Token accepts a [+|-] suffix
		/// </summary>
		public bool AllowPlusMinSuffix
		{
			get { return false; }
		}

		public IList<string> Aliases
		{
			get { return _aliases ?? new string[0]; }
		}

		internal void Evaluate<T>(string value, TokenizerState<T> state)
			where T : class, new()
		{
			throw new NotImplementedException("The method or operation is not implemented.");
		}

		internal void EvaluateDirect(string value)
		{
			throw new NotImplementedException("The method or operation is not implemented.");
		}
	}
}
