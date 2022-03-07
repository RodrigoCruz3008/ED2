
#include <xc.h>
#include "I2C.h"
#include "MPU6050.h"
#include "LCD.h"
#include "UART.h"
#include <stdio.h>

//-------------[ MPU6050 Routines ]------------------
//---------------------------------------------------

void MPU6050_Init() {
  // Power-Up Delay & I2C_Init
  __delay_ms(100);
  I2C_Master_Init(100000);
  
  // Setting The Sample (Data) Rate
  I2C_Start(0xD0);
  I2C_Master_Write(SMPLRT_DIV);
  I2C_Master_Write(0x07);
  I2C_Master_Stop();
 
  // Setting The Clock Source
  I2C_Start(0xD0);
  I2C_Master_Write(PWR_MGMT_1);
  I2C_Master_Write(0x01);
  I2C_Master_Stop();
 
  // Configure The DLPF
  I2C_Start(0xD0);
  I2C_Master_Write(CONFIG);
  I2C_Master_Write(0x00);
  I2C_Master_Stop();
 
  // Configure The ACCEL (FSR= +-2g)
  I2C_Start(0xD0);
  I2C_Master_Write(ACCEL_CONFIG);
  I2C_Master_Write(0x00);
  I2C_Master_Stop();
 
  // Configure The GYRO (FSR= +-2000d/s)
  I2C_Start(0xD0);
  I2C_Master_Write(GYRO_CONFIG);
  I2C_Master_Write(0x18);
  I2C_Master_Stop();
 
  // Enable Data Ready Interrupts
  I2C_Start(0xD0);
  I2C_Master_Write(INT_ENABLE);
  I2C_Master_Write(0x01);
  I2C_Master_Stop();
}

void MPU6050_Read(void)
{
  int Ax,Ay,Az,Gx,Gy,Gz,T;
  //int AX, AY, AZ, GX, GY, GZ;
  int t;
  // Prepare For Reading, Starting From ACCEL_XOUT_H
  I2C_Start(0xD0);
  I2C_Master_Write(ACCEL_XOUT_H);
  I2C_Master_Stop();
  I2C_Start(0xD1);
  Ax = ((int)I2C_Read(0)<<8) | (int)I2C_Read(0);
  Ay = ((int)I2C_Read(0)<<8) | (int)I2C_Read(0);
  Az = ((int)I2C_Read(0)<<8) | (int)I2C_Read(0);
  T  = ((int)I2C_Read(0)<<8) | (int)I2C_Read(0);
  Gx = ((int)I2C_Read(0)<<8) | (int)I2C_Read(0);
  Gy = ((int)I2C_Read(0)<<8) | (int)I2C_Read(0);
  Gz = ((int)I2C_Read(0)<<8) | (int)I2C_Read(1);
  I2C_Master_Stop();
  
  //AX = (int)Ax/16384.0;
  //AY = (int)Ay/16384.0;
  //AZ = (int)Az/16384.0;
  //GX = (int)Gx/131.0 * 100;
  //GY = (int)Gy/131.0 * 100;
  //GZ = (int)Gz/131.0 * 100;
  t = (((int)T/340.00)+36.53) * 10;
  PORTA = t;
  setCursor(2,0);
  writeString("TEMP: ");
  Temp_Conversion(t);
  writeString(" C");
  txfunc(',');
}
void Temp_Conversion (int _temp) {
    int _temp_tenths, _temp_units, _temp_tens;
    int _write_tenths, _write_units, _write_tens;
    _temp_tenths = (_temp%10);
    _temp_units = (_temp%100 - _temp_tenths)/10;
    _temp_tens = (_temp - _temp_units*10 - _temp_tenths)/100;
    
    _write_tenths = _temp_tenths + 48;
    _write_units = _temp_units + 48;
    _write_tens = _temp_tens + 48;
    
    writeChar(_write_tens);
    txfunc(_write_tens);
    
    writeChar(_write_units);
    txfunc(_write_units);
    
    writeChar('.');
    
    writeChar(_write_tenths);
    txfunc(_write_tenths);
}