using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using SharpDns;
using System.Threading;

namespace SharpSvn.Tests
{
	[TestFixture]
	public class DnsTests
	{
		[Test, Explicit("Not complete")]
		public void CreateManager()
		{
			DnsRequestManager mgr = new DnsRequestManager();

			mgr.Dispose();
		}

		[Test, Explicit("Not complete")]
		public void SendMessage()
		{
			DnsRequestManager mgr = new DnsRequestManager();
			{
				DnsMessage msg = new DnsMessage();

				msg.AddQuestion("ijss.rr.competence.biz.", DnsDataClass.In, DnsDataType.A);

				mgr.Send(msg);

				for (int i = 0; i < 100; i++)
				{
					Thread.Sleep(200);
				}

				msg = new DnsMessage();

				mgr.Send(msg);

				mgr.Shutdown();

				Thread.Sleep(2000);
			}
		}
	}
}
