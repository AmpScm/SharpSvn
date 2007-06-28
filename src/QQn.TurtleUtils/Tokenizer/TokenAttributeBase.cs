using System;
using System.Collections.Generic;
using System.Text;

namespace QQn.TurtleUtils
{
	public abstract class TokenAttributeBase : Attribute
	{
		internal const AttributeTargets TokenTargets = AttributeTargets.Field | AttributeTargets.Property;

		protected TokenAttributeBase()
		{
		}
	}
}
