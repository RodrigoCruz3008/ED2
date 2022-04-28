
#include "pitches.h"

int buzzerPin = PF_3;

int receiving_data;



int gameOnMelody_1[] = {
  NOTE_G4, NOTE_AS4, NOTE_D5, NOTE_DS5, NOTE_G4, NOTE_AS4, NOTE_D5, NOTE_DS5, NOTE_G4, NOTE_AS4, NOTE_D5, NOTE_DS5, NOTE_G4, NOTE_AS4, NOTE_D5, NOTE_DS5,
  NOTE_FS4, NOTE_A4, NOTE_D5, NOTE_DS5, NOTE_FS4, NOTE_A4, NOTE_D5, NOTE_DS5, NOTE_FS4, NOTE_A4, NOTE_D5, NOTE_DS5, NOTE_FS4, NOTE_A4, NOTE_D5, NOTE_DS5,
  NOTE_F4, NOTE_GS4, NOTE_D5, NOTE_DS5, NOTE_F4, NOTE_GS4, NOTE_D5, NOTE_DS5, NOTE_F4, NOTE_GS4, NOTE_D5, NOTE_DS5, NOTE_F4, NOTE_GS4, NOTE_D5, NOTE_DS5,
  NOTE_E4, NOTE_G4, NOTE_D5, NOTE_DS5, NOTE_E4, NOTE_G4, NOTE_D5, NOTE_DS5, NOTE_E4, NOTE_G4, NOTE_D5, NOTE_DS5, NOTE_E4, NOTE_G4, NOTE_D5, NOTE_DS5,
  NOTE_DS4, NOTE_G4, NOTE_C5, NOTE_D5, NOTE_DS4, NOTE_G4, NOTE_C5, NOTE_D5, NOTE_DS4, NOTE_G4, NOTE_C5, NOTE_D5, NOTE_DS4, NOTE_G4, NOTE_C5, NOTE_D5,
  NOTE_D4, NOTE_G4, NOTE_C5, NOTE_D5, NOTE_D4, NOTE_G4, NOTE_C5, NOTE_D5, NOTE_D4, NOTE_G4, NOTE_C5, NOTE_D5, NOTE_D4, NOTE_G4, NOTE_C5, NOTE_D5,
  NOTE_C3, NOTE_FS3, NOTE_A3, NOTE_C5, NOTE_FS3, NOTE_A3, NOTE_C4, NOTE_DS4, NOTE_A3, NOTE_C4, NOTE_DS4, NOTE_C4, NOTE_DS4, NOTE_FS4, NOTE_DS4, NOTE_FS4, NOTE_A4, NOTE_FS4, NOTE_A4, NOTE_C5
};
int gameOnNoteDurations_1[] = {
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};

int gameOnMelody_2 [] = {
  NOTE_A5, NOTE_D5, NOTE_AS4, NOTE_G4, NOTE_G5, NOTE_D5, NOTE_AS4, NOTE_G4, NOTE_FS5, NOTE_D5, NOTE_AS4, NOTE_G4, NOTE_G5, NOTE_D5, NOTE_AS4, NOTE_G4, 
  NOTE_G5, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_F5, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E5, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_F5, NOTE_C5, NOTE_A4, NOTE_F4, 
  NOTE_F5, NOTE_AS4, NOTE_G4, NOTE_E4, NOTE_E5, NOTE_AS4, NOTE_G4, NOTE_E4, NOTE_DS5, NOTE_AS4, NOTE_G4, NOTE_E4, NOTE_E5, NOTE_AS4, NOTE_G4, NOTE_E4, 
  NOTE_E5, NOTE_A4, NOTE_F4, NOTE_D4, NOTE_D5, NOTE_A4, NOTE_F4, NOTE_D4, NOTE_CS5, NOTE_A4, NOTE_F4, NOTE_D4, NOTE_D5, NOTE_A4, NOTE_F4, NOTE_D4, 
  NOTE_A5, NOTE_D5, NOTE_AS4, NOTE_G4, NOTE_G5, NOTE_D5, NOTE_AS4, NOTE_G4, NOTE_FS5, NOTE_D5, NOTE_AS4, NOTE_G4, NOTE_G5, NOTE_D5, NOTE_AS4, NOTE_G4, 
  NOTE_AS5, NOTE_DS5, NOTE_C5, NOTE_A4, NOTE_A5, NOTE_DS5, NOTE_C5, NOTE_A4, NOTE_GS5, NOTE_DS5, NOTE_C5, NOTE_A4, NOTE_A5, NOTE_DS5, NOTE_C5, NOTE_A4,
  NOTE_C6, NOTE_D5, NOTE_AS4, NOTE_G4, NOTE_AS5, NOTE_D5, NOTE_AS4, NOTE_G4, NOTE_A5, NOTE_D5, NOTE_AS4, NOTE_G4, NOTE_AS5, NOTE_D5, NOTE_AS4, NOTE_G4,
  NOTE_A5, NOTE_AS4, NOTE_G4, NOTE_E4, NOTE_G5, NOTE_AS4, NOTE_G4, NOTE_E4, NOTE_F5, NOTE_AS4, NOTE_G4, NOTE_E4, NOTE_E5, NOTE_AS4, NOTE_G4, NOTE_E4 
};
int gameOnNoteDurations_2 [] = {
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};

  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  Serial.println("recibido");
  
// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
  
  
}

void loop() {
  // put your main code here, to run repeatedly: 
  if (Serial2.available()) {
        receiving_data = Serial2.read();
        Serial.println(receiving_data);
        Serial.println("recibido");
      }

  if (receiving_data == 49){
    for (int thisNote = 0; thisNote < 116; thisNote++) {
      if (Serial2.available()) {
        receiving_data = Serial2.read();
        Serial.println(receiving_data);
      }

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/gameOnNoteDurations_1[thisNote];
    tone(buzzerPin, gameOnMelody_1[thisNote],noteDuration);

    int pauseBetweenNotes = noteDuration + 50;      //delay between pulse
    delay(pauseBetweenNotes);
    
    noTone(buzzerPin);                // stop the tone playing

    if (receiving_data == 48){
      thisNote = 116;
    }

    //Serial.println("te has salido");
  }
 }
  if (receiving_data == 50){
    for (int thisNote = 0; thisNote < 128; thisNote++) {

    if (Serial2.available()) {
        receiving_data = Serial2.read();
        Serial.println(receiving_data);
      }
    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/gameOnNoteDurations_2[thisNote];
    tone(buzzerPin, gameOnMelody_2[thisNote],noteDuration);

    int pauseBetweenNotes = noteDuration + 50;      //delay between pulse
    delay(pauseBetweenNotes);
    
    noTone(buzzerPin);                // stop the tone playing
    

    if (receiving_data == 48){
      thisNote = 128;
    }
  }
}
      
}
