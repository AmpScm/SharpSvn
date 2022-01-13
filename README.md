# SharpSvn
[![latest version](https://img.shields.io/nuget/v/SharpSvn)](https://www.nuget.org/packages/SharpSvn)
[![MSBuild-CI](https://github.com/AmpScm/SharpSvn/actions/workflows/MSBuild.yml/badge.svg)](https://github.com/AmpScm/SharpSvn/actions/workflows/MSBuild.yml)

Subversion wrapped for .Net 4.0+ and .Net Core

SharpSvn wraps the Subversion client (and a few more low level) apis in an easy to use .Net friendly client.


## Usage
The whole idea behind SharpSvn is that it should be as easy to install a simple NuGet package in your
C#/.Net application and use Subversion, without thinking about all the backing dependencies.

SharpSvn implements this idea by compiling all needed libraries via C++/CLI and including them in
the final library. (A pretty modern design decision around 2008 when the work on this library started)

The whole library survived from .Net 2.0 to the most recent .Net Core and now .Net 5/6 libraries,
but without a complete rewrite it is not really possible to support Linux/Mac/OS. So unless somebody
has a nice solution for this, those platforms are out of scope.

## History
This project started as a one man project at CollabNet's open.collab.net site around 2008. A few years
later Bert Huijben was hired by CollabNet to work on Subversion and AnkhSVN full-time and as AnkhSVN
was a SharpSvn user there was quite a focus on SharpSvn.

In 2017 Bert left CollabNet, and the project mostly stalled. After many requests Bert moved the hosting
to GitHub and wrote scripts to automate the builds there. Patches are very welcome to co-maintain
SharpSvn there, but releases will now most likely follow the Subversion releases again.

## NuGet
The last packages are now just available on
https://nuget.org/packages/SharpSvn

This package contains x86, x64 and ARM64 binaries that are automatically selected by the NuGet tool for you.
We are no longer building .Net 2.0 compatible versions as the build support for that is not available on GitHub.
