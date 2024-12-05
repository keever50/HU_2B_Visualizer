/*
 * video.h
 *
 *  Created on: Dec 5, 2024
 *      Author: Kevin
 */

#ifndef INC_VIDEO_H_
#define INC_VIDEO_H_

#define VIDEO_HEIGHT	480
#define VIDEO_WIDTH 	640

#include <main.h>

void video_test();

void video_frame_start();
void video_frame_end();
int video_command( uint16_t cmd );
int video_sendline( uint16_t* data );

#endif /* INC_VIDEO_H_ */
