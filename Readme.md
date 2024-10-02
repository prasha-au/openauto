
# OpenAuto


## About this variant
This is a modified version of OpenAuto for personal usage. In particular, it strips a lot of cross compatibility and additional features in favour of simplicity. It aims to run well on a Raspberry Pi 3 using Raspbian Bookworm.

The [autoapp](./autoapp/) folder contains a simple application that hands off to AndroidAuto immediately. For a more customizable experience, I recommend looking into the work done in the [OpenDash](https://github.com/openDsh/dash) project with it's associated [Slack channel](https://join.slack.com/t/opendsh/shared_invite/zt-la398uly-a6eMH5ttEQhbtE6asVKx4Q).

I have also aimed to add some integrated tooling to this repository to support things like containered development and cross compilation.



## Structure
- [aasdk_proto](./aasdk_proto/) - Protobuf definitions to communicate with Android Auto.
- [aasdk](./aasdk/) - The code to facilitate communication with the Android phone. This has been squashed in from [this repository](https://github.com/OpenDsh/aasdk).
- [openauto](./openauto/) - A library that contains (to an extent) generic code that can be used across different app implementations. It uses AASDK to communicate and handles things like rendering and user events.
- [autoapp](./autoapp/) - The lightweight application that runs OpenAuto. It adds custom services (SWC, Alsa controls) and provides some controls pre-AndroidAuto starting up.
- [tooling](./tooling/) - Scripts and tools to assist with setting up devcontainer and for cross compiling in Docker.
- [docs](./docs) - Various documentation and notes.



## UNHACK ME
```
root@docker-desktop:/usr/local/include# ls -al
total 16
drwxr-xr-x  1 root root 4096 Aug  4 13:54 .
drwxr-xr-x  1 root root 4096 Jun 27 14:20 ..
drwxr-xr-x 10 root root 4096 Aug  4 13:54 aasdk
drwxr-xr-x  2 root root 4096 Aug  4 13:54 aasdk_proto
root@docker-desktop:/usr/local/include# mv aasdk assdk_bak
root@docker-desktop:/usr/local/include# mv aasdk_proto assdk_proto_bak
root@docker-desktop:/usr/local/include# ls -al
total 24
drwxr-xr-x  1 root root 4096 Oct  2 00:58 .
drwxr-xr-x  1 root root 4096 Jun 27 14:20 ..
drwxr-xr-x 10 root root 4096 Aug  4 13:54 assdk_bak
drwxr-xr-x  2 root root 4096 Aug  4 13:54 assdk_proto_bak
```
