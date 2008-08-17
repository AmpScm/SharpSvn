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
    public class HookTestBase : TestBase
    {
        public class ReposHookEventArgs : CancelEventArgs
        {
            int _exitCode;
            string[] _args;
            string _stdinText;

            SvnHookArguments _ha;
                

            public ReposHookEventArgs(string[] args, string stdinText)
            {
                _args = args;
                _stdinText = stdinText;

                SvnHookArguments ha;
                if (SvnHookArguments.ParseHookArguments(args, SvnHookType.PreCommit, false, out ha))
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

            stopper.Start(
            delegate
            {
                Environment.SetEnvironmentVariable("SHARPSVNHOOK_FILE", args);
                Environment.SetEnvironmentVariable("SHARPSVNHOOK_STDIN", stdin);
                Environment.SetEnvironmentVariable("SHARPSVNHOOK_WAIT", wait);
                Environment.SetEnvironmentVariable("SHARPSVNHOOK_OUT_STDOUT", outTxt);
                Environment.SetEnvironmentVariable("SHARPSVNHOOK_OUT_STDERR", errTxt);
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
                            string stdinText = File.ReadAllText(stdin);

                            File.Delete(args);
                            File.Delete(stdin);

                            ReposHookEventArgs ra = new ReposHookEventArgs(argCollection.ToArray(), stdin);

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

            File.Copy(Path.Combine(ProjectBase,"..\\tools\\hooknotifier\\bin\\" + Configuration + "\\HookNotifier.exe"), file);

            return stopper;
        }
    }
}
