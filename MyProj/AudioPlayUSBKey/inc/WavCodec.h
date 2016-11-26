/**
  *******************************************************************************
  * @file    WavCodec.h
  * @author  '^_^'
  * @version V0.0.1
  * @date    20-August-2015
  * @brief   Header for WavCodec.c module.
  *******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __WAVCODEC_H
#define __WAVCODEC_H

/* Includes -------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants ---------------------------------------------------------*/
#define ChunkRIFF										0x46464952 /* correspond to the letters 'RIFF' */
#define FormatWAVE									0x45564157 /* correspond to the letters 'WAVE' */
#define ChunkFMT_										0x20746D66 /* correspond to the letters 'fmt ' */
#define ChunkFACT										0x74636166 /* correspond to the letters 'fact' */
#define ChunkLIST										0x5453494C /* correspond to the letters 'list' */
#define ChunkDATA										0x61746164 /* correspond to the letters 'data' */

#define WAVE_FORMAT_PCM             0x01
#define FORMAT_CHNUK_SIZE           0x10

#define CHANNEL_MONO                0x01
#define CHANNEL_STEREO              0x02

#define SAMPLE_RATE_8000            8000
#define SAMPLE_RATE_11025           11025
#define SAMPLE_RATE_22050           22050
#define SAMPLE_RATE_44100           44100
#define SAMPLE_RATE_48000           48000

#define BITS_PER_SAMPLE_8           8
#define BITS_PER_SAMPLE_16          16
#define BITS_PER_SAMPLE_24          24
/* Exported types -------------------------------------------------------------*/
typedef struct
{
	uint32_t ChunkID;
	uint32_t ChunkSize;
}ChunkDataDef;

typedef ChunkDataDef ChunkFactListDef;

typedef struct
{
	uint32_t ChunkID;
	uint32_t ChunkSize;
	uint16_t AudioFormat;
	uint16_t NumOfChannels;
	uint32_t SampleRate;
	uint32_t ByteRate;
	uint16_t BlockAlign;
	uint16_t BitsPerSample;
	uint16_t ByteExtraData;
}ChunkFmtDef;

typedef struct
{
	uint32_t ChunkID;
	uint32_t ChunkSize;
	uint32_t FmtWAVE;
}ChunkRiffDef;

typedef struct
{
	uint32_t RiffChunkSize;
	uint32_t FmtChunkSize;
	uint16_t AudioFormat;
	uint16_t NumOfChannels;
	uint32_t SampleRate;
	uint32_t ByteRate;
	uint16_t BlockAlign;
	uint16_t BitsPerSample;
	uint32_t FactListChunkSize;
	uint32_t NumOfSamples;
	uint32_t DataSize;
	uint32_t DataOffset;
}WAVE_InfoDef;

typedef enum
{
  Valid_WAVE_File = 0,
  Unvalid_RIFF_ID,
  Unvalid_WAVE_Format,
  Unvalid_FormatChunk_ID,
  Unsupporetd_FormatTag,
  Unsupporetd_Number_Of_Channel,
  Unsupporetd_Sample_Rate,
  Unsupporetd_Bits_Per_Sample,
  Unvalid_DataChunk_ID,
  Unsupporetd_ExtraFormatBytes,
  Unvalid_FactListChunk_ID
}ErrorCodeDef;
/* Exported macro -------------------------------------------------------------*/
/* Exported functions -------------------------------------------------------- */
ErrorCodeDef WaveParsing(uint8_t *HeaderBuf, WAVE_InfoDef *WaveInfo);

#endif /* __WAVCODEC_H */

/********************************* END OF FILE *********************************/
