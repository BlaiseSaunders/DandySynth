# Dandy Synth
Dandy Synth is a open source teensy based synthesizer with hardware MIDI in and PCM out through an MCP4822, designed to be low cost and easy to build from readily available components.

It is currently in it's prototyping stage with a PCB designs coming soon.

It is created mainly as a learning exercise rather than a commercial product.


## Installation
Clone into PlatformIO and hit upload `:)`

## Wiring
- MCP4822 -> SPI0 (pin 10 CS or tie low)
- MIDI RX -> RX (pin 0)
- Set of 1k linear pots on analog pins 0 and 1 (more coming)

Ideally optocoupling the MIDI RX with a 6N138