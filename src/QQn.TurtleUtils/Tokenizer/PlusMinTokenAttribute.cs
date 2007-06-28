using System;
using System.Collections.Generic;
using System.Text;
using QQn.TurtleUtils.Tokenizer.Definitions;

namespace QQn.TurtleUtils.Tokenizer
{
	[AttributeUsage(TokenAttributeBase.TokenTargets, AllowMultiple = true)]
	public class PlusMinTokenAttribute : TokenAttribute
	{
		public PlusMinTokenAttribute(string name)
			: base(name)
		{
		}

		public override QQn.TurtleUtils.Tokenizer.Definitions.Token CreateToken(QQn.TurtleUtils.Tokenizer.Definitions.TokenMember tokenMember)
		{
			return new PlusMinToken(tokenMember, Name, this);
		}

		class PlusMinToken : Token
		{
			public PlusMinToken(TokenMember member, string name, TokenAttribute attr)
				: base(member, name, attr)
			{
			}
		}
	}
}
