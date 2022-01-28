   
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
#include <EEPROM.h>
#include <math.h>


#define Version "1.0.3"

#define DefConfig     2
#define DefNormal     0

#define DevSerial     0
#define DevBluetooth  1
#define DevEthernet   2


#define SS_PIN 10
#define RST_PIN 9

/*Enderecos de firmware*/
#define endereco_HEX    0
#define endereco_DEB    1
#define endereco_ECHO   2
#define endereco_SERIAL 3
#define endereco_KEY    4



int counter = 0;   
// button push counter
char BufferKeypad[100];
int nivel = DefNormal;

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];
byte hexmode;
byte debugmode;
byte echomode;
byte serialmode;
byte keymode;


/*Funcao Le parametro Hexadeciamal*/
int LerENDPARAMKEY(){
  return EEPROM.read(endereco_KEY);
}

/*Funcao Grava parametro Hexadeciamal*/
void GravarENDPARAMKEY(byte value){
  EEPROM.write(endereco_KEY, value);
  delay(100);
  keymode = LerENDPARAMKEY();
}


void setup_keyboard(){
  keymode = LerENDPARAMKEY();
  if(keymode!=LOW){
    // initialize control over the keyboard:
    Keyboard.begin();
    Serial.println("Keyboard ready!");
  }
}

void setup_rfid(){
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  if(debugmode!=LOW){
    Serial.println(F("This code scan the MIFARE Classsic NUID."));
    Serial.print(F("Using the following key:"));
    printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  }
}

/*Funcao Le parametro Hexadeciamal*/
int LerENDPARAMECHO(){
  return EEPROM.read(endereco_ECHO);
}

/*Funcao Grava parametro Hexadeciamal*/
void GravarENDPARAMECHO(byte value){
  EEPROM.write(endereco_ECHO, value);
  delay(100);
  echomode = LerENDPARAMDEB();
}

void setup_echo(){
  echomode = LerENDPARAMDEB();
}


/*Funcao Le parametro Serial*/
int LerENDPARAMSERIAL(){
  return EEPROM.read(endereco_SERIAL);
}

/*Funcao Grava parametro Serial*/
void GravarENDPARAMSERIAL(byte value){
  EEPROM.write(endereco_SERIAL, value);
  delay(100);
  serialmode = LerENDPARAMSERIAL();
}

void setup_serialmode(){
  serialmode = LerENDPARAMSERIAL();
}


/*Funcao Le parametro Hexadeciamal*/
int LerENDPARAMDEB(){
  return EEPROM.read(endereco_DEB);
}

/*Funcao Grava parametro Hexadeciamal*/
void GravarENDPARAMDEB(byte value){
  EEPROM.write(endereco_DEB, value);
  delay(100);
  debugmode = LerENDPARAMDEB();
}


void setup_debugmode(){
  debugmode = LerENDPARAMDEB();
}



/*Funcao Le parametro Hexadeciamal*/
int LerENDPARAMHEX(){
  return EEPROM.read(endereco_HEX);
}

/*Funcao Grava parametro Hexadeciamal*/
void GravarENDPARAMHEX(byte value){
  EEPROM.write(endereco_HEX, value);
  delay(100);
  hexmode = LerENDPARAMHEX();
}

void Setup_HexValue(){

  // read the pushbutton:
  hexmode = LerENDPARAMHEX();
}

void setup_serial(){
  Serial.begin(9600);  

}



void Wellcome(){
  Serial.println("RFID Open Hardware");
  Serial.println("Project site: http://maurinsoft.com.br/index.php/projeto-leitor-rfid/");
  Serial.print("Version:");
  Serial.println(Version);
  Serial.println("Starting firmware...");
}

void setup() { 
  
  setup_serial();
  Wellcome();  
  setup_debugmode();      
  setup_rfid();    
  Setup_HexValue(); /*Le valor de Hexadecimal*/
  setup_keyboard();
  setup_echo();
  setup_serialmode();
  
  
  Serial.println("Firmware ready!");
}

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void ManSerial()
{  
  Serial.println(" "); 
  if(nivel==DefConfig){
    Serial.println("man - man page"); 
    Serial.println("status - status of reader");  
    Serial.println("normal - change use mode");  
    Serial.println("set hexon - hexadecimal enabled");  
    Serial.println("set hexoff -  hexadecimal not enabled");  
    Serial.println("set debugon - debug enabled");  
    Serial.println("set debugoff - debug not enabled");     
    Serial.println("set echoon - echo enabled in serial");  
    Serial.println("set echooff - echo not enabled in serial");     
    Serial.println("set serialon - return rfid in serial enabled");  
    Serial.println("set serialoff - return rfid in serial not enabled");      
    Serial.println("set keyon - return rfid in keyboard enabled");  
    Serial.println("set keyoff - return rfid in keyboard not enabled");    
       
  }
  if(nivel==DefNormal){
    Serial.println("man - man page"); 
    Serial.println("status - status of reader");  
    Serial.println("config - change config mode"); 
  }
}

void Man(byte Dev)
{
  if (Dev == DevSerial)
  {
    ManSerial();
  }
}



//Status da Serial
void StatusSerial()
{
  
  Serial.println(" ");
  
  Serial.println("Status reader");
  Serial.print("Reader Mode:");
  Serial.println((nivel==DefNormal)?"Normal":"Config");
  Serial.print( "Hex mode:"); 
  //byte value = digitalRead(buttonPin);
  byte value = LerENDPARAMHEX();
  Serial.println(((value!=LOW)?"On":"Off")); 
  Serial.print( "DEBUG mode:"); 
  debugmode = LerENDPARAMDEB();
  Serial.println(((debugmode!=LOW)?"On":"Off"));    
  Serial.print( "ECHO mode:"); 
  echomode = LerENDPARAMECHO();
  Serial.println(((echomode!=LOW)?"On":"Off"));    
  Serial.print( "SERIAL mode rfid:"); 
  serialmode = LerENDPARAMSERIAL();
  Serial.println(((serialmode!=LOW)?"On":"Off"));      
  Serial.print( "KEYBOARD mode rfid:"); 
  serialmode = LerENDPARAMKEY();
  Serial.println(((keymode!=LOW)?"On":"Off"));        
  Serial.println(" ");
  
}

//Status da Serial
void Status(byte Dev)
{
  if(Dev==DevSerial)
  {
    StatusSerial(); 
  }
 
}


//Imprime padrão demonstrando entrada de comando
void ImprimeSerial()
{
  //Serial.println(" ");
  Serial.print("/>"); 
}



//Imprime padrão demonstrando entrada de comando
void Imprime(byte Dev)
{
  if (Dev == DevSerial)
  {
    ImprimeSerial();
  }
}


void le_rfid() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;
  if(debugmode!=LOW){
    Serial.print(F("PICC type: "));
  } 
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  if(debugmode!=LOW){
    Serial.println(rfid.PICC_GetTypeName(piccType));
  }
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
    if(debugmode!=LOW){  
      Serial.println(F("A new card has been detected."));
    }

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
    if(debugmode!=LOW){
      Serial.println(F("The NUID tag is:"));
      Serial.print(F("In hex: "));
    }
    if(hexmode!=LOW){
      printHex(rfid.uid.uidByte, rfid.uid.size);
    }
    if(debugmode!=LOW){
      Serial.println();
    }
    if(serialmode!=LOW){
      Serial.print(F("In dec: "));
    }
    if(hexmode==LOW){
       printDec(rfid.uid.uidByte, rfid.uid.size);
    }
    if(debugmode!=LOW){
      Serial.println();
    }
  }
  else {
    if(debugmode!=LOW){
       Serial.println(F("Card read previously."));
    }
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

void Wellcome_Normal(){
  Serial.println("Normal mode is actived");
}

void Wellcome_Config(){
  Serial.println("Config Mode is activade");
}



//Comando de entrada do Teclado
void KeyCMD_Normal(byte Dev)
{
  bool resp = false;

  if(BufferKeypad[0] != '\0')
  {
    Serial.print("Comando:");
    Serial.println(BufferKeypad);
        
    if (strcmp( BufferKeypad,"config")==0)
    {
      nivel = DefConfig;
      resp = true;
      //LDC(Produto," ");
      Wellcome_Config();
    }
 
  
    if (strcmp( BufferKeypad,"status")==0)
    {
      if(debugmode!=LOW){
        Serial.println("Status command...");
      }
      Status(Dev);
      resp = true;        
    }
    if (strcmp( BufferKeypad,"man")==0)
    {    
      Man(Dev);
      resp = true;        
    }

    if (resp==false) 
    {
      if( Dev==DevSerial)
      {       
        //Serial.println(BufferKeypad);        
        Serial.println("Unknow command!"); 
        //strcpy(BufferKeypad,'\0');
        memset(BufferKeypad,0,sizeof(BufferKeypad));
      }      
    }
    else
    {
      //strcpy(BufferKeypad,'\0');
      memset(BufferKeypad,0,sizeof(BufferKeypad));
    }    
    Imprime(Dev); 
  }
}


//Comando de entrada do Teclado
void KeyCMD_Config(byte Dev)
{
  bool resp = false;

  if(BufferKeypad[0] != '\0')
  {
    if(debugmode!=LOW){
       Serial.print("Command:");
       Serial.println(BufferKeypad);
    }
   
    if (strcmp( BufferKeypad, "normal")==0)
    {
      nivel = DefNormal;
      resp = true;
      Wellcome_Normal();
    }
  
    if (strcmp( BufferKeypad,"status")==0)
    {
      if(debugmode!=LOW){
        Serial.println("Status command...");
        Status(Dev);
      }
      resp = true;        
    }

    if (strcmp( BufferKeypad,"set hexon")==0)
    {
      if(debugmode!=LOW){
         Serial.println("set hex on command...");
      }
      //Status(Dev);
      GravarENDPARAMHEX(HIGH);
      resp = true;        
    }

    if (strcmp( BufferKeypad,"set hexoff")==0)
    {
      if(debugmode!=LOW){
         Serial.println("set hex off command...");
      }
      //Status(Dev);
      GravarENDPARAMHEX(LOW);
      resp = true;        
    }    

    if (strcmp( BufferKeypad,"set debugon")==0)
    {
      if(debugmode!=LOW){
        Serial.println("set debug on command...");
      }
      //Status(Dev);
      GravarENDPARAMDEB(HIGH);
      resp = true;        
    }

    if (strcmp( BufferKeypad,"set debugoff")==0)
    {
      if(debugmode!=LOW){
         Serial.println("set debug off command...");
      }
      //Status(Dev);
      GravarENDPARAMDEB(LOW);
      resp = true;        
    }  

    if (strcmp( BufferKeypad,"set echoon")==0)
    {
      if(debugmode!=LOW){
        Serial.println("set echo on command...");
      }
      //Status(Dev);
      GravarENDPARAMECHO(HIGH);
      resp = true;        
    }

    if (strcmp( BufferKeypad,"set echooff")==0)
    {
      if(debugmode!=LOW){
         Serial.println("set echo off command...");
      }
      //Status(Dev);
      GravarENDPARAMECHO(LOW);
      resp = true;        
    }  

    if (strcmp( BufferKeypad,"set serialon")==0)
    {
      if(debugmode!=LOW){
        Serial.println("set serialmode on command...");
      }
      //Status(Dev);
      GravarENDPARAMSERIAL(HIGH);
      resp = true;        
    }

    if (strcmp( BufferKeypad,"set serialoff")==0)
    {
      if(debugmode!=LOW){
         Serial.println("set serialmode off command...");
      }
      //Status(Dev);
      GravarENDPARAMSERIAL(LOW);
      resp = true;        
    }      

    if (strcmp( BufferKeypad,"set keyon")==0)
    {
      if(debugmode!=LOW){
        Serial.println("set keymode on command...");
      }
      //Status(Dev);
      GravarENDPARAMKEY(HIGH);
      Serial.println("Reseting RFID READER");
      /*Força reset de maquina*/
      resetFunc();  //call reset
      resp = true;        
    }

    if (strcmp( BufferKeypad,"set keyoff")==0)
    {
      if(debugmode!=LOW){
         Serial.println("set keymode off command...");
      }
      //Status(Dev);
      GravarENDPARAMKEY(LOW);
      resp = true;        
    }     
    
    if (strcmp( BufferKeypad,"man")==0)
    {    
      Man(Dev);
      resp = true;        
    }

    if (resp==false) 
    {
      if( Dev==DevSerial)
      {
        
        
        //Serial.println(BufferKeypad);
        
        Serial.println("Unknow command!"); 
        //strcpy(BufferKeypad,'\0');
        memset(BufferKeypad,0,sizeof(BufferKeypad));
        
      }      
    }
    else
    {
      //strcpy(BufferKeypad,'\0');
      memset(BufferKeypad,0,sizeof(BufferKeypad));
    }    
    Imprime(Dev); 
  }
}

//Comando de entrada do Teclado
void KeyCMD(byte Dev)
{
  if(nivel==DefNormal){
    KeyCMD_Normal(Dev);
  } 
  if(nivel==DefConfig){
    KeyCMD_Config(Dev);
  }
}


//Le registro do bluetooth
void Le_Serial()
{  
  char key;
  while(Serial.available()>0) 
  {
    key = Serial.read();
    if (echomode!=LOW){ /*Echo ativo*/
      Serial.print(key);
    }
    if(key == '\r')
    {
      sprintf(BufferKeypad,"%s",BufferKeypad);
      if(debugmode!=LOW){
         Serial.print("Comando digitado:");
         Serial.println(BufferKeypad);
      }
      KeyCMD(DevSerial);      
      break;
    }
    else
    {
      //BufferKeypad += key;
      sprintf(BufferKeypad,"%s%c",BufferKeypad,key);
    }
  }   
}

void Leituras()
{
  if(nivel==DefNormal){
    le_rfid();
  }
  Le_Serial(); //Le dados do Serial
}
 
void loop() {
  Leituras();
}


/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  char hex[] = "0123456789abcdef";
  if (serialmode!=LOW){
    Serial.print("RFID:");
  }
  for (byte i = 0; i < bufferSize; i++) {
    if(serialmode!=LOW){
       Serial.print(buffer[i] < 0x10 ? " 0" : " ");
       Serial.print(buffer[i], HEX);
    }
    /*Ativa se o botão não for pressionado*/
    if(keymode != LOW) { 
        Keyboard.print(buffer[i], HEX);
    }
  }
  if(debugmode!=LOW){
    Serial.println(" ");
  }
  if(keymode != LOW) { 
    Keyboard.write(0x10);
    Keyboard.write(0x13);
  }
  
}

long long int strtoll(char *info, char *pos, int base){
  long long int value = 0;
  int cont = 0;
  if(debugmode!=LOW){
    Serial.println("Iniciou");
  }
  char info2[10];
  
  info2[0] = *info; /*Pega primeiro caracter*/
  
  while(info2[0] != '\0'){
    if(debugmode!=LOW){
      Serial.print("info2:");
      Serial.println(info2[0]);
    }
    value = value * base;
    if(debugmode!=LOW){    
      Serial.print("value:");
      Serial.println( (char)strtol(info2[0], NULL, base));
    }
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
  if(debugmode!=LOW){
    Serial.print("final:");
    //Serial.println(acumulador);
  }
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
  
  if(debugmode!=LOW){
    Serial.println("Partes");
  }
  do {
    memset(infoaux,'\0',sizeof(infoaux));    
    memcpy(infoaux,info,sizeof(infoaux));
    resto = tempo % 10;
    if(debugmode!=LOW){
      Serial.print("Antes:");
      Serial.println(infoaux);
    }
    sprintf(info,"%d%s\0",resto,infoaux);
    tempo = tempo-resto;
    tempo = tempo/10;    
    if(debugmode!=LOW){    
      Serial.print("Resto:"); 
      Serial.println(resto);
      Serial.print("Info:");
      Serial.println(info);
    }
  } while(tempo !=0);  
  //sprintf(info,"%s",infoaux);
}

/**
  char hex[] = "0123456789abcdef";
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
    /*Ativa se o botão não for pressionado* /
    if(hexmode != LOW) { 
        Keyboard.print(buffer[i], HEX);
    }
  }
  Serial.println(" ");
  Keyboard.write(0x10);
  Keyboard.write(0x13);
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  String buffer2;
  char number2[20];
  unsigned long int  number;


  //number = getNumber(buffer,bufferSize);
  
  number = 0;
  
  for (byte i = 0; i < bufferSize; i++) {
     number += buffer[i]*pow(0xFF,((bufferSize-1)-i));     
  }

  
  
  if (number!=0){
    if(debugmode!=LOW){    
      Serial.println("Diferente de zero");
    }
  }
  
  char info[20];
  memset(info,'\0',sizeof(info));
  sprintf(info,"%lu",number);
  //CharLongLongInt(info,number);
  if(serialmode!=LOW){   
      Serial.print("RFID:");
       Serial.println(info);
  }
      
  /*Ativa se o botão não for pressionado*/
  if(keymode != LOW) {    
    //Keyboard.print(number,DEC);
    Keyboard.print(info);
    Keyboard.write(0x10);
    Keyboard.write(0x13);
  }
  if(debugmode!=LOW){
    Serial.println("Finalizou");
  }
  //delay(2000);
}
