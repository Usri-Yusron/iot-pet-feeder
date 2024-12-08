#include <ESP32Servo.h>           //library buat servo
#include <LiquidCrystal_I2C.h>    //library buat lcd
#include <RTClib.h>              //library buat rtc waktu
#include <Buzzer.h>              //library buat buzzer
#include <WiFi.h>


//tamplet dari blynk
#define BLYNK_TEMPLATE_ID "TMPL6X_AZBiFX"
#define BLYNK_TEMPLATE_NAME "tubes aljut"
#define BLYNK_AUTH_TOKEN "GxFHTHYjiXIIgfoTPyFAoAqWB8BQOIk_"


#include <BlynkSimpleEsp32.h>     //library buat blynk 


RTC_DS1307 rtc;

//variabel buat hari
char daysOfTheWeek[7][12] = {"MINGGU", "SENIN", "SELASA", "RABU", "KAMIS", "JUMAT", "SABTU"};

#define r1 15
#define PIN_TRIG 4
#define PIN_ECHO 5
#define TRIG_PIN 26
#define ECHO_PIN 27
#define I2C_ADDR    0x27
#define LCD_COLUMNS 16
#define LCD_LINES   2
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);



Servo myservo;

#define merah 12        //pin led
#define BuzzerPIN 33    //pin buzzer

int Statuspakan;
int Statusminum;
int pos = 0;


void setup() {
  Serial.begin(115200);
  
  pinMode(r1, OUTPUT);

  //konek ke wifi
  WiFi.begin("ada gak", "kosongsemua");
  //UJI WIFI
  while(WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(500);
    }

    Serial.println("wifi terkoneksi");
    
    Blynk.begin(BLYNK_AUTH_TOKEN, "ada gak", "kosongsemua");

    Serial.println("blynk terkoneksi");
   
    
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(BuzzerPIN, OUTPUT);
  pinMode(merah, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("Hello, GAES!");  
  delay(2000);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("MESIN READY"); 
  delay(2000);

  myservo.attach(18);
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }
}

void berimakan () {

  for (pos = 50; pos >= 0; pos -= 1) { // goes from 90 degrees to 180 degrees
  //in steps of 1 degree
  myservo.write(pos);              // tell servo to go to position in variable 'pos'
  delay(20);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 0; pos <= 50; pos += 1) { // goes from 180 degrees to 0 degrees
  myservo.write(pos);              // tell servo to go to position in variable 'pos'
  delay(20);                       // waits 15ms for the servo to reach the position
  }

}

void beriminum() {
  digitalWrite(r1, HIGH);
  delay(10000);
  digitalWrite(r1, LOW);
  delay(100);
}


void loop() {
  // Start a new measurement:
  //cek blynk
  Blynk.run();


  //nampilin statuspakan di serialmonirot
  Serial.println("Status pakan : " + String(Statuspakan));

  //nampilin statusminum di serialmonirot
  Serial.println("Status minum: " + String(Statusminum));
 
  
  //bagian inti
  if(Statuspakan == 1)
  {
    //panggil si void berimakan nya
    berimakan();
    //kembaliin ke sudut awal tanpa looping
    Blynk.virtualWrite(V0, 0);
    Statuspakan = 0;
    delay(2000);
    }

  if (Statusminum == 1) 
  {
    //panggil si void beriminum nya
    beriminum();
    Blynk.virtualWrite(V1, 0);
    Statusminum = 0;
    delay(2000);
  }
  
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(50);
  digitalWrite(PIN_TRIG, LOW);

  // Read the result:
  int duration = pulseIn(PIN_ECHO, HIGH);

  int cm = duration / 58;
  Serial.print("Jarak - CM : ");
  Serial.print(cm);

  int inch = duration / 148;
  Serial.print(" Jarak - inc: ");
  Serial.print(inch);

  Serial.println();



  if (cm > 9){
    lcd.setCursor(2, 1);
    lcd.print("makanan habis");
    digitalWrite(merah, HIGH);   
    tone(BuzzerPIN, 1000, 5000);
    delay(1000);
    digitalWrite(merah, LOW);
    delay(1000);

  } else {
    lcd.setCursor(2, 1);
    lcd.print("makanan penuh");
    digitalWrite(merah, LOW);
  }

  
    //BUAT SENSOR SATUNYA LAGI
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(50);
  digitalWrite(TRIG_PIN, LOW);

  // Read the result:
  int dur = pulseIn(ECHO_PIN, HIGH);

  int centi = dur / 58;
  Serial.print("Jarak - centi minuman - : ");
  Serial.print(centi);

  int inc = dur / 148;
  Serial.print(" Jarak - inc minuman: ");
  Serial.print(inc);

  Serial.println();



  if (centi > 9 ){
    lcd.setCursor(2, 1);
    lcd.print("minuman habis");
    digitalWrite(merah, HIGH);    
    tone(BuzzerPIN, 1000, 5000);
    delay(1000);
    digitalWrite(merah, LOW);
    delay(1000);

  } else {
    lcd.setCursor(2, 1);
    lcd.print("makanan penuh");
    digitalWrite(merah, LOW);
  }
  
  DateTime now = rtc.now();
  Serial.print("Current time: ");  
  lcd.setCursor(0, 0);
  lcd.print("");
  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
  lcd.print(" ");
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  lcd.print(now.minute(), DEC);
  lcd.print(" WIB");

  Serial.print(now.second(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);



  Serial.println();
  delay(20);
}

  //baca datastream statuspakan
  BLYNK_WRITE(V0)
  {
    //BACA KE DATA STREAMNYA MASUKIN KE STATUSPAKAN
    Statuspakan = param.asInt ();
    }
    
    //baca datastream statusminum
   BLYNK_WRITE(V1) {
  Statusminum = param.asInt();
}
