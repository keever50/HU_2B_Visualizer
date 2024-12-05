/*
 * DSP.c
 *
 *  Created on: Nov 19, 2024
 *      Author: Niels
 */
#include "main.h"
#include "stm32f4xx_it.h"
#include <stdbool.h>
#include <stdlib.h>

extern ADC_HandleTypeDef hadc1;
//extern ADC_HandleTypeDef hadc2; // voor als we de rechter adc ook willen gebruiken.
extern DAC_HandleTypeDef hdac;

#define Dig_Bereik 4095.0 // het orginele bereik 12 bits
#define buffer_Groote 100 // groote van buffer
#define afwijking 25 // afwijking tussen punten dat gezien wordt als ruis
#define LengteRuis 5 // de lengte van deze punten gezien wordt als ruis.


bool klaar =false;
int16_t buffer[buffer_Groote];
int16_t AmBuffer[buffer_Groote];
int16_t inputbuffer[buffer_Groote * 4];
int16_t hoogste = -2048; // onthoud de hoogste waarde
int16_t laagste = 2047;
int tel = 0;

int Printtel = 0;


void trigger() // wordt door de interupt van tim3 aangeroepen.
{
	if(klaar)
	{
		//Shift(hoogste,laagste);
		//DA_CON(buffer[Printtel]); // stuurt de buffer uit voor debug
		if(Printtel >= buffer_Groote-LengteRuis)
			Printtel = 0;
		else
			tel++;
	}
	else
	 DF(); // roept de digitale filter.

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


void DF() // zet de waardes om van 0 tot 4095 naar -2048 tot 2047.
{
	static int testTel = 0;
	 // onthoud de laagste waarde

	int16_t value;

	value = AD_CON_Links(); // sample
	buffer[tel] = value; // sample wordt opgeslagen in de buffer.
	inputbuffer[testTel] = value;

	if(hoogste < value) // update de hoogste of laagste value
		hoogste = value;
	if(laagste > value)
		laagste = value;

	tel = Ruis(value,tel); // tel kan veranderen als er ruis is gevonden dan gaat hij deze overschrijven.

	if( tel >= buffer_Groote ) // als de buffer vol is
	{
		Shift(hoogste,laagste);
		//hoogste = 0;
		//laagste = 4096;
		//tel = 0;
		klaar = true;

	}
	else // anders wordt tel een hoger.
		tel++;

	//testTel++;

}

int Ruis(int value,int Tel) // zorgt voor de ruis detectie.
{
	static int16_t VorigeSample = -1;
	static int16_t Ruispunten = 0; // onthoud hoeveel punten een kleine afwijking heeft.

	int16_t verschil = abs(VorigeSample - value);
	if(verschil < afwijking) // als er een kleine afwijking inzit onthoud hij dat.
	{
		Ruispunten += 1; // telt de hoeveelheid punten die dicht bij elkaar liggen
		//TotRuis += verschil;
	}
	else if(Ruispunten > LengteRuis) // als ruis voor de lengte nog onderling te dicht bij elkaar ligt.
	{
		if(Ruispunten > tel ) // als tel -= Ruispunten onder de nul komt wordt tel 1.
			tel = 0;
		else // tel komt op de plek van het ruis.
			tel -= Ruispunten;
		Ruispunten = 0;
	}
	else
	{
		Ruispunten = 0;
	}

	if(tel == Tel)
		return Tel;

	VorigeSample = value;

	return tel;
}

void Shift(int Hoogste, int Laagste) // zorgt voor de verplaatsing voor DFT
{
	int16_t amplitude = Hoogste - Laagste;
	int32_t ScaleFactor = ((int32_t) Dig_Bereik *1024) / amplitude;
	//int ScaleFactor = 2;
	int16_t offset = Dig_Bereik/2;

	int32_t scaledoffset = offset + 1024;

	for(int i = 0; i < buffer_Groote; i++) // versterkt het signaal het hele bereik gebruikt en wordt meteen in juiste bereik gezet.
	{
		AmBuffer[i] = ( (buffer[i] - Laagste) * ScaleFactor) - scaledoffset;
	}

	//klaar = true; // debug om de buffer uit te sturen via de DACON

}


