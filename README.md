# 3ds-VoiceChat

A voice chat plugin for Nintendo 3DS. It was built using CTRPF.
It runs as a Luma3DS plugin: PTT (Push-to-Talk) sends mic audio over TCP, and received audio is played back via CSND.

## Completion Status

**Incomplete**

```
[======>...] 70%
```

- Connection may be unstable on real hardware and Citra.
- For same-PC (e.g. two Citra instances), connection may remain at "Waiting..." depending on the environment.

## Features

- **PTT send**: While holding R, mic input is sent over TCP
- **Receive & play**: Received PCM is played with libncsnd (CSND)
- **Host / Connect**: TCP client-server model. One instance hosts, the other connects.
- **IP:Port**: Set the address for hosting (bind) or connecting (server destination).

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

   This produces `3ds-VoiceChat.3gx`.

## Usage

1. Put the built `3ds-VoiceChat.3gx` in the plugin folder on the 3DS SD card (e.g. `luma/plugins/<title ID>/`).
2. Set IP:Port (e.g. `127.0.0.1:4660` for same-PC, or your LAN IP for network).
3. One instance: select **Host** (binds and waits for connection).
4. Other instance: select **Connect** (connects to the host).
5. When connected, hold R and speak to send; the other side will hear it.

## Troubleshooting

- **CTRPF menu does not open after Init** — Try disabling the connection before opening the menu, or open the menu before enabling Host/Connect.

- **Same-PC connection (e.g. two Citra instances)**  
  - Host: Set IP:Port to `127.0.0.1:4660` (or `0.0.0.0:4660`), then select Host.
  - Connect: Set IP:Port to `127.0.0.1:4660`, then select Connect.
  - If 127.0.0.1 does not work, try your PC's LAN IP (e.g. 192.168.1.x from `ipconfig`).

## Contributing

Issues and pull requests are welcome.

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

- **Patches (soc hooks)** — [tekito_256](https://github.com/tekito-256) (tekito_256)  
  Thanks for the implementation of `Patches.cpp` (socket service hooks for games that also use soc).

## License

This project is licensed under the same license as [CTRPluginFramework](https://github.com/Nanquitas/CTRPluginFramework).  
For the libraries and templates listed above, see their respective repositories for license information.
