# auto-toggle-vpn

[中文](./README.md) | **English**

Automatically toggle VPN according to Ethernet and Wi-Fi, only supported on macOS.

When connected to Ethernet or specified Wi-Fi networks, the VPN will be turned off. When connected to other Wi-Fi networks, the VPN will be turned on.

## Configuration

Run `auto-toggle-vpn -g` to generate an example configuration file at `~/.local/auto-toggle-vpn/config.json.example`, and open it to make the necessary modifications as instructed.

```json
{
    "vpnname": "Enter your vpn_name",
    "ssidlist": [
        "SSID_1",
        "SSID_2"
    ]
}
```

Find your VPN name under "System Preferences" - "VPN" and enter it for `"vpnname"`. You can also get the VPN name using `networksetup -listallnetworkservices`.

Then, add the Wi-Fi names to be considered in the `"ssidlist"` array. After filling in the details, rename the file to "config.json".

## Installation

### Homebrew

```zsh
brew tap moesnow/tools
brew install auto-toggle-vpn
```

## Usage

### Enable automatic VPN toggling

```zsh
brew services start auto-toggle-vpn
```

### Disable automatic VPN toggling

```zsh
brew services stop auto-toggle-vpn
```

### Test automatic VPN toggling

```zsh
auto-toggle-vpn
```

### Optional parameters

```shell
Usage: auto-toggle-vpn [options]
Options:
  -g, --generate   : Generate config file example
  -v, --version    : Display program version
  -h, --help       : Display this help message
```

## To-Do List

- [ ] Allow customization of when VPN should be enabled or disabled.
- [ ] Add graphical user interface.

## Compilation

```zsh
brew install nlohmann-json
git clone https://github.com/moesnow/auto-toggle-vpn
cd auto-toggle-vpn
make
```