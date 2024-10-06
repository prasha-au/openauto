
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)]
  [String]$SshTarget
)

$deployFolder = "buildcross"

$buildFolder = "/openauto/builds/cross"
$compileCmd=@'
set -e
cd /openauto
if [ ! -d {0} ]; then
  cmake -B{0} -DCMAKE_BUILD_TYPE=Release
fi
cmake --build {0} -j
rm -Rf {1}
mkdir {1}
cp -Rf {0}/assets/* {1}/
'@ -f ($buildFolder, $deployFolder)


docker run -l openauto-crosscompile -it --rm --mount type=volume,source=openauto-builds,target=/openauto/builds -v ${PWD}:/openauto openauto-crosscompile /bin/bash -c $compilecmd

if (!(Test-Path $deployFolder)) {
  Write-Error "Build failed."
  exit
}

scp -r ${deployFolder}/* ${SshTarget}
