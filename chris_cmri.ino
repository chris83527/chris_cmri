#include <CMRI.h>

#include "ht16k33.h"
#include <Adafruit_MCP23X17.h>

CMRI cmri(0, 128, 128);
Adafruit_MCP23X17 mcp23017_1;  // 12v outputs
Adafruit_MCP23X17 mcp23017_2;  // 12v outputs
Adafruit_MCP23X17 mcp23017_3;  // inputs
Adafruit_MCP23X17 mcp23017_4;  // inputs
HT16K33 ht;                    // buttons

static int8_t lastkey = 0;
int8_t key;


void setup() {
  Serial.begin(9600, SERIAL_8N2);

  pinMode(LED_BUILTIN, OUTPUT);

  mcp23017_1.begin_I2C(0x00);
  mcp23017_2.begin_I2C(0x01);
  mcp23017_3.begin_I2C(0x04);
  mcp23017_4.begin_I2C(0x05);
  ht.begin(0x00);

  for (int i = 0; i < 16; i++) {
    mcp23017_1.pinMode(i, OUTPUT);
    mcp23017_2.pinMode(i, OUTPUT);
    mcp23017_3.pinMode(i, INPUT);
    mcp23017_4.pinMode(i, INPUT);
  }

  for (int i = 0; i < 48; i++) {
    ht.setLedNow(i);
  }

  delay(2000);

  ht.clearAll();
}

void setLed(uint8_t led) {
  if (led < 16) {
    mcp23017_1.digitalWrite(led, HIGH);
  } else if (led >= 16 && led < 32) {
    mcp23017_2.digitalWrite((led - 16), HIGH);
  } else if (led >= 32 && led < 81) {
    ht.setLedNow((uint8_t)(led - 32));
  }
}

void clearLed(uint8_t led) {
  if (led < 16) {
    mcp23017_1.digitalWrite(led, LOW);
  } else if (led >= 16 && led < 32) {
    mcp23017_2.digitalWrite((led - 16), LOW);
  } else if (led >= 32 && led < 81) {
    ht.clearLedNow((uint8_t)(led - 32));
  }
}


void loop() {

  //digitalWrite(LED_BUILTIN, 1);
  cmri.process();

  // Outputs
  for (int i = 0; i < 81; i++) {
    if (cmri.get_bit(i)) {
      setLed(i);
    } else {
      clearLed(i);
    }
  }

  // Inputs. 0 - 31 == external inputs, 32 - 72 == buttons
  for (int i = 0; i < 32; i++) {
    if (i < 16) {
      cmri.set_bit(i, mcp23017_3.digitalRead(i));
    } else if (i >= 16 && i < 32) {
      cmri.set_bit(i, mcp23017_4.digitalRead(16 - i));
    }
  }

  key = ht.readKey();
  if (key != 0) {
    if (key != lastkey) {
      for (int i = 0; i < 40; i++) {
        cmri.set_bit(i + 32, (key == (i + 1)));
      }
      lastkey = key;
    }
  }
}
