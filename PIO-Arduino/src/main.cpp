/*=====Headers and Libraries=====*/
#include <Arduino.h>
#include <Ultrasonic.h>
#include <SoftwareSerial.h>
#include <GPRS_Shield_Arduino.h>

/*=====Macros=====*/
#define ID "238939abacdfe"  //Device ID
#define ECHO 11
#define TRIG 12
#define BAUDRATE 9600


/*=====Globals=====*/
Ultrasonic us1(TRIG, ECHO);

String HTTP_HOST = "https://cleanurge.herokuapp.com/";
int PORT = 80;
int bin_height = 200;
int waste_threshold = 90; //90%

//timing variables
unsigned long last_time = 0; //used to tick seconds, uses AVR Timer0
uint8_t seconds = 0;//counts 0 to 59
uint8_t minutes = 0;//counts 0 to 59
uint8_t hours = 0;//counts 0 to 23
uint8_t days = 0;//counts 0 to 6
//variable to store HTTP request rate
uint8_t http_timing = 1; //in hr
uint8_t http_event = 0; //counter variable

//variable to store debug message rate
uint8_t debug_event = 0;  //counter variable
//Overflow flags
bool s_ovf;
bool m_ovf;
bool h_ovf;
bool booted;  //used to do tasks once on boot

/*=====Function Prototypes=====*/
//All the function prototypes will be declared here
void init_gprs();
void init_http();
void init_sensor();
int fetch_sensor_data();  //in cm
void send_data_http();
//counter logics
void tick_seconds();
void tick_minutes();
void tick_hours();
void tick_days();

/*=====Main Functions=====*/
void setup() {
  //Serial Monitor
  Serial.begin(9600);
  //Setting up IOs - if any

  //Setting up objects/peripherals and connection
  init_gprs();
  init_http();
  init_sensor();
}

void loop() {
  //Put the logic for function loop()
  /*===================Alerts (if any)=====================*/
  //tick the timing functions
  tick_seconds();
  tick_minutes();
  tick_hours();

  //checking the sensor data in every loop
  int waste_level = fetch_sensor_data();

  //Schedule to send sensor stats every 1hr
  //checking if there is overlow of waste or not - comparing with variable "waste_threshold"
  // overflow results in calling the send
  if(http_event == http_timing || waste_level >= waste_threshold)
  {
    http_event = 0;//reset counter
    send_data_http();
  }

  //debugging section
  if(debug_event == 1)
  {
    debug_event = 0;
    //put debug messages here
    Serial.print("Waste level: ");
    Serial.println(waste_level);
    Serial.print("Seconds: ");
    Serial.println(seconds);
    Serial.print("Minutes: ");
    Serial.println(minutes);
    Serial.print("Hours: ");
    Serial.println(hours);
  }
}


/*=====User Functions go here=====*/
void init_gprs()
{
  //TODO - add the GPRS setup logic here
}
void init_http()
{
  //TODO - add HTTP connection logic
  //Checkout https://cleanurge.herokuapp.com/docs/ for accessing the routes
}
void init_sensor()
{
  //TODO - add sensor setup logic (if any)
  //sends data after boot to show device is online
  fetch_sensor_data();
  send_data_http();
}
int fetch_sensor_data()
{
  //TODO - debug logic to fetch sensor reading
  int reading = us1.read(); //reading in centimeters
  reading = map(reading, bin_height, 10,  //minimum is taken as 10cm
                0, 100);  //gives result in percentage (integer)
  return reading;
}

void send_http_alive()
{
  //GET method (/api/beacon/ID)
  //Send the ID as param
  //receive the waste threshold and update it
}

void send_data_http()
{
  //TODO - send the location and the level of waste
  //Checkout https://cleanurge.herokuapp.com/docs/ for accessing the routes

  //PUT method (/api/beacon/ID)
  //Send level (and coordinate once on boot) in request
  //receive the OK status
}

void tick_seconds()
{
  // updates every 1000ms or 1s
  uint16_t diff = millis() - last_time;
  if(diff >= 1000){
    last_time = millis();
    seconds+= (diff/1000);
    if(seconds >= 60)
    {
      s_ovf = true;
      seconds = 0;
    }
    //put task for every sec. here
    debug_event++;
  }
}
void tick_minutes()
{
  //updates every 60s or 1min
  if(s_ovf)
  {
    s_ovf = false;
    minutes++;
    if(minutes >= 60)
    {
      m_ovf = true;
      minutes = 0;
    }
    //put task for every min. here

  }
}
void tick_hours()
{
  //Updates every 60min or 1hr
  if(m_ovf)
  {
    m_ovf = false;
    hours++;
    if(hours >= 24)
    {
      h_ovf = true;
      hours = 0;
    }
    //put tasks for every hr. here
    http_event++;
  }
}
void tick_days()
{
  //Updates every 24hr or 1day
  if(h_ovf)
  {
    h_ovf = false;
    days = (days + 1) % 7;
  }
  //put tasks for every day here
}