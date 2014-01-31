@echo off
SETLOCAL ENABLEEXTENSIONS
set TO=%1
set CONFIGURATION=%2
set PLATFORM=%3
set SUFFIX=%4
IF NOT EXIST "%TO%" mkdir "%TO%"
copy "..\SharpPlink\bin%SUFFIX%\%PLATFORM%\%CONFIGURATION%\SharpPlink-%PLATFORM%.exe" %TO%\SharpPlink-%PLATFORM%.svnExe || echo Copying SharpPlink failed
IF ERRORLEVEL 1 EXIT /B 1
EXIT /B 0
