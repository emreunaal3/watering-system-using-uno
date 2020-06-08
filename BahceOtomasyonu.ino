/*
 * timer_blink_print
 *
 * Blinks the built-in LED every half second, and prints a messages every
 * second using the arduino-timer library.
 *
 */

#include <timer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//DEFINE PINS
#define ONE_WIRE_BUS 2
#define relay 5
#define ground_humidity 9

// Herhangi bir OneWire cihazıyla iletişim kurabilmesi için OneWire örneği belirle
// (sadece Maxim/Dallas entegreleriyle değil)
OneWire oneWire(ONE_WIRE_BUS);
// Onewire referansımızı Dallas Sıcaklık olarak belirle
DallasTemperature sensors(&oneWire);

//MAXIMUM VALUES OF TIME
const unsigned long max_hour = 24; //For 24 hour cycle
const unsigned long max_min = 60; //For 60 min cycle
const unsigned long max_sec = 60; //For 60 sec  cycle

//INITILIZE VALUES installation time you have to set before 
byte day = 0;
byte hour = 0;
byte min = 0;
byte sec = 0;

//FOR SEVEN DAYS LEARNING
bool seven_days_learn = true;
float curr_temp;
float temps_for_days[7][24];

//FLATTENING
float flatten [14];


//EXTRACT COLDEST HOUR
byte classifying[24]; //for every hour one cell 
byte inc_hour = 0 ;
byte coldest_hour = 0;  //But we need just first one 
byte second_coldest_hour = 0;
byte third_coldest_hour = 0;
byte first_max_hour = 0;
byte sec_max_hour = 0;
byte third_max_hour = 0;
byte first_max_hour_index = 0;
byte sec_max_hour_index = 0;
byte third_max_hour_index = 0;

//TAKING THE LOWEST HOURS
byte first_hour = 5;
byte second_hour = 0;
byte third_hour = 0;
float first_temp = 40.0;
float second_temp = 40.0;
float third_temp = 40.0;
byte index = 0;

//DURING SEVEN DAYS LEARN WE USE SENSORS FOR WATERİNG
float watering_temp_limit = 20.00 ;
bool water_flag = false ;

//WATERING TIME FOR HOT OR COLD DAYS 
long water_demand = 2700000; //for default
byte watering_count = 0;
bool calculation_permission = true;


byte relayState = 0;
auto timer = timer_create_default(); // create a timer with default settings


void WateringGarden(){
      digitalWrite(relay, LOW);   // turn the Relay on (HIGH is the voltage level)
      toggle_led();
      relayState = digitalRead(relay);
      //time_to_buzz();
      Serial.print("relay is :");
      Serial.println(relayState);
      Serial.println("Sulanıyor.");
      if(seven_days_learn == true){
        delay(2703000);              // wait for 1 hour =3603000  45 min = 2700000  30 min = 1800000  15 min = 900000  3 min =180000
        min = min + 45;
        sec = sec + 3;
      }
      else{
        delay(water_demand);
        if(water_demand == 4503000){
          hour = hour + 1;
          min = min + 15;
          sec = sec + 3;
        }
        else if(water_demand == 3603000){
          hour = hour + 1;
          sec = sec + 3;
        }
        else if(water_demand == 2703000){
          min = min + 45;
          sec = sec + 3;
        }
        else if(water_demand == 1803000){
          min = min + 30;
          sec = sec + 3;
        }
      }
      digitalWrite(relay, HIGH);    // turn the Relay off by making the voltage LOW
      relayState = digitalRead(relay);
      //time_to_buzz();
      Serial.print("relay is :");
      Serial.println(relayState);
      Serial.println("Sulama kesildi.");
      delay(200);              // wait for 200 second
      watering_count = 0 ;  //it calculates how much time after watering our plant need water
      calculation_permission = true;  //allowed to calculate water demand again.
}

void calc_water_demand(){
  if(calculation_permission == true)
    watering_count++;
    if( watering_count < 10 && digitalRead(ground_humidity) == true){
      water_demand = 4503000;
      calculation_permission = false;
    }
    else if( watering_count < 15 && digitalRead(ground_humidity) == true){
      water_demand = 3603000;
      calculation_permission = false;
    }
    else if( watering_count < 20 && digitalRead(ground_humidity) == true){
      water_demand = 2703000;
      calculation_permission = false;
    }
    else if( watering_count < 24 && digitalRead(ground_humidity) == true){
      water_demand = 1803000;
      calculation_permission = false;
    }
    else{
      water_demand = 1803000; //yeniyazıldı.
      calculation_permission = false;
    }
}


void toggle_led() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // toggle the LED
  Serial.println("Işık yandığı sürece o gün içinde sulama yapılmıştır. ");
  //Serial.println(millis());
}


float take_temp_now(){
  sensors.requestTemperatures();  
  Serial.print("Temperature is: ");
  Serial.println(sensors.getTempCByIndex(0));

  return sensors.getTempCByIndex(0);
}

void timeCheck(){
  if (sec >= max_sec){
    min++;
    sec = 0;
  }
  
  if(min >= max_min ){
    hour++;
    min = 0;
    calc_water_demand();
  }

  if(hour ==(first_max_hour_index -2)){
    if(seven_days_learn == false){
        WateringGarden();
      }
  }
  if(hour == 24){
    if(day < 8){
        day++;
        water_flag = false;
      }
      hour = 0;
      toggle_led();
      Serial.println("Yeni gün.");
  }
  /*switch (hour)
  {
    case (first_max_hour -2): //max_hours dan 2 saat önce SULAMAYA BASLA (24 SAATTE BIR KERE).Çünkü akşam soğuğunu yemesi gerekiyor bitkinin.
      if(seven_days_learn == false){
        WateringGarden();
      }
      break;
    case sec_max_hour: //SAAT 23 OLUNCA SULAMAYA BASLA (24 SAATTE BIR KERE)
      if(min >= 0 && min < 45 ){
        WateringGarden();
      }
      break;
    case third_max_hour: //SAAT 23 OLUNCA SULAMAYA BASLA (24 SAATTE BIR KERE)
      if(min >= 0 && min < 45 ){
        WateringGarden();
      }
      break;
    case (24):
      if(day < 8){
        day++;
        water_flag = false;
      }
      hour = 0;
      Serial.println("Yeni gün.");
  }  */
}



void before_learning_watering(){
  if(seven_days_learn == true){
    if( (take_temp_now() <  watering_temp_limit && water_flag == false)  ){     //Sensör verisi eşik değerini geçerse if içerisindeki kodlar uygulanır.
      WateringGarden();
      water_flag = true;
    }
  }
}

void extract_learn_result(){
  //Make all zero for classify
  for(int i = 0; i < 24 ; i++){
    classifying[i] = 0;
  }
  //How many times did we measure hours as a lowest
  for(int i = 0 ; i < 14; i++){
    inc_hour = flatten[i];
    classifying[inc_hour]++; 
  }
  //taking the best hour for watering
  for(int i = 0 ; i < 24; i++){
    if(classifying[i] > first_max_hour){
       third_max_hour = sec_max_hour;
       sec_max_hour = first_max_hour;
       third_max_hour_index = sec_max_hour_index;
       sec_max_hour_index = first_max_hour_index;
       first_max_hour = classifying[i];
       first_max_hour_index = i;
    }
    else if(classifying[i] > sec_max_hour && classifying[i] <= first_max_hour){
      third_max_hour = sec_max_hour ;
      third_max_hour_index = sec_max_hour_index;
      sec_max_hour = classifying[i];
      sec_max_hour_index = i;
    }
    else if(classifying[i] > third_max_hour && classifying[i] < first_max_hour && classifying[i] <= sec_max_hour){
      third_max_hour = classifying[i];
      third_max_hour_index = i;
    }
      Serial.println(" ");
      Serial.print("max hour  ");
      Serial.print(first_max_hour_index);
      Serial.print("sec max hour  ");
      Serial.print(sec_max_hour_index);
      Serial.print("third max hour ");
      Serial.print(third_max_hour_index);
  }
}

void take_the_lowest_hours(){
  for(int i = 0; i < 7; i++){
    for(int j = 0; j < 24; j++){
      if(temps_for_days[i][j] < first_temp){
        third_temp = second_temp;
        third_hour = second_hour;
        second_temp = first_temp;
        second_hour = first_hour;
        first_temp = temps_for_days[i][j] ;
        first_hour = j;
      }
      else if(temps_for_days[i][j] < second_temp &&  temps_for_days[i][j] >= first_temp ){
        third_temp = second_temp;
        third_hour = second_hour;
        second_temp = temps_for_days[i][j] ;
        second_hour = j;
      }
      else if(temps_for_days[i][j] < third_temp && temps_for_days[i][j] > first_temp && temps_for_days[i][j] >= second_temp ){
        third_temp = temps_for_days[i][j] ;
        third_hour = j;
      }
    }
    //Flattening
    flatten[index] = first_hour;
    flatten[index+1] = second_hour;
    index+=2;
    first_temp = 40.0;
    second_temp = 40.0;
    third_temp = 40.0;
  }
}


bool seven_days_learning(void*){
  if(day < 7){
    curr_temp = take_temp_now();
    for(int i=0; i < 24;i++){
      temps_for_days[day][hour] = curr_temp;
      Serial.print("DAY:");
      Serial.print(day);
      Serial.print("HOUR:");
      Serial.print(hour);
      Serial.print(", temp:");
      Serial.println(curr_temp);
      Serial.println("olarak kaydedildi.");
    }
    before_learning_watering();
  }
  else{
    take_the_lowest_hours();
    extract_learn_result();
    seven_days_learn = false;
  }
  
  return seven_days_learn;
}

bool control(void*){
  sec++;
  timeCheck();
  Serial.print("S:");
  Serial.print(hour);
  Serial.print(" DK:");
  Serial.print(min);  
  Serial.print(" SN:");
  Serial.println(sec);
  take_temp_now();
  Serial.print("Humidity is under the limit? : ");
  Serial.println(digitalRead(ground_humidity));
  return true;
}

void setup() {
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  Serial.println("5. PIN ROLE OLARAK AYARLANDI.");
  pinMode(ground_humidity, INPUT);
  Serial.println("9. PIN TOPRAK NEM SENSORU VE INPUT OLARAK AYARLANDI.");
  pinMode(LED_BUILTIN, OUTPUT); // set LED pin to OUTPUT
  Serial.println("LED OUTPUT OLARAK AYARLANDI.");
  for(int i = 0; i < 7; i++){
    for(int j = 0; j < 24; j++){
      temps_for_days[i][j] = 30;
    }
  }
  
      curr_temp = take_temp_now();
      temps_for_days[day][hour] = curr_temp;  // ilk ölçüm kurulduğu anda alınıcak. o saati de dahil edebilmek için.
      Serial.print("DAY:");
      Serial.print(day);
      Serial.print("HOUR:");
      Serial.print(hour);
      Serial.print(", temp:");
      Serial.println(curr_temp);
      Serial.println("olarak kaydedildi.");
  sensors.begin(); //to begin communication bw sensors and processor
  timer.every(1000, control);
  timer.every(3600300, seven_days_learning);
  // call the control function every 1000 millis (1 second)
  
}

void loop() {
  timer.tick(); // tick the timer
}
