// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>


// These are the pins used for the music maker shield
const int SHIELD_RESET = -1;      // VS1053 reset pin (unused!)
const int SHIELD_CS = 7;     // VS1053 chip select pin (output)
const int SHIELD_DCS = 6;     // VS1053 Data/command select pin (output)
const int CARDCS = 4;     // Card chip select pin
const int DREQ = 3;       // VS1053 Data request, ideally an Interrupt pin

// defines pins numbers
const int trigPin = 5;
const int echoPin = 2;
const int pressurePin = A5;

boolean insideRange = false; // whether a person is already inside the range or not
boolean handPressed = false; // whether a person is already pressed the sensor (i.e. hand) or not

int distance1 = 0;
int distance2 = 0;
int distanceDifference = 0;

int detectionRange = 30;
int pressureThreshhold = 400;

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  if (! musicPlayer.begin()) { // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1);
  }
  Serial.println(F("VS1053 found"));

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }

  // initialise the music player
  musicPlayer.setVolume(10, 10);
}



void loop() {
  // first go into the while loop
  while (true) {
    // ultrasonic sensor
    distance1 = proximityRead(trigPin, echoPin);
    distance2 = proximityRead(trigPin, echoPin);
    distanceDifference = distance2 - distance1;

    if (abs(distanceDifference) < 100) {
      if (insideRange == false) { // from outside to inside?
        Serial.println(distance1);
        Serial.println(distance2);
        if ((distance1 > 0) && (distance1 < detectionRange)) {
          Serial.println("hello!");
          insideRange = true;
          delay(500);
          break; // to play audio
        }
      } else { // still inside the range or leave the range?
        if ((distance1 > 30) || (distance1 < 0)) { // outside
          insideRange = false;
          Serial.println("leave");
          delay(500);
        }
      }
    }

    // pressure sensor
    int pressureReading = analogRead(pressurePin);

    if (handPressed == false) { // initially not pressed
      if (pressureReading >= pressureThreshhold) { // start to press
        Serial.println("feel better now"); // signal to play audio
        handPressed = true;
        delay(500);
        break; // to play audio
      }
    } else { // still pressing or pressing -> not pressing
      if (pressureReading < pressureThreshhold) { // pressing -> not pressing
        handPressed = false;
        Serial.println("leave hand"); // play nothing
        delay(500);
      }
    }
  }

  // select file and play
  if (insideRange) {
    Serial.println("playing hello.wav");
    musicPlayer.playFullFile("/track001.mp3");
  }

  if (handPressed) {
    Serial.println("playing feel better.wav");
    musicPlayer.playFullFile("/track002.mp3");
  }

}

int proximityRead(int proximityTrigPin, int proximityEchoPin) {
  long duration;
  int distance;
  digitalWrite(proximityTrigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(proximityTrigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(proximityTrigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(proximityEchoPin, HIGH);

  // Calculating the distance
  distance = duration * 0.034 / 2;

  return distance;
}
