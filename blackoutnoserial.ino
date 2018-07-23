/**************************************************************************/
/*!
    User instructions
*/
/**************************************************************************/
//Commands that can be sent via bluefruit app's serial consol
//'u' = opens curtain immediately
//'d' = closes curtain immediately
//any number = time delay until artificial sunrise
// the number will be read as, the first digit being the hours
//and the rest being minutes until the sunrise should start
//all other variables are set manually and the nano must be reflashed!


#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
#include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"
#define DeviceName "BLAKOUT"

#define DB4steps 1
#define DBsteps 0
#define DBsteppulse 0
#define DBstepsRise 0
#define DB4DIRchange 10
String command;
int backoffdistance = 10;
int stepsPERmm = 1600;//6400per rotation!

long startTime = 0UL;
long wait = 0UL;
int x;
int hours = 0;
int minutes = 0;
boolean runsim = false;


/*=========================================================================
    APPLICATION SETTINGS

      FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
     
                                Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                                running this at least once is a good idea.
     
                                When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.  If you are making changes to your
                                Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.  Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
         
                                Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    MODE_LED_BEHAVIOUR        LED activity, valid options are
                              "DISABLE" or "MODE" or "BLEUART" or
                              "HWUART"  or "SPI"  or "MANUAL"
    -----------------------------------------------------------------------*/
#define FACTORYRESET_ENABLE         0
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "DISABLE"
/*=========================================================================*/

// Create the bluefruit object, either software serial...uncomment these lines
/*
  SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

  Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
*/

/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
// Adafruit_BluefruitLE_UART ble(Serial1, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void simrise() {
  Serial.println("Sim");
  digitalWrite(SLP, HIGH); // Set SLEEP high
  digitalWrite(EN, LOW); // Set Enable low
  digitalWrite(DIR, HIGH); // Set Dir high
  delay(DB4steps);
  while (!limitHit()) {
    digitalWrite(STEP, HIGH); // Output high
    delay(DBsteppulse); // Wait
    digitalWrite(STEP, LOW); // Output low
    delay(DBstepsRise); // Wait
  }
  backoff(HIGH);
  digitalWrite(EN, HIGH); // motor disabled
  digitalWrite(SLP, LOW); // Set SLEEP low
}
void risenow() {
  Serial.println("Rise");
  ble.print("AT+BLEUARTTX=");
  ble.println("Rising...");
  ble.waitForOK();
  digitalWrite(SLP, HIGH); // Set SLEEP high
  digitalWrite(EN, LOW); // Set Enable low
  digitalWrite(DIR, HIGH); // Set Dir high
  delay(DB4steps);
  while (!limitHit()) {
    digitalWrite(STEP, HIGH); // Output high
    delay(DBsteppulse); // Wait
    digitalWrite(STEP, LOW); // Output low
    delay(DBsteps); // Wait
  }
  backoff(HIGH);
  digitalWrite(EN, HIGH); // motor disabled
  digitalWrite(SLP, LOW); // Set SLEEP low
  ble.print("AT+BLEUARTTX=");
  ble.println("risen!");
  ble.waitForOK();
}
void set() {
  Serial.println("Set");
  ble.print("AT+BLEUARTTX=");
  ble.println("Setting...");
  ble.waitForOK();
  digitalWrite(SLP, HIGH); // Set SLEEP high
  digitalWrite(EN, LOW); // Set Enable low
  digitalWrite(DIR, LOW); // Set Dir high
  delay(DB4steps);
  while (!limitHit()) {
    digitalWrite(STEP, HIGH); // Output high
    delay(DBsteppulse); // Wait
    digitalWrite(STEP, LOW); // Output low
    delay(DBstepsRise); // Wait
  }
  backoff(LOW);
  digitalWrite(EN, HIGH); // motor disabled
  digitalWrite(SLP, LOW); // Set SLEEP low
  ble.print("AT+BLEUARTTX=");
  ble.println("set!");
  ble.waitForOK();
}

boolean limitHit() {
  return !digitalRead(Endstop);
}

boolean isDigit(char c) {
  return (((c >= '0') && (c <= '9')));
}

void backoff(boolean currentDirection) {
  digitalWrite(DIR, !currentDirection);
  delay(DB4DIRchange);
  for (int x = 0; x < backoffdistance * stepsPERmm; x++) {
    digitalWrite(STEP, HIGH); // Output high
    delay(DBsteppulse); // Wait
    digitalWrite(STEP, LOW); // Output low
    delay(DBsteps); // Wait
  }
}

//essentially the arduino only part of the loop
//to be run in the loop when the bluetooth stuff is working
void BLAKOUT(String MyInput) {
  switch (MyInput.charAt(0)) {
    case 'u':
      risenow();
      break;
    case 'd':
      set();
      break;
    case 's':
      MyInput = MyInput.substring(1);
      hours = MyInput.toInt(); //hours till sunrise
      x = MyInput.indexOf(','); //checking to see if minutes were sent
      if (x >= 0) {
        minutes = MyInput.substring(x + 1).toInt(); //minutes till sunrise
      }
      wait = (hours * 60 + minutes) * 60000; //delay before sunrise
      startTime = millis();
      runsim = true;
      ble.print("AT+BLEUARTTX=");
      ble.println("Commencing simulation in ");
      ble.waitForOK();
      ble.print("AT+BLEUARTTX=");
      ble.print(hours);
      ble.println(" hours and ");
      ble.waitForOK();
      ble.print("AT+BLEUARTTX=");
      ble.print(minutes);
      ble.println(" minutes!");
      ble.waitForOK();
      break;
    default:
      break;
  }
}



void setup(void)
{
  pinMode(Endstop, INPUT_PULLUP); // endstop
  pinMode(EN, OUTPUT); // Enable
  pinMode(STEP, OUTPUT); // Step
  pinMode(DIR, OUTPUT); // Dir
  pinMode(SLP, OUTPUT); //Sleep
  digitalWrite(EN, HIGH); // Set Enable high(to disable)
  digitalWrite(SLP, LOW); // Set SLEEP low(to put to sleep)
  //while (!Serial);  // required for Flora & Micro
  //delay(500);

  Serial.begin(115200);

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ) {
      error(F("Couldn't factory reset"));
    } else {
      ble.sendCommandCheckOK("AT+GAPDEVNAME=" DeviceName);
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
    delay(500);
  }

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("******************************"));
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
    Serial.println(F("******************************"));
    Serial.println(F("Running BLAKOUT Software"));
    Serial.println(F("Prepare for the best life of your sleep!"));
    Serial.println(F("---------------------------------------"));
    ble.print("AT+BLEUARTTX=");
    ble.println("Running BLAKOUT Software");
    ble.waitForOK();
    ble.print("AT+BLEUARTTX=");
    ble.println("Prepare for the best life of your sleep!");
    ble.waitForOK();


  }
}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{
  if ((millis() - startTime > wait) && runsim) {
    simrise();
    runsim = false;
  }
  // Check for user input
  char inputs[BUFSIZE + 1];

  if ( getUserInput(inputs, BUFSIZE) )
  {
    // Send characters to Bluefruit
    Serial.print("[Send] ");
    Serial.println(inputs);

    ble.print("AT+BLEUARTTX=");
    ble.println(inputs);

    // check response stastus
    if (! ble.waitForOK() ) {
      Serial.println(F("Failed to send?"));
    }
  }

  // Check for incoming characters from Bluefruit
  ble.println("AT+BLEUARTRX");
  ble.readline();
  if (strcmp(ble.buffer, "OK") == 0) {
    // no data
    return;
  }
  // Some data was found, its in the buffer
  String ardInput = ble.buffer;
  Serial.print(F("[Recv] ")); Serial.println(ardInput);
  ble.waitForOK();
  BLAKOUT(ardInput);
}

/**************************************************************************/
/*!
    @brief  Checks for user input (via the Serial Monitor)
*/
/**************************************************************************/
bool getUserInput(char buffer[], uint8_t maxSize)
{
  // timeout in 100 milliseconds
  TimeoutTimer timeout(100);

  memset(buffer, 0, maxSize);
  while ( (!Serial.available()) && !timeout.expired() ) {
    delay(1);
  }

  if ( timeout.expired() ) return false;

  delay(2);
  uint8_t count = 0;
  do
  {
    count += Serial.readBytes(buffer + count, maxSize);
    delay(2);
  } while ( (count < maxSize) && (Serial.available()) );

  return true;
}

