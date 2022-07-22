# FreeDeck Configurator companion app (deprecated)

# THE FEATURES OF THIS COMPANION HAVE BEEN INTEGRATED INTO THE FREEDECK APP [DOWNLOAD](https://github.com/FreeYourStream/freedeck-configurator/releases)

This little app just runs in your tray (the thing next to your clock) and adds some features to the configurator.
For now it is used to provide the auto page switching functionality. (When you focus a window, the matching page on the FreeDeck will automatically be opened).

## Download

You can download the newest version right [here](https://github.com/FreeYourStream/freedeck-configurator-companion/releases) at the top.

## Compatibility

- Windows ✅
- Linux ✅
- MacOS ❌ (please help)

## Development

### compile

```bash
rustup override set 1.60
rustup target add x86_64-pc-windows-gnu
cargo build --target=x86_64-pc-windows-gnu --release
cargo build --release
```

### development help needed

As you can see, we currently don't have a mac version. If you know mac and rust, come and help us :)
