using System;
using System.Collections.Generic;
using System.Text;

namespace QQn.TurtleUtils.Tokenizer
{
	[AttributeUsage(TokenAttributeBase.TokenTargets, AllowMultiple=false)]
	public sealed class TokenDescriptionAttribute : TokenAttributeBase
	{
		readonly string _description;

		public TokenDescriptionAttribute(string description)
		{
			_description = description;
		}

		public string Description
		{
			get { return _description ?? ""; }
		} 
	}
}
