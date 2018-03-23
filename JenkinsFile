pipeline {
  agent any
  
  stages {
    stage('Build') {
      steps {
        bat """
@IF EXIST "%BUILD_CMDSETUP%" @CALL "%BUILD_CMDSETUP%"
@SET
nant -f:tools/buildbot.build "-D:CCNetNumericLabel=%BUILD_NUMBER%" "-D:CCNetProject=%JOB_NAME%" "-D:build.include=%BUILD_NANTINCLUDE%" buildbot-build buildbot-package
        """
      }
    }
  
  
  }
}