#include <ODriveArduino.h>


#include <SoftwareSerial.h>
#include <ODriveArduino.h>

// Printing with stream operator
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }

// Serial to the ODrive
SoftwareSerial odrive_serial(9, 8); //RX (ODrive TX), TX (ODrive RX)
// Note: you must also connect GND on ODrive to GND on Arduino!

// ODrive object
ODriveArduino odrive(odrive_serial);

int speed_init = 5;
int amp_init = 360;

int time_diff;
unsigned long time_start;

void setup() {
  // ODrive uses 115200 baud
  odrive_serial.begin(115200);

  // Serial to PC
  Serial.begin(115200);
  while (!Serial) ; // wait for Arduino Serial Monitor to open

  Serial.println("ODriveArduino");
  Serial.println("Setting parameters...");

  // In this example we set the same parameters to both motors.
  // You can of course set them different if you want.
  // See the documentation or play around in odrivetool to see the available parameters
  for (int axis = 0; axis < 1; ++axis) {
    //odrive_serial << "w axis" << axis << ".controller.config.vel_limit " << 22000.0f << '\n';
    odrive_serial << "w axis" << axis << ".controller.config.vel_limit " << 2.0f << '\n'; //4.0f
    odrive_serial << "w axis" << axis << ".motor.config.current_lim " << 10.0f << '\n'; //44.0f
    // This ends up writing something like "w axis0.motor.config.current_lim 10.0\n"
  }

  Serial.println("Ready!");
  Serial.println("Send the character '0' or '1' to calibrate respective motor (you must do this before you can command movement)");
  Serial.println("Send the character 's' to exectue test move");
  Serial.println("Send the character 'b' to read bus voltage");
  Serial.println("Send the character 'p' to read motor positions in a 10s loop");

}

void loop() {

  ///////////////////////// CALIBRATION //////////////////////////

  if (Serial.available()) {
    char c = Serial.read();

    // Run calibration sequence
    if (c == '0' || c == '1') {
      int motornum = c-'0';
      int requested_state;

      requested_state = ODriveArduino::AXIS_STATE_FULL_CALIBRATION_SEQUENCE;
      Serial << "Axis" << c << ": Requesting state " << requested_state << '\n';
      odrive.run_state(motornum, requested_state, false); //originally set to true but changed it to false and added a 30 sec delay instead
      delay(30000);

      //odrive_serial << "w axis0.requested_state = AXIS_STATE_FULL_CALIBRATION_SEQUENCE\n";  
     // delay(5000); 

//      requested_state = ODriveArduino::AXIS_STATE_MOTOR_CALIBRATION;
//      Serial << "Axis" << c << ": Requesting state " << requested_state << '\n';
//      odrive.run_state(motornum, requested_state, true);
//
//      requested_state = ODriveArduino::AXIS_STATE_ENCODER_OFFSET_CALIBRATION;
//      Serial << "Axis" << c << ": Requesting state " << requested_state << '\n';
//      odrive.run_state(motornum, requested_state, true);

      requested_state = ODriveArduino::AXIS_STATE_CLOSED_LOOP_CONTROL;
      Serial << "Axis" << c << ": Requesting state " << requested_state << '\n';
      odrive.run_state(motornum, requested_state, false); // don't wait

      delay(300);

      //odrive_serial << "w axis" << 0 << ".controller.config.control_mode " << 2.0f << '\n';
      //odrive_serial << "w axis" << 0 << ".controller.vel_setpoint " << 400.0f << '\n';

      odrive_serial << "w axis" << 0 << ".controller.config.control_mode " << 1.0f << '\n';
      delay(500);
      odrive_serial << "c " << 0 << " " << 1.0f << "\n";
      //odrive_serial << "w axis0.controller.input_torque = 2\n";
      //delay(500);
      //odrive_serial << "w axis0.controller.config.vel_limit = 2\n";
      //delay(500);

      Serial.println("DONE!");
    }
    ////////////////////////////END CALIBRATION
  }   
 ///////////////// BEGIN TORQUE CONTROL CODE
   delay(500);

  time_diff = 0;

  time_start = millis();

    Serial.println(time_diff);
    Serial.println("Before");

    //SET TORQUE VALUE TO 1.0 for 1000ms

    while (time_diff < 1000){
      time_diff = millis()-time_start;
      Serial.println("HI");
      Serial.println(time_diff);
      odrive_serial << "w axis" << 0 << ".controller.config.vel_limit " << 20.0f << '\n'; //4.0f
    odrive_serial << "w axis" << 0 << ".motor.config.current_lim " << 10.0f << '\n'; //44.0f
      odrive_serial << "c " << 0 << " " << 1.0f << "\n"; //Change 1.0f to other torque value
      }
      Serial.println("Bye");

      //delay(500);

      time_diff = 0;

  time_start = millis();

    Serial.println(time_diff);
    Serial.println("Before");
//SET TORUQE VALUE to 5.0 for 100ms
    while (time_diff < 100){
      time_diff = millis()-time_start;
      Serial.println("HI");
      Serial.println(time_diff);
      odrive_serial << "w axis" << 0 << ".controller.config.vel_limit " << 80.0f << '\n'; //4.0f
    odrive_serial << "w axis" << 0 << ".motor.config.current_lim " << 10.0f << '\n'; //44.0f
    odrive_serial << "c " << 0 << " " << 5.0f << "\n"; //CHANGE 5.0f to other torque value
      }
      Serial.println("Bye");
}
