////////////////////////////////////////////////////////////////////////////
// This code is part of the QQn TurtleBuild project but is reused as 
// embedded class in some other projects by Bert Huijben
// 
// For the last release please visit http://code.google.com/p/turtlebuild/
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Text;

namespace QQn.ProcessRunner
{
    public class LineReceivedEventArgs : EventArgs
    {
        readonly string _line;
        public LineReceivedEventArgs(string line)
        {
            if (line == null)
                throw new ArgumentNullException("line");
            _line = line;
        }

        public string Line
        {
            get { return _line; }
        }
    }
}
