
# OpenAuto


## About this variant
This is a modified version of OpenAuto for personal usage. In particular, it strips a lot of cross compatibility and additional features in favour of simplicity. It aims to run well on a Raspberry Pi 3 using Raspbian Bookworm.

The [autoapp](./autoapp/) folder contains is a simple application in this repository that hands off to AndroidAuto immediately. For a more customizable experience, I recommend looking into the work done in the [OpenDash](https://github.com/openDsh/dash) project with it's associated [Slack channel](https://join.slack.com/t/opendsh/shared_invite/zt-la398uly-a6eMH5ttEQhbtE6asVKx4Q).

I have also aimed to add some integrated tooling to this repository to support things like containered development and cross compilation.



## Structure
- [aasdk](https://github.com/openDsh/aasdk) - A library used to facilitate communication with the Android phone. It contains protobuf definitions and helpers.
- [openauto](./openauto/) - The main OpenAuto library. It contains (to an extent) generic code that can be used across different implementations. It handles things like rendering the video and playing audio.
- [autoapp](./autoapp/) - The lightweight application that runs OpenAuto. It adds custom services (SWC, Alsa controls) and provides some controls pre-AndroidAuto starting up.
- [extra_protos](./extra_protos/) - Additional protobuf definitions that are not part of the aasdk library. This is mostly just Bluetooth related things.
- [tooling](./tooling/) - Scripts and tools to assist with setting up devcontainer and for cross compiling in Docker.
- [docs](./docs) - Various documentation and notes.

