@echo off
setlocal enableextensions 
xcopy "%VSAPPIDDIR%\PublicAssemblies\CppCodeProvider.dll" .
resgen %* || exit /B 1
del CppCodeProvider.dll