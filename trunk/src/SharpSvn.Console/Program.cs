using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn;
using System.Reflection;
using SharpSvn.Security;
using System.IO;
using System.Windows.Forms;

namespace SharpSvn
{
	class Program
	{
		SvnClient _client;

		static void Main(string[] args)
		{
			Application.EnableVisualStyles();
			new Program().Run(args);
		}

		void Run(string[] args)
		{
			Console.WriteLine("SharpSvn version {0}, using Subversion {1} libraries", SvnClient.WrapperVersion, SvnClient.Version);

			_client = new SvnClient();

			_client.LoadConfigurationDefault();
			//_client.Authenticator.Clear();
			//_client.Authenticator.SslServerTrustHandlers += SharpSvn.Security.SvnAuthentication.SubversionFileSslServerTrustHandler;
			//_client.Authenticator.AddSubversionFileHandlers();
			_client.Authenticator.AddDialogHandlers();
			_client.Authenticator.UsernamePasswordHandlers += new SvnAuthenticationHandler<SvnUsernamePasswordArgs>(Authenticator_UsernamePasswordHandlers);

			//_client.Authenticator.
			//_client.Authentication.

			_client.ClientCancel += new EventHandler<SvnClientCancelEventArgs>(OnClientCancel);
			_client.ClientNotify += new EventHandler<SvnClientNotifyEventArgs>(OnClientNotify);
			_client.ClientProgress += new EventHandler<SvnClientProgressEventArgs>(OnClientProgress);

			_client.Status("..\\..\\SharpSvn.Console.csproj", delegate(object sender, SvnStatusEventArgs e)
			{
				Console.WriteLine(e.FullPath);
			});

			_client.Info((SvnPathTarget)"..\\..\\SharpSvn.Console.csproj", delegate(object sender, SvnInfoEventArgs e)
			{
				Console.WriteLine(e.FullPath);
			});

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
			_client.CheckOut(new Uri("https://studioturtle.googlecode.com/svn/trunk/src/SharpSvn/"), "f:\\svn-test-location\\gc", out ver);
			_client.Update("f:\\svn-test-location\\gc");
			//_client.CheckOut(new Uri("file:///F:/_local/repos/"), "f:\\test-co", out ver);
			//_client.Update(@"g:\Projects\SvnDotNet", new SvnUpdateArgs(), out ver);
			//long rev;
			
		}

		bool Authenticator_UsernamePasswordHandlers(object sender, SharpSvn.Security.SvnUsernamePasswordArgs e)
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
