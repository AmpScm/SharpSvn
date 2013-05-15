@echo off
SETLOCAL ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION

if "%PLATFORM%" == "" (
  SET Platform=win32
)

set VALS=-d
set PARALLEL=-p
set CLEANUP=-c
set ARGS=
set LOGLEVEL=
set LOCAL=1
set HTTPLOG=--httpd-no-log
:next

IF "%1" == "-d" (
   SET VALS=-d
   SHIFT
) ELSE IF "%1" == "-r" (
   SET VALS=-r
   SHIFT
) ELSE IF "%1" == "-k" (
   taskkill /im tsvncache.exe /im python.exe /im svn.exe /im httpd.exe /im svnrdump.exe /f
   SHIFT
) ELSE IF "%1" == "-s" (
   SET PARALLEL=
   SHIFT
) ELSE IF "%1" == "-c" (
   SET CLEANUP=
   SHIFT
) ELSE IF "%1" == "-l" (
   SET LOGLEVEL=--log-level=DEBUG
   SHIFT
) ELSE IF "%1" == "" (
   SHIFT
) ELSE IF "%1" == "--http" (
   SET HTTP=1
   SHIFT
) ELSE IF "%1" == "--http-log" (
   SET HTTPLOG=
   SHIFT
) ELSE IF "%1" == "--svnserve" (
   SET SVNSERVE=1
   SHIFT
) ELSE IF "%1" == "--no-local" (
   SET LOCAL=
   SHIFT
) ELSE (
   SET ARGS=!ARGS! -t %1
   SHIFT
)

if NOT "%1" == "" GOTO next

set BASE=R:\Tst-2012

set path=%BASE%\bin;%path%
set TEMP=%BASE%\Z-temp
set TMP=%TEMP%
set HTTPINFO=--httpd-dir %cd%\release\httpd --httpd-port 7829 -u http://127.0.0.1:7829 %HTTPLOG%
set SVN_DBG_REAL_QUIET=1
set LOCATION=R:\
IF NOT EXIST "%BASE%\bin" MKDIR "%BASE%\bin"
xcopy /y Release\bin "%BASE%\bin" > nul:
if EXIST "Release\bin\%PLATFORM%" (
  xcopy /y "Release\bin\%PLATFORM%" "%BASE%\bin" > nul:
)

mkdir %LOCATION%\subversion\tests\cmdline\svn-test-work\working_copies 2>nul:
subst w: /d > nul:
subst w: %LOCATION%\subversion\tests\cmdline\svn-test-work\working_copies

if NOT EXIST "%TEMP%\." MKDIR "%TEMP%"
if NOT EXIST "%LOCATION%" MKDIR "%LOCATION%"

del "%TEMP%\*" /q

IF NOT EXIST "to-delete" MKDIR "to-delete"

set ARGS=%ARGS% %CLEANUP% %LOCATION% %LOGLEVEL%
pushd dev
if NOT "%LOCAL%" == "" (
  del %LOCATION%tests.log %LOCATION%fails.log 2> nul:
  timethis win-tests.py %PARALLEL% %VALS% -f fsfs %ARGS%
)
if NOT "%SVNSERVE%" == "" (
  del %LOCATION%svn-tests.log %LOCATION%svn-fails.log 2> nul:
  timethis win-tests.py %PARALLEL% %VALS% -f fsfs -u svn://127.0.0.1 %ARGS%
)
if NOT "%HTTP%" == "" (
  echo del %LOCATION%dav-tests.log %LOCATION%dav-fails.log %LOCATION%subversion\tests\cmdline\httpd\log
  del %LOCATION%dav-tests.log %LOCATION%dav-fails.log %LOCATION%subversion\tests\cmdline\httpd\log
  timethis win-tests.py %VALS% -f fsfs %HTTPINFO% %ARGS%
)

REM timethis win-tests.py %PARALLEL% %VALS% -f bdb %ARGS%
REM timethis win-tests.py %PARALLEL% %VALS% -f bdb -u svn://localhost %ARGS%
REM timethis win-tests.py %VALS% -f bdb -u http://localhost:7890 %HTTPINFO% %ARGS%

popd
