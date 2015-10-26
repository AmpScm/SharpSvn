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
set SWIG=
set LOCAL=1
set HTTPLOG=--httpd-no-log
set FS=fsfs
:next

IF "%1" == "-d" (
   SET VALS=-d
   SHIFT
) ELSE IF "%1" == "-r" (
   SET VALS=-r
   SHIFT
) ELSE IF "%1" == "-k" (
   taskkill /im tsvncache.exe /im python.exe /im svn.exe /im httpd.exe /im svnrdump.exe /im svnsync.exe /im svnserve.exe /f
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
   SET HTTP_SCHEME=http
   SET HTTP_HOSTNAME=127.0.0.1
   SHIFT
) ELSE IF "%1" == "--https" (
   SET HTTP=1
   SET HTTP_SCHEME=https
   SET HTTP_HOSTNAME=localhost
   SET ARGS=--https %ARGS%
   SHIFT
) ELSE IF "%1" == "--http2" (
   SET ARGS=--http2 %ARGS%
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
) ELSE IF "%1" == "--bdb" (
   SET FS=bdb
   SHIFT
) ELSE IF "%1" == "--fsx" (
   SET FS=fsx
   SHIFT
) ELSE IF "%1" == "--perl" (
   SET SWIG=perl
   SHIFT
) ELSE IF "%1" == "--python" (
   SET SWIG=python
   SHIFT
) ELSE IF "%1" == "--ruby" (
   SET SWIG=ruby
   SHIFT
) ELSE IF "%1" == "--java" (
   SET JAVA=java
   SHIFT
) ELSE IF "%1" == "-v" (
   SET ARGS=-v !ARGS!
   SHIFT
) ELSE IF "%1" == "--disable-http-v2" (
   SET ARGS=--disable-http-v2 !ARGS!
   SHIFT   
) ELSE IF "%1" == "--enable-sasl" (
   SET ARGS=--enable-sasl !ARGS!
   SHIFT   
) ELSE IF "%1" == "--enable-memcached" (
   SET ARGS=--memcached-server=127.0.0.1:11211 --memcached-dir=%cd%\release\memcached !ARGS!
   SHIFT   
) ELSE (
   SET ARGS=!ARGS! -t %1
   SHIFT
)

IF NOT "%1" == "" GOTO next

IF NOT "%SWIG%" == "" (
  SET ARGS=--swig=%SWIG% %ARGS%
) ELSE IF NOT "%JAVA%" == "" (
  SET ARGS=--javahl %ARGS%
)

PATH %CD%\release\bin;%PATH%

set BASE=R:\Tst

set TEMP=%BASE%\Z-temp
set TMP=%TEMP%
set HTTPINFO=--httpd-dir %cd%\release\httpd --httpd-port 7829 -u %HTTP_SCHEME%://%HTTP_HOSTNAME%:7829 %HTTPLOG%
set SVN_DBG_REAL_QUIET=1
set LOCATION=R:\

mkdir %LOCATION%\subversion\tests\cmdline\svn-test-work\working_copies 2>nul:
subst w: /d > nul:
subst w: %LOCATION%\subversion\tests\cmdline\svn-test-work\working_copies

IF NOT EXIST "%TEMP%\." MKDIR "%TEMP%"
IF NOT EXIST "%LOCATION%" MKDIR "%LOCATION%"

del "%TEMP%\*" /q

IF NOT EXIST "to-delete" MKDIR "to-delete"

set ARGS=%ARGS% %CLEANUP% %LOCATION% %LOGLEVEL%
pushd dev
if NOT "%LOCAL%" == "" (
  del %LOCATION%tests.log %LOCATION%fails.log 2> nul:
  timethis win-tests.py %PARALLEL% %VALS% -f %FS% %ARGS%
)
if NOT "%SVNSERVE%" == "" (
  del %LOCATION%svn-tests.log %LOCATION%svn-fails.log 2> nul:
  timethis win-tests.py %PARALLEL% %VALS% -f %FS% -u svn://127.0.0.1 %ARGS%
)
if NOT "%HTTP%" == "" (
  echo del %LOCATION%dav-tests.log %LOCATION%dav-fails.log %LOCATION%subversion\tests\cmdline\httpd\log
  del %LOCATION%dav-tests.log %LOCATION%dav-fails.log %LOCATION%subversion\tests\cmdline\httpd\log
  timethis win-tests.py %PARALLEL% %VALS% -f %FS% %HTTPINFO% %ARGS%
)

REM timethis win-tests.py %PARALLEL% %VALS% -f bdb %ARGS%
REM timethis win-tests.py %PARALLEL% %VALS% -f bdb -u svn://localhost %ARGS%
REM timethis win-tests.py %VALS% -f bdb -u http://localhost:7890 %HTTPINFO% %ARGS%

popd
