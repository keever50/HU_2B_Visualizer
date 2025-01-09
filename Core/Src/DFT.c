/*
 * DFT.c
 *
 *  Created on: Dec 13, 2024
 *      Author: Niels
 */

#include "main.h"
#include "stm32f4xx_it.h"
#include <stdbool.h>
#include <stdlib.h>
#include "Filter.h"
#include <math.h>
#include "cmsis_os.h"
#include "Very_cool_simple_logging_thing/log.h"
int16_t Bintest(int bin);
void Shift(int16_t Hoogste, int16_t Laagste,int16_t buffer[buffer_Groote]);
int16_t Integersqrt(int32_t input);
void Windowing();

extern int16_t Buffer_1[buffer_Groote];
extern int16_t Amplitude_1;
extern int16_t Laagste_1;

extern int16_t Buffer_2[buffer_Groote];
extern int16_t Amplitude_2;
extern int16_t Laagste_2;

extern int Flag;

extern int Index;

int16_t Sin[buffer_Groote];
int16_t Cos[buffer_Groote];
int16_t bins[buffer_Groote/2 + 1];

int16_t testbuffer[buffer_Groote] = {694, 2050, 1580, 775, 970, 1860, 1987, 1156, 709, 1376, 2075, 1666, 826, 911, 1792, 2033, 1237, 702, 1293, 2052, 1746, 866, 845, 1696, 2061, 1326, 704, 1208, 2017, 1818, 934, 797, 1638, 2081, 1414, 714, 1125, 1979, 1886, 1000, 762, 1555, 2093, 1498, 741, 1048, 1921, 1944, 1070, 730}
;

int32_t AmBuffer[buffer_Groote];
int64_t Windowingbuf[buffer_Groote];

const int shiftbits = 2 * Dig_Bereik_bits - 2 - Bin_Bereik_bits; // hoeveel bits moet worden opgeschoven om het om te zetten in het bin bereik.
int Loop;

void DFTEST()
{
	Shift(2093,694,testbuffer);
}



void Windowing()
{
	int i = 0;
	double fl;
	switch(Filter)
	{
		case Blackman:
		{
			for(;i<buffer_Groote;i++)
			{
				fl = 0.42 - 0.5 *  cos ( (2 * M_PI * i) / (buffer_Groote - 1) ) + 0.08 * cos( (4 * M_PI * i) / (buffer_Groote - 1) );
				Windowingbuf[i] = (int16_t) (fl*( 1 << 10));
			}
			break;
		}

		case Hamming:
		{
			for(;i<buffer_Groote;i++)
			{
				fl = 0.54 - 0.46 * cos( (2 * M_PI * i) / (buffer_Groote - 1) );
				Windowingbuf[i] = (int16_t) (fl*( 1 << 10));

			}
			break;
		}
		case GEEN:
			for(;i<buffer_Groote;i++)
			{
				Windowingbuf[i] = 1024;
			}
		default:
			break;

	}


}

void Sin_Cos_Gen()
{
	for(int i = 0; i < buffer_Groote;i++)
		{
			float TweePie = (M_PI * 2);
			float Angle = TweePie * i / buffer_Groote;
			Sin[i] = sin(Angle) * 2048;
			Cos[i] = cos(Angle) * 2048;


		}
}


void DFT(void *argument)
{
//	for(;;)
//	{
//		osDelay(1000);
//	}

	osDelay(1000);
	Loop =  (buffer_Groote / 8) * 8;
	int Bins = buffer_Groote / 2 - 1;
	int Binlooping = (Bins/8) * 8;

	Windowing();
	Sin_Cos_Gen();

	while(1)
	{

		osDelay(10);

		if(Flag == 1)
		{
			//LOGF(LOG_DEBUG, "DFT Started", 0);
			HAL_GPIO_TogglePin(GPIOD, LD3_Pin);
			Flag = 0;

			if(Index == 1) // is de index waar de AD conv aan het vullen is.
				Shift(Amplitude_2,Laagste_2,Buffer_2);
			else
				Shift(Amplitude_1,Laagste_1,Buffer_1);
			int i = 1;
			for(; i < Binlooping; i+=8)
			{
				bins[i] = Bintest(i);
				bins[i + 1] = Bintest(i + 1);
				bins[i + 2] = Bintest(i + 2);
				bins[i + 3] = Bintest(i + 3);
				bins[i + 4] = Bintest(i + 4);
				bins[i + 5] = Bintest(i + 5);
				bins[i + 6] = Bintest(i + 6);
				bins[i + 7] = Bintest(i + 7);
			}
			for(;i < Bins;i++)
				bins[i] = Bintest(i);

			HAL_GPIO_TogglePin(GPIOD, LD3_Pin);
			//LOGF(LOG_DEBUG, "DFT Done", 0);
		}



	}
}



void Shift(int16_t Amplitude, int16_t Laagste,int16_t buffer[buffer_Groote]) // zorgt voor de verplaatsing voor DFT
{

	int32_t ScaleFactor = ((int32_t) Dig_Bereik << 10) / Amplitude;
	int16_t offset = Dig_Bereik/2;
	int32_t scaledOffset = offset << 10;

	int i = 0;
	for(; i < Loop; i+=8) // versterkt het signaal het hele bereik gebruikt en wordt meteen in juiste bereik gezet.
	{
		AmBuffer[i] = 	((buffer[i	] - Laagste) * ScaleFactor - scaledOffset) * Windowingbuf[i] >> 20;
		AmBuffer[i+1] = ((buffer[i+1] - Laagste) * ScaleFactor - scaledOffset) * Windowingbuf[i+1] >> 20;
		AmBuffer[i+2] = ((buffer[i+2] - Laagste) * ScaleFactor - scaledOffset) * Windowingbuf[i+2] >> 20;
		AmBuffer[i+3] = ((buffer[i+3] - Laagste) * ScaleFactor - scaledOffset) * Windowingbuf[i+3] >> 20;
		AmBuffer[i+4] = ((buffer[i+4] - Laagste) * ScaleFactor - scaledOffset) * Windowingbuf[i+4] >> 20;
		AmBuffer[i+5] = ((buffer[i+5] - Laagste) * ScaleFactor - scaledOffset) * Windowingbuf[i+5] >> 20;
		AmBuffer[i+6] = ((buffer[i+6] - Laagste) * ScaleFactor - scaledOffset) * Windowingbuf[i+6] >> 20;
		AmBuffer[i+7] = ((buffer[i+7] - Laagste) * ScaleFactor - scaledOffset) * Windowingbuf[i+7] >> 20;
	}
	for(;i < buffer_Groote;i++ )
	{
		AmBuffer[i] = ((buffer[i] - Laagste) * ScaleFactor - scaledOffset)* Windowingbuf[i] >> 20;
	}
}



int16_t Bintest(int bin)
{
	    int tel = 0;
	    int i = 0;
	    int64_t Costot = 0, Sintot = 0;
	    int32_t scaledSin,scaledCos;

	    for (i = 0; i < Loop; i +=8)
	    {
	        Sintot += AmBuffer[i] * Sin[tel];
	        Costot += AmBuffer[i] * Cos[tel];
	        tel += bin;
	        if (tel > buffer_Groote)
	            tel -= buffer_Groote;

	        Sintot += AmBuffer[i + 1] * Sin[tel];
	        Costot += AmBuffer[i + 1] * Cos[tel];
	        tel += bin;
	        if (tel > buffer_Groote)
	            tel -= buffer_Groote;

	        Sintot += AmBuffer[i + 2] * Sin[tel];
	        Costot += AmBuffer[i + 2] * Cos[tel];
	        tel += bin;
	        if (tel > buffer_Groote)
	            tel -= buffer_Groote;

	        Sintot += AmBuffer[i + 3] * Sin[tel];
	        Costot += AmBuffer[i + 3] * Cos[tel];
	        tel += bin;
	        if (tel > buffer_Groote)
	            tel -= buffer_Groote;

	        Sintot += AmBuffer[i + 4] * Sin[tel];
	        Costot += AmBuffer[i + 4] * Cos[tel];
	        tel += bin;
	        if (tel > buffer_Groote)
	            tel -= buffer_Groote;

	        Sintot += AmBuffer[i + 5] * Sin[tel];
	        Costot += AmBuffer[i + 5] * Cos[tel];
	        tel += bin;
	        if (tel > buffer_Groote)
	            tel -= buffer_Groote;

	        Sintot += AmBuffer[i + 6] * Sin[tel];
	        Costot += AmBuffer[i + 6] * Cos[tel];
	        tel += bin;
	        if (tel > buffer_Groote)
	            tel -= buffer_Groote;

	        Sintot += AmBuffer[i + 7] * Sin[tel];
	        Costot += AmBuffer[i + 7] * Cos[tel];
	        tel += bin;
	        if (tel > buffer_Groote)
	            tel -= buffer_Groote;


	    }

	    for (; i <= buffer_Groote; i++)
	        {
	            Sintot += (AmBuffer[i] * Sin[tel]);
	            Costot += (AmBuffer[i] * Cos[tel]);
	            tel += bin;
	            if (tel > buffer_Groote)
	                tel -= buffer_Groote;
	        }

	    scaledSin = ((Sintot >> shiftbits ) / buffer_Groote); // zet waarde in bereik testen is dat 0 tot 1024.
	    scaledCos = ((Costot >> shiftbits)  / buffer_Groote);
	    int32_t Total = (scaledSin * scaledSin) + (scaledCos * scaledCos);

	    if(Total < Rekenruis)
	    		return 0;

	    int16_t binwaarde = Integersqrt(Total);
	    return binwaarde;
	}

int16_t Integersqrt(int32_t input)
{
	int32_t testbit = 1 << 30; // hoogste waarde dat een macht van 4 is.
	int32_t Res = 0;

	while(input >= testbit)
		testbit >>=2; // macht 4 opgeschoven.

	while(testbit != 0)
	{
		if(input >= Res + testbit)
		{
			input -= Res + testbit;
			Res = (Res >> 1) + testbit;
		}
		else
			Res >>=1;

		testbit >>= 2;
	}

	if(input > Res)
		Res++;

	return Res;
}

