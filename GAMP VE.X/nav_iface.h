/* 
 * File:   nav_iface.h
 * Author: Paul
 *
 * Created on 7 March 2015, 3:30 PM
 */

#ifndef NAV_IFACE_H
#define	NAV_IFACE_H

#define NAVADDRESS          0b11010010

#define SELF_TEST_X_GYRO    0x00
#define SELF_TEST_Y_GYRO    0x01
#define SELF_TEST_Z_GYRO    0x02
#define SELF_TEST_X_ACCEL   0x0D
#define SELF_TEST_Y_ACCEL   0x0E
#define SELF_TEST_Z_ACCEL   0x0F


#define NAVReadRegister( reg, buffer, len) I2CReadRegister(NAVADDRESS, reg, buffer, len)
#define NAVWriteRegister( reg, buffer, len) I2CWriteRegister(NAVADDRESS, reg, buffer, len)


#endif	/* NAV_IFACE_H */

