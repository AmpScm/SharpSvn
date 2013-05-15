@echo off
setlocal
if "%PLATFORM%" == "" (
  SET Platform=win32
)
SET config=debug
SET TARGET=/t:__ALL_TESTS__
:next
IF "%1" == "-r" (
   SET CONFIG=release
) ELSE IF "%1" == "-d" (
   SET CONFIG=debug
) ELSE IF "%1" == "-q" (
   SET REST = %REST% /v:q
) ELSE IF "%1" == "-k" (
   taskkill /im tsvncache.exe /im python.exe /im svn.exe /im svnadmin.exe /im httpd.exe /f
) ELSE IF "%1" == "-s" (
   SET TARGET=/t:Programs\svn /t:Programs\svnadmin /t:Programs\entries-dump
)
SHIFT

if NOT "%1" == "" GOTO next

pushd dev
msbuild /m /p:Platform=%Platform% /v:m /p:Configuration=%CONFIG% %TARGET% %REST%
