@echo off
set TO=%1
set CONFIGURATION=%2
set PLATFORM=%3
set SUFFIX=%4
xcopy /r /y /q /i "..\..\imports\release\bin\*-%PLATFORM%*dll" %TO% || echo Copying dependencies failed
copy "..\SharpPlink\bin%SUFFIX%\%PLATFORM%\%CONFIGURATION%\SharpPlink-%PLATFORM%.exe" %TO%\SharpPlink-%PLATFORM%.svnExe || echo Copying SharpPlink failed

exit 0