using System;
using System.Collections.Generic;
using System.Text;

namespace QQn.TurtleUtils.Tokenizer.Definitions
{
	public interface IHasTokenDefinition
	{
		TokenizerDefinition GetTokenizerDefinition(TokenizerArgs args);
	}
}
