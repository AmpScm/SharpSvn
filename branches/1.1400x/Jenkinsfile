pipeline {
  agent any
  
  stages {
    stage('Build') {
      steps {
        bat """
@IF NOT "%CD%" == "%CD:@=%" EXIT /B 1

@IF EXIST "%BUILD_CMDSETUP%" @CALL "%BUILD_CMDSETUP%"

nant -f:tools/buildbot.build "-D:CCNetNumericLabel=%BUILD_NUMBER%" "-D:CCNetProject=%JOB_NAME%" "-D:build.include=%BUILD_NANTINCLUDE%" buildbot-build
@IF ERRORLEVEL 1 EXIT /B %ERRORLEVEL%

%MSBUILD_VS2019% src/SharpSvn.sln /p:Configuration=Release /p:Platform=x86 /p:PlatformToolset=v142
@IF ERRORLEVEL 1 EXIT /B %ERRORLEVEL%
%MSBUILD_VS2019% src/SharpSvn.sln /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v142
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
