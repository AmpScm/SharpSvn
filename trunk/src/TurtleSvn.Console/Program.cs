using System;
using System.Collections.Generic;
using System.Text;
using QQn.Svn;
using System.Reflection;

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

			_client.ClientCancel += new EventHandler<SvnClientCancelEventArgs>(OnClientCancel);
			_client.ClientGetCommitLog += new EventHandler<SvnClientCommitLogEventArgs>(OnClientGetCommitLog);
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

		void OnClientProgress(object sender, SvnClientProgressEventArgs e)
		{
		}

		void OnClientNotify(object sender, SvnClientNotifyEventArgs e)
		{
		}

		void OnClientGetCommitLog(object sender, SvnClientCommitLogEventArgs e)
		{
		}

		void OnClientCancel(object sender, SvnClientCancelEventArgs e)
		{
		}
	}
}
