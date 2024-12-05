/*
 * video.c
 *
 *  Created on: Dec 5, 2024
 *      Author: Kevin
 */


#include <video.h>
#include <string.h>
#include <main.h>
#include "Very_cool_simple_logging_thing/log.h"

extern SPI_HandleTypeDef hspi2;

//  [CTRL 4][RGB 12]
// CMD, -, -, -, ...RGB/CMD...

uint16_t _video_linebuffer[VIDEO_WIDTH];

void video_test()
{
	log_setlevel(LOG_INFO);
	LOGF(LOG_DEBUG, "Start test", 0);

	for(uint16_t i=0;i<VIDEO_WIDTH;i++)
	{
		_video_linebuffer[i]=i;
	}

	video_frame_start();
	video_command(0x00BA);


	video_sendline(_video_linebuffer);
	video_frame_end();

	LOGF(LOG_DEBUG, "End test", 0);
}

void video_frame_start()
{
	//LOGF(LOG_DEBUG, "Start frame", 0);
	//HAL_GPIO_WritePin(VID_EN_GPIO_Port, VID_EN_Pin, GPIO_PIN_SET);
}

void video_frame_end()
{
	//LOGF(LOG_DEBUG, "End frame", 0);
	//HAL_GPIO_WritePin(VID_EN_GPIO_Port, VID_EN_Pin, GPIO_PIN_RESET);
}
uint16_t cmd_static;
int video_command( uint16_t cmd )
{
	//LOGF(LOG_DEBUG, "Video command 0x%X", cmd);

	//cmd=cmd|(1<<15);
	HAL_SPI_StateTypeDef state = HAL_SPI_GetState(&hspi2);
	if(state != HAL_SPI_STATE_READY)
	{
		LOGF(LOG_WARNING, "SPI not ready 0x%X", state);
		return -2;
	}

	cmd_static=cmd;
	HAL_StatusTypeDef code = HAL_SPI_Transmit_DMA(&hspi2, (uint8_t*)&cmd_static, 1);
	if(code != HAL_OK)
	{
		LOGF(LOG_WARNING, "SPI Transmit command failed", 0);
		return -1;
	}
	return 0;
}

int video_sendline( uint16_t* data )
{
	//LOGF(LOG_DEBUG, "Video send line", 0);
	memcpy(_video_linebuffer, data, VIDEO_WIDTH);
	HAL_SPI_StateTypeDef state = HAL_SPI_GetState(&hspi2);
	if(state != HAL_SPI_STATE_READY)
	{
		LOGF(LOG_WARNING, "SPI not ready 0x%X", state);
		return -2;
	}

	HAL_StatusTypeDef code = HAL_SPI_Transmit_DMA(&hspi2, (uint8_t*)data, VIDEO_WIDTH);
	if(code != HAL_OK)
	{
		LOGF(LOG_WARNING, "SPI Transmit line failed", 0);
		return -1;
	}


	return 0;
}

