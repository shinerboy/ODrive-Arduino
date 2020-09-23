
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
    odrive_serial << "w axis" << axis << ".controller.config.vel_limit " << 22000.0f << '\n';
    odrive_serial << "w axis" << axis << ".motor.config.current_lim " << 44.0f << '\n';
    // This ends up writing something like "w axis0.motor.config.current_lim 10.0\n"
  }

  Serial.println("Ready!");
  Serial.println("Send the character '0' or '1' to calibrate respective motor (you must do this before you can command movement)");
  Serial.println("Send the character 's' to exectue test move");
  Serial.println("Send the character 'b' to read bus voltage");
  Serial.println("Send the character 'p' to read motor positions in a 10s loop");

}

void loop() {

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
    }

    // Sinusoidal test move
    if (c == 's') {
      char test = Serial.read();
      Serial.println("Executing test move");
      float ph = 0.0f;
      while (test != 't')
      { 
        float float_speed = speed_init/1000.0;
        ph += 0.001f * speed_init;
        test = Serial.read();
        float pos_m0 = 5000.f * cos(ph) * (amp_init/360); //20000.0f = 360 degrees. 
        //float pos_m1 = 20000.0f * sin(ph);
        odrive.SetPosition(0, pos_m0);
        //odrive.SetPosition(1, pos_m1);
        delay(5);
      }
      Serial.println("Stopped");
    }

    // Read bus voltage
    if (c == 'b') {
      odrive_serial << "r vbus_voltage\n";
      Serial << "Vbus voltage: " << odrive.readFloat() << '\n';
    }

    if (c == 'u') {
      Serial.println("Set speed: ");
      //Speed matched gait at speed of 40
      String sp = Serial.readString();
      String sp2 = sp;
      Serial.println(sp);

      while(sp == sp2)
      {
        sp = Serial.readString();
        //sp = sp.toInt();
        Serial.println(sp);
      }

      sp = sp.toInt();

      Serial.print("Set speed to: ");
      Serial.print(sp);
      Serial.println(" ?(y)(n)");
      char yn = Serial.read();
      while (yn != 'y')
        {
          yn = Serial.read();
          if (yn == 'n')
          {
            sp = speed_init;
            Serial.println("Speed not changed");
            break;
            }
          if (yn == 'y')
          {
            Serial.println("Succesfully set speed!");
            speed_init = sp.toInt();
            }
          delay(5);
        }
     }

     if (c == 'a') {
      Serial.println("Set amplitude (degrees): ");
      String amp = Serial.readString();
      String amp2 = amp;
      Serial.println(amp);

      while(amp == amp2)
      {
        amp = Serial.readString();
        //sp = sp.toInt();
        Serial.println(amp);
      }

      amp = amp.toInt();

      Serial.print("Set amplitude to: ");
      Serial.print(amp);
      Serial.println(" ?(y)(n)");
      char yn = Serial.read();
      while (yn != 'y')
        {
          yn = Serial.read();
          if (yn == 'n')
          {
            amp = amp_init;
            Serial.println("Amplitude not changed");
            break;
            }
          if (yn == 'y')
          {
            Serial.println("Succesfully set amplitude!");
            amp_init = amp.toInt();
            }
          delay(5);
        }
     }
      
    
    

    // print motor positions in a 10s loop
    if (c == 'p') {
      static const unsigned long duration = 10000;
      unsigned long start = millis();
      while(millis() - start < duration) {
        for (int motor = 0; motor < 2; ++motor) {
          odrive_serial << "r axis" << motor << ".encoder.pos_estimate\n";
          Serial << odrive.readFloat() << '\t';
        }
        Serial << '\n';
      }
    }
  }
}
