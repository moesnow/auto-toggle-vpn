# auto-toggle-vpn

**中文** | [English](./README_EN.md)

根据以太网和 Wi-Fi 自动切换 VPN，只支持 macOS。

当连接以太网或指定列表内 Wi-Fi 时关闭 VPN，连接其他 Wi-Fi 时开启 VPN。

## 配置

通过运行 `auto-toggle-vpn -g` 生成示例配置文件 `~/.local/auto-toggle-vpn/config.json.example` ，打开并按照要求修改。

```json
{
    "vpnname": "Enter your vpn_name",
    "ssidlist": [
        "SSID_1",
        "SSID_2"
    ]
}
```
在 “设置” - “VPN” 内找到名称填入 “vpnname”，也可以通过 `networksetup -listallnetworkservices` 获取。

然后将 Wi-Fi 名称依次填入 “ssidlist” 中，完成后重命名文件名为 “config.json”。

## 安装

### Homebrew

```zsh
brew tap moesnow/tools
brew install auto-toggle-vpn
```

## 使用

### 开启自动切换VPN

```zsh
brew services start auto-toggle-vpn
```

### 关闭自动切换VPN

```zsh
brew services stop auto-toggle-vpn
```

### 测试自动切换VPN

```zsh
auto-toggle-vpn
```

### 可选参数

```shell
Usage: auto-toggle-vpn [options]
Options:
  -g, --generate   : Generate config file example
  -v, --version    : Display program version
  -h, --help       : Display this help message
```

## 待办

- [ ] 可自定义应该开启或关闭VPN
- [ ] 增加图形界面

## 编译

```zsh
brew install nlohmann-json
git clone https://github.com/moesnow/auto-toggle-vpn
cd auto-toggle-vpn
make
```
