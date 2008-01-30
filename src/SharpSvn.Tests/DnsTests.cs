using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using SharpDns;
using System.Threading;
using NUnit.Framework.SyntaxHelpers;
using System.Net;

namespace SharpSvn.Tests
{
	//[TestFixture]
	public class DnsTests
	{
		[Test]
		public void LookupLocalhost()
		{
			using (DnsRequest rq = DnsRequest.Create())
			{
				rq.AddQuestion(IPAddress.Parse("127.0.0.1"));
				rq.Send();

				DnsResponse rsp = rq.GetResponse();

				Assert.That(rsp.Succeeded, "Succeeded");

				Assert.That(rsp.Answer.Count, Is.GreaterThan(0));
				Assert.That(rsp.Answer[0].Value.StartsWith("localhost."), "Starts with localhost"); // RFC specifies this should be the result on any DNS
				Assert.That(rsp.Result, Is.EqualTo(DnsResult.NoError));
			}
		}

		[Test]
		public void LookupMicrosoftMail()
		{
			using (DnsRequest rq = DnsRequest.Create())
			{
				rq.AddQuestion("microsoft.com", DnsDataType.Mx);
				rq.Send();

				DnsResponse rsp = rq.GetResponse();

				Assert.That(rsp.Answer, Is.Not.Null);
				Assert.That(rsp.Answer.Count, Is.GreaterThan(1));
				Assert.That(rsp.Answer[0].Value, Is.Not.Null);
				Assert.That(rsp.Answer[1].Value, Is.Not.Null);
			}
		}

		[Test]
		public void LookupNonExistant()
		{
			using (DnsRequest rq = DnsRequest.Create())
			{
				rq.AddQuestion("non-existant.intra.example.", DnsDataType.Mx);
				rq.Send();

				DnsResponse rsp = rq.GetResponse();

				Assert.That(!rsp.Succeeded, "Not Succeeded");

				Assert.That(rsp.Answer.Count, Is.EqualTo(0));

			}
		}

		[Test]
		public void SendMessage()
		{
			using(DnsRequest rq = DnsRequest.Create())
			{
				rq.AddQuestion("microsoft.com", DnsDataType.Mx);

				rq.Send();

				DnsResponse rsp = rq.GetResponse();

				Assert.That(rsp, Is.Not.Null);

				foreach (DnsItem item in rsp.Answer)
				{
					Assert.That(item.Name, Is.Not.Null);
				}
			}


			using (DnsRequest rq = DnsRequest.Create())
			{
				rq.AddQuestion("ijss.rr.competence.biz.", DnsDataClass.In, DnsDataType.A);

				rq.Send();

				DnsResponse rsp = rq.GetResponse();

				Assert.That(rq.Response, Is.Not.Null);

				Assert.That(rq.Response.Question, Is.Not.Null);
				Assert.That(rq.Response.Answer, Is.Not.Null);

				foreach (DnsItem dn in rq.Response.Answer)
				{
					Assert.That(dn, Is.Not.Null);
				}

				Assert.That(rq.Response.ToString(), Is.Not.Null);
			}
		}
	}
}
