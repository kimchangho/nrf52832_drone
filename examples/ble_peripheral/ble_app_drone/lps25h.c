/**
 *    ||          ____  _ __
 * +------+      / __ )(_) /_______________ _____  ___
 * | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie Firmware
 *
 * Copyright (C) 2011 Fabio Varesano <fvaresano@yahoo.it>
 * Copyright (C) 2011-2012 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * @file lps25h.c
 * Driver for the lps25h pressure sensor from ST.
 *
 */

#include "lps25h.h"

#include "stdio.h"
#include "stdlib.h"

#include "nrf_delay.h"

#include "math.h"

// Constants used to determine altitude from pressure
#define CONST_SEA_PRESSURE 102610.f //1026.1f //http://www.meteo.physik.uni-muenchen.de/dokuwiki/doku.php?id=wetter:stadt:messung
#define CONST_PF 0.1902630958f //(1/5.25588f) Pressure factor
#define CONST_PF2 44330.0f
#define FIX_TEMP 25         // Fixed Temperature. ASL is a function of pressure and temperature, but as the temperature changes so much (blow a little towards the flie and watch it drop 5 degrees) it corrupts the ASL estimates.
                            // TLDR: Adjusting for temp changes does more harm than good.

// addresses of the device
#define LPS25H_I2C_ADDR 0x5D // 1011101b

#define LPS25H_LSB_PER_MBAR      4096UL
#define LPS25H_LSB_PER_CELSIUS   480UL
#define LPS25H_ADDR_AUTO_INC     (1<<7)

// register addresses

// register addresses
// Note: Some of the register names in the datasheet are inconsistent
// between Table 14 in section 6 and the register descriptions in
// section 7. Where they differ, the names from section 7 have been
// used here.

#define LPS25H_REF_P_XL       0x08
#define LPS25H_REF_P_L        0x09
#define LPS25H_REF_P_H        0x0A

#define LPS25H_WHO_AM_I       0x0F

#define LPS25H_RES_CONF       0x10

#define LPS25H_CTRL_REG1      0x20
#define LPS25H_CTRL_REG2      0x21
#define LPS25H_CTRL_REG3      0x22
#define LPS25H_CTRL_REG4      0x23
#define LPS25H_INTERRUPT_CFG  0x24
#define LPS25H_INT_SOURCE     0x25
#define LPS25H_STATUS_REG     0x27

#define LPS25H_PRESS_OUT_XL   0x28
#define LPS25H_PRESS_OUT_L    0x29
#define LPS25H_PRESS_OUT_H    0x2A

#define LPS25H_TEMP_OUT_L     0x2B
#define LPS25H_TEMP_OUT_H     0x2C

#define LPS25H_FIFO_CTRL      0x2E
#define LPS25H_FIFO_STATUS    0x2F
#define LPS25H_THS_P_L        0x30
#define LPS25H_THS_P_H        0x31
#define LPS25H_RPDS_L         0x39
#define LPS25H_RPDS_H         0x3A

/* CUSTOM VALUES FOR LPS25H SENSOR */
#define LPS25H_WAI_ID             0xBD
#define LPS25H_ODR_ADDR           0x20
#define LPS25H_ODR_MASK           0x70
#define LPS25H_ODR_AVL_1HZ_VAL    0x01
#define LPS25H_ODR_AVL_7HZ_VAL    0x02
#define LPS25H_ODR_AVL_13HZ_VAL   0x03
#define LPS25H_ODR_AVL_25HZ_VAL   0x04
#define LPS25H_PW_ADDR            0x20
#define LPS25H_PW_MASK            0x80
#define LPS25H_FS_ADDR            0x00
#define LPS25H_FS_MASK            0x00
#define LPS25H_FS_AVL_1260_VAL    0x00
#define LPS25H_FS_AVL_1260_GAIN   KPASCAL_NANO_SCALE
#define LPS25H_FS_AVL_TEMP_GAIN   CELSIUS_NANO_SCALE
#define LPS25H_BDU_ADDR           0x20
#define LPS25H_BDU_MASK           0x04
#define LPS25H_DRDY_IRQ_ADDR      0x23
#define LPS25H_DRDY_IRQ_INT1_MASK 0x01
#define LPS25H_DRDY_IRQ_INT2_MASK 0x10
#define LPS25H_MULTIREAD_BIT      true
#define LPS25H_TEMP_OFFSET        (float)(42.5)
#define LPS25H_OUT_XL_ADDR        0x28
#define LPS25H_TEMP_OUT_L_ADDR    0x2B

// Self test parameters. Only checks that values are sane
#define LPS25H_ST_PRESS_MAX   (1100.0) //mbar
#define LPS25H_ST_PRESS_MIN   (450.0)  //mbar
#define LPS25H_ST_TEMP_MAX    (80.0)   //degree celcius
#define LPS25H_ST_TEMP_MIN    (-20.0)  //degree celcius

static uint8_t devAddr;
static bool isInit;

bool lps25hInit(void)
{
  if (isInit)
    return true;

  devAddr = LPS25H_I2C_ADDR;

  nrf_delay_ms(5);

  isInit = true;

  return true;
}

bool lps25hTestConnection(void)
{
  uint8_t id;
  bool status;

  if (!isInit)
	return false;
	uint8_t reg_addr[1] = {LPS25H_WHO_AM_I};

  //status = i2cdev_readBytes(devAddr, LPS25H_WHO_AM_I, 1, &id);
	nrf_drv_twi_tx(&m_twi, devAddr, reg_addr, 1, false);
	nrf_drv_twi_rx(&m_twi, devAddr, &id, 1);
  if (status == true && id == LPS25H_WAI_ID)
	{
		printf("%x\n", id);
	  return true;
	}
  return false;
}

bool lps25hSelfTest(void)
{
  bool testStatus = true;
  float pressure;
  float temperature;
  float asl;

  if (!isInit)
    return false;

  lps25hGetData(&pressure, &temperature, &asl);

  if (lps25hEvaluateSelfTest(LPS25H_ST_PRESS_MIN, LPS25H_ST_PRESS_MAX, pressure, "pressure") &&
      lps25hEvaluateSelfTest(LPS25H_ST_TEMP_MIN, LPS25H_ST_TEMP_MAX, temperature, "temperature"))
  {
    printf("Self test [OK].\n");
  }
  else
  {
   testStatus = false;
  }

  return testStatus;
}

bool lps25hEvaluateSelfTest(float min, float max, float value, char* string)
{
  if (value < min || value > max)
  {
    printf("Self test %s [FAIL]. low: %0.2f, high: %0.2f, measured: %0.2f\n", string, min, max, value);
    return false;
  }
  return true;
}

bool lps25hSetEnabled(bool enable)
{
  uint8_t enable_mask;
  bool status;
	uint8_t reg_addr[2] = {0,0};
	if (!isInit) {
	  return false;
	}
		
	if (enable) {
		enable_mask = 0xC0; // Power on, 25Hz, BDU, reset zero
		//status = i2cdev_writeBytes(devAddr, LPS25H_CTRL_REG1, 1, &enable_mask);
		reg_addr[0] = LPS25H_CTRL_REG1;
		reg_addr[1] = enable_mask;
		nrf_drv_twi_tx(&m_twi, devAddr, reg_addr, 2, false);
		
		enable_mask = 0x84; // Power on, 25Hz, BDU, reset zero
		//status = i2cdev_writeBytes(devAddr, LPS25H_CTRL_REG2, 1, &enable_mask);
		reg_addr[0] = LPS25H_CTRL_REG2;
		reg_addr[1] = enable_mask;
		nrf_drv_twi_tx(&m_twi, devAddr, reg_addr, 2, false);
		
		enable_mask = 0xc6; // Power on, 25Hz, BDU, reset zero
		//status = i2cdev_writeBytes(devAddr, LPS25H_CTRL_REG1, 1, &enable_mask);
		reg_addr[0] = LPS25H_CTRL_REG1;
		reg_addr[1] = enable_mask;
		nrf_drv_twi_tx(&m_twi, devAddr, reg_addr, 2, false);
		
		enable_mask = 0x0f; // AVG-P 512, AVG-T 64
		//status = i2cdev_writeBytes(devAddr, LPS25H_RES_CONF, 1, &enable_mask);
		reg_addr[0] = LPS25H_RES_CONF;
		reg_addr[1] = enable_mask;
		nrf_drv_twi_tx(&m_twi, devAddr, reg_addr, 2, false);
		
		// TODO: Investigate why temp values becomes wrong when FIFO averaging is enabled.
		enable_mask = 0xc3; // FIFO Mean mode, 4 moving average
		//status = i2cdev_writeBytes(devAddr, LPS25H_FIFO_CTRL, 1, &enable_mask);
		reg_addr[0] = LPS25H_FIFO_CTRL;
		reg_addr[1] = enable_mask;
		nrf_drv_twi_tx(&m_twi, devAddr, reg_addr, 2, false);
		
		enable_mask = 0x40; // FIFO Enable
		//status = i2cdev_writeBytes(devAddr, LPS25H_CTRL_REG2, 1, &enable_mask);
		reg_addr[0] = LPS25H_CTRL_REG2;
		reg_addr[1] = enable_mask;
		nrf_drv_twi_tx(&m_twi, devAddr, reg_addr, 2, false);
		
	}
	else {
	  enable_mask = 0x00;
	  //status = i2cdev_writeBytes(devAddr, LPS25H_CTRL_REG1, 1, &enable_mask);
		reg_addr[0] = LPS25H_CTRL_REG1;
		reg_addr[1] = enable_mask;
		nrf_drv_twi_tx(&m_twi, devAddr, reg_addr, 2, false);
	}

	return status;
}

bool lps25hGetData(float* temp_pressure_lbs, float* temp_temperature_lbs, float* temp_asl_lbs)
{
  uint8_t data[5];
  uint32_t rawPressure;
  bool status;

  int16_t rawTemp;
  //status = i2cdev_readBytes(devAddr, LPS25H_PRESS_OUT_XL | LPS25H_ADDR_AUTO_INC, 5, data);

	uint8_t reg_addr[1] = {LPS25H_PRESS_OUT_XL|LPS25H_ADDR_AUTO_INC};

	nrf_drv_twi_tx(&m_twi, devAddr, reg_addr, sizeof(reg_addr), false);
	nrf_drv_twi_rx(&m_twi, devAddr, data, 5);
	
  rawPressure = ((uint32_t)data[2] << 16) | ((uint32_t)data[1] << 8) | data[0];
  *temp_pressure_lbs = (float)rawPressure / LPS25H_LSB_PER_MBAR;

  rawTemp = ((int16_t)data[4] << 8) | data[3];
  *temp_temperature_lbs = LPS25H_TEMP_OFFSET + ((float)rawTemp / LPS25H_LSB_PER_CELSIUS);

  *temp_asl_lbs = lps25hPressureToAltitude(temp_pressure_lbs);

  return status;
}


//TODO: pretty expensive function. Rather smooth the pressure estimates and only call this when needed
/**
 * Converts pressure to altitude above sea level (ASL) in meters
 */
float lps25hPressureToAltitude(float* pressure/*, float* ground_pressure, float* ground_temp*/)
{
    if (*pressure > 0)
    {
        //return (1.f - pow(*pressure / CONST_SEA_PRESSURE, CONST_PF)) * CONST_PF2;
        //return ((pow((1015.7 / *pressure), CONST_PF) - 1.0) * (25. + 273.15)) / 0.0065;
        return ((powf((1015.7f / *pressure), CONST_PF) - 1.0f) * (FIX_TEMP + 273.15f)) / 0.0065f;
    }
    else
    {
        return 0;
    }
}
