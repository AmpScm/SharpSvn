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
			Console.WriteLine("SharpSvn version {0}, using Subversion {1}", SvnClient.SharpSvnVersion, SvnClient.Version);

			_client = new SvnClient();

			_client.LoadConfigurationDefault();
			_client.Authenticator.Clear();
			//_client.Authenticator.SslServerTrustHandlers += _client.Authenticator.CryptoStoreSslServerTrustHandler;
			_client.Authenticator.SslServerTrustHandlers += SvnAuthentication.SubversionWindowsSslServerTrustHandler;
			//_client.Authenticator.SslServerTrustHandlers += SharpSvn.Security.SvnAuthentication.SubversionFileSslServerTrustHandler;
			//_client.Authenticator.AddSubversionFileHandlers();
			_client.Authenticator.AddDialogHandlers();
			//_client.Authenticator.UsernamePasswordHandlers += new EventHandler<SvnUsernamePasswordEventArgs>(Authenticator_UsernamePasswordHandlers);

			//_client.Authenticator.
			//_client.Authentication.

			_client.Cancel += new EventHandler<SvnCancelEventArgs>(OnClientCancel);
			_client.Notify += new EventHandler<SvnNotifyEventArgs>(OnClientNotify);
			_client.Progress += new EventHandler<SvnProgressEventArgs>(OnClientProgress);

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
            _client.CheckOut(new Uri("http://sharpsvn.googlecode.com/svn/trunk/"), "f:\\svn-test-location\\gc", out ver);
			_client.Update("f:\\svn-test-location\\gc");

			_client.PropertyList((SvnPathTarget)"F:\\svn-test-location\\gc\\src\\SharpSvn.UI\\SharpSvn.UI.snk", new EventHandler<SvnPropertyListEventArgs>(OnPropertyItem));
			//_client.CheckOut(new Uri("file:///F:/_local/repos/"), "f:\\test-co", out ver);
			//_client.Update(@"g:\Projects\SvnDotNet", new SvnUpdateArgs(), out ver);
			//long rev;
			
		}

		void OnPropertyItem(object sender, SvnPropertyListEventArgs e)
		{
			GC.KeepAlive(e.Properties);

		}

		bool Authenticator_UsernamePasswordHandlers(object sender, SharpSvn.Security.SvnUsernamePasswordEventArgs e)
		{
			return false;
		}

		void OnClientProgress(object sender, SvnProgressEventArgs e)
		{

		}

		void OnClientNotify(object sender, SvnNotifyEventArgs e)
		{
			Console.WriteLine(e.FullPath);
		}

		void OnClientCancel(object sender, SvnCancelEventArgs e)
		{
		}
	}
}
