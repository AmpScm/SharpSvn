﻿//
// Copyright 2008-2025 The SharpSvn Project
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
using System.IO;

using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;

using SharpSvn;
using SharpSvn.Tests.Commands;

namespace SharpSvn.Tests.Misc
{
    /// <summary>
    /// Summary description for ExceptionTest.
    /// </summary>
    [TestClass]
    public class ExceptionTests : TestBase
    {
        /// <summary>
        /// Attempt to add a file that is not in a vc dir.
        /// </summary>
        [TestMethod]
        [ExpectedException(typeof(SvnInvalidNodeKindException))]
        public void TestAddFileInNonVersionedDir()
        {
            string tempFile = Path.GetTempFileName();
            this.Client.Add(tempFile);
        }

        /// <summary>
        /// Attempt to commit an out of date resource.
        /// </summary>
        /*[TestMethod]
        [ExpectedException(typeof(ResourceOutOfDateException))]
        public void TestResourceOutOfDate()
        {
            string wc2 = null;

            try
            {
                wc2 = this.FindDirName( Path.Combine( TestBase.BASEPATH, TestBase.WC_NAME ) );
                Zip.ExtractZipResource( wc2, this.GetType(), this.WC_FILE );
                this.RenameAdminDirs( wc2 );

                using (StreamWriter w = new StreamWriter( Path.Combine( this.WcPath, "Form.cs" ), true ) )
                    w.Write( "Moo" );

                this.RunCommand( "svn", "ci -m \"\" " + this.WcPath );

                using (StreamWriter w2 = new StreamWriter( Path.Combine( wc2, "Form.cs" ), true ) )
                    w2.Write( "Moo" );

                this.Client.Commit( wc2 });
            }
            finally
            {
                if ( wc2 != null )
                    PathUtils.RecursiveDelete( wc2 );

            }
        }*/

    }
}
