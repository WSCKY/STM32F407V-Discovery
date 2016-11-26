/**
  * @date    20/21-August-2015
  * @author  '^_^'
  * @version V0.0.1
	*/
#include "main.h"

char USBKey_Path[10] = "0:";

USB_OTG_CORE_HANDLE          USB_OTG_Core;
USBH_HOST                    USB_Host;

PlayerErrCodeDef Err;

WAVE_InfoDef WaveInfo;
FILELIST_FileTypeDef FileList;

UsrCmdTypeDef UserPlayCmd(uint16_t *pIndex, uint8_t *pVol);

int main(void)
{
	/* Initialize LEDS */
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
	STM_EVAL_LEDOn(LED3);

	/* Init Host Library */
  USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_MSC_cb, &USR_Callbacks);

  while(USB_MSC_InitOk != 1)
	{
		/* Host Task handler */
    USBH_Process(&USB_OTG_Core, &USB_Host);
	}
	if(WAVFileExplor((char *)USBKey_Path, &FileList) != FR_OK)//find wav file
	{
		STM_EVAL_LEDOn(LED4);
		while(1);
	}

	Err = WavePlayer((char *)USBKey_Path, &FileList, &WaveInfo, UserPlayCmd);
	STM_EVAL_LEDOn(LED3);
	STM_EVAL_LEDOn(LED4);
	STM_EVAL_LEDOn(LED5);
	STM_EVAL_LEDOn(LED6);
	while(1)
	{
		
	}
}

UsrCmdTypeDef UserPlayCmd(uint16_t *pIndex, uint8_t *pVol)
{
	UsrCmdTypeDef cmd;
	if(STM_EVAL_PBGetState(BUTTON_USER) == SET)
	{
//		while(STM_EVAL_PBGetState(BUTTON_USER) == SET);
		cmd = VolumeSet;
		*pVol += 10;
		if(*pVol > 100)
			*pVol = 30;
	}
	else
		cmd = OperateNone;
	return cmd;
}
