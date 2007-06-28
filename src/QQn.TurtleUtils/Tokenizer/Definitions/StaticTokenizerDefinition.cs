using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Reflection;

namespace QQn.TurtleUtils.Tokenizer.Definitions
{
	sealed class StaticTokenizerDefinition<T> : TokenizerDefinition
		where T : class, new()
	{
		public StaticTokenizerDefinition()
		{
			foreach (MemberInfo mi in typeof(T).GetMembers(BindingFlags.Public | BindingFlags.Instance))
			{
				if ((mi.MemberType != MemberTypes.Field) && (mi.MemberType != MemberTypes.Property))
					continue;

				if(null == GetFirstAttribute<TokenAttributeBase>(mi))
					continue;

				TokenMember token = new TokenMember(mi);
				_tokens.Add(token.Name, token);

				foreach (PositionTokenAttribute pos in mi.GetCustomAttributes(typeof(PositionTokenAttribute), true))
				{
					if(pos is RestTokenAttribute)
						SetRest(token);
					else
						AddPlaced(pos.Position, token);
				}
			}

			Validate();
		}
	}
}
