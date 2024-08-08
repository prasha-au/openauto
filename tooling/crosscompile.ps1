
$sshTarget=$args[0]

$buildFolder = "buildcross"
$resultingBinaryPath = "$buildFolder/assets/autoapp"

if (Test-Path $resultingBinaryPath) {
    Remove-Item -recurse $resultingBinaryPath
}

$compileCmd=@'
set -e
cd /openauto
cmake -B{0}
cmake --build {0} -j
'@ -f $buildFolder


docker run -l openauto-crosscompile -i --rm -v ${PWD}:/openauto openauto-crosscompile /bin/bash -c $compilecmd

if (!(Test-Path $resultingBinaryPath)) {
  Write-Error "Build failed."
}

scp -r ${buildFolder}/assets/* ${sshTarget}
