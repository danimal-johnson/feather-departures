# Feather Departures

Imagine a nice display at your bus or transit stop, counting down the times to the next departures.

Now think smaller... Still smaller...

Now about about 100x smaller. Good. That's what this is: a prototype of the real thing, running the built-in 1-inch display of a development board.

## The hardware

### The board: Adafruit ESP32-S2 Reverse TFT Feather

Reasons for choosing:

- Built-in display: one less thing to deal with
  - High contrast TFT. Wide viewing angles. Bright.
  - "Reverse" = mounted on the back-side of the board, next to user buttons
- 3 user buttons next to display (+ reset) for UI
- Built-in LiPo battery charger. Charges when board connected to USB
- Native USB C: easy to connect/program
- Built-in WiFi
- Stemma connector on I2C pins for easy wiring of peripherals

### The battery: 2AH 3.7V LiPo

Standard battery that fits the board's connector with adequate capacity for this application.

### Realtime-clock (RTC): PCF8523

Reasons for choosing

- An RTC is needed for a board that needs to keep time when powered down
- Connects over I2C using Stemma connectors (plug-and-play with this board)
- Budget option.
  - More precise clocks exist for a much higher cost.
  - Since we have WiFi, we can set it at any time.

## Dev environment

The board is programmed from the Arduino IDE because that is the easiest way to include the Adafruit libraries and board parameters.

The downside: the editor leaves *a lot* to be desired: themes, dark mode, high contrast, syntax highlighting, IntelliSense, speed, etc.

Note: because this uses the Arduino interface, C++ files have the `.ino` file extension and `.h` files are not used by default. Functions are automatically hoisted above the `setup()` and `loop()` functions by the IDE, that *nearly* always works properly.

### Getting started

You will need to create a `secrets.h` file to store your WiFi credentials that looks like this:

```h
String wifi_ssid = "mySSID";
String wifi_password = "myPassword";
int wifi_keyIndex = 0; // Only used for WEP.
```
