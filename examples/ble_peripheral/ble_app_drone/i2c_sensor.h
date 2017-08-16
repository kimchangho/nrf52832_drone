#ifndef __I2C_SENSOR_H__
#define __I2C_SENSOR_H__

#include "main.h"
#include "bno055.h"
#include "lps25h.h"

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

 /* Number of possible TWI addresses. */
 #define TWI_ADDRESSES      127

/* TWI instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);


/**
 * @brief TWI initialization.
 */
void twi_init (void);

static volatile bool m_xfer_done = false;
void twi_init (void);
void nrf_bno055_init(void);

#ifdef	BNO055_API
#define	BNO055_I2C_BUS_WRITE_ARRAY_INDEX	((u8)1)
/*	\Brief: The API is used as I2C bus read
 *	\Return : Status of the I2C read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register,
 *   will data is going to be read
 *	\param reg_data : This data read from the sensor,
 *   which is hold in an array
 *	\param cnt : The no of byte of data to be read
 */
s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
/*	\Brief: The API is used as SPI bus write
 *	\Return : Status of the SPI write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register,
 *   will data is going to be written
 *	\param reg_data : It is a value hold in the array,
 *	will be used for write the value into the register
 *	\param cnt : The no of byte of data to be write
 */
s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
/*
 * \Brief: I2C init routine
*/
s8 I2C_routine(void);
#endif

/*	Brief : The delay routine
 *	\param : delay in ms
*/
void BNO055_delay_msek(u32 msek);

/*-- example of Read bno055 Sensor data --*/

////////////Structure///////////////////////////
//struct bno055_accel_t accel_xyz;
//struct bno055_mag_t mag_xyz;
//struct bno055_gyro_t gyro_xyz;
//struct bno055_euler_t euler_hrp;
//struct bno055_euler_double_t d_euler_hpr;
//struct bno055_gravity_t gravity_xyz;
////////////////////////////////////////////////

////////////Read Function//////////////////////
//bno055_read_accel_xyz(&accel_xyz);
//bno055_read_mag_xyz(&mag_xyz);
//bno055_read_gyro_xyz(&gyro_xyz);
//bno055_convert_double_euler_hpr_deg(&d_euler_hpr);
//bno055_read_gravity_xyz(&gravity_xyz);
//bno055_convert_double_gravity_xyz_msq(&d_gravity_xyz);
/////////////////////////////////////////////////

/*--------------------------------------*/


#endif

