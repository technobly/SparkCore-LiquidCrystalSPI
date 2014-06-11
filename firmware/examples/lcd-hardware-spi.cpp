/*
 * HARDWARE AND SOFTWARE LIQUIDCRYSTAL SPI 
 * 74HC595 LIBRARY FOR SPARK CORE
 * =======================================================
 * Copy this into a new application at:
 * https://www.spark.io/build and go nuts!
 * !! Pinouts and diagrams on line 27 below !!
 * -------------------------------------------------------
 *  Author: BDub
 * Website: http://technobly.com
 *    Date: Feb 24th 2014
 * =======================================================
 * https://github.com/technobly/SparkCore-LiquidCrystalSPI
 */

/*
  LiquidCrystal SPI Library - Hardware SPI Example
 
  This is the same library that comes with Arduino for LCDs.
  There is added support for hardware and software SPI.
  Assumes you are using a 74HC595 shift register.
 
  SPARK CORE HARDWARE SPI PINOUTS 
  ( SS, SCLK and SDAT can be whatever you want for SOFTWARE SPI, 
    but must define all three in Class constructor, 
    see below for commented out example )
  http://docs.spark.io/#/firmware/communication-spi
  =========================================================
  Spark Core    74HC595
  A5 (MOSI)     14 - The DATA pin for incoming serial data.
  A3 (SCK)      11 - The CLOCK pin for the DATA bits.
  A2 (SS)       12 - The LATCH pin, latches data to 595 outputs.
  GND           8 - Ground pin
  GND           13 - Output enable pin (enable)
  VIN           16 - VCC power pin
  VIN           10 - Serial clear pin (disable)
  
  LCD CONN.     74HC595
  RS (typ. 4)    1 - QB pin
  E  (typ. 6)    2 - QC pin
  D7 (typ. 14)   3 - QD pin
  D6 (typ. 13)   4 - QE pin
  D5 (typ. 12)   5 - QF pin
  D4 (typ. 11)   6 - QG pin
  LITE (*)       7 - QH pin
  RW (typ. 5)    8 - Ground for always Write mode

  LCD CONN.     10K CONTRAST POT
  VCC (typ. 2)   1 - HIGH
  CTRST (typ. 3) 2 - WIPER
  GND (typ. 1)   3 - LOW
  
  * see Adafruit I2C/SPI BACKPACK schematic for backlight circuit suggestion:
  https://raw2.github.com/adafruit/i2c-SPI-LCD-backpack/master/i2cspilcdbackpacksch.png
  
  See this Fritzing diagram for a visual guide to the above connections
  http://i.imgur.com/pFFWmv9.png
  =========================================================
  Spark Core    Adafruit I2C/SPI BACKPACK http://www.adafruit.com/products/292
  A5 (MOSI)     DAT - The DATA pin for incoming serial data.
  A3 (SCK)      CLK - The CLOCK pin for the DATA bits.
  A2 (SS)       LAT - The LATCH pin, latches data to 595 outputs.
  VIN           5V  - The power pin
  GND           GND - The ground pin
  NOTES: Make sure SPI solder jumper is shorted!
  =========================================================

  Library originally added 18 Apr 2008
  by David A. Mellis
  library modified 5 Jul 2009
  by Limor Fried (http://www.ladyada.net)
  example added 9 Jul 2009
  by Tom Igoe
  modified 22 Nov 2010
  by Tom Igoe
  modified 12 Feb 2012
  by Juan Hernandez
  modified to work for Spark Core (HW & SW SPI) 24 Feb 2014
  by BDub (http://www.technobly.com)
 
  This example code is in the public domain.
  http://www.arduino.cc/en/Tutorial/LiquidCrystal
*/

/* ========= INCLUDES ==================== */

#include "application.h"

#include "liquid-crystal-spi.h"
  
/* ========= Application.cpp ============= */

// Create an instance of the library for HARDWARE SPI mode (define SS "latch" pin)
// Default SS pin is A2, but can be ANY of the A0 - A2, A6, A7 or D0 - D7 pins.
// Just make sure you don't redefine this pin as some other peripheral later in your code.
LiquidCrystal lcd(A2);

void setup() {
  // initialize the SPI ( Must call this before begin()! )
  lcd.initSPI();
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Hello, Sparky!");
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis()/1000);
  delay(100);
}