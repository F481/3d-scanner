#include <Stepper.h>

bool debugMode = true;

// you have to take care about this number! Normally the Nema17 stepper motor has 200 steps for 360 degree - means 1 step = 1.8°.
// We have a gear in our setup, so that 387 steps result in a full turn (360 degree) of the platet.
const int stepsPerRevolution = 388;
// the focus wire of the SLR is connected with the relay on D2
const int focusRelay = 2;
// the shoot wire of the SLR is connected with the relay on D3
const int shootRelay = 3;

String input = "";
int numberOfPicsPerRound = 0;
volatile byte focusRelayState = LOW;
volatile byte shootRelayState = LOW;
Stepper nema17(stepsPerRevolution, 8, 9, 10, 11);

void setup()
{
  pinMode(focusRelay, OUTPUT);
  pinMode(shootRelay, OUTPUT);

  nema17.setSpeed(5);
  
  Serial.begin(9600);
  Serial.print("3D-Scanner is now ready to use.\n\n");
  Serial.print("Please enter the number of pictures that should be taken per round (360 degree): ");
}

void loop()
{
  char character;
    
  while(Serial.available()) {
    character = Serial.read();    
    input.concat(character);
    delay(2);
  }

  if (input != "") {
    //String result = handleCommand(input);
    numberOfPicsPerRound = input.toInt();
    Serial.print(numberOfPicsPerRound);
    Serial.println("\n");
    input = "";
  }

  if (numberOfPicsPerRound > 0) {
    for (int i = 0; i < numberOfPicsPerRound; i++) {
    focusImage(3000);
    takePhoto();
    
    delay(3000);

    int everyXdegree = 360 / numberOfPicsPerRound;
    int gearmapping = map(everyXdegree, 0, 360, 0, 387);
    rotatePlate(gearmapping);

    delay(1000);
  }

   char buffer[50];
   sprintf(buffer, "Complete round finished and took %d photos.", numberOfPicsPerRound);
   Serial.println(buffer);

   // clear inserted number of pics per round when we are finished with the round
   numberOfPicsPerRound = 0;
  }
}


void focusImage(int focusTime)
{
  if (debugMode) {
    char buffer[50];
    sprintf(buffer, "Called focusImage() with %d ms focus time", focusTime);
    Serial.println(buffer);
  }
  
  digitalWrite(focusRelay, HIGH);
  delay(focusTime);
  
  // take care! We don't set the focus back to LOW here, because the used SLR works better when the focus is also HIGH while firing the shoot trigger.
  // We then set back the focus to LOW in the takePhoto() method
  //digitalWrite(focusRelay, LOW);
}

void takePhoto() 
{
  if (debugMode) {
      Serial.println("Called takePhoto()");
  }
  
  digitalWrite(shootRelay, HIGH);
  delay(300);
  digitalWrite(shootRelay, LOW);
  digitalWrite(focusRelay, LOW);
}

void rotatePlate(int degree)
{
  nema17.step(degree);
}
