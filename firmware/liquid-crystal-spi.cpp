/*
 * HARDWARE AND SOFTWARE LIQUIDCRYSTAL SPI 
 * 74HC595 LIBRARY FOR SPARK CORE
 * =======================================================
 *  Author: BDub
 * Website: http://technobly.com
 *    Date: Feb 24th 2014
 * =======================================================
 * https://github.com/technobly/SparkCore-LiquidCrystalSPI
 */

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

/* ========= INCLUDES ==================== */

#include "liquid-crystal-spi.h"

/* ========= LiquidCrystalSPI.cpp ======== */

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

LiquidCrystal::LiquidCrystal(uint8_t ss, uint8_t sclk, uint8_t sdat) //SPI  ##############################
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
  
  // Always 4-bit mode, don't waste pins!
  //
  //if (fourbitmode)
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
  //else 
    //_displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
  
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
  
  // 4-Bit initialization sequence from Technobly
  write4bits(0x03);         // Put back into 8-bit mode
  delayMicroseconds(5000);

  write4bits(0x08);         // Comment this out for V1 OLED
  delayMicroseconds(5000);  // Comment this out for V1 OLED
  
  write4bits(0x02);         // Put into 4-bit mode
  delayMicroseconds(5000);
  write4bits(0x02);
  delayMicroseconds(5000);
  write4bits(0x08);
  delayMicroseconds(5000);
  
  command(LCD_DISPLAYCONTROL);                  // Turn Off
  delayMicroseconds(5000);
  command(LCD_FUNCTIONSET | _displayfunction);  // Set # lines, font size, etc.
  delayMicroseconds(5000);
  clear();                                      // Clear Display
  command(LCD_ENTRYMODESET | LCD_ENTRYLEFT);    // Set Entry Mode
  delayMicroseconds(5000);
  home();                                       // Home Cursor
  delayMicroseconds(5000);
  command(LCD_DISPLAYCONTROL | LCD_DISPLAYON);  // Turn On - enable cursor & blink
  delayMicroseconds(5000);
}

/********** high level commands, for the user! */
void LiquidCrystal::clear()
{
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  delayMicroseconds(5000);  // this command takes a long time!
}

void LiquidCrystal::home()
{
  command(LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(5000);  // this command takes a long time!
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