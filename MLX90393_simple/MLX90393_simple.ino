#include <MLX90393.h>

//#include <Wire.h>
//#include <Arduino.h>

// prints Bx By Bz (in uT) and temperature (C) to serial console after receiving a character

MLX90393 mlx;

void setup(){
  // DRDY line connected to A3 (omit third parameter to used timed reads)
  //uint8_t status = mlx.begin(0, 0, A3);
  
  // this seems to un-funk other i2c stuff for esp32.  My board has SDA and SCL 
  //  mapped to these pins.

  Serial.begin(9600);
  Serial.println("hello");
  
  Wire.begin();

  delay(1000);
  uint8_t status = mlx.begin(0, 0);
  Serial.println(status);
  //uint8_t status = mlx.begin();
  
  Serial.println("setup done");
}

void loop(){
  MLX90393::txyz data;
  Serial.println("hello");
  //if (Serial.available()){
  //  Serial.read();
    mlx.readData(data);
    Serial.print(data.x);
    Serial.print(" ");
    Serial.print(data.y);
    Serial.print(" ");
    Serial.print(data.z);
    Serial.print(" ");
    Serial.println(data.t);
  //}
  delay(1000);
}
