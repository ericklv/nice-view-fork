# nice-view-fork

A nice!view status screen for split ZMK keyboards, inspired by the arc style (style 2) of the
[beekeeb Toucan2](https://github.com/beekeeb/zmk-keyboard-toucan2), which is itself a fork of
[nice-view-gem](https://github.com/M165437/nice-view-gem).

The Toucan2 draws on a 144x168 panel. This fork adapts its layout to the 160x68 nice!view used
in portrait (68x160): one battery arc per half, a 16-column WPM chart instead of 30, the layer
name in QuinqueFive 12 instead of 18, and the screen drawn on three rotated 68x68 canvases.

| Central (L) | Peripheral (R) |
|---|---|
| ![Central](docs/central.gif) | ![Peripheral](docs/peripheral.gif) |

**Central (L)**: battery arc, WPM chart, layer name, output (`USB`/`BLE`/`NULL`) and
5 Bluetooth profiles. Layer names up to 4 characters use the large font, longer ones the small
font, truncated to 5.

**Peripheral (R)**: battery arc and connection to the central (`BLE`/`NULL`). Rows 60 to 134 are
left free for a mascot.

The number and dots inside each arc are that half's battery: 10 dots, one per 10 %, filled dots
are the charge left.

WPM is only shown on the central, as the dot chart: one column per second, 6 rows for
0–100 WPM, no number. Keys from both halves are processed there, so it already
counts both. ZMK raises a WPM event only when the value changes, so the chart samples the WPM
every second itself, empties 16 s after typing stops and then stops redrawing.

Both branches render the same pixels. The GIFs were rendered by the real LVGL on the host.

## Branches

| Branch | ZMK | LVGL |
|---|---|---|
| `main` | `main` | 9 |
| `v0.3` | `v0.3` | 8 |
| `dongle-latest` | `main` | 9 |
| `dongle-v.03` | `v0.3` | 8 |

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
      url: https://github.com/ericklv/nice-view-fork
      revision: main
  self:
    path: config
```

Use `revision: v0.3` for both projects on ZMK v0.3. For a dongle, see [Dongle](#dongle).

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

## Dongle

With a dongle as the central, both halves are peripherals and use the peripheral screen: battery
arc and connection to the dongle (`BLE`/`NULL`). Layer, output, profile and WPM live on the dongle,
so the halves can't show them. The dongle has no screen and is built without `nice_view_fork`.

Use these revisions of `nice-view-fork` in `config/west.yml`:

| ZMK | `zmk` revision | `nice-view-fork` revision |
|---|---|---|
| latest | `main` | `dongle-latest` |
| v0.3 | `v0.3` | `dongle-v.03` |

Both halves:

```ini
CONFIG_ZMK_SPLIT_ROLE_CENTRAL=n
```

The left half also needs its label, as the default is `R` (for example in
`config/<keyboard>_left.conf`):

```ini
CONFIG_NICE_VIEW_WIDGET_SIDE_LABEL="L"
```

The dongle:

```ini
CONFIG_ZMK_SPLIT_BLE_CENTRAL_PERIPHERALS=2
```

When migrating, flash `settings_reset` on all three boards to clear the old pairings.
