#include <LiquidCrystal.h>
// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// some constants
const int messageLength = 8;  // length of the message
const int symbol[5] = {65, 66, 67, 68, 69};  // symbols in ascii
const char *code[] = {"11", "011", "001", "01", "00"};  // code word for each symbol
const String message = "CABEBDEA";  // constant message example

// some variables
String codeWord = "";  // using a constant String
int lcd_key     = 0;
int adc_key_in  = 0;
int laserPin = 13;  // the laser pin
int buttonPin = 2;  // the button pin
int buttonState = 0;
#define btnSELECT 'A'
#define btnLEFT   'B'
#define btnUP     'C'
#define btnRIGHT  'D'
#define btnDOWN   'E'
#define btnNONE    0
#define pushButton 1

// read the buttons
void read_LCD_buttons()
{
  adc_key_in = analogRead(0);      // read the value from the sensor
  if (adc_key_in > 1000) { //NONE
    lcd_key = btnNONE;
    return;
  }
  if (adc_key_in < 100) { //D
    lcd_key = btnRIGHT;
    return;
  }
  if (adc_key_in < 200)  { //C
    lcd_key = btnUP;
    return;
  }
  if (adc_key_in < 400)  { //E
    lcd_key = btnDOWN;
    return;
  }
  if (adc_key_in < 600)  { //B
    lcd_key = btnLEFT;
    return;
  }
  if (adc_key_in < 800)  { //A
    lcd_key = btnSELECT;  
    return;
  }

  if (adc_key_in <900) { //PUSH_BUTTON
    lcd_key = btnNONE;
    return;
  }
  
  return btnNONE;  // when all others fail, return this...
}

/*
 * cipher the message into binary string
 */
void writeCodeWordNoBtn() {
  for (int i = 0; i < message.length(); i++) {
    switch (message[i]) {
      case 'A':
        codeWord += code[0]; //A
        break;
      case 'B':
        codeWord += code[1]; //B
        break;
      case 'C':
        codeWord += code[2]; //C
        break;
      case 'D':
        codeWord += code[3]; //D
        break;
      case 'E':
        codeWord += code[4]; //E
        break;
      default:
        break;
    }
  }
}

/*
 * Adds the entered letter
 */
void writeCodeWord(){
  if (lcd_key == btnRIGHT){
    codeWord += code[3]; //D  
  } else if (lcd_key == btnUP){
    codeWord += code[2]; //C
  } else if (lcd_key == btnDOWN){
    codeWord += code[4]; //E
  } else if (lcd_key == btnLEFT){
    codeWord += code[1]; //B
  } else if (lcd_key == btnSELECT){
    codeWord += code[0]; //A
  }
}

void sendbit() {
  
}

/*
 * Send the message bit by bit using LASER
 */
void sendMessage() {
  digitalWrite(laserPin, LOW);

  //sends the message bit by bit
  for (int i = 0; i < codeWord.length(); i++) {
    
    // sends start signal
    Serial.println("Sending start signal ...");
    digitalWrite(laserPin, HIGH);
    delay(1000);  // LASER on for 1 sec

    
    if (codeWord[i] == '0') {  // make LASER LOW
      Serial.println("Sending 0 ...");
      digitalWrite(laserPin, LOW);
    } else {  // make LASER HIGH
      Serial.println("Sending 1 ...");
      digitalWrite(laserPin, HIGH);
    }
    delay(1500);
    digitalWrite(laserPin, LOW);
    delay(1000);
  }

  digitalWrite(laserPin, LOW);
    delay(1000);
  //sends finish signal
  Serial.println("Sending finish signal ...");
  digitalWrite(laserPin, HIGH);
  delay(1000);
  
}

/*
 * Reads the buttons
 */
void readInput() {
  while(true){
    read_LCD_buttons();
    writeCodeWord();
    if (lcd_key == pushButton){
      break;
    }
  }
}


void setup() {
  Serial.begin(9600);  // starts the serial
  pinMode(laserPin, OUTPUT);  // assign laser pin for output
  digitalWrite(laserPin, LOW);
  pinMode(buttonPin, INPUT);  // assign button pin for input
  /*writeCodeWordNoBtn();  // cipher message to binary string
  sendMessage();  // sends the ciphered message using LASER*/
}

void loop() {
  // put your main code here, to run repeatedly:
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LASER on:
    digitalWrite(laserPin, HIGH);
  } else {
    // turn LASER off:
    digitalWrite(laserPin, LOW);
  }
}
