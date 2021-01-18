//Receiver code
 #include <SoftwareSerial.h>
 //SoftwareSerial link(19, 18); // Rx, Tx
  
  byte greenLED = 12;
  char cString[20];
  byte chPos = 0;
  byte ch = 0;
  char dataStr[6];
  ////////////////////////////////////////////
  void setup() 
  {
    // Open serial communications and wait for port to open:
    Serial.begin(115200);
    Serial1.begin(115200);
    while (!Serial) {
    Serial.println("waiting"); // wait for serial port to connect. Needed for Native USB only
    }
   // link.begin(9600); //setup software serial
    //Serial.begin(9600);    //setup serial monitor
    Serial.println("Initializing...");
    //pinMode(greenLED, OUTPUT);
  }
  ////////////////////////////////////////////
  void loop()  
  {  
 //  Serial.println("Start");
  //if (Serial1.available())
    //  Serial.write(Serial1.read());
  //if (Serial.available())
    //  Serial1.write(Serial.read());  
   
   
   while(Serial1.available())
   {
    //read incoming char by char:
     ch = Serial1.read();
     cString[chPos] = ch;
     chPos++;     
     
   //digitalWrite(greenLED, HIGH); //flash led to show data is arriving
   //Serial.println("Link is available.....");
   delay(10);
   //digitalWrite(greenLED, LOW);
   }
   cString[chPos] = 0; //terminate cString
   chPos = 0;
   
   Serial.print(cString);
   //Serial.print(ch);
    
  }
  //////////////////////////////////////////////////////
