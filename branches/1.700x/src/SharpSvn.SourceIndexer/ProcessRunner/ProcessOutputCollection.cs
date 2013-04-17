// $Id$
//
// Copyright 2008-2009 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

////////////////////////////////////////////////////////////////////////////
// This code is part of the QQn TurtleBuild project but is reused as 
// embedded class in some other projects by Bert Huijben
// 
// For the last release please visit http://code.google.com/p/turtlebuild/
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Text;
using System.Collections.ObjectModel;
using System.Collections;
using System.Diagnostics;

namespace QQn.ProcessRunner
{
    [DebuggerDisplay("Lines={Count}, FirstLine={FirstLine}")]
    public class ProcessOutputCollection : Collection<string>
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="ProcessOutputCollection"/> class.
        /// </summary>
        public ProcessOutputCollection()
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="ProcessOutputCollection"/> class.
        /// </summary>
        /// <param name="lines">The lines.</param>
        public ProcessOutputCollection(IEnumerable<string> lines)
            : base(new List<string>(lines))
        {
        }

        /// <summary>
        /// Wraps the specified list
        /// </summary>
        /// <param name="list"></param>
        internal ProcessOutputCollection(List<string> list)
            : base(list)
        {
        }

        /// <summary>
        /// Returns a <see cref="T:System.String"/> that represents the current <see cref="T:System.Object"/>.
        /// </summary>
        /// <returns>
        /// A <see cref="T:System.String"/> that represents the current <see cref="T:System.Object"/>.
        /// </returns>
        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            foreach (string s in this)
                sb.AppendLine(s);

            return sb.ToString();
        }

        /// <summary>
        /// Gets the first line.
        /// </summary>
        /// <value>The first line.</value>
        public string FirstLine
        {
            get { return Count > 0 ? this[0] : null; }
        }

        /// <summary>
        /// Gets a value indicating whether this instance is empty.
        /// </summary>
        /// <value><c>true</c> if this instance is empty; otherwise, <c>false</c>.</value>
        public bool IsEmpty
        {
            get { return Count == 0; }
        }
    }
}
