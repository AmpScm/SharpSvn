using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using SharpDns;
using System.Threading;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests
{
	[TestFixture]
	public class DnsTests
	{
		[Test, Explicit("Not complete")]
		public void CreateManager()
		{
			using (DnsRequest rq = DnsRequest.Create())
			{
				rq.Send();
			}
		}

		[Test, Explicit("Not complete")]
		public void SendMessage()
		{
			using(DnsRequest rq = DnsRequest.Create())
			{
				DnsMessage msg = rq.Request;

				msg.AddQuestion("ijss.rr.competence.biz.", DnsDataClass.In, DnsDataType.A);

				rq.Send();

				for (int i = 0; i < 100; i++)
				{
					if (rq.Response != null)
						break;
					Thread.Sleep(200);
				}

				Assert.That(rq.Response, Is.Not.Null);

				Assert.That(rq.Response.ToString(), Is.Not.Null);

				/*msg = new DnsMessage();

				mgr.Send(msg);

				mgr.Shutdown();

				Thread.Sleep(2000);*/
			}
		}
	}
}
