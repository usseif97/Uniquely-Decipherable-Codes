// some constants
const int maxLength = 100;  // the max length of a ciphered message
const int numSymbols = 5;
const int symbol[numSymbols] = {65, 66, 67, 68, 69};  // symbols in ascii
const char symbolLetter[numSymbols] = {'A', 'B', 'C', 'D', 'E'};  // symbols as char
const char *codeWord[] = {"11", "011", "001", "01", "00"};  // code word for each symbol
const char *codeWordRev[] = {"11", "110", "100", "10", "00"};  // code word for each symbol
const int lowest = 2;
const int highest = 3;

// variables
String ciphered;  // the ciphered message
String cipheredRev;  // the ciphered message reversed
bool commas1[maxLength];
bool commas2[maxLength];
int n = 0;  // the size of the ciphered message
int sensorLDR = A0;  // LDR sensor port
int sensorValue = 0;  // LDR sensor value
int greenLED = 9;
int redLED = 11;
int yellowLED = 10;

/*
 * Gets the ciphered message bit by bit
 */
void getInput() {
  digitalWrite(yellowLED, HIGH);
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);
  
  while (true) {
    // waiting for start signal
    Serial.println("press the button to start ...");
    while(true) {
      int read = readLDRSensor();
      if (read == 1) {
        break;
      }
    }
    Serial.print("enter the input:  ");
    delay(2000);  // delay 2 sec before getting the input, but the sender will wait 1 sec and emmits the laser for 1 sec

  
    // getting the input
    int res = readLDRSensor();
    char c;
    if (res == 1) {
      c = '1';
    } else {
      c = '0';
    }
    Serial.print(addCharToCiph(c));  // adds the input and prints it
    Serial.println();


    // checks for finish signal
    Serial.println("press the button to finish ...");
    delay(2000); // waits for 1.5 sec then read LDR for finish signal
    int finish = readLDRSensor();
    if (finish == 1) {
      return;
    }
    Serial.println();
    
  }
  
}

/*
 * Reads the sensor value and returns 0 or 1 accordingly
 */
int readLDRSensor() {
  // get the sensor value
  sensorValue = analogRead(sensorLDR);
//  Serial.print("sensor value");
//  Serial.println(sensorValue);
  // checks whether it's 0 or 1
  if (sensorValue >= 990) {  // Reads 1
    return 1;
  } else {  // Reads 0
    return 0;
  }
}

/*
   adds a character to the ciphered message
*/
char addCharToCiph(char c) {
  ciphered += c;
  n++;
  return c;
}

/*
 * Reverses the ciphered message to use in step 2
 */
void reverseCiphMes() {
  for (int i = n - 1; i >= 0; i--) {
    cipheredRev += ciphered[i];
  }
}

/*
   Step 1 in deciphering the message (left to right)
*/
void step1Decipher() {
  for (int i = 0; i < n; i++) {
    commas1[i] = false;
  }
  commas1[0] = true; // because before start
  int m = 0;
  int j = 0;
  while (true) {

    if (j == m && (j != 0 && m != 0)) {
      break;
    }

    // adds the char to the test string
    if ((j - m) < highest && j < n) {
      j++;
    } else {
      m++;
    }

    // case it can't be code word w/o testing
    if (j - m < lowest) {
      continue;
    }

    // tests if it can be code word
    for (int v = m; v < j; v++) {
      bool isFound = false;
      for (int y = 0; y < numSymbols; y++) {
        int len1 = strlen(codeWord[y]);
        int len2 = j - v;
        if (len1 != len2) {
          continue;
        }
        int b = v;
        for (b = v; b < j; b++) {
          if (ciphered[b] != codeWord[y][b - v]) {
            break;
          }
        }
        if (b == j) {
          if (!commas1[v]) {
            continue;
          }
          isFound = true;
          break;
        }
      }
      if (isFound) {
        commas1[j] = true;
      }
    }

  }

  Serial.println();
  Serial.print("Step 1:  ");
  for (int i = 0; i < n; i++) {
    if (commas1[i] && i != 0) {
      Serial.print(",");
    }
    Serial.print(ciphered[i]);
  }
  Serial.println();
}

/*
   Step 2 in deciphering the message (right to left)
*/
void step2Decipher() {
  for (int i = 0; i < n; i++) {
    commas2[i] = false;
  }
  commas2[0] = true; // because before start
  int m = 0;
  int j = 0;
  while (true) {

    if (j == m && (j != 0 && m != 0)) {
      break;
    }

    // adds the char to the test string
    if ((j - m) < highest && j < n) {
      j++;
    } else {
      m++;
    }

    // case it can't be code word w/o testing
    if (j - m < lowest) {
      continue;
    }

    // tests if it can be code word
    for (int v = m; v < j; v++) {
      bool isFound = false;
      for (int y = 0; y < numSymbols; y++) {
        int len1 = strlen(codeWordRev[y]);
        int len2 = j - v;
        if (len1 != len2) {
          continue;
        }
        int b = v;
        for (b = v; b < j; b++) {
          if (cipheredRev[b] != codeWordRev[y][b - v]) {
            break;
          }
        }
        if (b == j) {
          if (!commas2[v]) {
            continue;
          }
          isFound = true;
          break;
        }
      }
      if (isFound) {
        commas2[j] = true;
      }
    }

  }

  Serial.println();
  Serial.print("Step 2:  ");
  for (int i = n - 1; i >= 0; i--) {
    Serial.print(cipheredRev[i]);
    if (commas2[i] && i != 0) {
      Serial.print(",");
    }
  }
  Serial.println();
}

/*
   Step 3 in deciphering the message (right to left)
*/
void step3Decipher() {
  int j = 0;
  char res[2*n];
  for (int i = 0; i < n; i++) {
    if (i != 0) {
      if (commas1[i] && commas2[n - i]) {
        res[j] = ':';
        j++;
        res[j] = '\0';
      }
    }
    res[j] = ciphered[i];
    j++;
    res[j] = '\0';
    
  }

  Serial.println();
  Serial.print("Step 3:  ");
  for (int i = 0; i < j; i++) {
    Serial.print(res[i]);
  }
  Serial.println();
  Serial.println();
  Serial.print("The deciphered message: ");

  char temp[highest+1];
  int k = 0;
  for (int i = 0; i < j; i++) {
    
    // if a separator is encountered i.e. new symbol
    if (res[i] == ':' || i == (j - 1)) {
      if (i == (j - 1)) {
        temp[k] = res[i];
        k++;
        temp[k] = '\0';
      }
      // find the corresponding symbol
      int y = 0;
      bool isFound = false;
      for (y = 0; y < numSymbols; y++) {
        int len1 = strlen(codeWord[y]);
        int len2 = strlen(temp);
        if (len1 != len2) {
          continue;
        }
        int b = 0;
        for (b = 0; b < k; b++) {
          if (temp[b] != codeWord[y][b]) {
            break;
          }
        }
        if (b == k) {
          isFound = true;
          break;
        }
      }
      if (isFound) {
//        Serial.print(symbol[y]);
//        Serial.print(":");
//        Serial.print(symbolLetter[y]);
//        Serial.print("  ");
        Serial.print(symbol[y]);
        if (i != j - 1) {
          Serial.print(",");
        }
        k = 0;
      } else {
        digitalWrite(yellowLED, LOW);
        digitalWrite(greenLED, LOW);
        digitalWrite(redLED, HIGH);
        Serial.println();
        Serial.println();
        Serial.println("ERROR !!  IT CAN'T BE A CORRECT MESSAGE (NOT UNIQUELY DECIPHERABLE)");
        return;
      }
    } else {
      if (k + 1 == highest + 1) {
        digitalWrite(yellowLED, LOW);
        digitalWrite(greenLED, LOW);
        digitalWrite(redLED, HIGH);
        Serial.println();
        Serial.println();
        Serial.println("ERROR !!  IT CAN'T BE A CORRECT MESSAGE (NOT UNIQUELY DECIPHERABLE)");
        return;
      }
      temp[k] = res[i];
      k++;
      temp[k] = '\0';
    }
  }

  digitalWrite(yellowLED, LOW);
  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, LOW);
}

/*
   deciphers the ciphered message
*/
void decipherMes() {
  Serial.println();
  Serial.println("Deciphering ...");
  step1Decipher();
  step2Decipher();
  step3Decipher();
}



void setup() {
  // starts the serial
  Serial.begin(9600);
  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  // 0011101100011010011  pdf example added
  Serial.println("Getting input ...");
  getInput();

  // prints the finalized ciphered message
  Serial.println();
  Serial.println();
  Serial.print("the ciphered message: ");
  Serial.println(ciphered);
  
  reverseCiphMes();  // calls the reverse ciphered method
  decipherMes();  // calls the decipher method
}

void loop() {
  // put your main code here, to run repeatedly:

}
