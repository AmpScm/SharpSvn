// $Id$
//
// Copyright 2008-2009 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using SharpSvn;
using System.Threading;
using System.Globalization;
using System.Reflection;

namespace SharpSvn.Tests
{
	[TestFixture]
	public class LocalizationTests
	{
		[Test]
		public void TestResourceLoading()
		{
			Type tp = typeof(SvnClient).Assembly.GetType("SharpSvn.SharpSvnStrings", false);

			Assert.That(tp, Is.Not.Null, "SharpSvnStrings type exists in SharpSvn");

			PropertyInfo pi = tp.GetProperty("ArgumentMustBeAValidRepositoryUri", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Static | System.Reflection.BindingFlags.GetProperty);

			Assert.That(pi, Is.Not.Null, "ArgumentMustBeAValidRepositoryUri is a valid property on SharpSvnStrings");

			Assert.That(pi.GetValue(null, null), Is.Not.Null);
		}


		Exception GetNotFoundError()
		{
			SvnClient client = new SvnClient();
			SvnInfoArgs a = new SvnInfoArgs();
			a.ThrowOnError = false;


			Assert.That(client.Info("c:/does/not/ever/exist/on/windows", a,
				delegate(object sender, SvnInfoEventArgs e)
				{
				}), Is.False, "Should fail");

			return a.LastException;
		}

		[Test]
		public void TestGermanError()
		{
			Exception exDefault = GetNotFoundError();
			Exception exGerman;
			Exception exSpanish;
			Exception exEnglish;

			SvnClient.EnableSubversionLocalization();

			Thread thread = Thread.CurrentThread;
			CultureInfo previousCulture = CultureInfo.CurrentUICulture;
			try
			{
				thread.CurrentUICulture = new CultureInfo("de-de");

				exGerman = GetNotFoundError();
			}
			finally
			{
				thread.CurrentUICulture = previousCulture;
			}

			try
			{
				thread.CurrentUICulture = new CultureInfo("es-es");

				exSpanish = GetNotFoundError();
			}
			finally
			{
				thread.CurrentUICulture = previousCulture;
			}

			try
			{
				thread.CurrentUICulture = new CultureInfo("en-us");

				exEnglish = GetNotFoundError();
			}
			finally
			{
				thread.CurrentUICulture = previousCulture;
			}

			Assert.That(exDefault, Is.Not.Null);
			Assert.That(exGerman, Is.Not.Null);
			Assert.That(exSpanish, Is.Not.Null);
			Assert.That(exEnglish, Is.Not.Null);

			Assert.That(exEnglish.Message, Is.EqualTo(exDefault.Message));
			Assert.That(exGerman.Message, Is.Not.EqualTo(exDefault.Message));
			Assert.That(exSpanish.Message, Is.Not.EqualTo(exDefault.Message));
			Assert.That(exSpanish.Message, Is.Not.EqualTo(exGerman.Message));

			Assert.That(exDefault.Message.Contains("does\\not"));
			Assert.That(exGerman.Message.Contains("does\\not"));
			Assert.That(exSpanish.Message.Contains("does\\not"));
		}
	}
}
