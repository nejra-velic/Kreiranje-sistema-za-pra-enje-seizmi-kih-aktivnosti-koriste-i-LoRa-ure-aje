#include <TinyGPS.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>


int vibr_pin = 7;
int gyro_x, gyro_y, gyro_z;

long gyro_x_cal, gyro_y_cal, gyro_z_cal;

boolean set_gyro_angles;

long acc_x, acc_y, acc_z, acc_total_vector;

float angle_roll_acc, angle_pitch_acc;

float angle_pitch, angle_roll;
int angle_pitch_buffer, angle_roll_buffer;
float angle_pitch_output, angle_roll_output;

float acc_total_vector1;

long loop_timer;
int temp;

float lat, lon;
unsigned long date, time, fix_age;
TinyGPS gps;

void setup() {
  Serial.begin(57600);
  while (!Serial);

  Serial.println("LoRa Sender");

  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  pinMode(vibr_pin, INPUT);

  Wire.begin();                                                                    
  setup_mpu_6050_registers();                                          

  for (int cal_int = 0; cal_int < 1000 ; cal_int ++) {                 

    read_mpu_6050_data();
    gyro_x_cal += gyro_x;
    gyro_y_cal += gyro_y;
    gyro_z_cal += gyro_z;
    delay(3);
  }
  
  gyro_x_cal /= 1000;
  gyro_y_cal /= 1000;
  gyro_z_cal /= 1000;
  loop_timer = micros();

}

void setup_mpu_6050_registers() {

  Wire.beginTransmission(0x68);                                        
  Wire.write(0x6B);                                                    
  Wire.write(0x00);                                                    
  Wire.endTransmission();

  Wire.beginTransmission(0x68);                                      
  Wire.write(0x1C);                                                    
  Wire.write(0x10);                                                   
  Wire.endTransmission();
 
  Wire.beginTransmission(0x68);                                        
  Wire.write(0x1B);                                                         
  Wire.write(0x08);                                                         
  Wire.endTransmission();
}


void read_mpu_6050_data() {                                           
  Wire.beginTransmission(0x68);                                      
  Wire.write(0x3B);                                                       
  Wire.endTransmission();                                               
  Wire.requestFrom(0x68, 14);                                          
  while (Wire.available() < 14);                                       
  acc_x = Wire.read() << 8 | Wire.read();
  acc_y = Wire.read() << 8 | Wire.read();
  acc_z = Wire.read() << 8 | Wire.read();
  temp = Wire.read() << 8 | Wire.read();
  gyro_x = Wire.read() << 8 | Wire.read();
  gyro_y = Wire.read() << 8 | Wire.read();
  gyro_z = Wire.read() << 8 | Wire.read();
}


void loop() {
   Serial1.begin(9600);
   while(Serial1.available()){
      
    
    if(gps.encode(Serial1.read()))
    { 
    gps.f_get_position(&lat,&lon); 

    Serial.println("The GPS Received Signal:");

    //Latitude
    Serial.print("Latitude: ");
    Serial.println(lat,6);
 
    
    //Longitude
    Serial.print("Longitude: ");
    Serial.println(lon,6); 
    }
   }
   
  
  int val;
  val = digitalRead(vibr_pin);
  if (val == 1)
  {

    read_mpu_6050_data();
   
    gyro_x -= gyro_x_cal;
    gyro_y -= gyro_y_cal;
    gyro_z -= gyro_z_cal;

    angle_pitch += gyro_x * 0.0000611;

    angle_roll += gyro_y * 0.0000611;

    angle_pitch += angle_roll * sin(gyro_z * 0.000001066);             
    angle_roll -= angle_pitch * sin(gyro_z * 0.000001066);              

    //Accelerometer angle calculations

    acc_total_vector = sqrt((acc_x * acc_x) + (acc_y * acc_y) + (acc_z * acc_z));

    angle_pitch_acc = asin((float)acc_y / acc_total_vector) * 57.296;    

    angle_roll_acc = asin((float)acc_x / acc_total_vector) * -57.296;   

    angle_pitch_acc -= 0.0;                                             
    angle_roll_acc -= 0.0;                                              

    if (set_gyro_angles) {
      angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004;

      angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004;
    }
    else {                                                           
      angle_pitch = angle_pitch_acc;               
      angle_roll = angle_roll_acc;                     
      set_gyro_angles = true;                           
    }


    angle_pitch_output = angle_pitch_output * 0.9 + angle_pitch * 0.1;
    angle_roll_output = angle_roll_output * 0.9 + angle_roll * 0.1;

    acc_total_vector = sqrt((acc_x * acc_x) + (acc_y * acc_y) + (acc_z * acc_z));
    acc_total_vector1 = acc_total_vector * 0.0000610352;                 //Skaliranje sa faktorom 16384

    Serial.print ("Ubrzanje: "); Serial.println (acc_total_vector1); 

    LoRa.beginPacket();
    LoRa.print ("Ubrzanje: "); LoRa.print (acc_total_vector1); 
    LoRa.endPacket();
    delay(100);
    while (micros() - loop_timer < 4000);
    loop_timer = micros();
      
      //Latitude
      LoRa.beginPacket();
      LoRa.print("Latitude: ");
      LoRa.print(lat, 6);
      LoRa.endPacket();

      //Longitude
      LoRa.beginPacket();
      LoRa.print("Longitude: ");
      LoRa.println(lon, 6);
      LoRa.endPacket();
    }

    

    

 
  delay(300);
}
