using System;
using System.Collections.Generic;
using System.Text;
using System.Reflection;

namespace QQn.TurtleUtils.Tokenizer.Definitions
{
	public abstract class TokenItemBase
	{
		internal static T GetFirstAttribute<T>(ICustomAttributeProvider attributeProvider)
			where T : Attribute
		{
			if (attributeProvider == null)
				throw new ArgumentNullException("attributeProvider");

			foreach (T a in attributeProvider.GetCustomAttributes(typeof(T), true))
			{
				return a;
			}

			return null;
		}
	}
}
