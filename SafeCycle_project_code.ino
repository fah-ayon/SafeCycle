#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TinyGPS++.h>


// COMMUNICATIONS
SoftwareSerial myBluetooth(5, 4); // RX, TX
SoftwareSerial gpsSerial(2, 3);   // RX, TX
TinyGPSPlus gps;

// HARDWARE PINS
const int TRIG_PIN = 6;
const int ECHO_PIN = 7;
const int BUZZER = 8;
const int VIB_MOTOR = 9;    
const int HEADLIGHT = 10;   
const int TAILLIGHT = 11;   
const int LDR_PIN = 12;     

// I2C OBJECTS
const int MPU_addr = 0x68;
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// VARIABLES
int16_t AcX, AcY, AcZ;
float totalForce;
long duration;
int distance, prevDistance = 0;
int isDark;

// FLAGS
bool isArmed = false;           
bool theftActive = false;       
bool crashActive = false;       
unsigned long flipStartTime = 0;

void setup() {
  Serial.begin(9600);      
  myBluetooth.begin(9600); 
  gpsSerial.begin(9600);   

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(VIB_MOTOR, OUTPUT);
  pinMode(HEADLIGHT, OUTPUT);
  pinMode(TAILLIGHT, OUTPUT);
  pinMode(LDR_PIN, INPUT);

  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0,0); lcd.print("SafeCycle Ult.");
  lcd.setCursor(0,1); lcd.print("Initializing...");
  
  myBluetooth.listen(); 
  
  delay(1500);
  lcd.clear();
}

void loop() {

  if (myBluetooth.isListening() && myBluetooth.available()) {
    handleCommand(myBluetooth.read());
  }

  // MPU6050
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 6, true);
  AcX = Wire.read()<<8|Wire.read();
  AcY = Wire.read()<<8|Wire.read();
  AcZ = Wire.read()<<8|Wire.read();
  totalForce = sqrt(pow(AcX,2) + pow(AcY,2) + pow(AcZ,2));

  // Ultrasonic & Speed Logic
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  int currentDistance = duration * 0.034 / 2;
  
  int approachSpeed = prevDistance - currentDistance;
  prevDistance = currentDistance;

  // LDR
  isDark = digitalRead(LDR_PIN);


  if (theftActive) {
    lcd.setCursor(0,0); lcd.print("!!! THEFT !!!   ");
    lcd.setCursor(0,1); lcd.print("DETECTED        ");
    digitalWrite(VIB_MOTOR, HIGH); 
    
    digitalWrite(HEADLIGHT, HIGH); digitalWrite(TAILLIGHT, HIGH);
    tone(BUZZER, 3000); delay(200);
    
    digitalWrite(HEADLIGHT, LOW); digitalWrite(TAILLIGHT, LOW);
    tone(BUZZER, 2000); delay(200);
    return;
  }

  if (crashActive) {
    lcd.setCursor(0,0); lcd.print("!!! CRASHED !!! ");
    lcd.setCursor(0,1); lcd.print("SOS SENT        ");
    digitalWrite(VIB_MOTOR, LOW);

    digitalWrite(HEADLIGHT, HIGH); digitalWrite(TAILLIGHT, HIGH);
    tone(BUZZER, 3000); delay(300);

    digitalWrite(HEADLIGHT, LOW); digitalWrite(TAILLIGHT, LOW);
    noTone(BUZZER); delay(300);
    return;
  }

  if (isArmed) {
    digitalWrite(HEADLIGHT, LOW);
    digitalWrite(TAILLIGHT, LOW);
    digitalWrite(VIB_MOTOR, LOW); 

    updateScreen("System LOCKED", "Sensors Active");

    if (totalForce > 19000) { 
       theftActive = true;
       myBluetooth.println("ALARM: THEFT DETECTED");
       delay(500); 
       getAndSendGPSLocation();
    }
    return; 
  }
  
  bool headLightState = LOW;
  int tailLightPWM = 0;
  bool vibState = LOW; 
  String l1 = "SafeCycle Active";
  String l2 = "Status: SAFE";

  // VISIBILITY & SMART BRAKE LIGHT
  if (isDark == HIGH) {
    headLightState = HIGH;
    tailLightPWM = 50;
    l1 = "Low Visibility";
    l2 = "Lights: ON";
  }

  if (AcY < -4000 || AcY > 4000) {
     tailLightPWM = 255;
     l2 = "BRAKING!";
  }

  // ADAPTIVE REAR WARNING
  if ((currentDistance > 0 && currentDistance < 15) || (approachSpeed > 5)) {
    if ((millis() / 100) % 2 == 0) tailLightPWM = 255; else tailLightPWM = 0;
    
    vibState = HIGH; 
    l1 = "!! WARNING !!";
    l2 = "Approaching!";
  }

  // CRASH DETECTION
  if (AcZ < 6000) { 
    if (flipStartTime == 0) flipStartTime = millis();
    long secondsDown = (millis() - flipStartTime) / 1000;
    l1 = "RIDER DOWN?";
    l2 = "Timer: " + String(secondsDown) + "s";
    
    if (secondsDown > 10) { 
      crashActive = true;
      myBluetooth.println("COLLISION DETECTED - SOS");
      delay(500);
      getAndSendGPSLocation();
    }
  } else {
    flipStartTime = 0; 
  }

  digitalWrite(HEADLIGHT, headLightState);
  analogWrite(TAILLIGHT, tailLightPWM);
  digitalWrite(VIB_MOTOR, vibState);
  
  updateScreen(l1, l2);
  delay(50);
}


void getAndSendGPSLocation() {
  gpsSerial.listen();
  
  unsigned long start = millis();
  bool gotValidFix = false;
  
  // Try to get GPS data
  while (millis() - start < 3000) {
    while (gpsSerial.available()) {
      if (gps.encode(gpsSerial.read())) {
        if (gps.location.isValid() && gps.location.age() < 2000) {
          gotValidFix = true;
          break;
        }
      }
    }
    if (gotValidFix) break;
  }

  // Send location or error message
  if (gotValidFix) {
    double lat = gps.location.lat();
    double lon = gps.location.lng();
    
    myBluetooth.print("https://www.google.com/maps?q=");
    myBluetooth.print(lat, 6);
    myBluetooth.print(",");
    myBluetooth.println(lon, 6);
  } else {
    myBluetooth.println("GPS signal lost. Move to open area.");
  }

  myBluetooth.listen(); 
}


void handleCommand(char cmd) {
  switch(cmd) {
    case 'A': 
      if (isArmed) myBluetooth.println("System Already Armed");
      else {
        isArmed = true; theftActive = false;
        tone(BUZZER, 2000, 150); delay(200); tone(BUZZER, 2000, 150);
        myBluetooth.println("System Locked");
        lcd.clear(); lcd.print("System LOCKED");
      }
      break;

    case 'D': 
      if (!isArmed) myBluetooth.println("System Already Unlocked");
      else {
        isArmed = false; theftActive = false; crashActive = false; flipStartTime = 0;
        noTone(BUZZER); digitalWrite(VIB_MOTOR, LOW);
        tone(BUZZER, 1000, 300);
        myBluetooth.println("System Unlocked");
        lcd.clear(); lcd.print("System UNLOCKED");
      }
      break;

    case 'C': 
      theftActive = false; crashActive = false; flipStartTime = 0;
      noTone(BUZZER); digitalWrite(VIB_MOTOR, LOW);
      digitalWrite(HEADLIGHT, LOW); digitalWrite(TAILLIGHT, LOW);
      myBluetooth.println("Alarm Reset");
      lcd.clear(); lcd.print("Alarm Reset");
      delay(1000);
      break;
      
    case 'S': 
      myBluetooth.println("=== SYSTEM STATUS ===");
      if(isArmed) myBluetooth.println("Status: LOCKED");
      else myBluetooth.println("Status: UNLOCKED");
      myBluetooth.print("Theft Alarm: ");
      myBluetooth.println(theftActive ? "ACTIVE" : "OFF");
      myBluetooth.print("Crash Alarm: ");
      myBluetooth.println(crashActive ? "ACTIVE" : "OFF");
      myBluetooth.println(""); // Blank line
      myBluetooth.println("Fetching Location...");
      getAndSendGPSLocation();
      break;

    case 'L': 
      myBluetooth.println("Location Request");
      getAndSendGPSLocation();
      break;
  }
}

// SCREEN UPDATE

void updateScreen(String line1, String line2) {
  static String oldL1 = "";
  static String oldL2 = "";
  if (line1 != oldL1 || line2 != oldL2) {
    lcd.clear();
    lcd.setCursor(0,0); lcd.print(line1);
    lcd.setCursor(0,1); lcd.print(line2);
    oldL1 = line1;
    oldL2 = line2;
  }
}