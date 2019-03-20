
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "wavfile.h"
#include "dr_wav.h"

#include <fstream>
#include <iostream>
#include <fstream>

#include <math.h>
#include "dynamic_bass_boost2.h"
#include "dynamic_bass_boost.h"
#include "response_measurement.h"
using namespace std;

void irUnitTest() {

	float fs = 48e3;
	float f1 = 1;
	float f2 = 20e3;
	float duration = 1;
	size_t len = duration * fs;
	float* ess = generateExpSineSweep(duration, f1, f2, fs);
	float* yFiltered = (float*)malloc(sizeof(float)*len);

	float coefs[2][3] = { 
		{ 0.00391612666054739,	0.00783225332109477,	0.00391612666054739} ,
		{ 1, - 1.81534108270457,	0.831005589346757 }
	};
	Biquad* filter = createBiquad(fs,coefs[0],coefs[1]);
	for (size_t n = 0; n < len; n++)
	{
		runBiquad(filter, ess[n], yFiltered[n]);
	}
	float* ir = (float*)malloc(sizeof(float)*len);
	float* mag_db =  findSystemIR(yFiltered, duration, f1,f2, fs,ir);
	


	fstream fo;
	fo.open("biquad out5.txt", ios::out);
	for (size_t n = 0; n < len+1 ; n++)
	{
		fo << mag_db[n]<< "\n";
	}
	fo.close();
	free(ess);
	free(yFiltered);
	free(mag_db);
	free(ir);
}


int main()
{
	dbbUnitTest();

	return 0;
}
