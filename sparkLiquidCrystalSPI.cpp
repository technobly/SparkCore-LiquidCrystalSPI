/*
 * HARDWARE AND SOFTWARE LIQUIDCRYSTAL SPI 
 * 74HC595 LIBRARY FOR SPARK CORE
 * =======================================================
 * Copy this into a new application at:
 * https://www.spark.io/build and go nuts!
 * !! Pinouts and diagrams on line 624 below !!
 * -------------------------------------------------------
 *  Author: BDub
 * Website: http://technobly.com
 *    Date: Feb 6th 2014
 * =======================================================
 * https://github.com/technobly/SparkCore-LiquidCrystalSPI
 */

/* ========================= INCLUDES ============================== */

#include <application.h>

/* ========= Arduino.h ========== */
 
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

/* ========= LiquidCrystalSPI.h ========== */

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

class LiquidCrystal : public Print {
public:
  LiquidCrystal(uint8_t rs, uint8_t enable,
    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
    uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
  LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
    uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
  LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
  LiquidCrystal(uint8_t rs, uint8_t enable,
    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
  LiquidCrystal(uint8_t ss, uint8_t sclk, uint8_t sdat); //SPI to ShiftRegister 74HC595 ##########

  void initSPI(void); //SPI ##################################

  void init(uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
      uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
      uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, uint8_t backlight);
    
  void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);

  void clear();
  void home();

  void noDisplay();
  void display();
  void noBlink();
  void blink();
  void noCursor();
  void cursor();
  void scrollDisplayLeft();
  void scrollDisplayRight();
  void leftToRight();
  void rightToLeft();
  void autoscroll();
  void noAutoscroll();
  void backlight();
  void noBacklight();

  void createChar(uint8_t, uint8_t[]);
  void setCursor(uint8_t, uint8_t); 
  virtual size_t write(uint8_t);
  void command(uint8_t);
private:
  void send(uint8_t, uint8_t);
  void spiSendOut();      // SPI ###########################################
  void write4bits(uint8_t);
  void write8bits(uint8_t);
  void pulseEnable();
  void writeSlow(uint8_t);
  void writeFast(uint8_t);
  
  uint8_t _rs_pin;        // LOW: command.  HIGH: character.
  uint8_t _rw_pin;        // LOW: write to LCD.  HIGH: read from LCD.
  uint8_t _enable_pin;    // activated by a HIGH pulse.
  uint8_t _backlight_pin; // activated by a HIGH pulse (adafruit SPI/I2C LCD Backpack only)
  uint8_t _data_pins[8];
  
  //SPI #####################################################################
  uint8_t _backlight; // 1 = backlight on, 0 = backlight off
  uint8_t _bitString; //for SPI  bit0=not used, bit1=RS, bit2=RW, bit3=Enable, bits4-7 = DB4-7
  bool    _usingSpi;  //to let send and write functions know we are using SPI 
  bool    _softSpi;   //to let send and write functions know we are using SPI 
  uint8_t _latchPin;
  uint8_t _sclkPin;
  uint8_t _sdatPin;
  uint8_t _clockDivider;
  uint8_t _dataMode;
  uint8_t _bitOrder;//SPI ####################################################
  
  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;

  uint8_t _initialized;

  uint8_t _numlines,_currline;
};

/* ========= LiquidCrystalSPI.cpp ========== */

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1 
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
           uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
           uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
  init(0, rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7, 255);
}

LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t enable,
           uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
           uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
  init(0, rs, 255, enable, d0, d1, d2, d3, d4, d5, d6, d7, 255);
}

LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
           uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
  init(1, rs, rw, enable, d0, d1, d2, d3, 0, 0, 0, 0, 255);
}

LiquidCrystal::LiquidCrystal(uint8_t rs,  uint8_t enable,
           uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
  init(1, rs, 255, enable, d0, d1, d2, d3, 0, 0, 0, 0, 255);
}

LiquidCrystal::LiquidCrystal(uint8_t ss, uint8_t sclk=255, uint8_t sdat=255) //SPI  ##############################
{
  _latchPin = ss;
  _softSpi = false; // assume we are using hardware SPI
  if(sclk != 255 && sdat != 255) {
    _softSpi = true; // on second thought, let's use software SPI
  }
  _sclkPin = sclk;
  _sdatPin = sdat;
  
  /*
  initSPI(ssPin);
  //shiftRegister pins 1,2,3,4,5,6,7 represent rs, rw, enable, d4-7 in that order
  //but we are not using RW so RW it's zero or 255
  init(1, 1, 255, 3, 0, 0, 0, 0, 4, 5, 6, 7, 255);   
  */
}

void LiquidCrystal::initSPI(void) //SPI ##########################################
{
  // initialize SPI:
  _usingSpi = true;
  
  pinMode (_latchPin, OUTPUT); // setup _latchPin used in hardware and software SPI
  digitalWrite(_latchPin, HIGH);

  // If we're using software SPI, setup the clock and data pins.
  if(_softSpi) {
    pinMode(_sclkPin, OUTPUT);
    pinMode(_sdatPin, OUTPUT);
    digitalWrite(_sclkPin, LOW);
    digitalWrite(_sdatPin, LOW);
  }
  else { // Else set up the hardware SPI
    SPI.begin();
    
    //_clockDivider = SPI_CLOCK_DIV4; // 72MHz / 4 = 18MHz (works but takes just as long as 9MHz)
    _clockDivider = SPI_CLOCK_DIV8; // 72MHz / 8 = 9MHz (Really is about twice as fast as 4.5MHz)
    //_clockDivider = SPI_CLOCK_DIV16; // 72MHz / 16 = 4.5MHz
    SPI.setClockDivider(_clockDivider);
    // FYI: Software SPI is about the same speed as SPI_CLOCK_DIV8 ! :)
    
    // Set data mode to SPI_MODE0 by default
    _dataMode = SPI_MODE0;
    SPI.setDataMode(_dataMode);
    
    // Set bitOrder to MSBFIRST by default
    _bitOrder = MSBFIRST; 
    SPI.setBitOrder(_bitOrder);
  }
  
  // Adafruit SPI/I2C LCD Backpack or Discrete hookup to 74HC595
  // pins represent: fourbitmode, rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7, backlight
  init(1, 1, 255, 2, 0, 0, 0, 0, 6, 5, 4, 3, 7);
}

void LiquidCrystal::init(uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
  uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
  uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, uint8_t backlight)
{
  _rs_pin = rs;
  _rw_pin = rw;
  _enable_pin = enable;
  _backlight_pin = backlight;
  _backlight = 0; // off by default
  
  _data_pins[0] = d0;
  _data_pins[1] = d1;
  _data_pins[2] = d2;
  _data_pins[3] = d3; 
  _data_pins[4] = d4;
  _data_pins[5] = d5;
  _data_pins[6] = d6;
  _data_pins[7] = d7; 
  
  pinMode(_rs_pin, OUTPUT);
  // we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
  if (_rw_pin != 255) { 
    pinMode(_rw_pin, OUTPUT);
  }
  pinMode(_enable_pin, OUTPUT);
  
  if (fourbitmode)
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
  else 
    _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
  
  //begin(16, 2); // commented out, make sure you call this in code!
  
  //since in initSPI constructor we set _usingSPI to true and we run it first
  //from SPI constructor, we do nothing here otherwise we set it to false
  if (_usingSpi) //SPI ######################################################
  {
    ;
  }
  else
  {
    _usingSpi = false;
  } 
}

void LiquidCrystal::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
  if (lines > 1) {
    _displayfunction |= LCD_2LINE;
  }
  _numlines = lines;
  _currline = 0;

  // for some 1 line displays you can select a 10 pixel high font
  if ((dotsize != 0) && (lines == 1)) {
    _displayfunction |= LCD_5x10DOTS;
  }

  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
  delayMicroseconds(50000); 
  // Now we pull both RS and R/W low to begin commands
  digitalWrite(_rs_pin, LOW);
  digitalWrite(_enable_pin, LOW);
  if (_rw_pin != 255) { 
    digitalWrite(_rw_pin, LOW);
  }
  
  //put the LCD into 4 bit or 8 bit mode
  if (! (_displayfunction & LCD_8BITMODE)) {
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    write4bits(0x03);
    delayMicroseconds(4500); // wait min 4.1ms

    // second try
    write4bits(0x03);
    delayMicroseconds(4500); // wait min 4.1ms
    
    // third go!
    write4bits(0x03); 
    delayMicroseconds(150);

    // finally, set to 4-bit interface
    write4bits(0x02); 
  } else {
    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    command(LCD_FUNCTIONSET | _displayfunction);
    delayMicroseconds(4500);  // wait more than 4.1ms

    // second try
    command(LCD_FUNCTIONSET | _displayfunction);
    delayMicroseconds(150);

    // third go
    command(LCD_FUNCTIONSET | _displayfunction);
  }

  // finally, set # lines, font size, etc.
  command(LCD_FUNCTIONSET | _displayfunction);  

  // turn the display on with no cursor or blinking default
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
  display();

  // clear it off
  clear();

  // Initialize to default text direction (for romance languages)
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  command(LCD_ENTRYMODESET | _displaymode);

}

/********** high level commands, for the user! */
void LiquidCrystal::clear()
{
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal::home()
{
  command(LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal::setCursor(uint8_t col, uint8_t row)
{
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row > _numlines ) {
    row = _numlines-1;    // we count rows starting w/0
  }
  
  command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidCrystal::noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal::display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LiquidCrystal::noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal::cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LiquidCrystal::noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal::blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal::scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidCrystal::scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystal::leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LiquidCrystal::rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal::autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal::noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LiquidCrystal::createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    write(charmap[i]);
  }
}

// Turn the backlight on/off
// Backlight will turn on or off immediately
void LiquidCrystal::backlight(void) {
  _backlight = 1;
  // add the backlight bit on all transfers
  bitWrite(_bitString, _backlight_pin, 1);
  // and send it out
  spiSendOut();
}
void LiquidCrystal::noBacklight(void) {
  _backlight = 0;
  // add the backlight bit on all transfers
  bitWrite(_bitString, _backlight_pin, 0);
  // and send it out
  spiSendOut();
}

/*********** mid level commands, for sending data/cmds */

inline void LiquidCrystal::command(uint8_t value) {
  send(value, LOW);
}

inline size_t LiquidCrystal::write(uint8_t value) {
  send(value, HIGH);
  return 1; // assume sucess
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void LiquidCrystal::send(uint8_t value, uint8_t mode) {
  if (_usingSpi == false)
  {
    digitalWrite(_rs_pin, mode);

    // if there is a RW pin indicated, set it low to Write
    if (_rw_pin != 255) { 
      digitalWrite(_rw_pin, LOW);
    }
    
    if (_displayfunction & LCD_8BITMODE) {
      write8bits(value); 
    } else {
      write4bits(value>>4);
      write4bits(value);
    }
  }
  else //we use SPI  ##########################################
  {
    bitWrite(_bitString, _rs_pin, mode); //set RS to mode
    spiSendOut();
    
    // we are not using RW with SPI so we are not even bothering
    // or 8BITMODE so we go straight to write4bits
    write4bits(value>>4);
    write4bits(value);    
  }
}

void LiquidCrystal::pulseEnable(void) {
  if (_usingSpi == false)
  {
    digitalWrite(_enable_pin, LOW);
    delayMicroseconds(1);
    digitalWrite(_enable_pin, HIGH);
    delayMicroseconds(1);    // enable pulse must be >450ns
    digitalWrite(_enable_pin, LOW);
    delayMicroseconds(100);   // commands need > 37us to settle
  }
  else //we use SPI #############################################
  {
    bitWrite(_bitString, _enable_pin, LOW);
    spiSendOut();
    delayMicroseconds(1);
    bitWrite(_bitString, _enable_pin, HIGH);
    spiSendOut();
    delayMicroseconds(1);    // enable pulse must be >450ns
    bitWrite(_bitString, _enable_pin, LOW);
    spiSendOut();
    delayMicroseconds(40);   // commands need > 37us to settle
  }
}

void LiquidCrystal::write4bits(uint8_t value) {
  if (_usingSpi == false)
  {
    for (int i = 0; i < 4; i++) {
      pinMode(_data_pins[i], OUTPUT);
      digitalWrite(_data_pins[i], (value >> i) & 0x01);
    }
  }
  else //we use SPI ##############################################
  {
    for (int i = 4; i < 8; i++) {
      //we put the four bits into the _bitString
      bitWrite(_bitString, _data_pins[i], ((value >> (i - 4)) & 0x01));
    }
    // add the backlight bit on all transfers
    bitWrite(_bitString, _backlight_pin, (_backlight & 0x01));
    
    // and send it out
    spiSendOut();
  }
  pulseEnable();
}

void LiquidCrystal::write8bits(uint8_t value) {
  for (int i = 0; i < 8; i++) {
    pinMode(_data_pins[i], OUTPUT);
    digitalWrite(_data_pins[i], (value >> i) & 0x01);
  }
  pulseEnable();
}

void LiquidCrystal::spiSendOut() //SPI #############################
{
  if(_softSpi) {
    writeFast(_bitString);
  }
  else {
    digitalWrite(_latchPin, LOW);
    SPI.transfer(_bitString);
    digitalWrite(_latchPin, HIGH);
  }
}

void LiquidCrystal::writeSlow(uint8_t value) {
  digitalWrite(_latchPin, LOW);
  shiftOut(_sdatPin, _sclkPin, MSBFIRST, value);
  digitalWrite(_latchPin, HIGH);
}

inline void LiquidCrystal::writeFast(uint8_t value) {
  PIN_MAP[_latchPin].gpio_peripheral->BRR = PIN_MAP[_latchPin].gpio_pin; // Latch Low
  for (uint8_t i = 0; i < 8; i++)  {
    if (value & (1 << (7-i))) { // walks down mask from bit 7 to bit 0
      PIN_MAP[_sdatPin].gpio_peripheral->BSRR = PIN_MAP[_sdatPin].gpio_pin; // Data High
    } 
    else {
      PIN_MAP[_sdatPin].gpio_peripheral->BRR = PIN_MAP[_sdatPin].gpio_pin; // Data Low
    }
    asm volatile("mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" ::: "r0", "cc", "memory");
    PIN_MAP[_sclkPin].gpio_peripheral->BSRR = PIN_MAP[_sclkPin].gpio_pin; // Clock High (Data Shifted In)
    asm volatile("mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" ::: "r0", "cc", "memory");
    PIN_MAP[_sclkPin].gpio_peripheral->BRR = PIN_MAP[_sclkPin].gpio_pin; // Clock Low
  }
  asm volatile("mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" ::: "r0", "cc", "memory");
  PIN_MAP[_latchPin].gpio_peripheral->BSRR = PIN_MAP[_latchPin].gpio_pin; // Latch High (Data Latched)
}

/* ========= Application.cpp ========== */

/*
  LiquidCrystal SPI Library - Hello, Sparky!
 
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
  modified to work for Spark Core (HW & SW SPI) 6 Feb 2014
  by BDub (http://www.technobly.com)
 
  This example code is in the public domain.
  http://www.arduino.cc/en/Tutorial/LiquidCrystal
*/

// Create an instance of the library for HARDWARE SPI mode (define SS "latch" pin)
// Default SS pin is A2, but can be ANY of the A0 - A2, A6, A7 or D0 - D7 pins.
// Just make sure you don't redefine this pin as some other peripheral later in your code.
LiquidCrystal lcd(A2);

// Create an instance of the library for SOFTWARE SPI mode (define SS "latch" pin, SCLK pin, SDAT pin)
// These can be ANY of the A0 - A7 or D0 - D7 pins. 
// Just make sure you don't redefine them as some other peripheral later in your code.
//LiquidCrystal lcd(D2, D3, D4);

bool s = false;

void setup() {
  // Set D7 to an output so we can control the on-board LED.
  pinMode(D7,OUTPUT);
  // initialize the SPI ( Must call this before begin()! )
  lcd.initSPI();
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // turn on the backlight ( Requires initSPI() to be called first )
  lcd.backlight();
  // Print a message to the LCD.
  lcd.print("Hello, Sparky!");
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis()/1000);
  // Toggle the D7 LED for fun so we know our code is running!
  s = !s;
  digitalWrite(D7,s);
  delay(100);
}