#include <Wire.h>                   //To Establish I2C Communication
#include <Servo.h>                  //To Use Servo Motor
#define Slave 9                     //Slave Address
#define LASER 10                    //Laser pin declaration

Servo myServo;                      //Servo Object Creation

/*============================================================================
 * Variables Declaration
 *==========================================================================*/
 
int bcount;                       //Counter
int pos = 0;                      //Servo Position
int incrementer = 1;              //Incrementer
String passwd = "";               //Password to be read
bool flag = false;                //Password Authentication flag
bool servoFlag = true;            //Servo turn on or turn off flag
bool temp = true;         
byte Distance[3];                 //Readings of the sensors
byte x;
byte y;

/*============================================================================
 * Method Setup()
 *==========================================================================*/

void setup() {

  pinMode (LASER, OUTPUT);      //Declaring the mode of the Laser

  myServo.attach(6);            //Servo Attached to pin 6
  Wire.begin();                 //Begin I2C communication
  Serial.begin(115200);         //To establish Serial Communication
}

/*============================================================================
 * Method readData() reads data from the slave
 *==========================================================================*/

byte readData (int address) {
  byte bval;
  long entry = millis();            //millis() gives the total milliseconds elapsed since the program started
  Wire.requestFrom (address, 3);    //Data requsting from the slave
  while (Wire.available() == 0 && (millis() - entry < 100))   //Checking if the slave is ready to send the data
    Serial.println ("Waiting...");
  if (millis() - entry < 100) {   
    bval = Wire.read();             //Read data
  }
  return bval;
}

/*============================================================================
 * Method Loop()
 *==========================================================================*/

void loop() {
  
  /*
   * Input Password
   */
  
  while (Serial.available()) {
    passwd = Serial.readString();         //read password

    Wire.beginTransmission(Slave);        //Begin transmission
    for (int i = 0; i < passwd.length(); i++){
      Wire.write(passwd.charAt(i));       //send password to slave for authentication
    }
    Wire.endTransmission();               //End Transmission
    temp = !temp;
  }

  while (!temp) {         
    Wire.requestFrom(Slave, 1);
    while (Wire.available()) {            //checks if the slave is available for communication
      x = Wire.read();                    //request for authentication
    }

    if (x == 1) {                         //Password is correct
      flag = !flag;                       //Activation
      if (flag) {
        Serial.write(1);                  //Informs Processing password is correct and activate
        servoFlag = true;
      }
      else {                              //Deactivate
        Serial.write(1);                  //Informs Processing password is correct and deactivate
        digitalWrite(LASER, LOW);         //Turn off Laser
        servoFlag = false;                //Turn off Servo
      }
    }
    else                                //Password is incorrect
      Serial.write(-1);                 //Informs Processing password is incorrect
    temp = !temp;
  }

  /*==========================================================================================
                                Get distance values from the slave
    ==========================================================================================*/

  if (flag) {
    digitalWrite(LASER, HIGH);
    if (servoFlag) {
      myServo.write(pos);              //Servo Oscillation
      pos = pos + incrementer;
      Serial.write(pos);              //Writing the servo Position to Processing
      if (pos == 180 || pos == 0)
        incrementer = -incrementer;
    }
    for (int i = 0; i < 3; i++) {     //Reading distance values
      Distance[i] = readData(Slave);
      if (Distance[i] == HIGH && servoFlag == true) {
        servoFlag = false;            //Intruder Deteceted

        while (!servoFlag) {
          Serial.write(200);          //Infroms Processing that intruder is detected
          while (Serial.available()) {
            char ch = Serial.read();    //Acknowledgement character from Processing for the servo to continue oscillation
            if (ch)
              servoFlag = true;         //Servo Continues to oscillate
          }
        }
      }
    }
  }
  delay(30);                              //delay to coordinate the speed of the sensors and the microcontroller 
}
