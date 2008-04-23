////////////////////////////////////////////////////////////////////////////
// This code is part of the QQn TurtleBuild project but is reused as 
// embedded class in some other projects by Bert Huijben
// 
// For the last release please visit http://code.google.com/p/turtlebuild/
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Diagnostics;
using System.Threading;
using System.Collections.ObjectModel;

namespace QQn.ProcessRunner
{
    public class ProcessRunnerResult : IDisposable
    {
        readonly Process _process;
        readonly ProcessRunnerArgs _args;
        readonly Queue<string> _outputData;
        readonly Queue<string> _errorData;
        readonly TextReader _processInput;
        readonly ManualResetEvent _outputReady;
        readonly ManualResetEvent _errorReady;
        readonly ManualResetEvent _exited;
        readonly bool _processGui;
        readonly List<string> _output;
        readonly List<string> _error;
        readonly EventHandler<LineReceivedEventArgs> _outputEvent;
        readonly EventHandler<LineReceivedEventArgs> _errorEvent;

        internal ProcessRunnerResult(Process process, ProcessRunnerArgs args)
        {
            if (process == null)
                throw new ArgumentNullException("process");
            else if(args == null)
                throw new ArgumentNullException("args");

            _process = process;
            _processInput = args.ProcessInput;
            _outputReady = new ManualResetEvent(false);
            _errorReady = new ManualResetEvent(false);
            _exited = new ManualResetEvent(false);
            _outputData = new Queue<string>();
            _errorData = new Queue<string>();
            _args = args;
            _processGui = args.ProcessGuiEventsWhileRunning && ProcessRunner.IsRunningInGui;
            _output = new List<string>();
            _error = new List<string>();
            _outputEvent = args.OutputHandler;
            _errorEvent = args.ErrorHandler;
        }

        #region IDisposable Members

        public void Dispose()
        {
            if (_process != null)
                _process.Dispose();

            _outputReady.Close();
            _errorReady.Close();
            _exited.Close();
        }

        #endregion

        internal void HookStreams()
        {
            _process.OutputDataReceived += new DataReceivedEventHandler(Process_OutputDataReceived);
            _process.ErrorDataReceived += new DataReceivedEventHandler(Process_ErrorDataReceived);
            _process.Exited += new EventHandler(Process_Exited);
        }

        internal void StartReceiving()
        {
            _process.BeginErrorReadLine();
            _process.BeginOutputReadLine();
        }

        void Process_OutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            DataReceived(_outputReady, _outputData, e);
        }

        void Process_ErrorDataReceived(object sender, DataReceivedEventArgs e)
        {
            DataReceived(_errorReady, _errorData, e);
        }

        void DataReceived(ManualResetEvent readyEvent, Queue<string> dataQueue, DataReceivedEventArgs e)
        {
            lock (dataQueue)
            {
                dataQueue.Enqueue(e.Data);
                readyEvent.Set();
            }
        }

        void Process_Exited(object sender, EventArgs e)
        {
 	        _exited.Set();
        }

        internal void ProcessInput()
        {
            if (_processInput != null)
            {
                for (string line = _processInput.ReadLine(); line != null; line = _processInput.ReadLine())
                {
                    if (_processGui)
                        ProcessRunner.DoEvents();

                    _process.StandardInput.WriteLine(line); // Might block the UI for the time needed to process
                }
            }
                _process.StandardInput.Close(); // No more input
        }

        public bool WaitForExit(int timeoutMilliseconds)
        {
            DateTime breakTime = DateTime.Now + new TimeSpan(0,0,0,0, timeoutMilliseconds);
            WaitHandle[] handles = new WaitHandle[] { _errorReady, _outputReady, _exited };

            bool exited = false;
            while(!exited)
            {
                int msLeft = Timeout.Infinite;
                if(timeoutMilliseconds > 0)
                {
                    msLeft = (int)(breakTime - DateTime.Now).TotalMilliseconds;

                    if(msLeft <= 0)
                        return false;
                }

                if(_processGui)
                    msLeft = Math.Min(10, msLeft);

                switch (WaitHandle.WaitAny(handles, msLeft, true))
                {
                    case 2:
                        exited = true; // We have exited break loop
                        goto case 0; // Clear queues
                    case 0:
                        HandleQueue(_errorReady, _errorData, _error, _errorEvent);
                        goto case 1;
                    case 1:
                        HandleQueue(_outputReady, _outputData, _output, _outputEvent);
                        break;
                    
                    default:
                        if (_processGui)
                            ProcessRunner.DoEvents();
                        continue;
                }
            }
            

            return _process.HasExited;
        }

        public bool WaitForExit()
        {
            return WaitForExit(-1);
        }

        private void HandleQueue(ManualResetEvent ready, Queue<string> dataQueue, List<string> outQueue, EventHandler<LineReceivedEventArgs> ev)
        {          
            while(true)
            {
                string line;
                lock (dataQueue)
                {
                    if(dataQueue.Count == 0)
                    {
                        ready.Reset();
                        return;
                    }

                    line = dataQueue.Dequeue();
                }

                if (line == null)
                    continue;

                outQueue.Add(line);

                if(ev != null)
                    ev(this, new LineReceivedEventArgs(line));
            }
        }

        /// <summary>
        /// Gets the exit code.
        /// </summary>
        /// <value>The exit code.</value>
        public int ExitCode
        {
            get { return _process.ExitCode; }
        }

        /// <summary>
        /// Gets the process.
        /// </summary>
        /// <value>The process.</value>
        public Process Process
        {
            get { return _process; }
        }

        /// <summary>
        /// Gets a value indicating whether the process has exited.
        /// </summary>
        /// <value>
        /// 	<c>true</c> if this instance has exited; otherwise, <c>false</c>.
        /// </value>
        public bool HasExited
        {
            get { return _process.HasExited; }
        }

        ProcessOutputCollection _outputCollection;
        public ProcessOutputCollection Output
        {
            get { return _outputCollection ?? (_outputCollection = new ProcessOutputCollection(_output)); }
        }

        ProcessOutputCollection _errorCollection;
        public ProcessOutputCollection Error
        {
            get { return _errorCollection ?? (_errorCollection = new ProcessOutputCollection(_error)); }
        }

    }
}
