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
using System.Reflection;

namespace ShSvn
{
    /// <summary>
    /// Very minimalistic svn.exe replacement for usage in buildscripts
    /// </summary>
    /// <remarks>Currently implemenst svn export, svn wcversion and svn pdbindex</remarks>
    public static class Program
    {
        internal static Dictionary<string, CommandBase> _commands = new Dictionary<string, CommandBase>(StringComparer.OrdinalIgnoreCase);
        static void Main(string[] args)
        {
            Load();
            CommandState state = Parse(ref args);

            if (args.Length < 1 || !_commands.ContainsKey(args[0]))
            {
                if (args.Length > 0)
                    Console.Error.WriteLine("Unknown command '{0}'", args[0]);

                Console.Error.WriteLine("Type '{0} help' for usage", Assembly.GetEntryAssembly().FullName);
                Environment.ExitCode = 1;
                return;
            }


        }

        private static CommandState Parse(ref string[] args)
        {
            List<string> rest = new List<string>();

            CommandState state = new CommandState();
            int i;

            for (i = 0; i < args.Length; i++)
            {
                string a = args[i];

                if(a.Length < 2 || a[0] != '-')
                {
                    rest.Add(a);
                    continue;
                }

                if(a == "--")
                {
                    i++;
                    break;
                }

                if(i < args.Length-1)
                    switch(a.ToUpperInvariant())
                    {
                        case "--USERNAME":
                            state.SetUserName(args[++i]);
                            break;
                        case "--PASSWORD":
                            state.SetPassword(args[++i]);
                            break;
                        case "--CONFIG-DIR":
                            state.SetConfigDir(args[++i]);
                            break;
                        case "-R":
                            state.SetRevision(args[++i]);
                            break;
                    }
            }

            while(i < args.Length)
                rest.Add(args[i++]);

            args = rest.ToArray();
            return state;
        }

        private static void Load()
        {
            foreach (Type tp in typeof(Program).Assembly.GetTypes())
            {
                if (tp.IsAbstract || !typeof(CommandBase).IsAssignableFrom(tp))
                    continue;

                CommandBase instance = Activator.CreateInstance(tp) as CommandBase;

                if (instance == null)
                    throw new InvalidOperationException();

                foreach (string cmd in instance.Commands)
                {
                    _commands.Add(cmd, instance);
                }
            }
        }
    }
}
