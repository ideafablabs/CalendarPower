# 📆CalendarPower
Control power from a VCalendar 2.0 Feed using a ESP-01 relay board, USB wall wart, power cord, and a single gang plastic box (with cover).

<img height="400" src="https://raw.githubusercontent.com/ideafablabs/CalendarPower/main/pics/overview.png">

### Usage
* Read The Source
* [How To Find SSL Certificate Fingerprints](https://github.com/ideafablabs/CalendarPower/blob/main/fingerprint.md)

### ToDo

* handle maintaining the wifi connection
* handle server connection failures
* handle server fingerprint mismatches
* add network status LED (GPIO2) or NeoPixel (ws2812)
* add INTERVAL to vary the frequency for checking the calendar
* add filesystem (littlefs) and configuration file (upload-able separately from the firmware image)
