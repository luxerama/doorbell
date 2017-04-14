#define MY_DEBUG

#define MY_RADIO_NRF24

#include <SPI.h>
#include <MySensors.h>  

#define CHILD_ID 1

const int BUTTON_PIN = 2;
/*
 * The ringer pin needs to be set to either 5 or 6 as they are PWM pins needed
 * to get the solenoid to reach a good frequency
 * 
 * Pins 5 and 6: controlled by Timer 0 in fast PWM mode (cycle length = 256)
 * 
 * Setting  Divisor   Frequency
 * 0x01    1     62500
 * 0x02    8     7812.5
 * 0x03    64    976.5625   <--DEFAULT
 * 0x04    256   244.140625
 * 0x05    1024  61.03515625
 * 
 * TCCR0B = (TCCR0B & 0b11111000) | <setting>;
 */
const int RINGER_PIN = 6;

int buttonState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 5;

MyMessage ringerArmedMsg(CHILD_ID, V_VAR1);
MyMessage ringerTrippedMsg(CHILD_ID, V_VAR2);

boolean ringerArmed;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);
  // Setting up ringer pin and pwm frequency
  pinMode(RINGER_PIN, OUTPUT);
  TCCR0B = (TCCR0B & 0b11111000) | 0x05;

  ringerArmed = loadState(0);        // Read last lock status from eeprom
  setRingerState(ringerArmed, true); // Now set the last known state and send it to controller
}

void presentation()  {
  sendSketchInfo("Doorbell", "1.0");
  present(CHILD_ID, S_CUSTOM);
}

void setRingerState(bool state, bool doSend) {
  if (state) {
     Serial.println("Ringer enabled");
     ringerArmed = true;
  } else {
     Serial.println("Ringer disabled");
     ringerArmed = false;
  }
  if (doSend)
    send(ringerArmedMsg.set(state));
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        send(ringerTrippedMsg.set("1"));
        if (ringerArmed)
          ring();
        send(ringerTrippedMsg.set(false));
      }
    }
  }
  lastButtonState = reading;
}

void ring() {
  analogWrite(RINGER_PIN, 127);
  delay(100);
  digitalWrite(RINGER_PIN,LOW);
}

void receive(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
  if (message.type==V_VAR1) {
     // Change relay state
     setRingerState(message.getBool(), false); 
  
     // Write some debug info
     Serial.print("Incoming ringer status:");
     Serial.println(message.getBool());
   }
}

