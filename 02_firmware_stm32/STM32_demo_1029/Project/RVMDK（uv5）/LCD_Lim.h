#ifndef __LCD_LIM_H
#define	__LCD_LIM_H

#include "stdint.h"

struct Parameter{
 float MIN;
 float MAX;
 uint16_t circle_start;
 uint16_t circle_stop;
 //string name;
 };
 struct Parameter OCR,Pressure,Temp;
