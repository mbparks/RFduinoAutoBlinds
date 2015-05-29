#include <Servo.h>
#include <RFduinoBLE.h>

#define lightSensor 2           //photodiode connects to GPIO pin D2
#define servoPin 3              //servo control wire connects to GPIO pin D3

Servo blinds;                   //instance of the servo library, servo attached to blinds control mechanism

float brightness = 0.0;         //var to store the digitized output of the photodiode
float lightThreshold = 100.0;   //tweak this variable to adjust point at which blinds open

char BluetoothData;             //placeholder for data received over BT radio
String valRead;                 //String to hold char array for further manipulation

boolean autoMode = true;        //tracks mode user wishes the system to be in
boolean blindsOpenStatus = false;  //tracks if the blinds are open or closed. false means blinds closed, true means open

int blindsOpen = 77;            // angle of servo when we want blinds open, tweak as necessary
int blindsClose = 20;           // angle of servo when we want blinds closed, tweak as necessary


////////////////////////////////////////////
//Function:   setup
//
//Initialize the servo, close the blinds,
//start serial comms, and put BT into
//discovery mode.
////////////////////////////////////////////
void setup() {
  blinds.attach(servoPin);
  blinds.write(blindsClose);

  Serial.begin(9600);
  delay(1000);
  Serial.println("Auto Blinds v0.1 initialized...");

  // configure the RFduino BLE properties
  RFduinoBLE.advertisementInterval = 675;
  RFduinoBLE.advertisementData = "AutoBlinds";
  RFduinoBLE.deviceName = "AutoBlinds";
  Serial.println("RFduino BLE Advertising interval is 500ms");
  Serial.println("RFduino BLE DeviceName: AutoBlinds");

  // start the BLE  stack
  RFduinoBLE.begin();
  Serial.println("RFduino BLE stack started...");
}




////////////////////////////////////////////
//Function:   loop
//
//Main function that repeats continuosuly.
//Checks to see if if in autoMode, if so,
//take analog reading of photodiode and adjust
//blinds according to how bright it is.
////////////////////////////////////////////
void loop() {

  if (autoMode) {
    brightness = analogRead(lightSensor);
    Serial.print("Reading: ");
    Serial.println(brightness);

    if (brightness > lightThreshold && !blindsOpenStatus) {
      rotateBlinds(blindsOpen);
      blindsOpenStatus = true;
    }
    else if (brightness <= lightThreshold && blindsOpenStatus) {
      rotateBlinds(blindsClose);
      blindsOpenStatus = false;
    }
  }


  delay(2000);
}



////////////////////////////////////////////
//Function: RFduinoBLE_onConnect
//          Prints message to terminal
//          once a BT device makes a
//          connection.
////////////////////////////////////////////
void RFduinoBLE_onConnect() {
  Serial.println("Android connected.");
}




////////////////////////////////////////////
//Function: RFduinoBLE_onReceive
//Input: char* character string sent by
//       smartphone app.
//       integer representing lenght of
//       string received.
//
//Function is called anytime RFduino receives
//new data over BT radio.
////////////////////////////////////////////
void RFduinoBLE_onReceive(char *data, int len)
{
  // If the first byte is 0x01 / on / true
  Serial.println("Received data over BLE");
  switch (data[0]) {
    case 'A':
      autoMode = true;
      Serial.println("Auto Mode ON");
      delay(100);
      break;

    case 'B':
      autoMode = false;
      Serial.println("Auto Mode OFF");
      delay(100);
      break;

    case 'C':
      autoMode = false;
      Serial.println("Open Blinds");
      rotateBlinds(blindsOpen);
      blindsOpenStatus = true;
      delay(100);
      break;

    case 'D':
      autoMode = false;
      Serial.println("Close Blinds");
      rotateBlinds(blindsClose);
      blindsOpenStatus = false;
      delay(100);
      break;

    default:
      break;
  }
}


////////////////////////////////////////////
//Function: rotateBlinds
//Input: integer that represents angle to
//       rotate the servo to.
////////////////////////////////////////////
boolean rotateBlinds(int position) {
  blinds.write(position);
  Serial.print("Moving blinds: ");
  Serial.println(position);

  return true;
}
