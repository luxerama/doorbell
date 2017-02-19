const int BUTTON_PIN = 2;
const int BELL_PIN = 3;

int buttonState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {
  pinMode(BUTTON_PIN, INPUT);
  pinMode(BELL_PIN, OUTPUT);
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
        for (int i=0; i <= 25; i++){
          digitalWrite(BELL_PIN, HIGH);
          delay(17); 
          digitalWrite(BELL_PIN, LOW);
          delay(17);
        }
      }
    }
  }
  lastButtonState = reading;
}

