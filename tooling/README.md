# Tooling

This folder contains tooling used for both cross compiling, developing (and running) and can also be used for deployment setup.


## Files

### [setup_deps.sh](./setup_deps.sh)
Installs development dependencies for building this repository. QtGstreamer is also built here.


### [setup_runtime.sh](./setup_deps.sh)
Installs runtime dependencies for running autoapp.


### [crosscompile.Dockerfile](./crosscompile.Dockerfile)
This contains a Docker container that can cross compile to the Raspbian target. Adjust the version as necessary.


### [devcontainer.json](../.devcontainer/devcontainer.json)
The devcontainer image is built using the [Dockerfile](../.devcontainer/Dockerfile) that just runs the dependencies and runtime installations. This should also facilitate piping audio (via Pulse) and video (via X) out to the host Windows machine.


### [crosscompile.ps1](./crosscompile.ps1)
This runs the cross compile in Docker and SCP's files over to a host. Eg `./crosscompile.ps1 user@host:folder_in_home`


## Development (Windows)
Devcontainer should facilitate most of the heavy lifting.

- [PulseAudio (+ a bit of setup)](https://github.com/microsoft/WSL/issues/5816#issuecomment-682242686)
- [XOrg Server](https://github.com/marchaesen/vcxsrv)


```bash
cmake -B./builds/build
cmake --build ./builds/build
./builds/build/assets/autoapp
```


## Cross Compiling
```powershell
# create the container
docker build -t openauto-crosscompile -f .\tooling\crosscompile.Dockerfile .
# cross compile and copy to host:folder
.\tooling\crosscompile.ps1 openauto:openauto
```
