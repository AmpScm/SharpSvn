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

        [Parameter(HelpMessage="The default credential to use for authentication.")]
        public ICredentials Credential
        {
            get { return Client.Authenticator.DefaultCredentials; }
            set { Client.Authenticator.DefaultCredentials = value; }
        }

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
