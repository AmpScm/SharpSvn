@echo off
setlocal enableextensions 
xcopy /y "%VSAPPIDDIR%\PublicAssemblies\CppCodeProvider.dll" . > nul:
resgen.exe %* || exit /B 1
REM del CppCodeProvider.dll
