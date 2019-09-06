// defines pins numbers
const int trigPin = 10;
const int echoPin = 11;

boolean insideRange = false; // whether a person is already inside the range or not

int distance1 = 0;
int distance2 = 0;
int distanceDifference = 0;

int detectionRange = 30;

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600);
}



void loop() {

  distance1 = proximityRead(trigPin, echoPin);
  distance2 = proximityRead(trigPin, echoPin);
  distanceDifference = distance2 - distance1;

  if (abs(distanceDifference) < 100) { // delta filtering
    if (insideRange == false) { // from outside to inside?
      if ((distance1 > 0) && (distance1 < detectionRange)) {
        Serial.println("hello!");
        insideRange = true;
        delay(500);
      }
    } else { // still inside the range or leave the range?
      if ((distance1 > 30) || (distance1 < 0)) { // outside
        insideRange = false;
        Serial.println("leave");
        delay(500);
      }
    }
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
