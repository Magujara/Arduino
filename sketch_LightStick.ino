
/*
  RAINBOW LIGHT STICK
  Arduino Nano + RGB LED + Button

  RGB:
  R = D9
  G = D10
  B = D11
  V = 5V

  Button:
  IN = D2
  VCC = 5V
  GND = GND

  Common Anode:
  LOW = ON
  HIGH = OFF
*/

const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;

const int buttonPin = 2;

// Regenbogenfarben (RGB)
const int colors[][3] = {
  {255, 0, 0},      // Red
  {255, 100, 0},    // Orange
  {255, 255, 0},    // Yellow
  {0, 255, 0},      // Green
  {0, 255, 255},    // Cyan
  {0, 0, 255},      // Blue
  {130, 0, 255},    // Violet
  {255, 0, 255}     // Magenta
};

const int numberOfColors = 8;

int currentColor = 0;

bool breathingMode = false;

int lastButtonState = HIGH;
int buttonState = HIGH;

unsigned long pressStart = 0;
unsigned long lastDebounce = 0;

const unsigned long debounceDelay = 40;
const unsigned long longPressTime = 800;

bool longPressHandled = false;

unsigned long transitionStart = 0;
const unsigned long transitionDuration = 500;

int oldColor[3];
int targetColor[3];

bool transitioning = false;

// =================================
// RGB LED
// =================================

void setColor(int r, int g, int b) {

  // Common Anode: invert RGB values
  analogWrite(redPin, 255 - r);
  analogWrite(greenPin, 255 - g);
  analogWrite(bluePin, 255 - b);
}

// =================================
// NEXT COLOR
// =================================

void nextColor() {

  int previousColor = currentColor;

  currentColor++;

  if (currentColor >= numberOfColors) {
    currentColor = 0;
  }

  for (int i = 0; i < 3; i++) {
    oldColor[i] = colors[previousColor][i];
    targetColor[i] = colors[currentColor][i];
  }

  transitionStart = millis();
  transitioning = true;
}

// =================================
// COLOR TRANSITION
// =================================

void updateTransition() {

  if (!transitioning) return;

  float progress =
    (float)(millis() - transitionStart)
    / transitionDuration;

  if (progress >= 1.0) {
    progress = 1.0;
    transitioning = false;
  }

  int r = oldColor[0] +
    (targetColor[0] - oldColor[0]) * progress;

  int g = oldColor[1] +
    (targetColor[1] - oldColor[1]) * progress;

  int b = oldColor[2] +
    (targetColor[2] - oldColor[2]) * progress;

  setColor(r, g, b);
}

// =================================
// RAINBOW BREATHING EFFECT
// =================================

void updateBreathing() {

  float wave = (sin(millis() / 900.0) + 1.0) / 2.0;

  float brightness = 0.25 + wave * 0.75;

  int r = colors[currentColor][0] * brightness;
  int g = colors[currentColor][1] * brightness;
  int b = colors[currentColor][2] * brightness;

  setColor(r, g, b);
}

// =================================
// SETUP
// =================================

void setup() {

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);

  setColor(
    colors[0][0],
    colors[0][1],
    colors[0][2]
  );
}

// =================================
// MAIN LOOP
// =================================

void loop() {

  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounce = millis();
  }

  if (millis() - lastDebounce > debounceDelay) {

    if (reading != buttonState) {

      buttonState = reading;

      if (buttonState == LOW) {
        pressStart = millis();
        longPressHandled = false;
      }

      else {
        if (!longPressHandled && !breathingMode) {
          nextColor();
        }
      }
    }

    // Long press activates breathing mode
    if (buttonState == LOW &&
        !longPressHandled &&
        millis() - pressStart >= longPressTime) {

      breathingMode = !breathingMode;
      longPressHandled = true;
      transitioning = false;
    }
  }

  lastButtonState = reading;

  if (breathingMode) {
    updateBreathing();
  }
  else {
    updateTransition();
  }
}
