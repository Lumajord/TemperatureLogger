#include <avr/power.h>
#include "LowPower.h"

#define RTC_POWER_PIN 8
#define DHT_POWER_PIN 7
#define MOSFET_PIN 9


//#define DEBUG
#define PRESCALE clock_div_16

#include "Wire.h"
#include <DS3231.h>
// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
// Init a Time-data structure
Time  t;


#include "DHTP.h"
#define DHTPIN 6
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
float hum;
float tem;



#include <SPI.h>
#include "SdFat.h"


void Print(const char* text)
{
  clock_prescale_set(clock_div_1);
  delay(1);
  Serial.print(text);
  delay(10);
  clock_prescale_set(PRESCALE);  
}
void Println(const char* text)
{
  clock_prescale_set(clock_div_1);
  delay(1);
  Serial.println(text);
  delay(10);
  clock_prescale_set(PRESCALE);  
}
void Println(unsigned long num)
{
  clock_prescale_set(clock_div_1);
  delay(1);
  Serial.println(num);
  delay(10);
  clock_prescale_set(PRESCALE);  
}


void set_time()
{

  
  int hours = (__TIME__[0] - '0')*10 + __TIME__[1] - '0';
  int minutes = (__TIME__[3] - '0')*10 + __TIME__[4] - '0';
  int seconds = (__TIME__[6] - '0')*10 + __TIME__[7] - '0' + 6;


  const char *monthArr[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

  int month = 0;
  for(int i = 0; i < 12; ++i)
  {
    if(__DATE__[0] == monthArr[i][0] &&
      __DATE__[1] == monthArr[i][1] &&
      __DATE__[2] == monthArr[i][2])
      {
        month = i+1;
        break;
      }
  }

  int day = (__DATE__[4] - '0')*10 + (__DATE__[5] - '0');
  int yr  = (__DATE__[9] - '0')*10 + (__DATE__[10] - '0') + 2000;


  if(seconds > 59)
  {
    seconds = seconds % 60;
    ++minutes;
  }

  if(minutes > 59)
  {
    minutes = minutes % 60;
    ++hours;
  }

  if(hours > 23)
  {
    hours = hours % 24;
    ++day;
  }

#ifdef DEBUG
  clock_prescale_set(clock_div_1);
  delay(1);
  //careful: month is not corrected if 6 seconds shift jumps into new month
  Serial.print("Setting Date and Time to:  ");
  Serial.print(day);
  Serial.print('.');
  Serial.print(month);
  Serial.print('.');
  Serial.print(yr);
  Serial.print('\t');
  Serial.print(hours);
  Serial.print(':');
  Serial.print(minutes);
  Serial.print(':');
  Serial.print(seconds);
  Serial.print('\n');
  delay(100);
  clock_prescale_set(PRESCALE); 
  #endif // DEBUG

  pinMode(RTC_POWER_PIN, OUTPUT);
  digitalWrite(RTC_POWER_PIN, HIGH);
  delayMicroseconds(30); // Wait till powered 

  Wire.begin();
  rtc.begin();
  delayMicroseconds(10);
  
  // The following lines can be uncommented to set the date and time
  rtc.setTime(hours, minutes, seconds);     // hours, minutes, seconds
  rtc.setDate(day, month, yr);   // day, month, year

  delayMicroseconds(62);


    // turn off I2C
  TWCR &= ~(bit(TWEN) | bit(TWIE) | bit(TWEA));

  // turn off I2C pull-ups
  digitalWrite (A4, LOW);
  digitalWrite (A5, LOW);

  digitalWrite(RTC_POWER_PIN, LOW);
  
}



void power_off_all()
{
  // set all pins to OUTPUT
  for(int i = 0; i < 20; ++i)
  {
    pinMode(i, OUTPUT);
  }

  // set pins to LOW
  for(int i = 0; i < 14; ++i)
  {
    digitalWrite(i, LOW);
  }


  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, HIGH); // Turn off mosfet


  // disable all modules
  #ifndef DEBUG
  //power_all_disable();
  #endif
  // Disable the ADC by setting the ADEN bit (bit 7)  of the
  // ADCSRA register to zero.
  ADCSRA = ADCSRA & B01111111;
  
  // Disable the analog comparator by setting the ACD bit
  // (bit 7) of the ACSR register to one.
  ACSR = B10000000;

  ADMUX = 0;  // turn off internal Vref

    // turn off I2C
  TWCR &= ~(bit(TWEN) | bit(TWIE) | bit(TWEA));

  // turn off I2C pull-ups
  digitalWrite (A4, LOW);
  digitalWrite (A5, LOW);


#ifdef DEBUG
  Print("Power OFF\n");
#endif // DEBUG
    
}


void flashLED (const byte times)
  {
  // flash LED
  for (int i = 0; i < times; i++)
    {
    digitalWrite (LED_BUILTIN, HIGH);
    delay (1);  
    digitalWrite (LED_BUILTIN, LOW);
    delay (12);  
    }
  }  // end of 


void setup() {

#ifdef DEBUG
    Serial.begin(19200);
    while (!Serial)
    {
      ; // wait for serial port to connect. Needed for native USB port only
    }
    set_time();
#endif // DEBUG

//delayMicroseconds

  clock_prescale_set(PRESCALE);
  flashLED(4);
  power_off_all();

  
}





void get_time()
{

  pinMode(RTC_POWER_PIN, OUTPUT);
  digitalWrite(RTC_POWER_PIN, HIGH);
  delayMicroseconds(30); // Wait till powered 

  Wire.begin();
  rtc.begin();
  delayMicroseconds(10);

   // Get data from the DS3231
  t = rtc.getTime();

  delayMicroseconds(50);

  // turn off I2C
  TWCR &= ~(bit(TWEN) | bit(TWIE) | bit(TWEA));

  // turn off I2C pull-ups
  digitalWrite (A4, LOW);
  digitalWrite (A5, LOW);

  digitalWrite(RTC_POWER_PIN, LOW);

}



void get_T()
{

  pinMode(DHT_POWER_PIN, OUTPUT);
  digitalWrite(DHT_POWER_PIN, HIGH);
  delay(1);
  
  dht.begin();
  
  //delay(125);
  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);  

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  hum = dht.readHumidity();
  // Read temperature as Celsius (the default)
  tem = dht.readTemperature();

  
  digitalWrite(DHTPIN, LOW);
  digitalWrite(DHT_POWER_PIN, LOW);
  
}




void write_data()
{

  // Turn on mosfet
  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, LOW);

  delayMicroseconds(50);

  clock_prescale_set(clock_div_1);

  SdFat SD;

  if (!SD.begin(10, SPI_HALF_SPEED))
  {
    Serial.println("SD error!");
    return;
  }

  File myFile;

  SPI.begin();

  if(!myFile.open("data.dat", O_CREAT | O_WRITE | O_APPEND))
  {
#ifdef DEBUG
    Print("Could not open file!\n");
#endif // DEBUG
    flashLED(1);
    return;
  }

    myFile.seekEnd();
  
    myFile.print(t.date,DEC);
    myFile.print('.');  
    myFile.print(t.mon,DEC);
    myFile.print('.');
    myFile.print(t.year,DEC);
    myFile.print(' ');
    myFile.print(t.hour,DEC);
    myFile.print(':');
    myFile.print(t.min,DEC);
    myFile.print(':');
    myFile.print(t.sec,DEC);
    myFile.print('\t');
    myFile.print(tem, 1);
    myFile.print("\t");
    myFile.print(hum, 1);
    myFile.print('\n');
    myFile.sync();
    myFile.close();

  delay(30);
  SPI.end();

  clock_prescale_set(PRESCALE);

  
  digitalWrite(MOSFET_PIN, HIGH); // Turn off mosfet

  flashLED(2);

#ifdef DEBUG
  clock_prescale_set(clock_div_1);

  // DISPLAY DATA
  Serial.print(t.date,DEC);
  Serial.print('.');  
  Serial.print(t.mon,DEC);
  Serial.print('.');
  Serial.print(t.year,DEC);
  Serial.print(' ');
  Serial.print(t.hour,DEC);
  Serial.print(':');
  Serial.print(t.min,DEC);
  Serial.print(':');
  Serial.print(t.sec,DEC);
  Serial.print('\t');
  Serial.print(tem, 1);
  Serial.print("\t");
  Serial.print(hum, 1);
  Serial.print('\n');

  delay(50);
  clock_prescale_set(PRESCALE);

  #endif // DEBUG

}




void loop() {
  
  flashLED(3);

  get_T();
  
  get_time();

  unsigned long StartTime = millis();
  
  write_data();

  unsigned long CurrentTime = millis();
  unsigned long ElapsedTime = CurrentTime - StartTime;

  Print("write time = ");
  Println(ElapsedTime);

  for(int i = 0; i < 20; ++i)
  {
    pinMode(i, OUTPUT);
    if(i < 14)
    {
      digitalWrite(i, LOW);
    }
  }

  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, HIGH);

  #ifndef DEBUG
  //power_all_disable();
  #endif

  for(int i = 0; i < 37; ++i)
  {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);  
  }
  
}
