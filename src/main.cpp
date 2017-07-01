#include "Arduino.h"
#include "MIDIUSB.h"
#include "Adafruit_NeoPixel.h"

#define PIXEL_COUNT 7
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, 15, NEO_GRB + NEO_KHZ800);

const int pin_sync_in = 7;
const int pin_sync_out = 8;

int sync_in_ppqn = 24;
int sync_out_ppqn = 1;
int sync_out_qn_align = 16;

int steps = sync_in_ppqn * sync_out_qn_align / sync_out_ppqn / 2;

int start_step = steps - 1;
int step = start_step - 1;
int running = 0;

enum State
{
   UsbClock,
   SyncInClock,
   InternalClock,
   Idle
} state = Idle;

long last_step = millis();
long millis_per_step = 0;
void updateTempo() {
  millis_per_step = millis() - last_step;
}

void advance() {
  last_step = millis();;
  step = (step + 1) % steps;
  if((step * sync_out_ppqn * 2 / sync_in_ppqn) & 1) {
    digitalWrite(pin_sync_out, 0);
    TXLED1;
  } else {
    digitalWrite(pin_sync_out, 1);
    TXLED0;
  }
}

void pin_sync_in_change() {
  running = 1;
  if(digitalRead(pin_sync_in)) {
    if(state == Idle || state == SyncInClock) {
      updateTempo();
      advance();
      state = SyncInClock;
    }
    RXLED0;
  } else {
    RXLED1;
  }
}

void setup() {
  pinMode(pin_sync_out, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(pin_sync_in), pin_sync_in_change, CHANGE);

  digitalWrite(pin_sync_out, 0);
  RXLED1;

  Serial.begin(9600);

  strip.begin();
  strip.setBrightness(50);
  strip.show();
}

void loop() {
  if(millis() - last_step > (millis_per_step*4)) {
    state = Idle;
  }
  if((state == Idle || state == InternalClock) && step != start_step && (millis() - last_step > millis_per_step)) {
    state = InternalClock;
    advance();
  } else if (state == InternalClock && step == start_step) {
    state = Idle;
  }

  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    if (rx.header != 0) {
      if(rx.byte1 == 248) {
        if(state == Idle || state == UsbClock) {
          state = UsbClock;
          updateTempo();
          advance();
        }
      }
    }
  } while (rx.header != 0);

  switch(state) {

  case UsbClock:
    strip.setPixelColor(0, strip.Color(255, 0, 0));
    break;
  case SyncInClock:
    strip.setPixelColor(0, strip.Color(0, 255, 0));
    break;
  case InternalClock:
    strip.setPixelColor(0, strip.Color(0, 0, 255));
    break;
  case Idle:
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    break;
  }
  for(int i = 1; i < PIXEL_COUNT; i++) {
    if(state == Idle) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    } else {
      int v = constrain((sin(6.28*((float)(step%sync_in_ppqn)/(float)sync_in_ppqn + 1.0/6.0*i))+1)/2*255 - 128, 0, 255)/2;
      strip.setPixelColor(i, strip.Color(v, v, v));
    }
  }
  strip.show();
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
