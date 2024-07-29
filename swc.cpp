#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <chrono>
#include <thread>
#include <iomanip>


float read_value_ss(int fd, int adcIndex)
{
  const uint8_t regSelect[1] = { 0 };

  uint8_t adcIndexBits[] = { 0b100, 0b101, 0b110, 0b111 };

  // ADC config package
  // bit    | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  |
  // byte 1 | op |      adc #   |     gain     | op |
  // byte 2 |  data rate   |    comparator opts     |


  uint8_t configBuf[] = {
    1,
    0b10000011 | (adcIndexBits[adcIndex] << 4),
    0b10100000,
  };


  int stable_value = -1;
  for (int i = 0; i < 5; i++) {
    try {

      if (write(fd, configBuf, 3) != 3) { throw -1; }

      uint8_t isReadyCheck[1] = { 0 };
      do {
        if (read(fd, &isReadyCheck, 1) != 1) { throw -2; }
      } while ((isReadyCheck[0] & 0b10000000) == 0);


      if (write(fd, &regSelect, 1) != 1) { throw -3; }

      uint8_t valueBuf[2];

      if (read(fd, valueBuf, 2) != 2) { throw -4; }

      int value = (valueBuf[0] << 8) + valueBuf[1];
      stable_value = stable_value == -1 ? value : ((stable_value + value) / 2);


    } catch (int errorNumber) {
      std::cout << "FAILED TO READ ADC " << errorNumber << "\n";
    }
  }


  float vout = (stable_value / 32768.0) * 4.096;

  return vout;
}




int measure_resistance(float vout, int base_resistance)
{
  return (vout * base_resistance) / (3.3000 - vout);
}





int main(int argc, char* argv[])
{
  const int i2c_address = 0x48;

  int fd;

  // open device on /dev/i2c-1 the default on Raspberry Pi B
  if ((fd = open("/dev/i2c-1", O_RDWR)) < 0) {
    printf("Error: Couldn't open device! %d\n", fd);
    exit (1);
  }

  // connect to ADS1115 as i2c slave
  if (ioctl(fd, I2C_SLAVE, i2c_address) < 0) {
    printf("Error: Couldn't find device on address!\n");
    exit (1);
  }



  while (true) {
    float value_zero = read_value_ss(fd, 0);
    float value_one = read_value_ss(fd, 1);
    // int value_one = 0;
    std::cout << "Reading: " << std::fixed << std::setprecision(5) << std::setw(5) << measure_resistance(value_zero, 330) << "     " << measure_resistance(value_one, 330) << "\n";
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }







  // set config register and start conversion
  // AIN0 and GND, 4.096v, 128s/s
  // Refer to page 19 area of spec sheet
  // writeBuf[0] = 1; // config register is 1
  // writeBuf[1] = 0b11000011; // 0xC2 single shot off



  // bit 15 flag bit for single shot not used here
  // Bits 14-12 input selection:
  // 100 ANC0; 101 ANC1; 110 ANC2; 111 ANC3
  // Bits 11-9 Amp gain. Default to 010 here 001 P19
  // Bit 8 Operational mode of the ADS1115.
  // 0 : Continuous conversion mode
  // 1 : Power-down single-shot mode (default)



  // ==================================== SINGLE SHOT MODE
  // writeBuf[1] = 0b11000011;
  // 15 = 1 = force wake
  // 14-12 = 100 = ANC0
  // 11-9 = 001 = 4.096V
  // 8 = 1 = single shot



  // =================================== CONTINUOUS MODE
  // writeBuf[1] = 0b01000010;
  // 15 = 0 = not use for cont
  // 14-12 = 100 = ANC0
  // 11-9 = 001 = 4.096V
  // 8 = 1 = single shot


  // =================================== COMPARATOR STUFF

  // writeBuf[2] = 0b10000000; // bits 7-0  0x85
  // Bits 7-5 data rate default to 100 for 128SPS
  // Bits 4-0  comparator functions see spec sheet.






  return 0;

}
