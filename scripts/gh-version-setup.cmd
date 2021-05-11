@echo off
SETLOCAL ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION

SET CACHE=%0\..\gh.cache.bat
SET RSPFILE=%0\..\msbuild-version.rsp

echo @echo off > %CACHE%

for /F "usebackq tokens=2,3" %%i in (`"type %1 |findstr /C:_VERSION_"`) do (
  SET %%i=%%j
  echo SET %%i=%%j>> %CACHE%
) 

set    SHARPPROJ_MAJOR=%PROJ_VERSION_MAJOR%
set /a SHARPPROJ_MINOR=%PROJ_VERSION_MINOR% * 1000 + %PROJ_VERSION_PATCH%
set    SHARPPROJ_PATCH=%1

echo Found PROJ %PROJ_VERSION_MAJOR%.%PROJ_VERSION_MINOR%.%PROJ_VERSION_PATCH% from header
echo Prepare building SharpProj %PROJ_VERSION_MAJOR%.%SHARPPROJ_MINOR%.%1

(
  echo SET SHARPPROJ_MAJOR=%SHARPPROJ_MAJOR%
  echo SET SHARPPROJ_MINOR=%SHARPPROJ_MINOR%
  echo SET SHARPPROJ_PATCH=%SHARPPROJ_PATCH%
) >> %CACHE%

(
  echo /p:ForceAssemblyVersion=%SHARPPROJ_MAJOR%.%SHARPPROJ_MINOR%.%SHARPPROJ_PATCH%
  echo /p:ForceAssemblyCompany="SharpSvb Project, powered by AmpScm, QQn & GitHub"
  echo /p:ForceAssemblyCopyright="Apache 2.0 licensed. See https://github.com/ampscm/SharpSvn"
  echo /p:BuildBotBuild=true
  echo /p:RestoreForce=true
) >> %RSPFILE%
