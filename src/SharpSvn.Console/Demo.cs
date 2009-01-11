// $Id$
//
// Copyright 2007-2009 The SharpSvn Project
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
using SharpSvn;

class Demo
{
	public static void qMain()
	{
		using (SvnClient client = new SvnClient())
		{
			// Checkout the code to the specified directory
			client.CheckOut(new Uri("http://sharpsvn.googlecode.com/svn/trunk/"), "c:\\sharpsvn");

			// Update the specified working copy path to the head revision
			client.Update("c:\\sharpsvn");

			// Update the specified working copy path to the head revision and save the specific revision
			SvnUpdateResult result;
			client.Update("c:\\sharpsvn", out result);

			client.Move("c:\\sharpsvn\\from.txt", "c:\\sharpsvn\\new.txt");

			// Commit the changes with the specified logmessage
			SvnCommitArgs ca = new SvnCommitArgs();
			ca.LogMessage = "Moved from.txt to new.txt";
			client.Commit("c:\\sharpsvn");
		}
	}
}