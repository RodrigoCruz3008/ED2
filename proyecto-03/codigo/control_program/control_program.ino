#define led 2
#define RX2 16  //Pines GPIO
#define TX2 17
#define RX1 2
#define TX1 4

const int push1_J1 = 32;
const int push2_J1 = 33;
const int push3_J1 = 25;
const int push4_J1 = 26;
const int push5_J1 = 27;
const int push6_J1 = 14;

const int push1_J2 = 5;
const int push2_J2 = 18;
const int push3_J2 = 19;
const int push4_J2 = 21;
const int push5_J2 = 22;
const int push6_J2 = 23;

int antirrebote1;
int antirrebote2;
int antirrebote3;
int antirrebote4;
int antirrebote5;
int antirrebote6;
int antirrebote7;
int antirrebote8;
int antirrebote9;
int antirrebote10;
int antirrebote11;
int antirrebote12;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);

  pinMode(push1_J1, INPUT_PULLUP);
  pinMode(push2_J1, INPUT_PULLUP);
  pinMode(push3_J1, INPUT_PULLUP);
  pinMode(push4_J1, INPUT_PULLUP);
  pinMode(push5_J1, INPUT_PULLUP);
  pinMode(push6_J1, INPUT_PULLUP);
  
  pinMode(push1_J2, INPUT_PULLUP);
  pinMode(push2_J2, INPUT_PULLUP);
  pinMode(push3_J2, INPUT_PULLUP);
  pinMode(push4_J2, INPUT_PULLUP);
  pinMode(push5_J2, INPUT_PULLUP);
  pinMode(push6_J2, INPUT_PULLUP);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(push1_J1) == 0){
    Serial.println('1');
    Serial2.write('1');
  } else if (digitalRead(push2_J1) == 0){
    Serial.println('2');
    Serial2.write('2');
  } else if (digitalRead(push3_J1) == 0){
    Serial.println('3');
    Serial2.write('3');
  } else if (digitalRead(push4_J1) == 0){
    Serial.println('4');
    Serial2.write('4');
  } else if (digitalRead(push5_J1) == 0){
    Serial.println('5');
    Serial2.write('5');
  } else if (digitalRead(push6_J1) == 0){
    Serial.println('6');
    Serial2.write('6');
  }
  delay(5);
  if (digitalRead(push1_J2) == 0){
    Serial.println('7');
    Serial1.write('7');
  } else if (digitalRead(push2_J2) == 0){
    Serial.println('8');
    Serial1.write('8');
  } else if (digitalRead(push3_J2) == 0){
    Serial.println('9');
    Serial1.write('9');
  } else if (digitalRead(push4_J2) == 0){
    Serial.println('a');
    Serial1.write('a');
  } else if (digitalRead(push5_J2) == 0){
    Serial.println('b');
    Serial1.write('b');
  } else if (digitalRead(push6_J2) == 0){
    Serial.println('c');
    Serial1.write('c');
  }
  delay(5);
}
