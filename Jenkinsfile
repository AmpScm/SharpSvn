pipeline {
  agent any
  
  stages {
    stage('Build') {
      steps {
        bat """
@IF EXIST "%BUILD_CMDSETUP%" @CALL "%BUILD_CMDSETUP%"

nant -f:tools/buildbot.build "-D:CCNetNumericLabel=%BUILD_NUMBER%" "-D:CCNetProject=%JOB_NAME%" "-D:build.include=%BUILD_NANTINCLUDE%" buildbot-build
@IF ERRORLEVEL 1 EXIT /B %ERRORLEVEL%

%MSBUILD_VS2010% src/SharpSvn.sln /p:Configuration=Release /p:Platform=x86 /p:PlatformToolset=v90
@IF ERRORLEVEL 1 EXIT /B %ERRORLEVEL%
%MSBUILD_VS2010% src/SharpSvn.sln /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v90
@IF ERRORLEVEL 1 EXIT /B %ERRORLEVEL%

%MSBUILD_VS2010% v40/src/SharpSvn.sln /p:Configuration=Release /p:Platform=x86 /p:PlatformToolset=v100
@IF ERRORLEVEL 1 EXIT /B %ERRORLEVEL%
%MSBUILD_VS2010% v40/src/SharpSvn.sln /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v100
@IF ERRORLEVEL 1 EXIT /B %ERRORLEVEL%

nant -f:tools/buildbot.build "-D:CCNetNumericLabel=%BUILD_NUMBER%" "-D:CCNetProject=%JOB_NAME%" "-D:build.include=%BUILD_NANTINCLUDE%" buildbot-package
        """
      }
    }
  }

  post {
    always {
      archiveArtifacts 'release/**'
    }
  }
}
