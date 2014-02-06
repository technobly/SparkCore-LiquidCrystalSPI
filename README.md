LiquidCrystal SPI for Spark Core
================================

A Hardware and Software SPI version of the LiquidCrystal library for the Spark Core.

###Hardware SPI Wiring (default)###
![wiring](http://i.imgur.com/pFFWmv9.png)

###Hardware SPI Example Code (default)###
```cpp
// Create an instance of the library for HARDWARE SPI mode (define SS "latch" pin)
// Default SS pin is A2, but can be ANY of the A0 - A2, A6, A7 or D0 - D7 pins.
// Just make sure you don't redefine this pin as some other peripheral later in your code.
LiquidCrystal lcd(A2);

bool s = false;

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

bool s = false;

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