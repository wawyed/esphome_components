# A component that reads Sunamp serial values from the uart.

A configured uart is required.

Configure a list of sensors.  The index is required, the rest is the standard sensor config.

Example:
```yaml
sensor:
  - platform: sunamp
    uart_id: my_uart   # optional
    sensors:
      - index: temp1
        name: Temperature 1
      - index: temp2
        name: Temperature 2  
      - index: temp3
        name: Temperature 3
      - index: tempAvg
        name: Temperature Average
      - index: 
```

