
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)]
  [String]$sshTarget
)

$assetFolder = "buildcross"
$resultingBinaryPath = "$assetFolder/autoapp"

if (Test-Path $resultingBinaryPath) {
    Remove-Item -recurse $resultingBinaryPath
}

$buildFolder = "/openauto/builds/buildcross"
$compileCmd=@'
set -e
cd /openauto
if [ ! -d {0} ]; then
  cmake -B{0}
fi
cmake --build {0} -j
rm -Rf {1}
cp -Rf {0}/assets {1}
'@ -f ($buildFolder, $assetFolder)


docker run -l openauto-crosscompile -it --rm --mount type=volume,source=openauto-builds,target=/builds -v ${PWD}:/openauto openauto-crosscompile /bin/bash -c $compilecmd

if (!(Test-Path $resultingBinaryPath)) {
  Write-Error "Build failed."
  exit
}

scp -r ${assetFolder}/* ${sshTarget}
