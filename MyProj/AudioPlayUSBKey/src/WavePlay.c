/**
  *******************************************************************************
  * @file    WavePlay.c
  * @author  '^_^'
  * @version V0.0.1
  * @date    20/21-August-2015 
  * @brief   WAV Audio Player.
  *******************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "WavePlay.h"

/* Private define -------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------*/
/* Private typedef ------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------*/
uint16_t Buff1[1024] = {0x00};
uint16_t Buff2[1024] = {0x00};
uint32_t BytesRead;

char fPath[50];

uint8_t XferCplt = 0;
uint8_t BufferSwitch = 0;
uint32_t WaveDataLength = 0;

/* Private function prototypes ------------------------------------------------*/
/* Private functions ----------------------------------------------------------*/

/**
  * @brief  Copies disk content in the explorer list.
  * @param  None
  * @retval Operation result.
  */
FRESULT WAVFileExplor(const XCHAR *path, FILELIST_FileTypeDef *FileList)
{
	FRESULT res = FR_OK;
  FILINFO fno;
  DIR dir;
  char *fn;

#if _USE_LFN
  static char lfn[_MAX_LFN];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#endif

	res = f_opendir(&dir, path);//open directory
	FileList->ptr = 0;
	if(res == FR_OK)
	{
		while(1)
		{
			res = f_readdir(&dir, &fno);//read directory
			if(res != FR_OK || fno.fname[0] == 0)
      {
        break;
      }
      if(fno.fname[0] == '.')
      {
        continue;
      }
#if _USE_LFN
      fn = *fno.lfname ? fno.lfname : fno.fname;
#else
      fn = fno.fname;
#endif
			if(FileList->ptr < FILEMGR_LIST_DEPDTH)
      {
        if((fno.fattrib & AM_DIR) == 0)
        {
          if((strstr(fn, "wav")) || (strstr(fn, "WAV")))
          {
            strncpy((char *)FileList->file[FileList->ptr].name, (char *)fn, FILEMGR_FILE_NAME_SIZE);
            FileList->file[FileList->ptr].type = FILETYPE_FILE;
            FileList->ptr++;
          }
        }
      }
		}
	}
  return res;
}

/**
  * @brief  Get WAV file information.
  * @param  *path : file directory.
  * @param  *FileList : WAV file list in directory.
  * @param  *WaveInfo : pointer to a WaveInfoDef Structure to save WAV file information.
  * @param  fIndex : Index of file list.
  * @retval Operation result.
  */
PlayerErrCodeDef GetWaveFileInfo(const XCHAR *path, FILELIST_FileTypeDef *FileList, WAVE_InfoDef *WaveInfo, uint8_t fIndex)
{
	DIR dir;
	FIL fileR;
	uint8_t Index = 0;

	if(fIndex >= FileList->ptr)//Check file list
		return FileNotExist;
	if(f_opendir(&dir, path) != FR_OK)//open directory
		return fDirectoryErr;
	for(Index = 0; Index < 50; Index ++)//flush
		fPath[Index] = 0;
	strncpy(fPath, (char *)path, 24);//copy path
	strcat(fPath, "/");
	if(f_open(&fileR, strncat(fPath, (char *)FileList->file[fIndex].name, 24), FA_READ))//open file
		return fOpenErr;
	if(f_read(&fileR, Buff1, 256, &BytesRead) != FR_OK)//read file header
		return fReadErr;
	f_close(&fileR);//close file
	if(WaveParsing((uint8_t *)Buff1, WaveInfo) != Valid_WAVE_File)//wave file parse
		return WaveParseErr;
	return Err_OK;//no error
}

/**
  * @brief  Play WAV Audio.
  * @param  ...
  * @retval Player return status.
  */
PlayerErrCodeDef WavePlayer(const XCHAR *path, FILELIST_FileTypeDef *FileList, WAVE_InfoDef *WaveInfo, UsrCmdTypeDef(*GetUsrEvt)(uint16_t *pJumpToFile, uint8_t *pVolume))
{
	FIL fileR;
	uint16_t FileIndex = 0;
	uint8_t Volume = 70;
	UsrCmdTypeDef UsrCmd = OperateNone, LastCmd = OperateNone;
	uint8_t Index = 0;

	/* Initialize I2S interface */
	EVAL_AUDIO_SetAudioInterface(AUDIO_INTERFACE_I2S);

loop:
	if(FileIndex >= FileList->ptr)
		return FileNotExist;
	for(Index = 0; Index < 50; Index ++)
		fPath[Index] = 0;
	strncpy(fPath, (char *)path, 24);
	strcat(fPath, "/");
	if(f_open(&fileR, strncat(fPath, (char *)FileList->file[FileIndex].name, 24), FA_READ))
		return fOpenErr;
	if(f_read(&fileR, Buff1, 256, &BytesRead))
		return fReadErr;
	if(WaveParsing((uint8_t *)Buff1, WaveInfo) != Valid_WAVE_File)
		return WaveParseErr;

	/* Initialize the Audio codec and all related peripherals (I2S, I2C, IOExpander, IOs...) */  
	EVAL_AUDIO_Init(OUTPUT_DEVICE_AUTO, Volume, WaveInfo->SampleRate);

	/* Get Data from USB Key */
	f_lseek(&fileR, WaveInfo->DataOffset);
	f_read (&fileR, Buff1, _MAX_SS, &BytesRead);
	f_read (&fileR, Buff2, _MAX_SS, &BytesRead);
	WaveDataLength = WaveInfo->DataSize;
  /* Start playing wave */
  Audio_MAL_Play((uint32_t)Buff1, _MAX_SS);//play
	BufferSwitch = 0;
	XferCplt = 0;
	while(WaveDataLength != 0)
	{
		/* wait for DMA transfert complete */
		if(XferCplt != 0)
//		while(XferCplt == 0x00)
//		{__asm("nop");}
//while,为什么不行? if,为什么行? #>_<!#
		{
			XferCplt = 0;
			STM_EVAL_LEDToggle(LED6);
			if(BufferSwitch == 0)
			{
				BufferSwitch = 1;
				Audio_MAL_Play((uint32_t)Buff2, _MAX_SS);//play
				f_read (&fileR, Buff1, _MAX_SS, &BytesRead);
			}
			else
			{
				BufferSwitch = 0;
				Audio_MAL_Play((uint32_t)Buff1, _MAX_SS);//play
				f_read (&fileR, Buff2, _MAX_SS, &BytesRead);
			}
		}
		UsrCmd = GetUsrEvt(&FileIndex, &Volume);
		if(UsrCmd != LastCmd)//New Command.
		{
			LastCmd = UsrCmd;//Update Command.
			switch(LastCmd)
			{
				case OperateNone: break;
				case PlayResume:
					EVAL_AUDIO_PauseResume(1);
					break;
				case PlayPause:
					EVAL_AUDIO_PauseResume(0);
					break;
				case Next:
					EVAL_AUDIO_Stop(CODEC_PDWN_HW);//AUDIO Hardware power down
					f_close(&fileR);
					WaveDataLength = 0;
					FileIndex ++;
					if(FileIndex >= FileList->ptr)
						FileIndex = 0;
				goto loop;
				case JumpToSpecFile:
					EVAL_AUDIO_Stop(CODEC_PDWN_HW);//AUDIO Hardware power down
					f_close(&fileR);
					WaveDataLength = 0;
				goto loop;
				case VolumeSet:
					EVAL_AUDIO_VolumeCtl(Volume);
					break;
				case VolumeUp:
					if(Volume <= 90)
						Volume += 10;
					EVAL_AUDIO_VolumeCtl(Volume);
					break;
				case VolumeDown:
					if(Volume >= 10)
						Volume -= 10;
					EVAL_AUDIO_VolumeCtl(Volume);
					break;
				case ExitPlay:
					EVAL_AUDIO_Stop(CODEC_PDWN_HW);//AUDIO Hardware power down
					f_close(&fileR);
					return PlayExit;
				default: break;
			}
		}
	}
	EVAL_AUDIO_Stop(CODEC_PDWN_HW);//AUDIO Hardware power down
	f_close(&fileR);
	if(FileIndex <= FileList->ptr - 1)/* Infinite loop */
	{
		if(FileIndex == (FileList->ptr - 1))
			FileIndex = 0;
		else
			FileIndex ++;
		goto loop;
	}
	return Err_OK;
}

/*--------------------------------
Callbacks implementation:
the callbacks prototypes are defined in the stm324xg_eval_audio_codec.h file
and their implementation should be done in the user code if they are needed.
--------------------------------------------------------*/
/**
* @brief  Calculates the remaining file size and new position of the pointer.
* @param  None
* @retval None
*/
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size)
{
  /* Calculate the remaining audio data in the file and the new size 
  for the DMA transfer. If the Audio files size is less than the DMA max 
  data transfer size, so there is no calculation to be done, just restart 
  from the beginning of the file ... */
  /* Check if the end of file has been reached */
	XferCplt = 1;
	if (WaveDataLength) WaveDataLength -= _MAX_SS;
	if (WaveDataLength < _MAX_SS) WaveDataLength = 0;
}

/**
* @brief  Manages the DMA Half Transfer complete interrupt.
* @param  None
* @retval None
*/
void EVAL_AUDIO_HalfTransfer_CallBack(uint32_t pBuffer, uint32_t Size)
{  
#ifdef AUDIO_MAL_MODE_CIRCULAR
    
#endif /* AUDIO_MAL_MODE_CIRCULAR */
  
  /* Generally this interrupt routine is used to load the buffer when 
  a streaming scheme is used: When first Half buffer is already transferred load 
  the new data to the first half of buffer while DMA is transferring data from 
  the second half. And when Transfer complete occurs, load the second half of 
  the buffer while the DMA is transferring from the first half ... */
  /* 
  ......
  */
}

/**
* @brief  Manages the DMA FIFO error interrupt.
* @param  None
* @retval None
*/
void EVAL_AUDIO_Error_CallBack(void* pData)
{
  /* Stop the program with an infinite loop */
  while (1)
  {}
  
  /* could also generate a system reset to recover from the error */
  /* .... */
}

/**
* @brief  Get next data sample callback
* @param  None
* @retval Next data sample to be sent
*/
uint16_t EVAL_AUDIO_GetSampleCallBack(void)
{
  return 0;
}

#ifndef USE_DEFAULT_TIMEOUT_CALLBACK
/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t Codec_TIMEOUT_UserCallback(void)
{   
  return (0);
}
#endif /* USE_DEFAULT_TIMEOUT_CALLBACK */
/*-----------------------------------------------------------------------------*/

/********************************* END OF FILE *********************************/
