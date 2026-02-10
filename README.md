# 3ds-VoiceChat

A voice chat plugin for Nintendo 3DS. It was built using CTRPF.
It runs as a Luma3DS plugin: PTT (Push-to-Talk) sends mic audio over UDP, and received audio is played back via CSND.

## Features

- **PTT send**: While holding R, mic input is sent over UDP
- **Receive & play**: Received PCM is played with libncsnd (CSND)
- **Server IP**: Send destination can be set from the menu (broadcast IP when not set)

## Requirements

- [devkitPro](https://devkitpro.org/) (devkitARM)
- [CTRPluginFramework](https://github.com/Nanquitas/CTRPluginFramework) (libctrpf)
- [libctru](https://github.com/devkitPro/libctru) (3DS C/C++ library included with devkitPro)
- [libncsnd](https://github.com/PabloMK7/libncsnd) (CSND library by PabloMK7)

## Build

1. Install devkitPro and set the `DEVKITARM` environment variable.
2. Install CTRPluginFramework (libctrpf).
3. Install libncsnd:

   ```bash
   git clone https://github.com/PabloMK7/libncsnd.git
   cd libncsnd
   make install
   ```

4. Build this repository:

   ```bash
   make
   ```

   This produces `3ds-voice-chat.3gx`.

## Usage

1. Put the built `3ds-voice-chat.3gx` in the plugin folder on the 3DS SD card (e.g. `luma/plugins/<title ID>/`).
2. After starting the game, enable "Connect" in the plugin’s "Voice Chat" menu and optionally set the send destination with "Set server IP".
3. Hold R and speak to send; the other side will hear it.

## Thanks / Libraries

This project relies on the following libraries and templates. Thanks to the authors for developing and releasing them.

- **[libctru](https://github.com/devkitPro/libctru)** ([devkitPro](https://github.com/devkitPro))  
  Official library for 3DS system calls and services (socket, memory, mic, etc.).

- **[CTRPluginFramework](https://github.com/Nanquitas/CTRPluginFramework)** ([Nanquitas](https://github.com/Nanquitas))  
  Created by Nanquitas. Provides CTRPF.

- **[CTRPluginFramework - Blank Template](https://github.com/PabloMK7/CTRPluginFramework-BlankTemplate)** ([PabloMK7](https://github.com/PabloMK7))  
  Blank CTRPF plugin template. Used as the base for this project.

- **[libncsnd](https://github.com/PabloMK7/libncsnd)** ([PabloMK7](https://github.com/PabloMK7))  
  Library that provides an interface to the CSND service. Used here for audio playback when DSP is not available.

## License

This project is licensed under the same license as [CTRPluginFramework](https://github.com/Nanquitas/CTRPluginFramework).  
For the libraries and templates listed above, see their respective repositories for license information.

---

The English translation of this README was produced with the help of GAI.
