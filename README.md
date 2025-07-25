# 🎛️ Chase Bliss Mood MKII MIDI Controller (Arduino-Based)

This project is a custom MIDI controller for the [Chase Bliss Audio Mood MKII](https://www.chaseblissaudio.com/), built using an Arduino Nano. It sends MIDI CC messages to control functions such as WET toggle, Looper control, and modulation.


## 🔧 Features

- Toggle **WET** and **Looper** functions via footswitches  
- Toggle **Bounce** and **Smooth** functions via toggle switches
- Always control **Mix** value using a potentiometer  
- MIDI output via TRS (SoftwareSerial)  
- Lightweight and customizable firmware  


## 📦 Components

| Component             | Description                      |
|-----------------------|----------------------------------|
| Arduino Nano          | Main microcontroller             |
| 2× Momentary switches | Footswitch input                 |
| 1× Potentiometer      | Analog expression (10kΩ recommended) |
| 2× LEDs               | WET and Looper state indicators  |
| 220–330Ω resistors    | Current-limiting for LEDs, TX        |
| TRS jack  | MIDI output                      |


## 🧰 Arduino Libraries

Install the following libraries via the Arduino Library Manager:

- [`MIDI`](https://github.com/FortySevenEffects/arduino_midi_library)  
- [`JC_Button`](https://github.com/JChristensen/JC_Button)  
- [`ResponsiveAnalogRead`](https://github.com/dxinteractive/ResponsiveAnalogRead)  
- [`JLed`](https://github.com/jandelgado/jled)  


## 📌 Pin Assignments

```cpp
const int PIN_TOGGLE_BOUNCE     = 4;
const int PIN_TOGGLE_SMOOTH     = 5;
const int PIN_FOOTSWITCH_WET    = 6;
const int PIN_FOOTSWITCH_LOOPER = 7;
const int PIN_POT               = A0;
const int PIN_RX                = 2;
const int PIN_TX                = 3;
const int PIN_WET_LED           = 9;
const int PIN_LOOPER_LED        = 10;
```

## 🎚️ MIDI CC Map

| Function        | MIDI CC | Value       | Description                |
|-----------------|---------|-------------|----------------------------|
| WET Toggle      | 103     | 127 / 0     | Short press footswitch      |
| WET Hold Alt    | 105     | 127 / 0     | Long press footswitch       |
| Looper Toggle   | 102     | 127 / 0     | Short press footswitch      |
| Looper Hold     | 106     | 127 / 0     | Long press footswitch       |
| Bounce      | 66      | 0 – 127     | Toggle switch  |
| Smooth      | 78      | 0 – 127     | Toggle switch  |
| Mix      | 15      | 0 – 127     | Analog potentiometer value  |


## ⚙️ MIDI Wiring

Use TRS connector.

- TIP: N.C.
- Ring: TX → 220Ω resistor
- SLEEVE: GND


## 🪪 License

MIT License.  
Feel free to use, modify, and distribute.
