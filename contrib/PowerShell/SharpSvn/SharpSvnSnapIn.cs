using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;
using System.ComponentModel;
using System.Management.Automation.Runspaces;
using SharpSvn.PowerShell.Commands;

namespace SharpSvn.PowerShell
{
    [RunInstaller(true)]
    public class SharpSvnSnapIn : CustomPSSnapIn
    {
        public SharpSvnSnapIn()
            : base()
        {
            foreach (Type t in typeof(SharpSvnSnapIn).Assembly.GetTypes())
            {
                CmdletConfigurationEntry entry = GetEntry(t);
                if (entry != null)
                    Cmdlets.Add(entry);
            }
        }

        static CmdletConfigurationEntry GetEntry(Type t)
        {
            CmdletHelpfileAttribute[] helpfileAttributes = t.GetCustomAttributes(typeof(CmdletHelpfileAttribute), false) as CmdletHelpfileAttribute[];
            CmdletAttribute[] cmdletAttributes = t.GetCustomAttributes(typeof(CmdletAttribute), false) as CmdletAttribute[];
            if(cmdletAttributes == null || cmdletAttributes.Length == 0)
                return null;
            
            if(helpfileAttributes != null && helpfileAttributes.Length == 1 && cmdletAttributes.Length == 1)
                return new CmdletConfigurationEntry(cmdletAttributes[0].VerbName + "-" + cmdletAttributes[0].NounName,
                    t, helpfileAttributes[0].HelpFileName);

            if ((helpfileAttributes == null || helpfileAttributes.Length == 0) && cmdletAttributes.Length == 1)
                return new CmdletConfigurationEntry(cmdletAttributes[0].VerbName + "-" + cmdletAttributes[0].NounName,
                    t, null);
            
            throw new InvalidOperationException("Multiple attributes should not be possible");

        }
        public override string Description
        {
            get { return ""; }
        }

        public override string Name

        {
            get { return "SvnCommands"; }
        }

        public override string Vendor
        {
            get { return "AnkhSVN"; }
        }
    }

    [AttributeUsage(AttributeTargets.Class)]
    public sealed class CmdletHelpfileAttribute : Attribute
    {
        string _helpFileName;

        public string HelpFileName
        {
            get{ return _helpFileName;}
            set{_helpFileName =value;}
        }
    }
}
