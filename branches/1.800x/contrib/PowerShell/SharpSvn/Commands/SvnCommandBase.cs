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

using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;
using System.Net;

namespace SharpSvn.PowerShell
{
    public abstract class SvnCommandBase<TArgument>
        : PSCmdlet
        where TArgument : SvnClientArgs, new()
    {
        protected SvnCommandBase()
            : base()
        { }

        //[Parameter(HelpMessage="The default credential to use for authentication.")]
        //public ICredentials Credential
        //{
        //    get { return Client.Authenticator.DefaultCredentials; }
        //    set { Client.Authenticator.DefaultCredentials = value; }
        //}

        SvnClient _client;
        protected SvnClient Client
        {
            get
            {
                if (_client == null)
                {
                    _client = new SvnClient();
                    _client.SvnError += delegate(object sender, SvnErrorEventArgs e)
                    {
                        WriteError(new ErrorRecord(e.Exception, e.Exception.SvnErrorCode.ToString(), ErrorCategory.FromStdErr, null));
                        e.Cancel = true;
                    };
                    _client.Processing += delegate(object sender, SvnProcessingEventArgs e)
                    {
                    };
                    _client.Progress += delegate(object sender, SvnProgressEventArgs e)
                    {

                    };
                    _client.Notify += delegate(object sender, SvnNotifyEventArgs e)
                    {
                    };
                }
                return _client;
            }
        }

        TArgument _args;

        protected TArgument SvnArguments
        {
            get
            {
                if (_args == null)
                    _args = new TArgument();
                return _args;
            }
        }
    }
}
