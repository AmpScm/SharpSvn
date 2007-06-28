using System;
using System.Collections.Generic;
using System.Text;
using System.Reflection;

namespace QQn.TurtleUtils.Tokenizer.Definitions
{
	public class TokenMember : TokenItemBase
	{
		readonly string _name;
		readonly IList<Token> _tokens;

		public TokenMember(MemberInfo member)
		{
			if (member == null)
				throw new ArgumentNullException("member");

			_name = member.Name;

			List<Token> tokens = null;
			foreach (TokenAttribute a in member.GetCustomAttributes(typeof(TokenAttribute), true))
			{
				if (a.Name == null)
					continue; // Position token
				if(tokens == null)
					tokens = new List<Token>();
				
				tokens.Add(a.CreateToken(this));
			}
			if (tokens != null)
				_tokens = tokens.AsReadOnly();
			else
				_tokens = new Token[0];
		}

		public string Name
		{
			get { return _name; }
		}

		public IList<Token> Tokens
		{
			get { return _tokens; }
		}
	}
}
