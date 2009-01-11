// $Id$
//
// Copyright 2007-2009 The SharpSvn Project
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

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;
using System.Runtime.InteropServices;
using System.Drawing;
using SharpSvn.UI.Properties;

namespace SharpSvn.UI.Authentication
{
    /// <summary>
    /// Former public api used for binding to SharpSvn
    /// </summary>
    static class SharpSvnGui
    {
        class ParentProvider : System.Windows.Forms.IWin32Window
        {
            readonly IntPtr _handle;

            public ParentProvider(IntPtr handle)
            {
                _handle = handle;
            }

            public IntPtr Handle
            {
                get { return _handle; }
            }
        }

        static Regex _realmRegex;
        /// <summary>
        /// Makes the description.
        /// </summary>
        /// <param name="realm">The realm.</param>
        /// <param name="format">The format.</param>
        /// <returns></returns>
        internal static string MakeDescription(string realm, string format)
        {
            if (null == _realmRegex)
                _realmRegex = new Regex("^\\<(?<server>[a-z]+://[^ >]+)\\> (?<realm>.*)$", RegexOptions.ExplicitCapture | RegexOptions.Singleline);

            Match m = _realmRegex.Match(realm);
            Uri uri;
            if (m.Success && Uri.TryCreate(m.Groups[1].Value, UriKind.Absolute, out uri))
            {
                return string.Format(format, uri, m.Groups[2].Value);
            }
            else
                return string.Format(format, "", realm);
        }

        /// <summary>
        /// Determines whether [has win32 handle] [the specified owner].
        /// </summary>
        /// <param name="owner">The owner.</param>
        /// <returns>
        /// 	<c>true</c> if [has win32 handle] [the specified owner]; otherwise, <c>false</c>.
        /// </returns>
        public static bool HasWin32Handle(Object owner)
        {
            IWin32Window window = owner as IWin32Window;

            return (window != null) && window.Handle != IntPtr.Zero;
        }


        /// <summary>
        /// Gets the win32 handle.
        /// </summary>
        /// <param name="owner">The owner.</param>
        /// <returns></returns>
        public static IntPtr GetWin32Handle(Object owner)
        {
            IWin32Window window = owner as IWin32Window;

            return (window != null) ? window.Handle : IntPtr.Zero;
        }

    }
}
