@echo off
setlocal enableextensions
if "%PLATFORM%" == "" (
  SET Platform=win32
)
SET config=debug
SET VERBOSITY=/v:m
SET TARGET=/t:__ALL_TESTS__ /t:Programs\svn-wc-db-tester
:next
IF "%1" == "-r" (
   SET CONFIG=release
) ELSE IF "%1" == "-d" (
   SET CONFIG=debug
) ELSE IF "%1" == "-q" (
   SET REST = %REST% /v:q
) ELSE IF "%1" == "-k" (
   taskkill /im tsvncache.exe /im python.exe /im svn.exe /im svnadmin.exe /im httpd.exe /im svnsync.exe /im svnserve.exe /f
) ELSE IF "%1" == "-s" (
   SET TARGET=/t:Programs\svn /t:Programs\svnadmin /t:Programs\entries-dump
) ELSE IF "%1" == "-j" (
   SET TARGET=%TARGET% /t:__JAVAHL_TESTS__
) ELSE IF "%1" == "-v" (
   SET VERBOSITY=/v:n
)
SHIFT

if NOT "%1" == "" GOTO next

pushd dev
call msbuild /m %VERBOSITY% /p:Platform=%Platform% /p:Configuration=%CONFIG% %TARGET% %REST%
IF ERRORLEVEL 1 (
  echo == Build Failed ==
  exit /B 1
)
