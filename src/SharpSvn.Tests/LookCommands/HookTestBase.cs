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
using SharpSvn.Tests.Commands;
using System.Threading;
using System.IO;
using System.CodeDom.Compiler;
using System.ComponentModel;

namespace SharpSvn.Tests.LookCommands
{
    public abstract class HookTestBase : TestBase
    {
        public class ReposHookEventArgs : CancelEventArgs
        {
            int _exitCode;
            string[] _args;
            string _stdinText;
            string _errorText;
            string _outputText;

            SvnHookArguments _ha;


            public ReposHookEventArgs(SvnHookType ht, string[] args, string stdinText)
            {
                _args = args;
                _stdinText = stdinText;

                SvnHookArguments ha;
                if (SvnHookArguments.ParseHookArguments(args, ht, false, out ha))
                    _ha = ha;

            }

            public string[] Args
            {
                get { return _args; }
            }

            public SvnHookArguments HookArgs
            {
                get { return _ha; }
            }

            public string StdInText
            {
                get { return _stdinText; }
            }

            public string ErrorText
            {
                get { return _errorText; }
                set { _errorText = value; }
            }

            public string OutputText
            {
                get { return _outputText; }
                set { _outputText = value; }
            }

            public int ExitCode
            {
                get { return _exitCode; }
                set { _exitCode = value; }
            }
        }

        delegate void Action();

        class ThreadStopper : IDisposable
        {
            public bool Cancel;
            Action Action;
            IAsyncResult _result;


            public ThreadStopper()
            {
            }

            public void Start(Action action)
            {
                Action = action;
                _result = action.BeginInvoke(null, null);
            }

            #region IDisposable Members

            public void Dispose()
            {
                Cancel = true;
                Action.EndInvoke(_result);
            }

            #endregion
        }

        public IDisposable InstallHook(Uri reposUri, SvnHookType type, EventHandler<ReposHookEventArgs> hook)
        {
            if (reposUri == null)
                throw new ArgumentNullException("reposUri");
            else if (!reposUri.IsFile)
                throw new InvalidOperationException();

            string reposPath = reposUri.LocalPath;

            TempFileCollection tfc = new TempFileCollection();
            string dir = Path.GetTempPath();

            string suffix = Guid.NewGuid().ToString("N");
            string args = Path.Combine(dir, suffix + "-args.txt");
            string stdin = Path.Combine(dir, suffix + "-stdin.txt");
            string wait = Path.Combine(dir, suffix + "-wait.txt");

            string errTxt = Path.Combine(dir, suffix + "-errTxt.txt");
            string outTxt = Path.Combine(dir, suffix + "-outTxt.txt");

            tfc.AddFile(args, false);
            tfc.AddFile(stdin, false);
            tfc.AddFile(wait, false);
            tfc.AddFile(errTxt, false);
            tfc.AddFile(outTxt, false);

            ThreadStopper stopper = new ThreadStopper();

            Environment.SetEnvironmentVariable("SHARPSVNHOOK_FILE", args);
            Environment.SetEnvironmentVariable("SHARPSVNHOOK_STDIN", stdin);
            Environment.SetEnvironmentVariable("SHARPSVNHOOK_WAIT", wait);
            Environment.SetEnvironmentVariable("SHARPSVNHOOK_OUT_STDOUT", outTxt);
            Environment.SetEnvironmentVariable("SHARPSVNHOOK_OUT_STDERR", errTxt);

            stopper.Start(
            delegate
            {
                try
                {
                    while (!stopper.Cancel)
                    {
                        if (File.Exists(args) && File.Exists(stdin))
                        {
                            List<string> argCollection = new List<string>();
                            using (StreamReader fs = File.OpenText(args))
                            {
                                string line;
                                while (null != (line = fs.ReadLine()))
                                    argCollection.Add(line);
                            }
                            string stdinText = RetriedReadAllText(stdin);

                            File.Delete(args);
                            File.Delete(stdin);

                            ReposHookEventArgs ra = new ReposHookEventArgs(type, argCollection.ToArray(), stdin);

                            try
                            {
                                hook(this, ra);
                            }
                            catch
                            {
                                ra.ExitCode = 129;
                            }
                            finally
                            {

                                if (ra.ErrorText != null)
                                    File.WriteAllText(errTxt, ra.ErrorText);

                                if (ra.OutputText != null)
                                    File.WriteAllText(outTxt, ra.OutputText);


                                File.WriteAllText(wait, ra.ExitCode.ToString());
                            }

                            if (ra.Cancel)
                                break;
                        }
                        Thread.Sleep(50);
                    }

                }
                finally
                {
                    Environment.SetEnvironmentVariable("SHARPSVNHOOK_FILE", null);
                    Environment.SetEnvironmentVariable("SHARPSVNHOOK_STDIN", null);
                    Environment.SetEnvironmentVariable("SHARPSVNHOOK_WAIT", null);
                    Environment.SetEnvironmentVariable("SHARPSVNHOOK_OUT_STDOUT", null);
                    Environment.SetEnvironmentVariable("SHARPSVNHOOK_OUT_STDERR", null);
                }

                GC.KeepAlive(tfc);
            });

            string file = Path.ChangeExtension(SvnHookArguments.GetHookFileName(reposPath, type), ".exe");

            File.Copy(Path.Combine(ProjectBase, "..\\tools\\hooknotifier\\bin\\" + Configuration + "\\HookNotifier.exe"), file);

            return stopper;
        }

        private string RetriedReadAllText(string path)
        {
            for(int i = 0; i < 10; i++)
                try
                {
                    return File.ReadAllText(path);
                }
                catch(IOException)
                {
                    Thread.Sleep(500 * i);
                }

            return null;
        }

        protected string Configuration
        {
            get
            {
#if DEBUG
                return "DEBUG";
#else
                return "RELEASE";
#endif
            }
        }

    }
}
