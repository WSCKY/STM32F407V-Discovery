/*
@brief   FAT File System Test.
@author  '^_^'
@date    20-August-2015
*/
#include "main.h"

USB_OTG_CORE_HANDLE          USB_OTG_Core;
USBH_HOST                    USB_Host;

DIR dir;
FIL fileR;

uint8_t WriteBuf[] = "Hello, World!\r\n";
uint32_t BytesWritten;

int main(void)
{
	/* Initialize LEDS */
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);

	STM_EVAL_LEDOn(LED3);

	/* Init Host Library */
  USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_MSC_cb, &USR_Callbacks);
  
  while(USB_MSC_InitOk != 1)
	{
		/* Host Task handler */
    USBH_Process(&USB_OTG_Core, &USB_Host);
	}
loop:	if(f_opendir(&dir, "0:/Test") != FR_OK)//open a directory
	{
		if(f_mkdir("0:/Test") != FR_OK)//if failed, create a new directory.
		{
			STM_EVAL_LEDOn(LED4);//failed
			while(1);
		}
		else goto loop;//open again
	}
	else
	{
		STM_EVAL_LEDOff(LED4);
		if(f_open(&fileR, "0:/Test/Hello.txt", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)//create a new txt format file.
		{
			STM_EVAL_LEDOn(LED5);
			while(1);
		}
		else
		{
			if(f_write(&fileR, WriteBuf, sizeof(WriteBuf) - 1, (void *)&BytesWritten) != FR_OK)//write any bytes to this file.
			{
				STM_EVAL_LEDOn(LED6);
				while(1);
			}
			if(BytesWritten == 0)
			{
				STM_EVAL_LEDOn(LED6);
				while(1);
			}
			for(int i = 0; i < 200; i ++)
				f_printf(&fileR, "f_printf function test.--Count:%d\r\n", i);//write any string to this file use f_printf function.
			f_close(&fileR);//close this file.
		}
	}
  /* operation OK! */
	STM_EVAL_LEDOn(LED3);
	STM_EVAL_LEDOn(LED4);
	STM_EVAL_LEDOn(LED5);
	STM_EVAL_LEDOn(LED6);
	while(1)
	{
		
	}
}
