using System;
using System.Collections.Generic;
using System.Text;
using TurtleSvn;
using System.Reflection;
using TurtleSvn.Security;
using System.IO;

namespace TurtleSvn
{
	class Program
	{
		SvnClient _client;

		static void Main(string[] args)
		{
			new Program().Run(args);
		}

		void Run(string[] args)
		{
			Console.WriteLine("TurtleSvn version {0}, using Subversion {1} libraries", SvnClient.WrapperVersion, SvnClient.Version);

			_client = new SvnClient();

			_client.LoadConfigurationDefault();
			_client.Authenticator.AddSubversionFileHandlers();
			_client.Authenticator.UsernamePasswordHandlers += new SvnAuthenticationHandler<SvnUsernamePasswordArgs>(Authenticator_UsernamePasswordHandlers);

			//_client.Authenticator.
			//_client.Authentication.

			_client.ClientCancel += new EventHandler<SvnClientCancelEventArgs>(OnClientCancel);
			_client.ClientNotify += new EventHandler<SvnClientNotifyEventArgs>(OnClientNotify);
			_client.ClientProgress += new EventHandler<SvnClientProgressEventArgs>(OnClientProgress);

			//SvnTarget target;

			string _tmpDir = "f:\\svn-test-location";

			if (Directory.Exists("f:\\svn-test-location"))
			{
				DirectoryInfo dir = new DirectoryInfo(_tmpDir);

				foreach (FileInfo file in dir.GetFiles("*", SearchOption.AllDirectories))
				{
					if ((file.Attributes & FileAttributes.ReadOnly) != 0)
						file.Attributes = FileAttributes.Normal;
				}

				Directory.Delete("f:\\svn-test-location", true);
			}
			Directory.CreateDirectory("f:\\svn-test-location");

			long ver;
			_client.CheckOut(new Uri("https://studioturtle.googlecode.com/svn/trunk/src/TurtleSvn/"), "f:\\svn-test-location\\gc", out ver);
			_client.Update("f:\\svn-test-location\\gc");
			//_client.CheckOut(new Uri("file:///F:/_local/repos/"), "f:\\test-co", out ver);
			//_client.Update(@"g:\Projects\SvnDotNet", new SvnUpdateArgs(), out ver);
			//long rev;
			
		}

		bool Authenticator_UsernamePasswordHandlers(object sender, TurtleSvn.Security.SvnUsernamePasswordArgs e)
		{
			return false;
		}

		void OnClientProgress(object sender, SvnClientProgressEventArgs e)
		{

		}

		void OnClientNotify(object sender, SvnClientNotifyEventArgs e)
		{
			Console.WriteLine(e.FullPath);
		}

		void OnClientCancel(object sender, SvnClientCancelEventArgs e)
		{
		}
	}
}
