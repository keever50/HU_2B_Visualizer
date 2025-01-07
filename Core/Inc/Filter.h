
#ifndef FILTER_H
#define FILTER_H

#define buffer_Groote 50
#define Dig_Bereik_bits 12 // het orginele bereik 12 bits
#define Dig_Bereik 4096
#define Bin_Bereik_bits 10 //in hoeveel bits de bin waarde is bv 10 bit is 0:0 0.5 = 512 1=1024

#define afwijking 50 // afwijking tussen punten dat gezien wordt als ruis

#define Rekenruis 2500 // DFT Bin: drempelwaarde dat waardes worden toegelaten. voor de sqrt calculatie.

#define DcLaag  0
#define DCHoog  10

#define Blackman 0
#define Hamming 1
#define GEEN 2
#define Filter Blackman

extern int16_t Buffer_1[buffer_Groote];
extern int16_t Amplitude_1;
extern int16_t Laagste_1;

extern int16_t Buffer_2[buffer_Groote];
extern int16_t Amplitude_2;
extern int16_t Laagste_2;

extern int Flag;

extern int Index;

#endif
