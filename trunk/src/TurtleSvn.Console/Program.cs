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

			SvnTarget target;

			if (SvnTarget.TryParse("f:\\projects@{2000-03-20T13:02:03}", out target))
				Console.WriteLine(target.ToString());

			if (SvnTarget.TryParse("http://www.vmoo.com/@HEAD", out target))
				Console.WriteLine(target.ToString());

			Console.WriteLine(((SvnUriTarget)new Uri("http://qqn.nl/")).ToString());
		}
	}
}
