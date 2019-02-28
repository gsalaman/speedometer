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
  static float last_range=-1.0;
  static unsigned long last_time_us=0;
  unsigned long current_time_us;
  float delta_t_us;
  float delta_x_inches;
  float current_speed;
  char speed_string[16];
  int speed_whole;
  int speed_first_decimal;
  
  range = get_range_inches();
  current_time_us = micros();

  // +0.5 for rounding since int cast will truncate
  range = range + 0.5;
  range_whole = range;

  
  // Only do the calc if range AND last range are valid (eg, not -1);
  if ((range != -1) && (last_range != -1)) 
  {
    delta_t_us = current_time_us - last_time_us;
    delta_x_inches = range - last_range;

    // we just want the magnitued of speed, so make delta_x_inches positive
    if (delta_x_inches < 0) delta_x_inches = -delta_x_inches;

    // Converting units:  
    //    inches * 12 * 5280 = miles
    //    us * 1,000,000 * 3600 = hours
    //  So, we take inches/us and multiply by 1000000*3600/12*5280 or 56818.18
    current_speed = 56818.18 * delta_x_inches / delta_t_us;

    // print only one decimal digit.  
    speed_whole = current_speed;
    speed_first_decimal = 10.0 * (current_speed - (float) speed_whole + 0.05);   // 0.05 for rounding
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("MPH: ");
    lcd.print(speed_whole);
    lcd.print(".");
    lcd.print(speed_first_decimal);
    
  }
    
  lcd.setCursor(12,0);
  lcd.print("    ");
  lcd.setCursor(12,0);
  lcd.print(range_whole);

  last_time_us = current_time_us;
  last_range = range;
  
  

  delay(1000);
  
}
