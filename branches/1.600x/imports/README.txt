 The SharpSvn /imports/ directory
 =================================================
  $Id$

This directory contains a number of scripts to allow building Subversion
in a number of ways.

  1) An almost static release for SharpSvn with minor patches
  
  2) A standard windows release with dll prefixes renamed
  
  3) A clean development environment for development on subversion 
     itself


The scripts require the following tools: (currently used version)
  * NAnt                        (0.86 Beta 1)
  * Perl 5                      (Activestate perl 5.8.8)
  * Python                      (Activestate python 2.5.1)
  * Wix                         (3.0.4214 daily build)
  * Visual C++ 2005 or 2008     (2005 SP1)

It is assumed all these tools are in the PATH and the settings for
the 'Visual Studio Command Prompt' are loaded.

All environments accept the following defines

 Setting platform
  -D:platform=x64
  -D:platform=win32 (Default)


1) To build the dependencies
	Go to sharpsvn/imports
	Type NAnt
	After about 20 a 30 minutes you should see build succeeded
	(with maybe a few ignored errors)
	
2) To build the windows release
    Go to sharpsvn/imports/svn-dist
    Type NAnt
    
3) To build the subversion development environment
    Copy the file sharpsvn/imports/dev-default.build to sharpsvn/default.build    
    Type NAnt
    
    Open wc\subversion_vcnet.sln in your IDE.
    

This directory contains a script which builds subversion and its 
dependencies.

You can use this script to retrieve the dependencies; but you can also
use the precompiled files from the SharpSvn website.

Only the SharpSvn-DB-XXXXXX.dll is needed at runtime as all c code is
compiled into SharpSvn.dll. All unmanaged PDB information is copied into 
SharpSvn.pdb and annotated with SourceServerSharp to allow debugging without
your own import directory.
