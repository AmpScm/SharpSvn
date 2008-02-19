// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using NUnit.Framework;
using System.Text.RegularExpressions;

using SharpSvn;
using System.Collections.ObjectModel;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests Client::List
	/// </summary>
	[TestFixture]
	public class ListTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this.ExtractRepos();
		}

		/// <summary>
		/// Compares the list from the command line client with that obtained
		/// from Client::List
		/// </summary>
		[Test]
		public void TestList()
		{
			string list = this.RunCommand("svn", "list -v " + this.ReposUrl);

			// clean out whitespace
			string[] entries = Regex.Split(list, @"\r\n");
			//string[] entries = list.Trim().Split( '\n' );
			Hashtable ht = new Hashtable();
			foreach (string e in entries)
			{
				if (e != "")
				{
					Entry ent = new Entry(e);
					ht[ent.Path] = ent;
				}
			}

			Collection<SvnListEventArgs> apiList;

			SvnListArgs a = new SvnListArgs();
			a.Depth = SvnDepth.Children;

			Assert.That(Client.GetList(new SvnUriTarget(ReposUrl, SvnRevision.Head), a, out apiList));

			Assert.That(apiList.Count, Is.EqualTo(ht.Count), "Wrong number of entries returned");

			foreach (SvnListEventArgs ent in apiList)
			{
				string path = ent.Path;

				if (path == "")
					break; // New in 1.4+ ; was not available in ankh tests, as svn_client_ls was used instead of svn_client_list

				if (ent.Entry.NodeKind == SvnNodeKind.Directory)
					path += "/";

				Entry entry = (Entry)ht[path];
				Assert.IsNotNull(entry, "No entry found for " + path);

				entry.Match(ent.Entry);
			}
		}

		private class Entry
		{
			public Entry(string line)
			{
				if (!Reg.IsMatch(line))
					throw new Exception("Commandline client bad output");

				Match match = Reg.Match(line);

				this.createdRevision = int.Parse(match.Groups[1].ToString());
				this.author = match.Groups[2].ToString();

				if (match.Groups[3].Success)
					this.size = long.Parse(match.Groups[3].ToString());
				else
					this.size = 0;

				System.IFormatProvider format =
					System.Globalization.CultureInfo.CurrentCulture;

				// get the month and day
				string date = match.Groups[4].ToString();
				this.time = DateTime.ParseExact(date, "MMM' 'dd",
					format);

				// the year
				if (match.Groups[5].Success)
				{
					this.time = this.time.AddYears(-this.time.Year +
						int.Parse(match.Groups[5].ToString()));
				}

				// or the time of day?
				DateTime timeOfDay = DateTime.Today;
				if (match.Groups[6].Success)
				{
					timeOfDay = DateTime.ParseExact(match.Groups[6].ToString(),
						"HH':'mm", format);
				}
				this.time = this.time.AddHours(timeOfDay.Hour);
				this.time = this.time.AddMinutes(timeOfDay.Minute);

				this.path = match.Groups[7].ToString();
			}

			public void Match(SvnDirEntry ent)
			{
				Assert.AreEqual(this.createdRevision, ent.Revision,
					"CreatedRevision differs");
				Assert.AreEqual(this.size, ent.FileSize,
					"Size differs");

				// strip off time portion
				DateTime entryTime = ent.Time.ToLocalTime();
				entryTime = entryTime - entryTime.TimeOfDay;

				long delta = Math.Abs(this.time.Ticks - entryTime.Ticks);
				Assert.IsTrue(delta < TICKS_PER_MINUTE,
					"Time differs: " + this.time + " vs " +
					entryTime + " Delta is " + delta);
				Assert.AreEqual(this.author, ent.Author, "Last author differs");
			}

			public string Path
			{
				get { return this.path; }
			}

			private const long TICKS_PER_MINUTE = 600000000;

			private long createdRevision;
			private string author;
			private long size;
			private DateTime time;
			private string path;
			private static readonly Regex Reg = new Regex(
				@"\s+(\d+)\s+(\w+)\s+(\d+)?\s+(\w+\s\d+)\s+(?:(\d{4})|(\d\d:\d\d))\s+(\S+)");
		}
	}
}
