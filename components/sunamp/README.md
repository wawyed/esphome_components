# A component that reads Sunamp serial values from the uart.

A configured uart is required.

Configure a list of sensors.  The `sunamp_id` is required, the rest is the standard sensor config.
Any named field in the Sunamp serial line can be used as a `sunamp_id` (for example `temp1`, `temp2`,
`temp3`, `tempAvg`, plus any `name:value` field the controller sends after the temperatures).
If a `sunamp_id` is not present in the stream the sensor is skipped (a warning is logged once).

Example:
```yaml
sensor:
  - platform: sunamp
    uart_id: my_uart   # optional
    sensors:
      - sunamp_id: temp1
        name: Temperature 1
      - sunamp_id: temp2
        name: Temperature 2
      - sunamp_id: temp3
        name: Temperature 3
      - sunamp_id: tempAvg
        name: Temperature Average
```

## Raw line (optional, for finding out what the controller sends)

```yaml
sensor:
  - platform: sunamp
    raw:
      name: Sunamp raw
    sensors:
      - sunamp_id: temp1
        name: Temperature 1
```

`raw` publishes every received line as a diagnostic text sensor, and the same line is written to the
log at DEBUG level. Home Assistant limits a state to 255 characters. The sensor changes whenever a
temperature changes, so it adds to the recorder database: remove `raw` (or exclude it from the
recorder) once the field names are known.
