/*
  Listfiles
 
 This example shows how print out the files in a 
 directory on a SD card 
 	
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4

 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 2 Feb 2014
 by Scott Fitzgerald
 
 This example code is in the public domain.

 */
#include <SPI.h>
#include <SD.h>

File myFile;

int selection;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  SPI.setModule(0);


  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(PA_3, OUTPUT);

  if (!SD.begin(PA_3)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  myFile = SD.open("/");

  printDirectory(myFile, 0);

  Serial.println("done!");

  displayMenu();
}

void loop()
{
  
  if (Serial.available() > 0) {
    selection = Serial.read();
  }
  readFile(selection);
}

void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

void readFile (int option) {
  if (option == 49) {
    // re-open the file for reading:
    myFile = SD.open("mario.txt");
    if (myFile) {
      Serial.println("mario.txt:");
  
      // read from the file until there's nothing else in it:
      while (myFile.available()) {
        Serial.write(myFile.read());
      }
      // close the file:
      myFile.close();
      displayMenu();
    }
  } 
    
  if (option == 50) {
    // re-open the file for reading:
    myFile = SD.open("pikachu.txt");
    if (myFile) {
      Serial.println("pikachu.txt:");
  
      // read from the file until there's nothing else in it:
      while (myFile.available()) {
        Serial.write(myFile.read());
      }
    }
      // close the file:
      myFile.close();
      displayMenu();
  } 
    
    if (option == 51) {
      // re-open the file for reading:
      myFile = SD.open("sonic.txt");
      if (myFile) {
        Serial.println("sonic.txt:");
        // read from the file until there's nothing else in it:
        while (myFile.available()) {
          Serial.write(myFile.read());
        }
      }
      // close the file:
      myFile.close();
      displayMenu();
    }
}
void displayMenu () {
  Serial.println("Seleccione una imagen para desplegar:");
  Serial.println("1. mario.txt");
  Serial.println("2. pikachu.txt");
  Serial.println("3. sonic.txt");
}
