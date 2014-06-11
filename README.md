SparkCore-LiquidCrystalSPI
==========================

A Hardware and Software SPI driven LiquidCrystal library for Spark Core.

Universally Supports:
* Standard Hitachi HD44780 based LCDs in 4-bit mode
* Adafruit's 16x2 OLED Display (Winstar)
* Sparkfun's 16x2 OLED Display (ADH Technology Co. Ltd.)

Grab the RAW version of each file and place into your web IDE as follows:

![image](http://i.imgur.com/2Zm5fmG.png)

If you are building locally, place the files here:

```
..\core-firmware\inc\liquid-crystal-spi.h
..\core-firmware\src\application.cpp (renamed from lcd-soft-spi.cpp)
..\core-firmware\src\liquid-crystal-spi.cpp
..\core-firmware\src\build.mk (optional, if you have your own make file going, just add the liquid-crystal-spi.cpp to it)
```

###Hardware SPI Wiring (default)###
![wiring](http://i.imgur.com/pFFWmv9.png)

###Hardware SPI Example Code (default)###
```cpp
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
```
<hr>

###Software SPI Wiring###
![wiring](http://i.imgur.com/RneHSOw.png)

###Software SPI Example Code###
```cpp
// Create an instance of the library for SOFTWARE SPI mode (define SS "latch" pin, SCLK pin, SDAT pin)
// These can be ANY of the A0 - A7 or D0 - D7 pins. 
// Just make sure you don't redefine them as some other peripheral later in your code.
LiquidCrystal lcd(D2, D3, D4);

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
```
<hr>

![wiring](http://i.imgur.com/Y3J6199.jpg)

Software SPI is just as fast as 9MHz Hardware SPI so don't be afraid to use it!

Control the backlight via SPI if you have an Adafruit I2C/SPI LCD Backpack

```cpp
// Create an instance of the library for SOFTWARE SPI mode (define SS "latch" pin, SCLK pin, SDAT pin)
// These can be ANY of the A0 - A7 or D0 - D7 pins. 
// Just make sure you don't redefine them as some other peripheral later in your code.
LiquidCrystal lcd(D2, D3, D4);

bool s = false;

void setup() {
  // initialize the SPI ( Must call this before begin()! )
  lcd.initSPI();
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // turn on the backlight ( Requires initSPI() to be called first )
  lcd.backlight();
  // Print a message to the LCD.
  lcd.print("Backlight Rave!");
}

void loop() {
  // Toggle the Backlight on and off, Backlight RAVE!
  if(s)
    lcd.backlight();
  else
    lcd.noBacklight();
  s = !s;
  delay(100);
}
```