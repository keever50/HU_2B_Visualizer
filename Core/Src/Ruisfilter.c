/*
 * Ruisfilter.c
 *
 *  Created on: Nov 19, 2024
 *      Author: Niels
 */
#include "main.h"
#include "stm32f4xx_it.h"
#include <stdbool.h>
#include <stdlib.h>
#include "Filter.h"

extern ADC_HandleTypeDef hadc1;
extern DAC_HandleTypeDef hdac;


int16_t Buffer_1[buffer_Groote];
int16_t Hoogste_1 = 0;
int16_t Laagste_1 = 4096;
int16_t Amplitude_1 = 0;

int16_t Buffer_2[buffer_Groote];
int16_t Hoogste_2 = 0;
int16_t Laagste_2 = 4096;
int16_t Amplitude_2 = 0;


int Flag = 0;
int Index = 1;

extern void DFTEST(); // om timings van dft te testen

void trigger() // wordt door de interupt van tim3 aangeroepen.
{
	RuisFilt();

}

volatile int AD_CON_Links() // kijkt op linker AD.
{
	 HAL_ADC_Start(&hadc1);										// start conversion
	 HAL_ADC_PollForConversion(&hadc1,11);
	 volatile int value = HAL_ADC_GetValue(&hadc1);// wait for conversion to end -- mux ADC123_IN11 input PC1

	 return value;
}

void DA_CON(uint16_t value_Links) //Debug voor controleren van output
{
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value_Links);	// setvalue on DAC1	PA4
	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);							// execute new value
}

void DuurDraad() // debug: kan input direct outputen
{
	  uint16_t value;
	  value = AD_CON_Links();
	  DA_CON(value);
}


void RuisFilt()
{
	static int tel = 0;
	int16_t value;

	value = AD_CON_Links(); // sample


	if(Index == 1)
	{
		if(value > Hoogste_1)
			Hoogste_1 = value;
		if(Laagste_1 > value)
			Laagste_1 = value;

		Buffer_1[tel] = value;
	}
	else
	{
		if(value >= Hoogste_2) // update de hoogste of laagste value
			Hoogste_2 = value;
		if(Laagste_2 > value)
			Laagste_2 = value;

		Buffer_2[tel] = value;
	}




	if( tel >= buffer_Groote - 1 ) // als de buffer vol is
	{
		HAL_GPIO_TogglePin(GPIOD, LD4_Pin);
		tel = 0;

		if(Index == 2)
		{
			Amplitude_1 = Hoogste_1 - Laagste_1;
			if(Amplitude_2 > afwijking)
				Flag = 1;
			else
				Flag = 0;

			Index = 1;
			Hoogste_1 = 0;
			Laagste_1 = 4096;
		}
		else
		{
			Amplitude_2 = Hoogste_2 - Laagste_2;
			if(Amplitude_1 > afwijking)
				Flag = 1;
			else
				Flag = 0;

			Index = 2;
			Hoogste_2 = 0;
			Laagste_2 = 4096;
		}
	}

	else // anders wordt tel een hoger.
		tel+=1;

}






