// Conncetion Infos
#define BLYNK_TEMPLATE_ID "TMPL2Au-0tTGm"
#define BLYNK_TEMPLATE_NAME "poulaillet"
#define BLYNK_AUTH_TOKEN "9x2ZHk0KiEr97J2SNxTCDh8lhQICaZDK"
#define BLYNK_PRINT Serial
// Including libraries
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>


// Auth infos
char auth[] = "9x2ZHk0KiEr97J2SNxTCDh8lhQICaZDK";
char ssid[] = "Wokwi-GUEST"; 
char pass[] = ""; 


// Constants
#define DHTPIN 23          // Mention the digital pin where you connected 
#define DHTTYPE DHT22     // DHT 11  
// Sound speed in air
#define SOUND_SPEED 340
#define TRIG_PULSE_DURATION_US 10
#define POTENTIOMETER_PIN 34 // ESP32 pin GPIO36 (ADC0) connected to Potentiometer pin
#define LED_PIN           16 // ESP32 pin GPIO21 connected to LED's pin
#define ANALOG_THRESHOLD  1000

// Variables
const int trig_pin = 5;
const int echo_pin = 18;
long ultrason_duration;
float distance_cm;
// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
// Global Variables
int temp,humidity,dist;


// Creating Objects
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;



void setup(){
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  dht.begin();

  pinMode(trig_pin, OUTPUT); // We configure the trig as output
  pinMode(echo_pin, INPUT); // We configure the echo as input

  timer.setInterval(2500L, sendTempratureAndHumidity);
  timer.setInterval(2500L, sendDistance);

  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();

  pinMode(LED_PIN, OUTPUT); // set ESP32 pin to output mode
}

void loop(){
  Blynk.run();
  timer.run();
  printDataInLCD();
  controll_LED_UsingPotontiometer();
}


// Function to send the temprature and the humidity to the server cloud
void sendTempratureAndHumidity(){
  float h = dht.readHumidity();
  humidity = h;
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  temp = t;
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V0, t);
  Serial.print("Temprature : ");
  Serial.print(t);
  Serial.print("    Humidity : ");
  Serial.println(h);


  if(t > 30){
   // Blynk.email("isamahfudi08@gmail.com", "Alert", "Temperature over 28C!");
    Blynk.logEvent("temp_alert","Temp above 30 degrees");
  }
}

void sendDistance(){
    // Set up the signal
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
 // Create a 10 µs impulse
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(TRIG_PULSE_DURATION_US);
  digitalWrite(trig_pin, LOW);

  // Return the wave propagation time (in µs)
  ultrason_duration = pulseIn(echo_pin, HIGH);

//distance calculation
  distance_cm = ultrason_duration * SOUND_SPEED/2 * 0.0001;
  dist = distance_cm;
 // send the distance to blynk
   Blynk.virtualWrite(V2, distance_cm);
  // We print the distance on the serial port
  Serial.print("Distance (cm): ");
  Serial.println(distance_cm);

  delay(1000);
}

void printDataInLCD()
{
  //print temprature
  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print message
  lcd.print("Température: ");
  delay(1000);
  // set cursor to first column, second row
  lcd.setCursor(0,1);
  lcd.print(temp);
  delay(1000);
  lcd.clear();
  // *********************
    //print temprature
  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print message
  lcd.print("Humidity: ");
  delay(1000);
  // set cursor to first column, second row
  lcd.setCursor(0,1);
  lcd.print(humidity);
  delay(1000);
  lcd.clear();
}

void controll_LED_UsingPotontiometer()
{
  int analogValue = analogRead(POTENTIOMETER_PIN); // read the input on analog pin

  if (analogValue > ANALOG_THRESHOLD)
    digitalWrite(LED_PIN, HIGH); // turn on LED
  else
    digitalWrite(LED_PIN, LOW);  // turn off LED
}