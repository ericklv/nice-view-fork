# nice-view-fork

A nice!view status screen for split ZMK keyboards, based on the arc style of the
[beekeeb Toucan2](https://github.com/beekeeb/zmk-keyboard-toucan2), which is itself a fork of
[nice-view-gem](https://github.com/M165437/nice-view-gem).

**Central (L)**: battery arc, WPM chart, layer name, output (`USB`/`BLE`/`NULL`) and
5 Bluetooth profiles.

**Peripheral (R)**: battery arc and connection to the central (`BLE`/`NULL`). The middle of the
screen is left free for a mascot.

WPM is only shown on the central. Keys from both halves are processed there, so it already
counts both.

## Branches

| Branch | ZMK | LVGL |
|---|---|---|
| `main` | `main` | 9 |
| `v0.3` | `v0.3` | 8 |

## Usage

`config/west.yml`:

```yaml
manifest:
  remotes:
    - name: zmkfirmware
      url-base: https://github.com/zmkfirmware
  projects:
    - name: zmk
      remote: zmkfirmware
      revision: main
      import: app/west.yml
    - name: nice-view-fork
      url: <this repository>
      revision: main
  self:
    path: config
```

Use `revision: v0.3` for both projects on ZMK v0.3.

`build.yaml`:

```yaml
include:
  - board: nice_nano_v2
    shield: <keyboard>_left nice_view_adapter nice_view_fork
  - board: nice_nano_v2
    shield: <keyboard>_right nice_view_adapter nice_view_fork
```

## Configuration

| Option | Description | Default |
|---|---|---|
| `CONFIG_NICE_VIEW_WIDGET_INVERTED` | Light pixels on a dark screen | n |

## License

MIT, see [LICENSE](LICENSE). The embedded QuinqueFive font is by GGBotNet, under the
SIL Open Font License 1.1, see [QuinqueFive_License.txt](QuinqueFive_License.txt).
