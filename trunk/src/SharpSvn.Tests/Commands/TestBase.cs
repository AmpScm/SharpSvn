// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using ICSharpCode.SharpZipLib.Zip;

using SharpSvn;
using SharpSvn.Tests.Commands.Utils;

namespace SharpSvn.Tests.Commands
{
	public enum TestReposType
	{
		Empty,
		AnkhRepos,
		CollabRepos
	}
	/// <summary>
	/// Serves as a base class for tests for NSvn::Core::Add
	/// </summary>
	[TestFixture]
	public class TestBase
	{
		List<string> pathsToDelete = new List<string>();
		SortedList<TestReposType, string> _reposs = new SortedList<TestReposType, string>();
		Uri _reposUri;
		string _reposPath;
		string _wcPath;

		public TestBase()
		{
			string asm = this.GetType().FullName;
			this.REPOS_FILE = "repos.zip";
			this.WC_FILE = "wc.zip";
		}

		[TestFixtureTearDown]
		public virtual void TestFixtureTearDown()
		{
			foreach (string path in pathsToDelete)
			{
				RecursiveDelete(path);
			}
			pathsToDelete.Clear();
		}

		protected string GetTempDir()
		{
			string file = Path.GetTempFileName();
			File.Delete(file);
			Directory.CreateDirectory(file);
			pathsToDelete.Add(file);
			return file;
		}

		protected Uri PathToUri(string path)
		{
			return new Uri("file:///" + Path.GetFullPath(path).Replace(Path.DirectorySeparatorChar, '/'));
		}


		protected string CreateRepos(TestReposType type)
		{
			string path = GetTempDir();
			pathsToDelete.Add(path);

			switch (type)
			{
				case TestReposType.Empty:
					{
						SvnRepositoryClient rc= new SvnRepositoryClient();
						rc.CreateRepository(path);
						return path;
					}
				case TestReposType.CollabRepos:
					{
						SvnRepositoryClient rc = new SvnRepositoryClient();
						rc.CreateRepository(path);
						using(FileStream fs = File.OpenRead(Path.Combine(ProjectBase, "Zips/Collabnet-MT.repos")))
						{
							rc.LoadRepository(path, fs);
						}

						
						return path;
					}
				case TestReposType.AnkhRepos:
					{
						UnzipToFolder(Path.Combine(ProjectBase, "Zips\\repos.zip"), _reposPath);
						return path;
					}
				default:
					throw new ArgumentException();
			}
		}

		public string GetRepos(TestReposType type)
		{
			string dir;
			if (!_reposs.TryGetValue(type, out dir))
			{
				dir = CreateRepos(type);
				_reposs[type] = dir;
			}
			return dir;
		}

		public Uri GetReposUri(TestReposType type)
		{
			Uri u = PathToUri(GetRepos(type));

			if (!u.ToString().EndsWith("/"))
				u = new Uri(u.ToString() + "/");

			return u;
		}

		protected Uri CollabReposUri
		{
			get { return GetReposUri(TestReposType.CollabRepos); }
		}

		[SetUp]
		public virtual void SetUp()
		{
			this.notifications = new List<SvnNotifyEventArgs>();
			this.client = new SvnClient();
            client.Configuration.LogMessageRequired = false;
            client.Notify += new EventHandler<SvnNotifyEventArgs>(OnClientNotify);
        }

        protected virtual void OnClientNotify(object sender, SvnNotifyEventArgs e)
        {
            bool found = File.Exists(e.FullPath) || Directory.Exists(e.FullPath) || Directory.Exists(Path.GetDirectoryName(e.FullPath));

            if (e.CommandType == SvnCommandType.Commit)
            {
                switch (e.Action)
                {
                    case SvnNotifyAction.CommitModified:
                    case SvnNotifyAction.CommitSendData:
                    case SvnNotifyAction.CommitAdded:
                    case SvnNotifyAction.CommitDeleted:
                    case SvnNotifyAction.CommitReplaced:
                        // Verify if the pre subversion 1.5.x rc1 bug is still present
                        //Assert.That(!found);
                        return;
                }
            }

            Assert.That(found,
                "{0} is not a valid path and it's directory does not exist\n (Raw value = {1}, Current Directory = {2}, Action = {3}, CommandType = {4})", 
                e.FullPath, e.Path, Environment.CurrentDirectory, e.Action, e.CommandType);
        }

		protected static void RecursiveDelete(string path)
		{
			if (Directory.Exists(path))
			{
				foreach (FileInfo f in new DirectoryInfo(path).GetFiles("*", SearchOption.AllDirectories))
				{
					f.Attributes = FileAttributes.Normal;
				}

				foreach (DirectoryInfo f in new DirectoryInfo(path).GetDirectories("*", SearchOption.AllDirectories))
				{
					f.Attributes = FileAttributes.Normal;
				}

				Directory.Delete(path, true);
			}
		}

		[TearDown]
		public virtual void TearDown()
		{
			// clean up
			try
			{
				if (_reposPath != null)
					RecursiveDelete(_reposPath);
				if (_wcPath != null)
					RecursiveDelete(_wcPath);
			}
			catch (Exception)
			{
				// swallow 
			}
			finally
			{
				_reposPath = null;
				_reposUri = null;
				_wcPath = null;
			}
		}
		/// <summary>
		/// extract our test repository
		/// </summary>
		public void ExtractRepos()
		{
			if (_reposPath == null)
				_reposPath = Path.GetFullPath(Path.Combine(BASEPATH, REPOS_NAME)); // \tmp\repos on current disk at this time

			if (Directory.Exists(_reposPath))
				RecursiveDelete(_reposPath);

			UnzipToFolder(Path.Combine(ProjectBase, "Zips\\repos.zip"), _reposPath);

			_reposUri = new Uri("file:///" + _reposPath.Replace(Path.DirectorySeparatorChar, '/') + '/');
		}

		public void ExtractWorkingCopy()
		{
			if (ReposUrl == null)
				ExtractRepos();

			System.Diagnostics.Debug.Assert(Directory.Exists(ReposPath));

			this._wcPath = Path.GetFullPath(this.FindDirName(Path.Combine(BASEPATH, WC_NAME)));

			UnzipToFolder(Path.Combine(ProjectBase, "Zips/" + WC_FILE), _wcPath);

			this.RenameAdminDirs(_wcPath);

			SvnClient cl = new SvnClient(); // Fix working copy to real location
			cl.Relocate(_wcPath, new Uri("file:///tmp/repos/"), ReposUrl);
		}

		public static string ProjectBase
		{
			get
			{
				string assemblyDir = Path.GetDirectoryName(new Uri(typeof(TestBase).Assembly.CodeBase).LocalPath);

				return Path.GetFullPath(Path.Combine(assemblyDir, "..\\..\\.."));
			}
		}

		protected static void UnzipToFolder(string zipFile, string toFolder)
		{
			FastZip fz = new FastZip();
			fz.CreateEmptyDirectories = true;
			fz.RestoreAttributesOnExtract = true;
			fz.RestoreDateTimeOnExtract = true;

			fz.ExtractZip(zipFile, toFolder, null);
		}

		/// <summary>
		/// Determines the SVN status of a given path
		/// </summary>
		/// <param name="path">The path to check</param>
		/// <returns>Same character codes as used by svn st</returns>
		public char GetSvnStatus(string path)
		{
			string output = this.RunCommand("svn", "st --non-recursive \"" + path + "\"");

			if (output.Trim() == String.Empty)
				return (char)0;

			string[] lines = output.Trim().Split('\n');
			Array.Sort(lines, new StringLengthComparer());

			string regexString = String.Format(@"(.).*\s{0}\s*", Regex.Escape(path));
			Match match = Regex.Match(lines[0], regexString, RegexOptions.IgnoreCase);
			if (match != Match.Empty)
				return match.Groups[1].ToString()[0];
			else
			{
				Assert.Fail("TestBase.GetSvnStatus - Regex match failed: " + output);
				return (char)0; // not necessary, but compiler complains..
			}

		}

		private class StringLengthComparer : IComparer
		{
			public int Compare(object x, object y)
			{
				return ((string)x).Length - ((string)y).Length;
			}
		}




		/// <summary>
		/// Runs a command
		/// </summary>
		/// <param name="command">The command to run</param>
		/// <param name="args">Arguments to the command</param>
		/// <returns>The output from the command</returns>
		public string RunCommand(string command, string args)
		{
			if (command == "svn")
				command = Path.GetFullPath(Path.Combine(ProjectBase, "..\\..\\imports\\release\\bin\\svn-win32.exe"));

			//System.Diagnostics.Trace.Assert(File.Exists(command), "Command exists");

            ProcessStartInfo psi = new ProcessStartInfo(command, args);
			psi.CreateNoWindow = true;
			psi.RedirectStandardOutput = true;
			psi.RedirectStandardError = true;
			psi.UseShellExecute = false;

			Process proc = Process.Start(psi);

			//Console.WriteLine( proc.MainModule.FileName );

			ProcessReader outreader = new ProcessReader(proc.StandardOutput);
			ProcessReader errreader = new ProcessReader(proc.StandardError);
			outreader.Start();
			errreader.Start();

			proc.WaitForExit();

			outreader.Wait();
			errreader.Wait();

			if (proc.ExitCode != 0)
				throw new ApplicationException("command exit code was " +
					proc.ExitCode.ToString() +
					Environment.NewLine + errreader.Output + Environment.NewLine +
					"Command was " +
					proc.StartInfo.FileName + " " + proc.StartInfo.Arguments);


			// normalize newlines
			string[] lines = Regex.Split(outreader.Output, @"\r?\n");
			return String.Join(Environment.NewLine, lines);
		}



		/// <summary>
		/// The fully qualified URI to the repository
		/// </summary>
		public Uri ReposUrl
		{
			get
			{
				if (_reposUri == null)
					ExtractRepos();

				System.Diagnostics.Debug.Assert(Directory.Exists(_reposPath));

				return _reposUri;
			}
		}

		/// <summary>
		/// The path to the repository
		/// </summary>
		public string ReposPath
		{
			get
			{
				if (_reposPath == null)
					ExtractRepos();

				System.Diagnostics.Debug.Assert(Directory.Exists(_reposPath));

				return _reposPath;
			}
		}

		/// <summary>
		/// The path to the working copy
		/// </summary>
		public string WcPath
		{
			get
			{
				if (_wcPath == null)
					ExtractWorkingCopy();

				return _wcPath;
			}
		}

		/// <summary>
		/// The notifications generated during a call to Client::Add
		/// </summary>
		public SvnNotifyEventArgs[] Notifications
		{
			get
			{
				return (SvnNotifyEventArgs[])this.notifications.ToArray();
			}
		}

		/// <summary>
		/// The client object.
		/// </summary>
		public SvnClient Client
		{
			[System.Diagnostics.DebuggerStepThrough]
			get { return this.client; }
		}

		/// <summary>
		/// Callback method to be used as ClientContext.NotifyCallback
		/// </summary>
		/// <param name="notification">An object containing information about the notification</param>
		public virtual void NotifyCallback(object sender, SvnNotifyEventArgs e)
		{
			e.Detach();
			this.notifications.Add(e);
		}

		/// <summary>
		/// Creates a textfile with the given name in the WC
		/// </summary>
		/// <param name="name">The name of the ifle to create</param>
		/// <returns>The path to the created text file</returns>
		protected string CreateTextFile(string name)
		{
			string path = Path.Combine(this.WcPath, name);
			using (StreamWriter writer = File.CreateText(path))
				writer.Write("Hello world");

			return path;
		}

		/// <summary>
		/// generate a unique directory name
		/// </summary>
		/// <param name="baseName"></param>
		/// <returns></returns>
		protected string FindDirName(string baseName)
		{
			string dir = baseName;
			int i = 1;
			while (Directory.Exists(dir))
			{
				dir = string.Format("{0}-{1}", baseName, i);
				++i;
			}

			return Path.GetFullPath(dir);
		}

		protected string GetTempFile()
		{
			// ensure we get a long path
			StringBuilder builder = new StringBuilder(260);
			Win32.GetLongPathName(Path.GetTempFileName(), builder, 260);
			string tmpPath = builder.ToString();
			File.Delete(tmpPath);

			return tmpPath;
		}

		/// <summary>
		/// Rename the administrative subdirectories if necessary.
		/// </summary>
		/// <param name="path"></param>
		protected void RenameAdminDirs(string path)
		{
			string adminDir = Path.Combine(path, TRAD_WC_ADMIN_DIR);
			string newDir = Path.Combine(path, SvnClient.AdministrativeDirectoryName);
			if (Directory.Exists(adminDir) &&
				TRAD_WC_ADMIN_DIR != SvnClient.AdministrativeDirectoryName)
			{
				Directory.Move(adminDir, newDir);
			}

			foreach (string dir in Directory.GetDirectories(path))
				this.RenameAdminDirs(dir);
		}

		/// <summary>
		/// Starts a svnserve instance.
		/// </summary>
		/// <param name="root">The root directory to use for svnserve.</param>
		/// <returns></returns>
		protected Process StartSvnServe(string root)
		{
			ProcessStartInfo psi = new ProcessStartInfo("svnserve",
				String.Format("--daemon --root {0} --listen-host 127.0.0.1 --listen-port {1}", root,
				PortNumber));
			//psi.UseShellExecute = true;
            psi.CreateNoWindow = true;

			return Process.Start(psi);
		}

		protected void SetReposAuth()
		{
			string conf = Path.Combine(this.ReposPath,
				Path.Combine("conf", "svnserve.conf"));
			string authConf = Path.Combine(this.ReposPath,
				Path.Combine("conf", "svnserve.auth.conf"));
			File.Copy(authConf, conf, true);
		}

		protected static readonly int PortNumber = 7777 + new Random().Next(5000);



        protected static void TouchFile(string filename)
        {
            TouchFile(filename, false);
        }

        protected static void Touch2(string filename)
        {
            File.WriteAllText(filename, Guid.NewGuid().ToString());
        }

        protected static void TouchFile(string filename, bool createDir)
        {
            string dir = Path.GetDirectoryName(filename);
            if (createDir && !Directory.Exists(dir))
                Directory.CreateDirectory(dir);

            using (FileStream fs = File.Create(filename))
            {
                fs.Write(new byte[0], 0, 0);
            }
        }



		protected readonly string REPOS_FILE;
		private const string REPOS_NAME = "repos";
		protected const string BASEPATH = @"\tmp";
		protected readonly string WC_FILE;
		protected const string WC_NAME = "wc";
		protected const string TRAD_WC_ADMIN_DIR = ".svn";
		private SvnClient client;

		protected List<SvnNotifyEventArgs> notifications;
	}
}
