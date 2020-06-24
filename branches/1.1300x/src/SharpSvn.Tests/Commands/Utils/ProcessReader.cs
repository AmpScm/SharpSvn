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

using System.IO;
using System.Threading;
using System;

namespace SharpSvn.Tests.Commands.Utils
{
    public class ProcessReader
    {
        readonly StreamReader _processReader;
        readonly MemoryStream _buffer;
        readonly Thread _thread;
        StreamReader _reader;

        public ProcessReader(StreamReader reader)
        {
            if (reader == null)
                throw new ArgumentNullException("reader");

            _processReader = reader;
            _buffer = new MemoryStream();
            _thread = new Thread(Read);
        }

        public void Start()
        {
            _thread.Start();
        }

        public void Wait()
        {
            _thread.Join();
        }

        string _outputText;
        public string Output
        {
            get
            {
                if (_outputText == null && _reader == null)
                {
                    _buffer.Position = 0;
                    _outputText = new StreamReader(_buffer).ReadToEnd();
                }
                return _outputText;
            }
        }

        /// <summary>
        /// Whether the reader thread has exited.
        /// </summary>
        public bool HasExited
        {
            get
            {
                return !_thread.IsAlive;
            }
        }

        /// <summary>
        /// Whether the queue is empty.
        /// </summary>
        public bool Empty
        {
            get
            {
                return _buffer.Length == 0;
            }
        }

        /// <summary>
        /// Retrieves the 'next' line, blocking if necessary. Use in conjunction with the
        /// WaitHandle, which will be signaled when the queue is non-empty.
        /// </summary>
        /// <returns></returns>
        public string ReadLine()
        {
            if (_reader == null)
            {
                _buffer.Position = 0;
                _reader = new StreamReader(_buffer);
            }

            return _reader.ReadLine();
        }

        private void Read()
        {
            StreamWriter writer = new StreamWriter(_buffer);

            string line = null;

            while ((line = _processReader.ReadLine()) != null)
            {
                writer.WriteLine(line);
            }

            line = _processReader.ReadToEnd();
            if (line != null)
                writer.Write(line);

            writer.Flush();
        }
    }
}
