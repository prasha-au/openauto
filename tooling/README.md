# Tooling

This folder contains tooling used for both cross compiling, developing (and running) and can also be used for deployment setup.


## Files

### [setup_deps.sh](./setup_deps.sh)
Installs development dependencies for building this repository. QtGstreamer is also built here.


### [setup_runtime.sh](./setup_deps.sh)
Installs runtime dependencies for running autoapp.


### [crosscompile.Dockerfile](./crosscompile.Dockerfile)
This builds a Docker image that can be used to cross compile for the Raspbian target.

There is a lot of dynamic linking and I was running into issues when using prebuilt images like [balenalib/raspberrypi3](https://hub.docker.com/r/balenalib/raspberrypi3). Given that, this approach uses `qemu-debootstrap` to create an armhf root with Raspbian repositories. We create a base off that and run the same scripts as we do for development.


### [devcontainer.json](../.devcontainer/devcontainer.json)
The devcontainer image is built using the [Dockerfile](../.devcontainer/Dockerfile) that just runs the dependencies and runtime installations. This should also facilitate piping audio (via Pulse) and video (via X) out to the host Windows machine.



## Development (Windows)
Devcontainer should facilitate most of the heavy lifting.

- [PulseAudio (+ a bit of setup)](https://github.com/microsoft/WSL/issues/5816#issuecomment-682242686)
- [XOrg Server](https://github.com/marchaesen/vcxsrv)


