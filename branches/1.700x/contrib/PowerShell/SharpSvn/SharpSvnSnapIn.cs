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
                {
                    Cmdlets.Add(entry);
                }
            }
        }

        static CmdletConfigurationEntry GetEntry(Type t)
        {
            CmdletHelpfileAttribute[] helpfileAttributes = t.GetCustomAttributes(typeof(CmdletHelpfileAttribute), false) as CmdletHelpfileAttribute[];
            CmdletAttribute[] cmdletAttributes = t.GetCustomAttributes(typeof(CmdletAttribute), false) as CmdletAttribute[];
            if (cmdletAttributes == null || cmdletAttributes.Length == 0)
            {
                return null;
            }

            if (helpfileAttributes != null && helpfileAttributes.Length == 1 && cmdletAttributes.Length == 1)
            {
                return new CmdletConfigurationEntry(cmdletAttributes[0].VerbName + "-" + cmdletAttributes[0].NounName,
                    t, helpfileAttributes[0].HelpFileName);
            }

            if ((helpfileAttributes == null || helpfileAttributes.Length == 0) && cmdletAttributes.Length == 1)
            {
                return new CmdletConfigurationEntry(cmdletAttributes[0].VerbName + "-" + cmdletAttributes[0].NounName,
                    t, "SharpSvnPSSnapin.dll-Help.xml");
            }
            
            throw new InvalidOperationException("Multiple attributes should not be possible");
        }

        public override string Description
        {
            get { return "Powershell implementation of the Subversion commandline interface"; }
        }

        public override string Name
        {
            get { return "SvnCommands"; }
        }

        public override string Vendor
        {
            get { return "SharpSvn Contrib"; }
        }
    }

    [AttributeUsage(AttributeTargets.Class, AllowMultiple = false)]
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
