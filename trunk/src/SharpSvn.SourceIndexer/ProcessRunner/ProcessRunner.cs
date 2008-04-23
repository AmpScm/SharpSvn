////////////////////////////////////////////////////////////////////////////
// This code is part of the QQn TurtleBuild project but is reused as 
// embedded class in some other projects by Bert Huijben
// 
// For the last release please visit http://code.google.com/p/turtlebuild/
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using System.Security;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Collections;
using System.IO;
using System.Threading;

namespace QQn.ProcessRunner
{    
    /// <summary>
    /// Simple wrapper around
    /// </summary>
    public static class ProcessRunner
    {
        /// <summary>
        /// Runs the process resource that is specified by the parameter containing
        /// process start information (for example, the file name of the process to start)
        /// and associates the resource with a new System.Diagnostics.Process component.
        /// </summary>
        /// <param name="startInfo">The start info.</param>
        /// <returns></returns>
        public static ProcessRunnerResult Run(ProcessStartInfo startInfo, ProcessRunnerArgs args)
        {
            if (args == null)
                throw new ArgumentNullException("args");
            else if (startInfo == null)
                throw new ArgumentNullException("startInfo");

            return DoRun(args, startInfo);
        }

        /// <summary>
        /// Runs a process resource by specifying the name of a document or application
        /// file and associates the resource with a new System.Diagnostics.Process component.
        /// </summary>
        /// <param name="fileName">Name of the file.</param>
        /// <returns></returns>
        public static ProcessRunnerResult Run(string fileName, ProcessRunnerArgs args)
        {
            if (args == null)
                throw new ArgumentNullException("args");

            ProcessStartInfo psi = new ProcessStartInfo(fileName);

            return DoRun(args, psi);
        }

        /// <summary>
        /// Runs a process resource by specifying the name of an application and a
        /// set of command-line arguments, and associates the resource with a new System.Diagnostics.Process
        /// component.
        /// </summary>
        /// <param name="fileName">Name of the file.</param>
        /// <param name="arguments">The arguments.</param>
        /// <returns></returns>
        public static ProcessRunnerResult Run(string fileName, string arguments, ProcessRunnerArgs args)
        {
            if (args == null)
                throw new ArgumentNullException("args");

            ProcessStartInfo psi = new ProcessStartInfo(fileName, arguments);

            return DoRun(args, psi);
        }
        /// <summary>
        /// Runs a process resource by specifying the name of an application, a user
        /// name, a password, and a domain and associates the resource with a new System.Diagnostics.Process
        /// component.
        /// </summary>
        /// <param name="fileName">Name of the file.</param>
        /// <param name="userName">Name of the user.</param>
        /// <param name="password">The password.</param>
        /// <param name="domain">The domain.</param>
        /// <returns></returns>
        public static ProcessRunnerResult Run(string fileName, string userName, SecureString password, string domain, ProcessRunnerArgs args)
        {
            if (args == null)
                throw new ArgumentNullException("args");

            ProcessStartInfo psi = new ProcessStartInfo(fileName);
            psi.UseShellExecute = false;
            psi.UserName = userName;
            psi.Password = password;
            psi.Domain = domain;

            return DoRun(args, psi);
        }
        /// <summary>
        /// Runs a process resource by specifying the name of an application, a set
        /// of command-line arguments, a user name, a password, and a domain and associates
        /// the resource with a new System.Diagnostics.Process component.
        /// </summary>
        /// <param name="fileName">Name of the file.</param>
        /// <param name="arguments">The arguments.</param>
        /// <param name="userName">Name of the user.</param>
        /// <param name="password">The password.</param>
        /// <param name="domain">The domain.</param>
        /// <returns></returns>
        public static ProcessRunnerResult Run(string fileName, string arguments, string userName, SecureString password, string domain, ProcessRunnerArgs args)
        {
            if (args == null)
                throw new ArgumentNullException("args");

            ProcessStartInfo psi = new ProcessStartInfo(fileName, arguments);
            psi.UseShellExecute = false;
            psi.UserName = userName;
            psi.Password = password;
            psi.Domain = domain;

            return DoRun(args, psi);
        }

        private static ProcessRunnerResult DoRun(ProcessRunnerArgs args, ProcessStartInfo psi)
        {
            psi.CreateNoWindow = true;
            psi.RedirectStandardInput = true;
            psi.RedirectStandardOutput = true;
            psi.RedirectStandardError = true;
            psi.UseShellExecute = false;
            
            bool disposeProcess = true;
            Process process = new Process();
            process.StartInfo = psi;
            process.EnableRaisingEvents = true;

            try
            {
                ProcessRunnerResult psr = new ProcessRunnerResult(process, args);

                process.Start();

                psr.HookStreams();


                psr.StartReceiving();

                psr.ProcessInput();

                disposeProcess = false;

                return psr;
            }
            finally
            {
                if (disposeProcess)
                    process.Dispose();
            }
        }


        [ThreadStatic]
        static bool? _runningInGui;

        /// <summary>
        /// Gets a value indicating whether this instance is running in GUI.
        /// </summary>
        /// <value>
        /// 	<c>true</c> if this instance is running in GUI; otherwise, <c>false</c>.
        /// </value>
        /// <remarks>
        /// Checks whether the process is: Interactive, owns a window and the current thread is 
        /// marked as a gui thread. Otherwise the pumping is ignored</remarks>
        public static bool IsRunningInGui
        {
            get { return (bool)(_runningInGui ?? (_runningInGui = GetRunningInGui())); }
        }

        static bool GetRunningInGui()
        {
            if (!Environment.UserInteractive)
                return false;

            Process p = Process.GetCurrentProcess();
            try
            {
                if (p.MainWindowHandle == IntPtr.Zero)
                    return false;
            }
            catch { }

            if (NativeMethods.IsGUIThread(false))
                return true;
            else
                return false;
        }

        delegate void DoEventsHandler();
        static DoEventsHandler _doEvents;
        static DoEventsHandler DoDoEvents { get { return _doEvents ?? (_doEvents = GetDoEvents()); } }

        private static DoEventsHandler GetDoEvents()
        {
            Assembly asm = Assembly.Load("System.Windows.Forms, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089");
            Type tp = asm.GetType("System.Windows.Forms.Application");
            MethodInfo doEvents = tp.GetMethod("DoEvents", BindingFlags.Static | BindingFlags.Public | BindingFlags.InvokeMethod);

            return (DoEventsHandler)Delegate.CreateDelegate(typeof(DoEventsHandler), null, doEvents);
        }

        /// <summary>
        /// Runs Application.DoEvents() if running inside a GUI thread; otherwise just returns
        /// </summary>
        /// <remarks>See <see cref="IsRunningInGui" /> for the checks applied</remarks>
        public static void DoEvents()
        {
            if (!IsRunningInGui)
                return;

            if (DoDoEvents != null)
                DoDoEvents();
        }

        static class NativeMethods
        {
            [DllImport("user32.dll", ExactSpelling = true)]
            public extern static bool IsGUIThread(bool bConvert);
        }
    }
}
