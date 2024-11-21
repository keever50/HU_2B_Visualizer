/*
 * DSP.c
 *
 *  Created on: Nov 19, 2024
 *      Author: Niels
 */
#include "main.h"
#include "stm32f4xx_it.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DAC_HandleTypeDef hdac;

#define buffer_Groote 4


volatile int AD_CON_Links()
{
	 HAL_ADC_Start(&hadc1);										// start conversion
	 HAL_ADC_PollForConversion(&hadc1,11);
	 volatile int value = HAL_ADC_GetValue(&hadc1);// wait for conversion to end -- mux ADC123_IN11 input PC1

	 return value;
}



void DA_CON(volatile int value_Links)
{
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value_Links);	// setvalue on DAC1	PA4
	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);							// execute new value

}

void DuurDraad()
{
	  volatile int value;

	  value = AD_CON_Links();

	  DA_CON(value);

}

void MAF()

{
	volatile int value;
	int i;
	int totaal=0,Gem=0;
	static volatile int buffer[buffer_Groote];



	value = AD_CON_Links();

	for(i=0;i<buffer_Groote-1;i++)
	{
		buffer[i] = buffer[i+1];


		totaal += buffer[i];

	}

	buffer[buffer_Groote-1] = value;


	totaal += value;


	Gem = totaal/buffer_Groote;


	DA_CON(Gem);


}

