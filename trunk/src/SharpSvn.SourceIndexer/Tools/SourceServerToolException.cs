using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace SharpSvn.SourceIndexer.Tools
{
    public class SourceServerToolException : ExternalException
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="SourceServerToolException"/> class.
        /// </summary>
        public SourceServerToolException()
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="SourceServerToolException"/> class.
        /// </summary>
        /// <param name="message">The message.</param>
        public SourceServerToolException(string message)
            : base(message)
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="SourceServerToolException"/> class.
        /// </summary>
        /// <param name="message">The message.</param>
        /// <param name="inner">The inner.</param>
        public SourceServerToolException(string message, Exception inner)
            : base(message, inner)
        {
        }

    }
}
