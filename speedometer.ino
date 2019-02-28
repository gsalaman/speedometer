// Speedometer
// Uses the HC-SR04 Ultrasonic sensor to calculate range. This device has a limit of 4 meters (about 13 feet)
// It will display range and speed on the LCD matrix.

#include <LiquidCrystal.h>
/*================
 * LCD CONNECTIONS:  (note...we're using 4 bit mode here...)
 *   1 to GND
 *   2 to 5V
 *   3 to the contrast control...I did a hardcoded voltage divider.
 *   4 to Arduino digital pin LCD_REG_SEL
 *   5 to GND
 *   6 to Arduino digital pin LCD_ENABLE
 *   7 (no connection)
 *   8 (no connection)
 *   9 (no connection)
 *   10 (no connection)
 *   11 to Arduino  digital pin LCD_D4
 *   12 to Arduino  digital pin LCD_D5
 *   13 to Arduino  digital pin LCD_D6
 *   14 to Arduino  digital pin LCD_D7
 *   15 to 5V
 *   16 to GND
 *====================*/
 
#define LCD_D7         7 
#define LCD_D6         6
#define LCD_D5         5
#define LCD_D4         4
#define LCD_ENABLE     8
#define LCD_REG_SEL    9

// Our LCD has 2 rows of 16 characters.
#define LCD_CHARS 16
#define LCD_ROWS 2

LiquidCrystal lcd(LCD_REG_SEL, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// Pins for the range sensor
#define TRIG_PIN 10
#define ECHO_PIN 11

// Anything over 400 cm (23200 us pulse) is "out of range"
#define MAX_DIST 23200

// This function triggers a sensor measuremnt, and returns the range in inches
float get_range_inches( void )
{
  unsigned long t1;
  unsigned long t2;
  unsigned long pulse_width;

  // Hold the trigger pin high for at least 10 us
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Wait for pulse on echo pin
  while ( digitalRead(ECHO_PIN) == 0 );

  // Measure how long the echo pin was held high (pulse width)
  // Note: the micros() counter will overflow after ~70 min
  // Glenn note #2:  should do this with an interrupt instead...
  t1 = micros();
  while ( digitalRead(ECHO_PIN) == 1);
  t2 = micros();

  pulse_width = t2 - t1;

  // Calcluate the distance in inches.  Constants found in the sensor's datasheet.
  // -1 means "out of range"

  if (pulse_width > MAX_DIST)
  {
    return -1;
  }
  else
  {
    return (pulse_width / 148.0);
  }
}

void setup(void) 
{
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);

  Serial.begin(9600);

  lcd.begin(LCD_CHARS, LCD_ROWS);
  lcd.clear();

  lcd.print("Range (in):");

}

void loop(void) 
{
  float range;
  int  range_whole;

  range = get_range_inches();

  // +0.5 for rounding since int cast will truncate
  range = range + 0.5;
  range_whole = range;
  
  lcd.setCursor(12,0);
  lcd.print("    ");
  lcd.setCursor(12,0);
  lcd.print(range_whole);
  

  delay(100);
  
}
