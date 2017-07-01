Operatorer
==========

[Demo on YouTube](https://youtu.be/UB1Q2wsLjR8)

Operatorer is a clock divider designed to help synchronize Pocket Operators to 
external devices that aren't natively compatible. It extends the built in 
synchronization to accept a 24ppqn signal from a synth or MIDI Clock over USB,
and make sure the Pocket Operator's pattern gets reset whenever the clock stops.

[![Operatorer](http://i.imgur.com/qItXMh2.jpg)](http://imgur.com/qItXMh2)

The Pocket Operator expects a 2ppqn (pulses per quarter note) sync signal which
is not all devices support (like my Arturia Keystep). Operatorer will act as a
converter from 24ppqn which is used by many modern devices and is related to DIN
sync and MIDI Clock signals. Additionally, an external device has no way to
reset the Pocker Operator's pattern position natively so Operatorer will track
it's position and continue sending clock pulses until the Pocket Operator
reaches the beginning of it's pattern.

Operatorer is based on a Arduino "Pro Micro" module which contains an ATmega32U4
MCU with native USB support so it can function as a standard USB MIDI
device.

Just for fun, it supports a ring of 7 addressable RGB LEDs (NeoPixel Jewel) to
display the clock and the current clock source. This is certainly an optional
part of the project but has been helpful for debugging and looks nice.
