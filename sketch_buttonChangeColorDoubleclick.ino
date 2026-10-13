const int buttonPin = 2;
const int RledPin = 3;
const int GledPin = 4;
const int BledPin = 5;

const unsigned long debounceTime = 30;
const unsigned long doubleClickTime = 350;

int lastRawState = LOW;
int buttonState = LOW;  // Letzter stabiler Tasterzustand

unsigned long lastChangeTime = 0;
unsigned long timeFirstClick = 0;

int clickAmount = 0;
int ledcolor = 0;
bool lampOff = false;

void setup() {
  pinMode(RledPin, OUTPUT);
  pinMode(GledPin, OUTPUT);
  pinMode(BledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
}

void loop() {
  unsigned long currentTime = millis();
  int rawState = digitalRead(buttonPin);

  // Taster entprellen: Eine Änderung muss zunächst stabil bleiben.
  if (rawState != lastRawState) {
    lastChangeTime = currentTime;
  }

  if (currentTime - lastChangeTime >= debounceTime) {
    if (rawState != buttonState) {
      buttonState = rawState;

      // Ein neuer Tastendruck: LOW -> HIGH
      if (buttonState == HIGH) {
        if (clickAmount == 0) {
          timeFirstClick = currentTime;
          clickAmount = 1;
        } else if (currentTime - timeFirstClick <= doubleClickTime) {
          lampOff = true;
          clickAmount = 0;
        }
      }
    }
  }

  lastRawState = rawState;

  // Kein zweiter Klick innerhalb des Zeitfensters: Einzelklick.
  if (clickAmount == 1 &&
      currentTime - timeFirstClick > doubleClickTime) {
    ledcolor = (ledcolor + 1) % 8;
    lampOff = false;
    clickAmount = 0;
  }

  // LED-Zustand ausgeben.
  if (lampOff) {
    digitalWrite(RledPin, HIGH);
    digitalWrite(GledPin, HIGH);
    digitalWrite(BledPin, HIGH);
  } else if (ledcolor == 0) {
    digitalWrite(RledPin, HIGH);
    digitalWrite(GledPin, HIGH);
    digitalWrite(BledPin, HIGH);
  } else if (ledcolor == 1) {
    digitalWrite(RledPin, LOW);
    digitalWrite(GledPin, HIGH);
    digitalWrite(BledPin, HIGH);
  } else if (ledcolor == 2) {
    digitalWrite(RledPin, HIGH);
    digitalWrite(GledPin, LOW);
    digitalWrite(BledPin, HIGH);
  } else if (ledcolor == 3) {
    digitalWrite(RledPin, HIGH);
    digitalWrite(GledPin, HIGH);
    digitalWrite(BledPin, LOW);
  } else if (ledcolor == 4) {
    digitalWrite(RledPin, LOW);
    digitalWrite(GledPin, LOW);
    digitalWrite(BledPin, HIGH);
  } else if (ledcolor == 5) {
    digitalWrite(RledPin, LOW);
    digitalWrite(GledPin, HIGH);
    digitalWrite(BledPin, LOW);
  } else if (ledcolor == 6) {
    digitalWrite(RledPin, HIGH);
    digitalWrite(GledPin, LOW);
    digitalWrite(BledPin, LOW);
  } else if (ledcolor == 7) {
    digitalWrite(RledPin, LOW);
    digitalWrite(GledPin, LOW);
    digitalWrite(BledPin, LOW);
  }
}