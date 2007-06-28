using System;
using System.Collections.Generic;
using System.Text;

namespace QQn.TurtleUtils.Tokenizer
{
	[AttributeUsage(TokenAttributeBase.TokenTargets, AllowMultiple=false)]
	public sealed class OptionalTokenAttribute : TokenAttributeBase
	{
		bool _notOptional;

		public OptionalTokenAttribute()
		{
		}

		public OptionalTokenAttribute(bool optional)
		{
			_notOptional = !optional;
		}

		public bool Optional
		{
			get { return !_notOptional; }
		}
	}
}
