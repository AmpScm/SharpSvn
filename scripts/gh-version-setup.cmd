@echo off
SETLOCAL ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION

SET CACHE=%0\..\gh.cache.bat
SET RSPFILE=%0\..\msbuild-version.rsp
SET VERFILE=%0\..\..\imports\release\include\svn_version.h

IF NOT EXIST "%VERFILE%" (
  echo %VERFILE% does not exist
  exit /b 1
)

echo @echo off > %CACHE%

for /F "usebackq tokens=2,3" %%i in (`"type %VERFILE% |findstr /C:SVN_VER_"`) do (
  if NOT "%%j" == "" (
    SET %%i=%%j
    echo SET %%i=%%j>> %CACHE%
  )
) 

pushd %0\..
FOR /F "usebackq" %%i in (`git rev-parse HEAD`) do (
  SET GIT_SHA=%%i
)
popd

set    SHARPSVN_MAJOR=%SVN_VER_MAJOR%
set /a SHARPSVN_MINOR=%SVN_VER_MINOR% * 1000 + %SVN_VER_PATCH%
set    SHARPSVN_PATCH=%1

echo Found Subversion %SVN_VER_MAJOR%.%SVN_VER_MINOR%.%SVN_VER_PATCH% from header
echo Prepare building SharpProj %SVN_VER_MAJOR%.%SHARPSVN_MINOR%.%1

(
  echo SET SHARPSVN_MAJOR=%SHARPSVN_MAJOR%
  echo SET SHARPSVN_MINOR=%SHARPSVN_MINOR%
  echo SET SHARPSVN_PATCH=%SHARPSVN_PATCH%
  echo SET GIT_SHA=%GIT_SHA%
) >> %CACHE%

(
  REM For SDK Projects
  echo /p:Version=%SHARPSVN_MAJOR%.%SHARPSVN_MINOR%.%SHARPSVN_PATCH%
  echo /p:Company="SharpSvn Project, powered by AmpScm, QQn & GitHub"
  echo /p:Copyright="Apache 2.0 licensed. See https://github.com/ampscm/SharpSvn"
  echo /p:InformationalVersion=%SHARPSVN_MAJOR%.%SHARPSVN_MINOR%.%SHARPSVN_PATCH%-%GIT_SHA%

  REM For SharpSvn.AnnotateAssembly
  echo /p:ForceAssemblyVersion=%SHARPSVN_MAJOR%.%SHARPSVN_MINOR%.%SHARPSVN_PATCH%
  echo /p:ForceAssemblyInformationalVersion=%SHARPSVN_MAJOR%.%SHARPSVN_MINOR%.%SHARPSVN_PATCH%-%GIT_SHA%
  echo /p:ForceAssemblyCompany="SharpSvn Project, powered by AmpScm, QQn & GitHub"
  echo /p:ForceAssemblyCopyright="Apache 2.0 licensed. See https://github.com/ampscm/SharpSvn"

  REM For scripting
  echo /p:BuildBotBuild=true
  echo /p:RestoreForce=true
  echo /p:RestorePackagesConfig=true
) > %RSPFILE%
