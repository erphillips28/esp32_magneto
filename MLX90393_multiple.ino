//#include <Wire.h>
//#include <Arduino.h>
#include <MLX90393.h>

// prints Bx By Bz (in uT) and temperature (C) to serial console after receiving a character

MLX90393 mlx1;
MLX90393 mlx2;

void setup(){
  // DRDY line connected to A3 (omit third parameter to used timed reads)
  //uint8_t status = mlx.begin(0, 0, A3);
  
  // this seems to un-funk other i2c stuff for esp32.  My board has SDA and SCL 
  //  mapped to these pins.

  Serial.begin(9600);
  Serial.println("hello");
  
  Wire.begin();

  delay(1000);
  uint8_t status = mlx1.begin(0, 0);
  Serial.println(status);
  uint8_t status2 = mlx2.begin(1, 0);
  Serial.println(status2);
  Serial.println( mlx1.begin(0, 0));
  Serial.println(mlx2.begin(1, 0));
  delay(1000);
  Serial.println( mlx1.begin(0, 0));
  Serial.println(mlx2.begin(1, 0));
  delay(1000);
  Serial.println( mlx1.begin(0, 0));
  Serial.println(mlx2.begin(1, 0));
  //uint8_t status = mlx.begin();
  
  Serial.println("setup done");
}

void loop(){
  MLX90393::txyz data1;
  MLX90393::txyz data2;
  Serial.println("hello");
  //if (Serial.available()){
  //  Serial.read();
    mlx1.readData(data1);
    mlx2.readData(data2);
    Serial.println("Magnetometer 1 Readings:");
    Serial.print(data1.x);
    Serial.print(" ");
    Serial.print(data1.y);
    Serial.print(" ");
    Serial.print(data1.z);
    Serial.print(" ");
    Serial.println(data1.t);
    Serial.println("Magnetometer 2 Readings:");
    Serial.print(data2.x);
    Serial.print(" ");
    Serial.print(data2.y);
    Serial.print(" ");
    Serial.print(data2.z);
    Serial.print(" ");
    Serial.println(data2.t); 
  //}
  delay(3000);
}
