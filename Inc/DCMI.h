#ifndef __DCMI_H__
#define __DCMI_H__

#include "stm32l4r5xx.h"

/*DCMI Pins
 * 		HSYNC		PH8
 * 		VSYNC		PI5
 * 		PIXCLK		PH5
 * 		D0			PH9
 * 		D1			PH10
 * 		D2			PH11
 * 		D3			PH12
 * 		D4			PH14
 * 		D5			PI4
 * 		D6			PI6
 * 		D7			PI7
 */

void DCMI_Init(void);
void DCMI_DeInit(void);
void DCMI_StartCapture(void);
void DCMI_StopCapture(void);
void DCMI_ReadData(void);
void DCMI_GetStatus(void);
void DCMI_ErrorHandler(void);


#endif
