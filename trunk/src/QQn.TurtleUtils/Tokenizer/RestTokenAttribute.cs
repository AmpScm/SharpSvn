using System;
using System.Collections.Generic;
using System.Text;

namespace QQn.TurtleUtils.Tokenizer
{
	[AttributeUsage(TokenAttributeBase.TokenTargets, AllowMultiple=false)]
	public sealed class RestTokenAttribute : PositionTokenAttribute
	{
		public RestTokenAttribute()
			: base(int.MaxValue)
		{
		}
	}
}
