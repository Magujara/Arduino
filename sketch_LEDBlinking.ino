
/*
  ARDUINO NANO
  BUTTON + LED

  LED:
  Positive (+) = D3
  Negative (-) = GND

  BUTTON MODULE:
  IN  = D2
  VCC = 5V
  GND = GND
*/

// =================================
// PIN DEFINITIONS
// =================================

const int ledPin = 3;
const int buttonPin = 2;

// =================================
// VARIABLES
// =================================

// 0 = Steady
// 1 = Slow blink
// 2 = Medium blink
// 3 = Fast blink

int mode = 0;

int buttonState = HIGH;
int lastButtonState = HIGH;

unsigned long lastDebounceTime = 0;
unsigned long lastBlinkTime = 0;

const unsigned long debounceDelay = 50;

// Blinking intervals in milliseconds
const unsigned long slowInterval = 1000;
const unsigned long mediumInterval = 500;
const unsigned long fastInterval = 150;

bool ledState = HIGH;


// =================================
// SETUP
// =================================

void setup() {

  pinMode(ledPin, OUTPUT);

  // Button module uses digital output
  pinMode(buttonPin, INPUT);

  // Initial state: LED ON
  digitalWrite(ledPin, HIGH);

}


// =================================
// MAIN LOOP
// =================================

void loop() {

  // Read button
  int reading = digitalRead(buttonPin);

  // Debounce button
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {

    if (reading != buttonState) {

      buttonState = reading;

      // Button pressed (assuming active LOW)
      if (buttonState == LOW) {

        mode++;

        // Return to steady lighting after fourth press
        if (mode > 3) {
          mode = 0;
        }

        lastBlinkTime = millis();
        ledState = HIGH;

      }
    }
  }

  lastButtonState = reading;


  // =================================
  // LED MODES
  // =================================

  // MODE 0: STEADY LIGHT
  if (mode == 0) {

    digitalWrite(ledPin, HIGH);

  }


  // MODE 1: SLOW BLINK
  else if (mode == 1) {

    blinkLED(slowInterval);

  }


  // MODE 2: MEDIUM BLINK
  else if (mode == 2) {

    blinkLED(mediumInterval);

  }


  // MODE 3: FAST BLINK
  else if (mode == 3) {

    blinkLED(fastInterval);

  }

}


// =================================
// BLINK FUNCTION
// =================================

void blinkLED(unsigned long interval) {

  if (millis() - lastBlinkTime >= interval) {

    lastBlinkTime = millis();

    ledState = !ledState;

    digitalWrite(ledPin, ledState);

  }

}
