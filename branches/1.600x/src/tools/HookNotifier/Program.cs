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
using System.IO;
using System.Threading;

namespace HookNotifier
{
    class Program
    {
        static void Main(string[] args)
        {
            string file = Environment.GetEnvironmentVariable("SHARPSVNHOOK_FILE");

            if (!string.IsNullOrEmpty(file))
            {
                using (StreamWriter sw = File.CreateText(file))
                {
                    foreach (string a in args)
                    {
                        sw.WriteLine(a);
                    }
                }
            }

            file = Environment.GetEnvironmentVariable("SHARPSVNHOOK_STDIN");

            if (!string.IsNullOrEmpty(file))
            {
                using (StreamWriter sw = File.CreateText(file))
                {
                    string line;
                    while(null != (line = Console.In.ReadLine()))
                        sw.WriteLine(line);

                    sw.Write(Console.In.ReadToEnd());
                }
            }

            file = Environment.GetEnvironmentVariable("SHARPSVNHOOK_WAIT");

            if (!string.IsNullOrEmpty(file))
            {
                DateTime end = DateTime.Now.AddMinutes(5);
                while (!File.Exists(file))
                {
                    Thread.Sleep(100); // 1/10th of a second

                    if (DateTime.Now > end)
                    {
                        Console.Error.WriteLine("HookNotifier: Timeout exceeded");
                        Environment.Exit(1);
                    }
                }

                Thread.Sleep(100); // 1/10th of a second
                int result = int.Parse(File.ReadAllText(file).Trim());

                string outText = Environment.GetEnvironmentVariable("SHARPSVNHOOK_OUT_STDOUT");
                if (!string.IsNullOrEmpty(outText) && File.Exists(outText))
                {
                    Console.Out.Write(File.ReadAllText(outText));
                    File.Delete(outText);
                }

                string errText = Environment.GetEnvironmentVariable("SHARPSVNHOOK_OUT_STDERR");
                if (!string.IsNullOrEmpty(errText) && File.Exists(errText))
                {
                    Console.Error.Write(File.ReadAllText(errText));
                    File.Delete(errText);
                }


                File.Delete(file);
                Environment.Exit(result);
            }
        }
    }
}
