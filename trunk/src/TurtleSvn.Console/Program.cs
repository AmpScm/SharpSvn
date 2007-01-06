using System;
using System.Collections.Generic;
using System.Text;
using TurtleSvn;
using System.Reflection;
using TurtleSvn.Security;

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
			_client.Authenticator.UsernamePasswordHandlers += new EventHandler<TurtleSvn.Security.SvnUsernamePasswordArgs>(Authentication_UsernamePasswordHandlers);

			//_client.Authenticator.
			//_client.Authentication.

			_client.ClientCancel += new EventHandler<SvnClientCancelEventArgs>(OnClientCancel);
			_client.ClientNotify += new EventHandler<SvnClientNotifyEventArgs>(OnClientNotify);
			_client.ClientProgress += new EventHandler<SvnClientProgressEventArgs>(OnClientProgress);

			SvnTarget target;

			if (SvnTarget.TryParse("f:\\projects@{2000-03-20T13:02:03}", out target))
				Console.WriteLine(target.ToString());

			if (SvnTarget.TryParse("http://www.vmoo.com/@HEAD", out target))
				Console.WriteLine(target.ToString());

			Console.WriteLine(((SvnUriTarget)new Uri("http://qqn.nl/")).ToString());

			long ver;
			//_client.CheckOut(new Uri("file:///F:/_local/repos/"), "f:\\test-co", out ver);
			_client.Update(@"g:\Projects\SvnDotNet", new SvnUpdateArgs(), out ver);
		}

		void Authentication_UsernamePasswordHandlers(object sender, TurtleSvn.Security.SvnUsernamePasswordArgs e)
		{
			
		}

		void OnClientProgress(object sender, SvnClientProgressEventArgs e)
		{
		}

		void OnClientNotify(object sender, SvnClientNotifyEventArgs e)
		{
		}

		void OnClientCancel(object sender, SvnClientCancelEventArgs e)
		{
		}
	}
}
