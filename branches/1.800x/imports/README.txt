 The SharpSvn /imports/ directory
 =================================================
  $Id$

This directory contains a number of scripts to allow building Subversion
in a number of ways. Normally you only build one variant.

  1) An almost static release for SharpSvn with minor patches
  
  2) A standard windows release with dll prefixes renamed
  
  3) A clean development environment for development on subversion 
     itself


The scripts require the following tools: (currently used version)
  * NAnt        (0.92)
  * Perl 5      (Activestate perl   5.16.3 x86)
  * Python      (Activestate python 2.7.2  x86)
  * Visual Studio 2008, 2010 or 2012
    (Untested, so might require tweaks: 2005 and 2013)

It is assumed all these tools are in the PATH and the settings for
the 'Visual Studio Command Prompt' are loaded.

1) To build all the dependencies (including Subversion) as used by SharpSvn
   Open the visual studio command prompt for the right architecture,
   e.g. VS2012 x64 Cross Tools Command Prompt
   
   I recommend using the 'native x86' or 'x64 cross' tools as the native
   x64 variants can't always find everything in MSBuild.
	
	 * Go to 'imports'
	 * Type> 'nant build'
	 After something between 5 and 30 minutes you should see build succeeded
	 (with maybe 'a few' ignored errors)
	
2) If (instead of building SharpSvn) you just want to build a windows release
   of Subversion open a command prompt (see 1).
   
   Go to 'imports/svn-dist'
   * Type> 'nant'
   After something between 5 and 30 minutes you should see build succeeded
	 (with maybe 'a few' ignored errors)
    
3) Or if you want to do development on Subversion itself, open a command
   prompt (See 1)
   
   * Create a base directory
   * Type> 'svn co http://sharpsvn.open.collab.net/svn/sharpsvn/trunk/imports'
   * Type> 'copy /y imports\dev-default.build'
   * Type> 'nant'
   After something between 5 and 30 minutes you should see build succeeded
	 (with maybe 'a few' ignored errors)
    
   Open the generated 'dev\subversion_vcnet.sln' in your IDE.
   
   You can type 'nant gen-dev' to regenerate the solution whenever the
   build scripts have changed.
