@echo off
setlocal enableextensions
set PYTHONPATH=%CD%\build\scons\lib\site-packages\scons
set INTDIR=%CD%\build\release
set CONFIG=Debug
PATH %INTDIR%\bin;%PATH%

IF NOT EXIST "%dev%\%CONFIG%\" (
  mkdir "%dev%\%CONFIG%\"
)
pushd dev
SET VPATH=%CD:\=/%
cd %CONFIG%

set ARGS=-Y "%VPATH%" SOURCE_LAYOUT=No
set ARGS=%ARGS% "APR=%INTDIR%" "APU=%INTDIR%"
set ARGS=%ARGS% "OPENSSL=%INTDIR%" "ZLIB=%INTDIR%"
set ARGS=%ARGS% APR_STATIC=Yes

call ..\..\build\scons\scripts\scons.bat DEBUG=yes %ARGS% %*
