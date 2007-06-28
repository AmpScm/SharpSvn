using System;
using System.Collections.Generic;
using System.Text;

namespace QQn.TurtleUtils.Tokenizer
{
	[AttributeUsage(TokenAttributeBase.TokenTargets, AllowMultiple=true)]
	public class PositionTokenAttribute : TokenAttribute
	{
		readonly int _position;

		public PositionTokenAttribute(int position)
		{
			if (position < 0)
				throw new ArgumentOutOfRangeException("position", position, "Position must be at least 0");

			_position = position;
		}

		public int Position
		{
			get { return _position; }
		} 
	}
}
