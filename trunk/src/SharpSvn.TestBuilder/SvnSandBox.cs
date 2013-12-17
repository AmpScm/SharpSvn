﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SharpSvn.TestBuilder.Implementation;

namespace SharpSvn.TestBuilder
{

    public class SvnSandBox
    {
        readonly SvnSandboxContainer _container;
        readonly TestContext _context;

        public SvnSandBox(TestContext context)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            _container = SvnSandboxContainer.Current;
            _context = context;
        }

        public SvnSandBox(IHasTestContext context)
            : this(context.TestContext)
        {
        }

        SvnSandboxContainer Container
        {
            get { return _container; }
        }

        string _name;
        string TestName
        {
            get { return _name ?? (_name = _context.TestName); }
        }

        string _wc;
        public string Wc
        {
            get { return _wc ?? (_wc = GetTempDir("wc")); }
        }

        Uri _repositoryUri;
        public Uri RepositoryUri
        {
            get { return _repositoryUri; }
        }

        Uri _uri;
        public Uri Uri
        {
            get { return _uri ?? _repositoryUri; }
        }

        public string GetTempDir(string suffix)
        {
            return Container.GetTempDir(TestName + '_' + suffix);
        }

        public string GetTempDir()
        {
            return Container.GetTempDir(TestName);
        }

        public string GetTempFile(string suffix)
        {
            return Container.GetTempFile(TestName + '_' + suffix);
        }

        public string GetTempFile()
        {
            return Container.GetTempFile(TestName);
        }

        public void Create(SandBoxRepository type)
        {
            Create(type, true);
        }

        public void Create(SandBoxRepository type, bool useTrunk)
        {
            _repositoryUri = CreateRepository(type);

            _uri = _repositoryUri;

            if (useTrunk)
                switch (type)
                {
                    case SandBoxRepository.Default:
                    case SandBoxRepository.DefaultBranched:
                    case SandBoxRepository.MergeScenario:
                    case SandBoxRepository.AnkhSvnCases:
                        _uri = new Uri(_repositoryUri, "trunk/");
                        break;
                }

            using (SvnClient cl = new SvnClient())
            {
                cl.UseDefaultConfiguration(); // Don't load config

                cl.CheckOut(_uri, Wc);
            }
        }

        public Uri CreateRepository(SandBoxRepository type)
        {
            string dir = GetTempDir("repos");

            using (SvnRepositoryClient rc = new SvnRepositoryClient())
            {
                rc.UseDefaultConfiguration(); // Don't load config

                rc.CreateRepository(dir);

                Uri uri = SvnTools.LocalPathToUri(dir, false);

                switch (type)
                {
                    case SandBoxRepository.Empty:
                        break;
                    case SandBoxRepository.Default:
                    case SandBoxRepository.DefaultBranched:
                        SvnRepositoryOperationArgs ra = new SvnRepositoryOperationArgs();
                        ra.LogMessage = "r1";
                        using (SvnMultiCommandClient mucc = new SvnMultiCommandClient(uri, ra))
                        {
                            mucc.CreateDirectory("trunk");
                            mucc.CreateDirectory("trunk/src");
                            mucc.CreateDirectory("branches");
                            mucc.CreateDirectory("branches/A");
                            mucc.CreateDirectory("branches/B");
                            mucc.CreateFile("trunk/README.txt", new MemoryStream(Encoding.UTF8.GetBytes("Read me, please?\n")));
                            mucc.CreateFile("trunk/src/file1.cs", new MemoryStream(Encoding.UTF8.GetBytes("using SharpSvn;\n")));
                            mucc.CreateFile("trunk/src/file2.vb", new MemoryStream(Encoding.UTF8.GetBytes("Imports SharpSvn;\n")));
                            mucc.CreateFile("trunk/src/file3.cpp", new MemoryStream(Encoding.UTF8.GetBytes("using namespace SharpSvn;\n")));
                            mucc.SetProperty("trunk/src/file3.cpp", SvnPropertyNames.SvnEolStyle, "native");
                            mucc.CreateFile("trunk/src/file4.fs", new MemoryStream(Encoding.UTF8.GetBytes("namespace SharpSvn\n")));
                            mucc.SetProperty("trunk/src/file4.fs", SvnPropertyNames.SvnEolStyle, "LF");

                            mucc.Commit();
                        }
                        if (type == SandBoxRepository.Default)
                            break;
                        ra.LogMessage = "r2";
                        using (SvnMultiCommandClient mucc = new SvnMultiCommandClient(uri, ra))
                        {
                            mucc.CreateDirectory("branches/C");
                            mucc.Copy("trunk", "branches/trunk-r1");
                            mucc.UpdateFile("trunk/README.txt", new MemoryStream(Encoding.UTF8.GetBytes("Read me, please?\n\"Ok, read!\"\n")));

                            mucc.Commit();
                        }
                        ra.LogMessage = "r3";
                        using (SvnMultiCommandClient mucc = new SvnMultiCommandClient(uri, ra))
                        {
                            mucc.CreateDirectory("branches/D");
                            mucc.Copy("trunk", "branches/trunk-r2");
                            mucc.UpdateFile("trunk/README.txt", new MemoryStream(Encoding.UTF8.GetBytes("Read me, please?\n\"Ok, read!\"\n")));

                            mucc.Commit();
                        }
                        ra.LogMessage = "r4";
                        using (SvnMultiCommandClient mucc = new SvnMultiCommandClient(uri, ra))
                        {
                            mucc.CreateDirectory("branches/E");
                            mucc.Copy("trunk", "branches/trunk-r3");
                            mucc.UpdateFile("trunk/README.txt", new MemoryStream(Encoding.UTF8.GetBytes("Read me, please?\n\"Ok, read!\"\nNo way!\n")));

                            mucc.Commit();
                        }
                        break;
                    case SandBoxRepository.MergeScenario:
                        rc.LoadRepository(dir, typeof(SvnSandBox).Assembly.GetManifestResourceStream("SharpSvn.TestBuilder.Resources.MergeScenario.repos"));
                        break;
                    case SandBoxRepository.AnkhSvnCases:
                        BuildAnkhCases(uri);
                        break;
                    default:
                        break;
                }
            }

            return SvnTools.LocalPathToUri(dir, true);
        }

        Stream StringToFile(string data)
        {
            return new MemoryStream(Encoding.UTF8.GetBytes(data));
        }
        private void BuildAnkhCases(Uri uri)
        {
            using(SvnClient client = new SvnClient())
            {
                client.UseDefaultConfiguration();
                client.Configuration.LogMessageRequired = false;

                client.RepositoryOperation(uri,
                    delegate(SvnMultiCommandClient mucc)
                    {
                        mucc.CreateDirectory("trunk");
                        mucc.CreateFile("trunk/App.ico", StringToFile("icon\n"));
                        mucc.CreateFile("trunk/AssemblyInfo.cs", StringToFile("AssemblyInfo.cs"));
                        mucc.CreateFile("trunk/Form.cs", StringToFile("Form.cs\n"));
                        mucc.CreateFile("trunk/Form.resx", StringToFile("Form.resx\n"));
                        mucc.CreateFile("trunk/GoogleOne.csproj", StringToFile("GoogleOne.csproj\n"));
                        mucc.CreateFile("trunk/GoogleOne.csproj.user", StringToFile("GoogleOne.csproj.user\n"));
                        mucc.CreateFile("trunk/GoogleOne.sln", StringToFile("GoogleOne.sln\n"));
                        mucc.CreateFile("trunk/GoogleOne.suo", StringToFile("GoogleOne.suo\n"));
                        mucc.CreateFile("trunk/GoogleSearch.wsdl", StringToFile("GoogleSearch.wsdl\n"));
                        mucc.CreateFile("trunk/GoogleSearchService.cs", StringToFile("GoogleSearchService.cs\n"));
                        mucc.CreateDirectory("trunk/bin");
                        mucc.CreateDirectory("trunk/bin/Debug");
                        mucc.CreateFile("trunk/bin/Debug/GoogleOne.exe", StringToFile("GoogleOne.exe\n"));
                        mucc.CreateFile("trunk/bin/Debug/GoogleOne.pdb", StringToFile("GoogleOne.pdb\n"));
                        mucc.CreateDirectory("trunk/obj");
                        mucc.CreateDirectory("trunk/obj/Debug");
                        mucc.CreateFile("trunk/obj/Debug/GoogleOne.exe", StringToFile("GoogleOne.exe\n"));
                        mucc.CreateFile("trunk/obj/Debug/GoogleOne.exe.incr", StringToFile("GoogleOne.exe.incr\n"));
                        mucc.CreateFile("trunk/obj/Debug/GoogleOne.Form.resources", StringToFile("GoogleOne.Form.resources\n"));
                        mucc.CreateFile("trunk/obj/Debug/GoogleOne.pdb", StringToFile("GoogleOne.pdb\n"));
                        mucc.CreateFile("trunk/obj/Debug/GoogleOne.projdata", StringToFile("GoogleOne.projdata\n"));

                        mucc.SetProperty("trunk/App.ico", SvnPropertyNames.SvnMimeType, "application/octet-stream");
                        mucc.SetProperty("trunk/GoogleOne.suo", SvnPropertyNames.SvnMimeType, "application/octet-stream");
                        mucc.SetProperty("trunk/bin/Debug/GoogleOne.exe", SvnPropertyNames.SvnMimeType, "application/octet-stream");
                        mucc.SetProperty("trunk/bin/Debug/GoogleOne.pdb", SvnPropertyNames.SvnMimeType, "application/octet-stream");
                        mucc.SetProperty("trunk/obj/Debug/GoogleOne.exe", SvnPropertyNames.SvnMimeType, "application/octet-stream");
                        mucc.SetProperty("trunk/obj/Debug/GoogleOne.exe.incr", SvnPropertyNames.SvnMimeType, "application/octet-stream");
                        mucc.SetProperty("trunk/obj/Debug/GoogleOne.Form.resources", SvnPropertyNames.SvnMimeType, "application/octet-stream");
                        mucc.SetProperty("trunk/obj/Debug/GoogleOne.pdb", SvnPropertyNames.SvnMimeType, "application/octet-stream");
                        mucc.SetProperty("trunk/obj/Debug/GoogleOne.projdata", SvnPropertyNames.SvnMimeType, "application/octet-stream");
                    });

                client.RepositoryOperation(uri,
                    delegate(SvnMultiCommandClient mucc)
                    {
                        mucc.CreateDirectory("trunk/doc");
                        mucc.CreateFile("trunk/doc/text.txt", StringToFile("Hello\n"));
                    });

                client.RepositoryOperation(uri,
                    delegate(SvnMultiCommandClient mucc)
                    {
                        mucc.CreateFile("trunk/doc/text_r3.txt", StringToFile("Hei\n"));
                    });

                client.RepositoryOperation(uri,
                    delegate(SvnMultiCommandClient mucc)
                    {
                        mucc.CreateFile("trunk/doc/text_r4.txt", StringToFile("Hay\n"));
                    });

                client.RepositoryOperation(uri,
                    delegate(SvnMultiCommandClient mucc)
                    {
                        mucc.CreateFile("trunk/doc/text_r5.txt", StringToFile("Bye\n"));
                    });

                client.RepositoryOperation(uri,
                    delegate(SvnMultiCommandClient mucc)
                    {
                        mucc.Delete("trunk/obj");
                    });
            }
        }

        /// <summary>
        /// 
        /// </summary>
        public static void Clear()
        {
            SvnSandboxContainer.Current.Clear();
        }
    }
}
