using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;

namespace SharpSvn.PowerShell
{
    public abstract class SvnCommandBase<TArgument> 
        : PSCmdlet
        where TArgument : SvnClientArgs, new()
    {
        protected SvnCommandBase()
            : base()
        { }

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
