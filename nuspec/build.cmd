@echo off
setlocal enableextensions

REM Do this as separate if to avoid using stale variables later on
if "%1" == "-gh" (
  call "%0\..\..\scripts\gh.cache.bat" || exit /B 1
)

if "%1" == "-gh" (
  SET SVN_VER=%SVN_VER_MAJOR%.%SVN_VER_MINOR%.%SVN_VER_PATCH%
  SET SHARPSVN_VER=%SHARPSVN_MAJOR%.%SHARPSVN_MINOR%.%SHARPSVN_PATCH%
) else if "%2" == "" (
  echo "%0 <svn-ver> <sharpsvn-ver>"
  exit /b 1
) else (
  SET SVN_VER=%1
  SET SHARPSVN_VER=%2

  pushd ..\src
  msbuild /m /p:Configuration=Release /p:Platform=x86 /p:ForceAssemblyVersion=%SHARPSVN_VER% /v:m /nologo || exit /B 1
  msbuild /m /p:Configuration=Release /p:Platform=x64 /p:ForceAssemblyVersion=%SHARPSVN_VER% /v:m /nologo || exit /B 1
  popd
)

echo Packaging using version %SHARPSVN_VER% (svn version=%SVN_VER%)
pushd %0\..
if NOT EXIST ".\obj\." mkdir obj
if NOT EXIST ".\bin\." mkdir bin

pushd obj
..\..\imports\build\refasmer\refasmerExe ..\..\src\SharpSvn\bin\Win32\Release\SharpSvn.dll && copy SharpSvn.dll.refasm.dll SharpSvn.dll || exit /B 1
popd


CALL :xmlpoke SharpSvn.nuspec //nu:metadata/nu:version %SHARPSVN_VER% || EXIT /B 1
IF NOT "%GIT_SHA%" == "" CALL :xmlpoke SharpSvn.nuspec "//nu:repository[@type='git']/@commit" %GIT_SHA% || EXIT /B 1

CALL :xmlpoke SharpSvn.UI.nuspec //nu:metadata/nu:version %SHARPSVN_VER% || EXIT /B 1
CALL :xmlpoke SharpSvn.UI.nuspec "//nu:dependency[@id='SharpSvn']/@version" "[%SHARPSVN_VER%]" || EXIT /B 1
IF NOT "%GIT_SHA%" == "" CALL :xmlpoke SharpSvn.UI.nuspec "//nu:repository[@type='git']/@commit" %GIT_SHA% || EXIT /B 1

CALL :xmlpoke SharpSvn.AnnotateAssembly.nuspec //nu:metadata/nu:version %SHARPSVN_VER% || EXIT /B 1
IF NOT "%GIT_SHA%" == "" CALL :xmlpoke SharpSvn.AnnotateAssembly.nuspec "//nu:repository[@type='git']/@commit" %GIT_SHA% || EXIT /B 1
CALL :xmlpoke SharpSvn.ShortMSDeployWebContentPath.nuspec //nu:metadata/nu:version %SHARPSVN_VER% || EXIT /B 1
IF NOT "%GIT_SHA%" == "" CALL :xmlpoke SharpSvn.ShortMSDeployWebContentPath.nuspec "//nu:repository[@type='git']/@commit" %GIT_SHA% || EXIT /B 1

nuget pack -Symbols SharpSvn.nuspec -version %SHARPSVN_VER% -OutputDirectory bin || exit /B 1
nuget pack -Symbols SharpSvn.UI.nuspec -version %SHARPSVN_VER% -OutputDirectory bin || exit /B 1
nuget pack SharpSvn.AnnotateAssembly.nuspec -version %SHARPSVN_VER% -OutputDirectory bin || exit /B 1
nuget pack SharpSvn.ShortMSDeployWebContentPath.nuspec -version %SHARPSVN_VER% -OutputDirectory bin || exit /B 1
echo "--done--"
popd
goto :eof

:xmlpoke
echo msbuild /nologo /v:m xmlpoke.build "/p:File=%1" "/p:XPath=%2" "/p:Value=%3" || exit /B 1
msbuild /nologo /v:m xmlpoke.build "/p:File=%1" "/p:XPath=%2" "/p:Value=%3" || exit /B 1
exit /B 0
