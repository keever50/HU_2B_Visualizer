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
#include "cmsis_os.h"
#include "Filter.h"

extern int16_t bins[buffer_Groote/2 + 1];
extern SPI_HandleTypeDef hspi2;

//  [CTRL 4][RGB 12]
// CMD, -, -, -, ...RGB/CMD...

void video_frame_start();
void video_frame_end();

uint16_t _video_create_cmd(uint8_t cmd, uint16_t data)
{
	uint16_t out=0;
	out = cmd<<12;
	data=data&0x0FFF;
	out = out | data;
	//LOGF(LOG_DEBUG, "CMD 0x%X", out);
	return out;
}

int _video_transmit(uint16_t *data, size_t len16)
{
//	if(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY)
//	{
//		LOGF(LOG_WARNING, "SPI not ready waiting...", 0);
//		osDelay(100);
//	}
//
//	if(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY)
//	{
//		LOGF(LOG_WARNING, "SPI still not ready aborting", 0);
//		return -1;
//	}

	video_frame_start();

	//HAL_StatusTypeDef code = HAL_SPI_Transmit_DMA(&hspi2, (uint8_t*)data, len16);
	HAL_StatusTypeDef code = HAL_SPI_Transmit(&hspi2, (uint8_t*)data, len16, 1000);

//	if(code != HAL_OK)
//	{
//		LOGF(LOG_WARNING, "SPI Transmit line failed 0x%X", code);
//		video_frame_end();
//		return -1;
//	}
//
//	while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY)
//	{
//		osDelay(1);
//	}

	video_frame_end();
	return 0;
}



void video_frame_start()
{
	//LOGF(LOG_DEBUG, "Start frame", 0);
	//osDelay(1);
	HAL_GPIO_WritePin(VID_DC_GPIO_Port, VID_DC_Pin, GPIO_PIN_RESET);
	//osDelay(1);
}

void video_frame_end()
{
	//LOGF(LOG_DEBUG, "End frame", 0);
	//osDelay(1);
	HAL_GPIO_WritePin(VID_DC_GPIO_Port, VID_DC_Pin, GPIO_PIN_SET);
	//osDelay(1);

}

int video_shift_line()
{
	//LOGF(LOG_DEBUG, "SHIFT", 0);
	uint16_t cmd = _video_create_cmd(VIDEO_COMMAND_SETLINE, 0);
	//cmd = 0b1100110011001111;
	return _video_transmit(&cmd, 1);
}

int video_pixel(uint16_t rgb)
{
	//LOGF(LOG_DEBUG, "PIXEL %d", rgb);
	uint16_t cmd = _video_create_cmd(VIDEO_COMMAND_PIXEL, rgb);
	return _video_transmit(&cmd, 1);
}

int video_reset()
{
	LOGF(LOG_DEBUG, "RESET", 0);
	uint16_t cmd = _video_create_cmd(VIDEO_COMMAND_RESET, 0);
	return _video_transmit(&cmd, 1);
}

void video_test()
{
	log_setlevel(LOG_DEBUG);
	LOGF(LOG_DEBUG, "Start test", 0);

	video_reset();
	for(size_t y=0;y<VIDEO_HEIGHT-1;y++)
	{

		for(size_t x=0;x<VIDEO_WIDTH-1;x++)
		{
			video_pixel(x*20);
		}
		video_shift_line();
	}

	osDelay(1000);

	LOGF(LOG_DEBUG, "End test", 0);
}

void render_task(void *argument)
{
	video_reset();
	for(;;)
	{
		//LOGF(LOG_NOTICE, "Logging test", 0);
		for(size_t i=0;i<buffer_Groote/2+1;i++)
		{
			//LOGF(LOG_INFO, "BIN %d = %d", i, bins[i]);
			for(size_t w=0;w<1;w++)
			{
				uint8_t R, G, B;
				R=bins[i]/30;
				G=bins[i]/20;
				B=bins[i]/30;
				uint16_t rgb=0;
				rgb|=(R&0xF);
				rgb|=(G&0xF)<<4;
				rgb|=(B&0xF)<<8;


				video_pixel(0b0000000000110000+rgb);
			}
		}
		video_shift_line();
		osDelay(50);
	}

}

