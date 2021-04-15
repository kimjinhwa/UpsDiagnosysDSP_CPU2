/*
 * SerialCommandUI.h
 *
 *  Created on: Apr 8, 2021
 *      Author: STELLA
 */

#ifndef SERIALCOMMANDUI_H_
#define SERIALCOMMANDUI_H_

typedef enum
{
    STATE_NO_DEVICE, // No device is present.
    STATE_DEVICE_ENUM, // Mass storage device is being enumerated.
    STATE_DEVICE_READY, // Mass storage device is ready.
    STATE_UNKNOWN_DEVICE, // An unsupported device has been attached.
    STATE_POWER_FAULT // A power fault has occurred.
} tState;

void Loop_ReadLine(void);

void ReadLine(void);


#endif /* SERIALCOMMANDUI_H_ */
