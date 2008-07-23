using System;
using System.Collections.Generic;
using System.Text;

namespace ShSvn
{
    public abstract class CommandBase
    {
        public abstract string[] Commands { get; }

        public abstract int Run(string command, string[] args, CommandState e);

        public void ShowHelp()
        {
            Console.WriteLine("Usage: ShSvn [<options>] {0}", Commands[0]);
        }
    }
}
