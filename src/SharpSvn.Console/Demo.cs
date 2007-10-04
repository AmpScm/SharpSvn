﻿using System;
using SharpSvn;

class Demo
{
	public static void Main()
	{
		using (SvnClient client = new SvnClient())
		{
			// Checkout the code to the specified directory
			client.CheckOut(new Uri("http://sharpsvn.googlecode.com/svn/trunk/"), "c:\\sharpsvn");

			// Update the specified working copy path to the head revision
			client.Update("c:\\sharpsvn");

			// Update the specified working copy path to the head revision and save the specific revision
			long revision;
			client.Update("c:\\sharpsvn", out revision);


			client.Move("c:\\sharpsvn\\from.txt", "c:\\sharpsvn\\new.txt");

			// Commit the changes with the specified logmessage
			SvnCommitArgs ca = new SvnCommitArgs();
			ca.LogMessage = "Moved from.txt to new.txt";
			client.Commit("c:\\sharpsvn");
		}
	}
}