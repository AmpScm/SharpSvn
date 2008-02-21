// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Runtime.InteropServices;
using System.Text;

namespace SharpSvn.Tests.Commands.Utils
{
	/// <summary>
	/// Contains P/Invoke declarations for Win32 API calls.
	/// </summary>
	public static class Win32
	{
		[DllImport("Kernel32.Dll")]
		public static extern int GetLongPathName(string shortPath,
			StringBuilder longPath, int bufSize);
	}
}
