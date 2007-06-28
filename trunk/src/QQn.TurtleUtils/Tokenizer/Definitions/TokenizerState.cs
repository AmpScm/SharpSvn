using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace QQn.TurtleUtils.Tokenizer.Definitions
{
	class TokenizerState<T> : Hashtable
		where T : class, new()
	{
		readonly T _instance;
		readonly TokenizerDefinition _definition;
		readonly TokenizerArgs _args;

		public TokenizerState(T instance, TokenizerDefinition definition, TokenizerArgs args)
		{
			if (instance == null)
				throw new ArgumentNullException("instance");
			else if (definition == null)
				throw new ArgumentNullException("definition");
			else if (args == null)
				throw new ArgumentNullException("args");

			_instance = instance;
			_definition = definition;
			_args = args;
		}

		public T Instance
		{
			get { return _instance; }
		}

		public TokenizerDefinition Definition
		{
			get { return _definition; }
		}
	}
}
