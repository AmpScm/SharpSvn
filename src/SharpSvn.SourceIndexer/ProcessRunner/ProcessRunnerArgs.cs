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

namespace QQn.ProcessRunner
{
    /// <summary>
    /// 
    /// </summary>
    public class ProcessRunArgs
    {
        bool _processGuiEventsWhileRunning;
        /// <summary>
        /// Initializes a new instance of the <see cref="ProcessRunArgs"/> class.
        /// </summary>
        public ProcessRunArgs()
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="ProcessRunArgs"/> class.
        /// </summary>
        /// <param name="args">The args.</param>
        public ProcessRunArgs(ProcessRunArgs args)
        {
            if (args != null)
            {
                ProcessGuiEventsWhileRunning = args.ProcessGuiEventsWhileRunning;
            }
        }

        /// <summary>
        /// Gets or sets a boolean indicating whether to call <see cref="ProcessRunner.DoEvents"/> if needed
        /// </summary>
        /// <remarks>See <see cref="IsRunningInGui" /> for the checks applied</remarks>
        public bool ProcessGuiEventsWhileRunning
        {
            get { return _processGuiEventsWhileRunning; }
            set { _processGuiEventsWhileRunning = value; }
        }

    }

    /// <summary>
    /// 
    /// </summary>
    public class ProcessRunnerArgs : ProcessRunArgs
    {
        TextReader _input;
        /// <summary>
        /// Initializes a new instance of the <see cref="ProcessRunnerArgs"/> class.
        /// </summary>
        public ProcessRunnerArgs()
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="ProcessRunnerArgs"/> class.
        /// </summary>
        /// <param name="args">The args.</param>
        public ProcessRunnerArgs(ProcessRunnerArgs args)
            : base(args)
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="ProcessRunnerArgs"/> class.
        /// </summary>
        /// <param name="args">The args.</param>
        public ProcessRunnerArgs(ProcessRunArgs args)
            : base(args)
        {
        }

        /// <summary>
        /// Gets or sets the process input.
        /// </summary>
        /// <value>The process input.</value>
        public TextReader ProcessInput
        {
            get { return _input; }
            set { _input = value; }
        }

        EventHandler<LineReceivedEventArgs> _outputHandler;
        internal EventHandler<LineReceivedEventArgs> OutputHandler
        {
            get { return _outputHandler; }
        }

        EventHandler<LineReceivedEventArgs> _errorHandler;
        internal EventHandler<LineReceivedEventArgs> ErrorHandler
        {
            get { return _errorHandler; }
        }

        /// <summary>
        /// Occurs when an Output line has been received
        /// </summary>
        public event EventHandler<LineReceivedEventArgs> Output
        {
            add { _outputHandler += value; }
            remove { _outputHandler -= value; }
        }

        /// <summary>
        /// Occurs when an Error line has been received
        /// </summary>
        public event EventHandler<LineReceivedEventArgs> Error
        {
            add { _errorHandler += value; }
            remove { _errorHandler -= value; }
        }
    }
}
