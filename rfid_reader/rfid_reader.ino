   
/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read new NUID from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to the read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the type, and the NUID if a new card has been detected. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */

#include <SPI.h>
#include <MFRC522.h>
#include <stdlib.h>
#include "Keyboard.h"


#define SS_PIN 10
#define RST_PIN 9
const int buttonPin = 4;          // input pin for pushbutton
int previousButtonState = HIGH;   // for checking the state of a pushButton
int counter = 0;                  // button push counter

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];
int buttonState;

void setup_keyboard(){
  
  // initialize control over the keyboard:
  Keyboard.begin();
}

void setup_rfid(){
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void setup_button(){
  // make the pushButton pin an input:
  pinMode(buttonPin, INPUT);
}

void setup_serial(){
  Serial.begin(9600);  
}

void setup() { 
  setup_serial();
  setup_rfid();    
  Serial.println("Iniciando...");
  setup_button();
  delay(5000);
  Serial.println("Ativou o keyboard!");
  setup_keyboard();
   // read the pushbutton:
  buttonState = digitalRead(buttonPin);
  
  
}
 
void loop() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
   
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
  }
  else Serial.println(F("Card read previously."));

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}


/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  char hex[] = "0123456789abcdef";
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
    /*Ativa se o botão não for pressionado*/
    if(buttonState != LOW) { 
        Keyboard.print(buffer[i], HEX);
    }
  }
  Serial.println(" ");
  Keyboard.write(0x10);
  Keyboard.write(0x13);
  
}

long long int strtoll(char *info, char *pos, int base){
  long long int value = 0;
  int cont = 0;
  Serial.println("Iniciou");
  char info2[10];
  
  info2[0] = *info; /*Pega primeiro caracter*/
  
  while(info2[0] != '\0'){

    Serial.print("info2:");
    Serial.println(info2[0]);
    value = value * base;
    Serial.print("value:");
    Serial.println( (char)strtol(info2[0], NULL, base));
    value = value + strtol(info2[0], NULL, base);
    cont ++;
    //memcpy(info2,'\0',sizeof(info2));
    info2[0] = *(info+cont);
  } 
  if (pos!=NULL) {
    pos = (info + cont); /*Retorna posicao que parou*/
  }
  //Serial.print("Passo");
  //Serial.println(cont);
  return value; /*Retorna valor*/
}

int getNum(char ch)
{
    int num = 0;
    if (ch >= '0' && ch <= '9') {
        num = ch - 0x30;
    }
    else {
        switch (ch) {
        case 'A':
        case 'a':
            num = 10;
            break;
        case 'B':
        case 'b':
            num = 11;
            break;
        case 'C':
        case 'c':
            num = 12;
            break;
        case 'D':
        case 'd':
            num = 13;
            break;
        case 'E':
        case 'e':
            num = 14;
            break;
        case 'F':
        case 'f':
            num = 15;
            break;
        default:
            num = 0;
        }
    }
    return num;
}


unsigned  long int getNumber(byte *info, byte tamanho){
  unsigned long  int acumulador = 0;
  char parte ;
  
  
  //Serial.print("Tamanho");
  //Serial.println(tamanho);
  for( int cont = 0;(cont<tamanho); cont++){
    parte = (char) *(info+cont);
    acumulador = acumulador * 0xFF;
    acumulador = acumulador + ((unsigned long int)parte&0xFF);
    //Serial.print("Parte:");
    //Serial.println( (unsigned short int)(parte&0xFF));
  }
  Serial.print("final:");
  //Serial.println(acumulador);
  
  //memcpy(acumulador,info,tamanho);
  //acumulador = ((*info)<<24)+((*(info+1))<<16)+((*(info+2))<<8)+((*(info+3)));
  //acumulador = (long long int) info;
  return acumulador;
}


void CharLongLongInt(char *info, unsigned  long int numero){
  unsigned  long int tempo =0;
  int resto = 0;
  tempo = numero;
  char infoaux[20];
  

  Serial.println("Partes");
  do {
    memset(infoaux,'\0',sizeof(infoaux));    
    memcpy(infoaux,info,sizeof(infoaux));
    resto = tempo % 10;
    Serial.print("Antes:");
    Serial.println(infoaux);
    sprintf(info,"%d%s\0",resto,infoaux);
    tempo = tempo-resto;
    tempo = tempo/10;    
    Serial.print("Resto:"); 
    Serial.println(resto);
    Serial.print("Info:");
    Serial.println(info);
  } while(tempo !=0);  
  //sprintf(info,"%s",infoaux);
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  String buffer2;
  char number2[20];
  unsigned  long int  number;


  number = getNumber(buffer,bufferSize);
  if (number!=0){
    Serial.println("Diferente de zero");
  }
  //Serial.print("Numero:");
  char info[20];
  memset(info,'\0',sizeof(info));
  //sprintf(info,"%llu",number);
  CharLongLongInt(info,number);
  Serial.print("Nro:");
  Serial.println(info);
      
  /*Ativa se o botão não for pressionado*/
  if(buttonState == LOW) {
    
      //Keyboard.print(number,DEC);
    Keyboard.print(info);
    Keyboard.write(0x10);
    Keyboard.write(0x13);
  }
  Serial.println("Finalizou");
  //delay(2000);
}
