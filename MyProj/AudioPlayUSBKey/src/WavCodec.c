/**
  *******************************************************************************
  * @file    WavCodec.c
  * @author  '^_^'
  * @version V0.0.1
  * @date    20-August-2015
  * @brief   WAV Format File Parsing.
  *******************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "WavCodec.h"

/* Private define -------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------*/
/* Private typedef ------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------*/
/* Private functions ----------------------------------------------------------*/

/**
  * @brief  Checks the format of the .WAV file and gets information about
  *   the audio format. This is done by reading the value of a
  *   number of parameters stored in the file header and comparing
  *   these to the values expected authenticates the format of a
  *   standard .WAV  file (44 bytes will be read). If  it is a valid
  *   .WAV file format, it continues reading the header to determine
  *   the  audio format such as the sample rate and the sampled data
  *   size. If the audio format is supported by this application,
  *   it retrieves the audio format in WAVE_Format structure and
  *   returns a zero value. Otherwise the function fails and the
  *   return value is nonzero.In this case, the return value specifies
  *   the cause of  the function fails. The error codes that can be
  *   returned by this function are declared in the header file.
  * @param  None
  * @retval Zero value if the function succeed, otherwise it return
  *         a nonzero value which specifies the error code.
  */
ErrorCodeDef WaveParsing(uint8_t *HeaderBuf, WAVE_InfoDef *WaveInfo)
{
	ChunkRiffDef *pChunkRiff = (ChunkRiffDef *)HeaderBuf;
	ChunkFmtDef *pChunkFmt;
	ChunkFactListDef *pChunkFactList;
	ChunkDataDef *pChunkData;
	uint32_t DataOffset = 0;

	/* Read chunkID, must be 'RIFF' */
	if(pChunkRiff->ChunkID != ChunkRIFF)
		return Unvalid_RIFF_ID; /* RIFF ID Unvalid */
	DataOffset += 4;
	/* Read the file length */
	WaveInfo->RiffChunkSize = pChunkRiff->ChunkSize;
	DataOffset += 4;
	/* Read the file format, must be 'WAVE' */
	if(pChunkRiff->FmtWAVE != FormatWAVE)
		return Unvalid_WAVE_Format; /* WAVE Format Unvalid */
	DataOffset += 4;
	pChunkFmt = (ChunkFmtDef *)(HeaderBuf + DataOffset);
	/* Read the format chunk, must be'fmt ' */
	if(pChunkFmt->ChunkID != ChunkFMT_)
		return Unvalid_FormatChunk_ID; /* fmt ID Unvalid */
	DataOffset += 4;
	/* Read the length of the 'fmt' data */
	WaveInfo->FmtChunkSize = pChunkFmt->ChunkSize;
	DataOffset += 4;
	/* Read the audio format, must be 0x01 (PCM) */
	if(pChunkFmt->AudioFormat != WAVE_FORMAT_PCM)
		return Unsupporetd_FormatTag; /* Format Tag Unsupported */
	WaveInfo->AudioFormat = pChunkFmt->AudioFormat;
	DataOffset += 2;
	/* Read the number of channels, must be 0x01 (Mono) or 0x02 (Stereo) */
	WaveInfo->NumOfChannels = pChunkFmt->NumOfChannels;
	DataOffset += 2;
	/* Read the Sample Rate */
	WaveInfo->SampleRate = pChunkFmt->SampleRate;
	DataOffset += 4;
	/* Read the Byte Rate */
	WaveInfo->ByteRate = pChunkFmt->ByteRate;
	DataOffset += 4;
	/* Read the block alignment */
	WaveInfo->BlockAlign = pChunkFmt->BlockAlign;
	DataOffset += 2;
	/* Read the number of bits per sample */
	WaveInfo->BitsPerSample = pChunkFmt->BitsPerSample;
	DataOffset += 2;
	/* If there is Extra format bytes, these bytes will be defined in "Fact Chunk" or "List Chunk" */
	if(WaveInfo->FmtChunkSize != FORMAT_CHNUK_SIZE)
	{
		/* Read th Extra format bytes, must be 0x00 */
		if(pChunkFmt->ByteExtraData != 0x0000)
			return Unsupporetd_ExtraFormatBytes;
		DataOffset += 2;
		pChunkFactList = (ChunkFactListDef *)(HeaderBuf + DataOffset);
		/* Read the Fact/List chunk, must be 'fact' or 'list' */
		if((pChunkFactList->ChunkID != ChunkFACT) && (pChunkFactList->ChunkID != ChunkLIST))
			return Unvalid_FactListChunk_ID;
		DataOffset += 4;
		WaveInfo->FactListChunkSize = pChunkFactList->ChunkSize;
		DataOffset += 4;
		if(pChunkFactList->ChunkID == ChunkFACT)
			WaveInfo->NumOfSamples = *((uint32_t *)(HeaderBuf + DataOffset));
		DataOffset += WaveInfo->FactListChunkSize;
	}
	pChunkData = (ChunkDataDef *)(HeaderBuf + DataOffset);
	/* Read the Data chunk, must be 'data' */
	if(pChunkData->ChunkID != ChunkDATA)
		return Unvalid_DataChunk_ID;
	DataOffset += 4;
	WaveInfo->DataSize = pChunkData->ChunkSize;
	DataOffset += 4;
	WaveInfo->DataOffset = DataOffset;
	return Valid_WAVE_File; /* File OK. */
}

/********************************* END OF FILE *********************************/
