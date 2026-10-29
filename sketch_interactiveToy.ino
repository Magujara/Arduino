
/*
  INTERAKTIVES STOFFTIER
  Funktionen:
  1. Streicheln per Knopfdruck macht das Stofftier glücklicher.
  2. Wenn es lange nicht berührt wird, verliert es Stimmungspunkte.
  3. Bei Dunkelheit schläft das Stofftier.
  4. Bei sehr hellem Licht wird es aufgeregt.
  5. Die RGB-LED zeigt die aktuelle Stimmung an.
*/

// --------------------------------------------------
// 1. PINS UND EINSTELLUNGEN
// --------------------------------------------------

// Druckknopf zum Streicheln
const int buttonPin = 2;

// Anschlüsse der RGB-LED (PWM-Pins)
const int RledPin = 9;
const int GledPin = 10;
const int BledPin = 11;

// Photodiode bzw. Lichtmess-Schaltung
const int lightSensorPin = A0;

// Stimmungspunkte
int mood = 50;
const int maxMood = 100;
const int minMood = 0;

// Wie viele Punkte ein Streicheln bringt
const int happyPoints = 10;

// Zeit bis zum ersten Punktverlust ohne Berührung
const unsigned long unTouchInterval = 5000;

// Abstand zwischen den einzelnen Punktverlusten
const unsigned long reducedInterval = 1000;

// Schwellenwerte des Lichtsensors
// Diese Werte müssen eventuell angepasst werden.
const int darkThreshold = 350;
const int brightThreshold =  290;

// --------------------------------------------------
// 2. ZEITMESSUNG UND ZUSTÄNDE
// --------------------------------------------------

// Zeitpunkt der letzten Berührung
unsigned long touchedTimer = 0;

// Zeitpunkt des letzten Punktverlusts
unsigned long reducedTimer = 0;

// Letzter Zustand des Knopfs
int lastButtonState = HIGH;

// Speichert den aktuellen Zustand des Knopfs
int buttonState = HIGH;

// Zeitmessung für die Entprellung des Knopfs
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 40;

// Aktueller Zustand des Knopfs nach Entprellung
int stableButtonState = HIGH;

// Aktueller Zustand des Stofftiers
bool isSleeping = false;
bool isExcited = false;


// --------------------------------------------------
// 3. RGB-LED STEUERN
// --------------------------------------------------

// Diese Funktion setzt die Farbe der RGB-LED.
// Die Werte reichen von 0 (aus) bis 255 (maximal hell).
// Voraussetzung: RGB-LED mit gemeinsamer Kathode.


void setColor(int red, int green, int blue) {
  analogWrite(RledPin, 255 - red);
  analogWrite(GledPin, 255 - green);
  analogWrite(BledPin, 255 - blue);
}

void turnOffLED() {
  setColor(0, 0, 0);
}

// --------------------------------------------------
// 4. STIMMUNG ANZEIGEN
// --------------------------------------------------

// Die Farbe zeigt die Stimmung des Stofftiers an.
// Rot    = traurig
// Gelb   = neutral
// Grün   = glücklich
// Blau   = aufgeregt
// Aus    = schlafend

void showMood() {

  // Im Schlaf leuchtet die LED nicht.
  if (isSleeping) {
    turnOffLED();
    return;
  }

  // Bei sehr hellem Licht wird das Stofftier aufgeregt.
  if (isExcited) {
    setColor(0, 0, 255);
    return;
  }

  // Niedrige Stimmung: Rot
  if (mood <= 30) {
    setColor(255, 0, 0);
  }

  // Mittlere Stimmung: Gelb
  else if (mood <= 65) {
    setColor(255, 150, 0);
  }

  // Hohe Stimmung: Grün
  else {
    setColor(0, 255, 0);
  }
}


// --------------------------------------------------
// 5. SETUP: EINMALIGE INITIALISIERUNG
// --------------------------------------------------

void setup() {

  // Der Knopf wird mit internem Pull-up-Widerstand betrieben.
  // Der Knopf muss zwischen Pin 2 und GND angeschlossen sein.
  pinMode(buttonPin, INPUT_PULLUP);

  // RGB-LED-Ausgänge
  pinMode(RledPin, OUTPUT);
  pinMode(GledPin, OUTPUT);
  pinMode(BledPin, OUTPUT);

  // Anfangszustand der LED
  turnOffLED();

  // Startzeit setzen
  unsigned long currentTime = millis();
  touchedTimer = currentTime;
  reducedTimer = currentTime;

  // Anfangsstimmung anzeigen
  showMood();

  // Serielle Ausgabe zur Kontrolle der Sensorwerte
  Serial.begin(9600);
}


// --------------------------------------------------
// 6. HAUPTSCHLEIFE
// --------------------------------------------------

void loop() {

  unsigned long currentTime = millis();

  // ----------------------------------------------
  // A. LICHT MESSEN
  // ----------------------------------------------

  // Liest den analogen Wert der Lichtmess-Schaltung.
  // Annahme: Höhere Werte bedeuten mehr Helligkeit.
  int lightValue = analogRead(lightSensorPin);

  // Sensorwerte im seriellen Monitor anzeigen
  Serial.print("Lichtwert: ");
  Serial.print(lightValue);
  Serial.print(" | Stimmung: ");
  Serial.println(mood);


  // ----------------------------------------------
  // B. SCHLAFEN ODER AUFGEREGT SEIN
  // ----------------------------------------------
  // Schlafen bei Dunkelheit

// B. SCHLAFEN ODER AUFGEREGT SEIN

if (lightValue >= darkThreshold) {
  isSleeping = true;
  isExcited = false;
}
else {
  isSleeping = false;
  isExcited = (lightValue <= brightThreshold);
}

Serial.print(" | Schlafen: ");
Serial.print(isSleeping);
Serial.print(" | Aufgeregt: ");
Serial.print(isExcited);
Serial.print(" | Stimmung: ");
Serial.println(mood);

  // ----------------------------------------------
  // C. KNOPF EINLESEN UND ENTPRELLEN
  // ----------------------------------------------

  // INPUT_PULLUP bedeutet:
  // HIGH = nicht gedrückt
  // LOW  = gedrückt
  int reading = digitalRead(buttonPin);

  // Wenn sich der Messwert ändert, beginnt die
  // Entprellzeit erneut.
  if (reading != lastButtonState) {
    lastDebounceTime = currentTime;
  }

  // Erst nach einer stabilen Zeit den neuen Zustand
  // des Knopfs übernehmen.
  if (currentTime - lastDebounceTime >= debounceDelay) {

    if (reading != stableButtonState) {
      stableButtonState = reading;

      // Eine neue Berührung wird nur beim Drücken erkannt.
      if (stableButtonState == LOW) {

        // Stimmungspunkte erhöhen
        mood = mood + happyPoints;

        // Stimmung auf maximal 100 begrenzen
        if (mood > maxMood) {
          mood = maxMood;
        }

        // Zeitpunkt der letzten Berührung speichern
        touchedTimer = currentTime;

        // Timer für den Punktverlust zurücksetzen
        reducedTimer = currentTime;
      }
    }
  }

  lastButtonState = reading;


  // ----------------------------------------------
  // D. STIMMUNG BEI AUSBLEIBENDER BERÜHRUNG SENKEN
  // ----------------------------------------------

  // Erst wenn längere Zeit niemand das Stofftier berührt,
  // beginnt es, Stimmungspunkte zu verlieren.
  if (currentTime - touchedTimer >= unTouchInterval) {

    // Danach wird jede Sekunde ein Punkt abgezogen.
    if (currentTime - reducedTimer >= reducedInterval) {

      if (mood > minMood) {
        mood = mood - 1;
      }

      // Zeitpunkt des letzten Punktverlusts aktualisieren
      reducedTimer = currentTime;
    }
  }


  // ----------------------------------------------
  // E. RGB-LED AKTUALISIEREN
  // ----------------------------------------------

  // Die LED zeigt Schlaf, Aufregung oder Stimmung an.
  showMood();

}