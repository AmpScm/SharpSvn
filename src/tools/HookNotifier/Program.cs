using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace HookNotifier
{
    class Program
    {
        static void Main(string[] args)
        {
            string envPrefix;

#if NET6_0_OR_GREATER
            envPrefix = Path.GetFileNameWithoutExtension(Environment.ProcessPath) + ".";
#else
            envPrefix = Path.GetFileNameWithoutExtension(Environment.GetCommandLineArgs()[0]) + ".";
#endif

            string file = Environment.GetEnvironmentVariable(envPrefix + "SHARPSVNHOOK_FILE");

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

            file = Environment.GetEnvironmentVariable(envPrefix + "SHARPSVNHOOK_STDIN");

            if (!string.IsNullOrEmpty(file))
            {
                using (StreamWriter sw = File.CreateText(file))
                using (StreamReader sr = new StreamReader(Console.OpenStandardInput()))
                {
                    var cts = new CancellationTokenSource(); // Set timeout for 5 seconds

                    Task readTask = Task.Run(() =>
                    {
                        try
                        {
                            string line;
                            while ((line = sr.ReadLine()) != null)
                            {
                                sw.WriteLine(line);
                            }
                        }
                        catch (IOException)
                        {
                            // STDIN was not passed or encountered an error
                        }
                    }, cts.Token);

                    try
                    {
                        readTask.Wait(cts.Token);
                    }
                    catch (OperationCanceledException)
                    {
                    }
                }
            }

            file = Environment.GetEnvironmentVariable(envPrefix + "SHARPSVNHOOK_DONE");

            if (!string.IsNullOrEmpty(file))
            {
                File.WriteAllText(file, "");
            }

            file = Environment.GetEnvironmentVariable(envPrefix + "SHARPSVNHOOK_WAIT");

            if (!string.IsNullOrEmpty(file))
            {
                DateTime end = DateTime.Now.AddMinutes(5);
                while (!File.Exists(file))
                {
                    Thread.Sleep(100); // Wait for file to appear

                    if (DateTime.Now > end)
                    {
                        Console.Error.WriteLine("HookNotifier: Timeout exceeded");
                        Environment.Exit(1);
                    }
                }

                Thread.Sleep(100);
                int result = int.Parse(File.ReadAllText(file).Trim());

                string outText = Environment.GetEnvironmentVariable(envPrefix + "SHARPSVNHOOK_OUT_STDOUT");
                if (!string.IsNullOrEmpty(outText) && File.Exists(outText))
                {
                    Console.Out.Write(File.ReadAllText(outText));
                    File.Delete(outText);
                }

                string errText = Environment.GetEnvironmentVariable(envPrefix + "SHARPSVNHOOK_OUT_STDERR");
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
