This directory contains a script which builds subversion and its 
dependencies.

You can use this script to retrieve the dependencies; but you can also
use the precompiled files from the StudioTurtle website.

Only the SharpSvn-DB-XXXXXX.dll is needed at runtime as all c code is
compiled into SharpSvn.dll. All unmanaged PDB information is copied into 
SharpSvn.pdb and annotated with SourceServerSharp to allow debugging without
your own import directory.
