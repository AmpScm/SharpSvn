using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;
using SharpSvn.MSBuild.FileParsers;

namespace SharpSvn.MSBuild
{
    public class PdbSourceAnnotate : ToolTask
    {
        [Required]
        public ITaskItem[] Sources
        {   get; set; }


        protected override string ToolName
        {
            get { return "SharpSvn.PdbAnnotate.exe"; }
        }

        protected override string GenerateFullPathToTool()
        {
            return Path.Combine(ToolPath, ToolName);
        }

        protected override string GenerateCommandLineCommands()
        {
            CommandLineBuilder cb = new CommandLineBuilder();

            //cb.AppendSwitch("-q");
            cb.AppendSwitch("--");

            cb.AppendFileNamesIfNotNull(Sources, " ");

            return cb.ToString();
        }
    }
}
