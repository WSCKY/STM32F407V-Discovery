/**
  *******************************************************************************
  * @file    WavePlay.h
  * @author  '^_^'
  * @version V0.0.1
  * @date    20/21-August-2015
  * @brief   Header for WavePlay.c module.
  *******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __WAVEPLAY_H
#define __WAVEPLAY_H

/* Includes -------------------------------------------------------------------*/
#include "ff.h"
#include <string.h>
#include "WavCodec.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_audio_codec.h"

/* Exported constants ---------------------------------------------------------*/
#define FILEMGR_LIST_DEPDTH                        24
#define FILEMGR_FILE_NAME_SIZE                     24
#define FILETYPE_FILE                              1
/* Exported types -------------------------------------------------------------*/
typedef struct _FILELIST_LineTypeDef {
  uint8_t type;
  uint8_t name[FILEMGR_FILE_NAME_SIZE];
}FILELIST_LineTypeDef;

typedef struct _FILELIST_FileTypeDef {
  FILELIST_LineTypeDef  file[FILEMGR_LIST_DEPDTH] ;
  uint16_t              ptr;
}FILELIST_FileTypeDef;

typedef enum
{
	OperateNone = 0,
	PlayResume,
	PlayPause,
	Next,
	JumpToSpecFile,
	VolumeSet,
	VolumeUp,
	VolumeDown,
	ExitPlay
}UsrCmdTypeDef;

typedef enum
{
	Err_OK = 0,
	FileNotExist,
	fDirectoryErr,
	fOpenErr,
	fReadErr,
	WaveParseErr,
	PlayExit
}PlayerErrCodeDef;
/* Exported macro -------------------------------------------------------------*/
/* Exported functions -------------------------------------------------------- */
FRESULT WAVFileExplor(const XCHAR *path, FILELIST_FileTypeDef *FileList);
PlayerErrCodeDef GetWaveFileInfo(const XCHAR *path, FILELIST_FileTypeDef *FileList, WAVE_InfoDef *WaveInfo, uint8_t fIndex);
PlayerErrCodeDef WavePlayer(const XCHAR *path, FILELIST_FileTypeDef *FileList, WAVE_InfoDef *WaveInfo, UsrCmdTypeDef(*GetUsrEvt)(uint16_t *pJumpToFile, uint8_t *pVolume));

#endif /* __WAVEPLAY_H */

/********************************* END OF FILE *********************************/
