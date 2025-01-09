/*
 * video.h
 *
 *  Created on: Dec 5, 2024
 *      Author: Kevin
 */

#ifndef INC_VIDEO_H_
#define INC_VIDEO_H_

#define VIDEO_HEIGHT	240
#define VIDEO_WIDTH 	320

#define VIDEO_COMMAND_SETLINE 0b0010
#define VIDEO_COMMAND_RESET 0b0001
#define VIDEO_COMMAND_PIXEL 0x0

#include <main.h>

void video_test();


int video_shift_line();
int video_pixel(uint16_t rgb);
int void_reset();

void render_task(void *argument);
#endif /* INC_VIDEO_H_ */
