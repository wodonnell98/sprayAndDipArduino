
#include <Wire.h>

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define UP_BUTTON 18
#define DOWN_BUTTON 3
#define SELECT_BUTTON 2

#define SOLENOID_1 32             //CHECK ALL PIN DEFINITIONS WHEN CONNECTED
#define SOLENOID_2 34
#define SOLENOID_3 36


//Dip Motor

#define BEASY_stp 42
#define BEASY_dir 40
#define BEASY_MS1 48
#define BEASY_MS2 46
#define BEASY_MS3 44
#define BEASY_EN  50

//Base Rotator Motor

#define EASY_1_stp 7
#define EASY_1_dir 8

//Substrate Rotation Motor

#define EASY_2_stp 10
#define EASY_2_dir 11


/*digitalWrite(EASY_2_stp, LOW);
  digitalWrite(EASY_2_dirr, LOW);
  digitalWrite(EASY_2_MS1, LOW);
  digitalWrite(EASY_2_MS2, LOW);
  digitalWrite(EASY_2_EN, LOW);
*/
enum State
{
  LAYER_SELECT,
  MODE_SELECT,
  SPRAY_MODE,
  SPRAY_ZERO,
  SPRAY_FIVE,
  SPRAY_TEN,
  DIP
};


// Boolean Variables governing spray/dip dierction

bool leftRightDip = true;
bool leftRightSprayZ = true;
bool leftRightSprayF = true;
bool leftRightSprayT = true;

//Buttons and Interface

int upButtonState = 0;         // current state of the button
int downButtonState = 0;         // current state of the button
int selectButtonState = 0;
int lastButtonState = 0;     // previous state of the button
volatile unsigned long debounceTimer = 0;
int buttonPushCounter = 0;   // counter for the number of button presses
bool select = false;
int layers = 0;

State state = LAYER_SELECT;


void setup() {

  //Buttons

  pinMode(UP_BUTTON, INPUT);
  pinMode(DOWN_BUTTON, INPUT);
  pinMode(SELECT_BUTTON, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(UP_BUTTON), UP_BUTTON_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(DOWN_BUTTON), DOWN_BUTTON_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(SELECT_BUTTON), SELECT_BUTTON_ISR, RISING);

  //Solenoids
  
  pinMode(SOLENOID_1, OUTPUT);
  pinMode(SOLENOID_2, OUTPUT);
  pinMode(SOLENOID_3, OUTPUT);

  //Big Easy -Dip
  pinMode(BEASY_stp, OUTPUT);
  pinMode(BEASY_dir, OUTPUT);


  //Easy Driver 1 -Base Rotation
  pinMode(EASY_1_stp, OUTPUT);
  pinMode(EASY_1_dir, OUTPUT);


  //Easy Driver 2 -Substrate Rotation
  pinMode(EASY_2_stp, OUTPUT);
  pinMode(EASY_2_dir, OUTPUT);


  lcd.begin();
  Serial.begin(9600);
  Serial.println("SETUP");
}

//Main Loop

void loop()
{
  switch (state)
  {
    case LAYER_SELECT:
      layerSelect();
      break;

    case MODE_SELECT:
      modeSelect();
      break;

    case SPRAY_MODE:
      sprayMode();
      break;

    case SPRAY_ZERO:
      sprayZero();
      break;

    case SPRAY_FIVE:
      sprayFive();
      break;

    case SPRAY_TEN:
      sprayTen();
      break;

    case DIP:
      dip();
      break;
  }
}

//Code Governing Menu Display and Transitions

void layerSelect()
{
  if (buttonPushCounter < 0)
  {
    buttonPushCounter = 0;
  }

  lcd.setCursor(0, 0);
  lcd.print("Layer Number:");
  lcd.setCursor(14, 0);
  lcd.print(String(buttonPushCounter) + " ");
  lcd.setCursor(0, 1);
  lcd.print("A)Up(B)Down(C)Ok");

  if (select)
  {
    layers = buttonPushCounter;
    state = MODE_SELECT;
    buttonPushCounter = 0;
    select = false;
    lcd.clear();
    delay(100);
  }
}

void modeSelect()
{
  lcd.setCursor(2, 0);
  lcd.print("Choose Mode:");
  lcd.setCursor(0, 1);
  lcd.print("(A)Spray  (B)Dip");
  lcd.setCursor(13, 1);


  if (buttonPushCounter >= 1)
  {
    state = SPRAY_MODE;
    buttonPushCounter = 0;
    select = false;
    lcd.clear();
    delay(100);
  }
  else if (buttonPushCounter <= -1)
  {
    state = DIP;
    leftRightDip = true;
    buttonPushCounter = 0;
    select = false;
    lcd.clear();
    delay(100);
  }
}

void sprayMode()
{
  lcd.setCursor(0, 0);
  lcd.print("Spray Time /Secs:");
  lcd.setCursor(0, 1);
  lcd.print("(A)0");
  lcd.setCursor(5, 1);
  lcd.print("(B)5");
  lcd.setCursor(11, 1);
  lcd.print("(C)10");

  if (buttonPushCounter >= 1)
  {
    state = SPRAY_ZERO;
    buttonPushCounter = 0;
    select = false;
    lcd.clear();
    delay(100);
  }
  else if (buttonPushCounter <= -1)
  {
    state = SPRAY_FIVE;
    buttonPushCounter = 0;
    select = false;
    lcd.clear();
    delay(100);
  }
  else if (select)
  {
    state = SPRAY_TEN;
    buttonPushCounter = 0;
    select = false;
    lcd.clear();
    delay(100);
  }
}


//Spray and Dip Void Functions

void dip()
{
  if (layers == 0)
  {
    state = LAYER_SELECT;
    buttonPushCounter = 0;
    select = false;
    lcd.clear();
    delay(100);
    return;
  }
  diptest();
  leftRightDip = ! leftRightDip;

  layers--;
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(String(layers) + " " + "layers left");
}

void sprayZero()
{
  if (layers == 0)
  {
    state = LAYER_SELECT;
    buttonPushCounter = 0;
    select = false;
    lcd.clear();
    delay(100);
    return;
  }
  SprayTestZero();
  leftRightSprayZ = ! leftRightSprayZ;

  layers--;
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(String(layers) + " " + "layers left");
}

void sprayFive()
{
  if (layers == 0)
  {
    state = LAYER_SELECT;
    buttonPushCounter = 0;
    select = false;
    lcd.clear();
    delay(100);
    return;
  }
  SprayTestFive();
  leftRightSprayF = ! leftRightSprayF;

  layers--;
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(String(layers) + " " + "layers left");
}

void sprayTen()
{
  if (layers == 0)
  {
    state = LAYER_SELECT;
    buttonPushCounter = 0;
    select = false;
    lcd.clear();
    delay(100);
    return;
  }
  SprayTestTen();
  leftRightSprayT = ! leftRightSprayT;

  layers--;
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(String(layers) + " " + "layers left");
}

//Zero Second Wash, Spray Assembly

void SprayTestZero() {

  if (leftRightSprayZ) {

    anticlockwise90();
    delay(1000);
    for (int x = 0; x < 10 ; x++)
    {
      dipdown();
    }
    delay(2000);
    digitalWrite(SOLENOID_1, HIGH);    //Switch Solenoid ON
    delay(10000);                      //Wait 10 Second
    digitalWrite(SOLENOID_1, LOW);     //Switch Solenoid OFF
    delay(2000);
    for (int x = 0; x < 10; x++)
    {
      digup();
    }
    delay(1000);
    clockwise90();
    delay(1000);
  }
  else
  {
    clockwise90();
    delay(1000);
    for (int x = 0; x < 10; x++)
    {
      dipdown();
    }
    delay(2000);
    digitalWrite(SOLENOID_3, HIGH);    
    delay(10000);                      
    digitalWrite(SOLENOID_3, LOW);     
    delay(2000);
    for (int x = 0; x < 10; x++)
    {
      digup();
    }
    delay(1000);
    anticlockwise90();
    delay(1000);
  }
}

//Five Second Wash, Spray Assembly

void SprayTestFive() {

  if (leftRightSprayF) {

    anticlockwise90();
    delay(1000);
    for (int x = 0; x < 10; x++)
    {
      dipdown();
    }
    delay(2000);
    digitalWrite(SOLENOID_1, HIGH);    
    delay(10000);                   
    digitalWrite(SOLENOID_1, LOW);    
    delay(2000);
    for (int x = 0; x < 10; x++)
    {
      digup();
    }
    delay(1000);
    clockwise90();
    delay(1000);
    for (int x = 0; x < 10; x++)
    {
      dipdown();
    }
    delay(2000);
    digitalWrite(SOLENOID_2, HIGH);    
    delay(5000);                       // 5 Second wash
    digitalWrite(SOLENOID_2, LOW);    
    delay(2000);
    for (int x = 0; x < 10; x++)
    {
      digup();
    }
  }
  else
  {
    clockwise90();
    delay(1000);
    for (int x = 0; x < 10; x++)
    {
      dipdown();
    }
    delay(2000);
    digitalWrite(SOLENOID_3, HIGH);   
    delay(10000);                    
    digitalWrite(SOLENOID_3, LOW);     
    delay(2000);
    for (int x = 0; x < 10; x++)
    {
      digup();
    }
    delay(1000);
    anticlockwise90();
    delay(1000);
    for (int x = 0; x < 10; x++)
    {
      dipdown();
    }
    delay(2000);
    digitalWrite(SOLENOID_2, HIGH);  
    delay(5000);                      // 5 Second wash
    digitalWrite(SOLENOID_2, LOW);     
    delay(2000);
    for (int x = 0; x < 10; x++)
    {
      digup();
    }
  }
}

//Ten Second Wash, Spray Assembly

void SprayTestTen() {

  if (leftRightSprayT) {

    anticlockwise90();
    delay(1000);
    for (int x = 0; x < 10; x++)
    {
      dipdown();
    }
    delay(2000);
    digitalWrite(SOLENOID_1, HIGH);   
    delay(10000);                      
    digitalWrite(SOLENOID_1, LOW);    
    delay(2000);
    for (int x = 0; x < 10; x++)
    {
      digup();
    }
    delay(1000);
    clockwise90();
    delay(1000);
    for (int x = 0; x < 10; x++)
    {
      dipdown();
    }
    delay(2000);
    digitalWrite(SOLENOID_2, HIGH);  
    delay(10000);                    // 10 Second wash
    digitalWrite(SOLENOID_2, LOW);     
    delay(2000);
    for (int x = 0; x < 10; x++)
    {
      digup();
    }
  }
  else
  {
    clockwise90();
    delay(1000);
    for (int x = 0; x < 10; x++)
    {
      dipdown();
    }
    delay(2000);
    digitalWrite(SOLENOID_3, HIGH);    
    delay(10000);                      
    digitalWrite(SOLENOID_3, LOW);    
    delay(2000);
    for (int x = 0; x < 10; x++)
    {
      digup();
    }
    delay(1000);
    anticlockwise90();
    delay(1000);
    for (int x = 0; x < 10; x++)
    {
      dipdown();
    }
    delay(2000);
    digitalWrite(SOLENOID_2, HIGH);    
    delay(4000);                      // 10 Second wash
    digitalWrite(SOLENOID_2, LOW);  
    delay(2000);
    for (int x = 0; x < 10; x++)
    {
      digup();
    }
  }
}

//Dip Testing

void diptest() {

  if (leftRightDip)
  {
    anticlockwise90();
    delay(1000);
    for (int x = 0; x < 32; x++)                
    {
      Serial.println("acwdown* " + String(x));
      Serial.flush();
      dipdown();
    }
    Serial.println("DONE");
    delay(900000);        //900000
    /*for (int x = 0; x < 3; x++)
      {
      Serial.println("sub* " + String(x));
      subRotation();
      }*/
    for (int x = 0; x < 35; x++)
    {
      Serial.println("upSlow* " + String(x));
      digupSlow();
    }
    for (int x = 0; x < 26; x++)
    {
      Serial.println("up* " + String(x));
      digup();
    }
    delay(1000);
    clockwise90();
    delay(1000);
    for (int x = 0; x < 32; x++)
    {
      Serial.println("down " + String(x));
      dipdown();
    }
    delay(300000);    //300000
    /*for (int x = 0; x < 3; x++)
      {
      Serial.println("sub* " + String(x));
      subRotation();
      }*/
        for (int x = 0; x < 35; x++)
    {
      Serial.println("upSlow* " + String(x));
      digupSlow();
    }
    for (int x = 0; x < 26; x++)
    {
      Serial.println("up* " + String(x));
      digup();
    }
  }

  else
  {
    clockwise90();
    delay(1000);
    for (int x = 0; x < 32; x++)
    {
      Serial.println("CWdown " + String(x));
      Serial.flush();
      dipdown();
    }
    Serial.println("TEST");
    delay(900000);    //900000
    for (int x = 0; x < 35; x++)
    {
      Serial.println("upSlow* " + String(x));
      digupSlow();
    }
    for (int x = 0; x < 26; x++)
    {
      Serial.println("up* " + String(x));
      digup();
    }
    delay(1000);
    clockwise90();
    delay(1000);
    for (int x = 0; x < 32; x++)
    {
      Serial.println("CWdown " + String(x));
      Serial.flush();
      dipdown();
    }
    Serial.println("TTEST");
    delay(300000);    //300000
    for (int x = 0; x < 35; x++)
    {
      Serial.println("upSlow* " + String(x));
      digupSlow();
    }
    for (int x = 0; x < 26; x++)
    {
      Serial.println("up* " + String(x));
      digup();
    }
    anticlockwise90();
    anticlockwise90();
  }
}


//Motor Step Settings

void dipdown() {
  digitalWrite(BEASY_stp, LOW);
  digitalWrite(BEASY_dir, LOW);         //Writes the direction to the EasyDriver DIR pin. (LOW is clockwise).
  digitalWrite(BEASY_MS1, LOW);
  digitalWrite(BEASY_MS2, LOW);
  digitalWrite(BEASY_MS3, LOW);
  digitalWrite(BEASY_EN, LOW);
  for (int i = 0; i < 5000; i++)
  {
    digitalWrite(BEASY_stp, HIGH);
    delayMicroseconds(100);
    digitalWrite(BEASY_stp, LOW);
    delayMicroseconds(100);
  }
}

void digup() {
  digitalWrite(BEASY_stp, LOW);
  digitalWrite(BEASY_dir, HIGH);      //Writes the direction to the EasyDriver DIR pin. (HIGH is clockwise).
  digitalWrite(BEASY_MS1, LOW);
  digitalWrite(BEASY_MS2, LOW);
  digitalWrite(BEASY_MS3, LOW);
  digitalWrite(BEASY_EN, LOW);
  for (int i = 0; i < 5000; i++) {
    digitalWrite(BEASY_stp, HIGH);
    delayMicroseconds(100);
    digitalWrite(BEASY_stp, LOW);
    delayMicroseconds(100);

  }
}

void digupSlow() {
  digitalWrite(BEASY_stp, LOW);
  digitalWrite(BEASY_dir, HIGH);
  digitalWrite(BEASY_MS1, LOW);
  digitalWrite(BEASY_MS2, LOW);
  digitalWrite(BEASY_MS3, LOW);
  digitalWrite(BEASY_EN, LOW);
  for (int i = 0; i < 930; i++) {
    digitalWrite(BEASY_stp, HIGH);
    delayMicroseconds(537.5);
    digitalWrite(BEASY_stp, LOW);
    delayMicroseconds(537.5);

  }
}

void clockwise90() {
  digitalWrite(EASY_1_stp, LOW);
  digitalWrite(EASY_1_dir, HIGH);

  for (int i = 0; i < 420.5; i++) {
    digitalWrite(EASY_1_stp, HIGH);
    delayMicroseconds(4000);
    digitalWrite(EASY_1_stp, LOW);
    delayMicroseconds(4000);
  }
}

void anticlockwise90() {
  digitalWrite(EASY_1_stp, LOW);
  digitalWrite(EASY_1_dir, LOW); //Writes the direction to the EasyDriver DIR pin. (LOW is anticlockwise).

  for (int i = 0; i < 420.5; i++) {
    digitalWrite(EASY_1_stp, HIGH);
    delayMicroseconds(4000);
    digitalWrite(EASY_1_stp, LOW);
    delayMicroseconds(4000);
  }
}

void subRotation() {
  digitalWrite(EASY_2_stp, LOW);
  digitalWrite(EASY_2_dir, LOW);
  for (int i = 0; i < 1000; i++) {
    digitalWrite(EASY_2_stp, HIGH);
    delayMicroseconds(3000);
    digitalWrite(EASY_2_stp, LOW);
    delayMicroseconds(3000);
  }
}


//Button Interrupts

void UP_BUTTON_ISR() //button A
{
  if (millis() > debounceTimer + 250)
  {
    buttonPushCounter++;
    debounceTimer = millis();
  }
}

void DOWN_BUTTON_ISR()//button B
{
  if (millis() > debounceTimer + 250)
  {
    buttonPushCounter--;
    debounceTimer = millis();
  }
}

void SELECT_BUTTON_ISR() //button C
{
  if (millis() > debounceTimer + 250)
  {
    select = true;
    debounceTimer = millis();
  }
}
