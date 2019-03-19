#pragma once
#include <stdint.h>
#include <stdlib.h>
//#include <math.h>
#include <string.h>
#include "biquad.h"
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif // M_PI
#define MAX(a,b) ( (a) > (b) ? (a) : (b) )
#define X_ABS(a) ( (a) > 0 ? (a) : (-a)) 

typedef struct 
{
	float f = 0;
	float q = 1.414;	// q = 1 / Q = 1/0.707 is fixed in DBB
	float samplerate;

	// 
	float state0;
	float state1;
	// variables for test to find the correct control parameters
	float A1;
	float A2;
	float f1;
	float f2;
	float slove;
}
VarFcFilter;
void setVarFcFilter(VarFcFilter *obj, float f,float q) {
	// set the f and q factor , as the q is fixed,we only set the f factor here.
	obj->f = f;
	obj->q = q;
}
VarFcFilter* newVarFcFilter(float samplerate) {
	VarFcFilter* obj = (VarFcFilter*)malloc(sizeof(VarFcFilter));
	if (!obj)
	{
		return NULL;
	}
	memset(obj, 0, sizeof(VarFcFilter));
	obj->samplerate = samplerate;
	return obj;
}
//  ========
void getControlFactor(VarFcFilter *obj, float envelope) {
	// get control factor from envelope.

	// your test here, set the reference A1,A2,f1,f2 and caculate the |f| factor.

	//	Get the control factor from envelope, f can be computed as follow formula:
	//	float slope = M_PI * ( sin( M_PI * f2 / fs ) - sin(M_PI *f1 / fs ) ) / (A2 - A1);
	//	float f = slope * 2.0 *(envelope - A1) / M_PI + 2.0 * sin(M_PI * f1 / fs);

	float cf = 0.0848946;
	float f = ((envelope > cf ? envelope : cf) + cf) * 0.024224 + 0.0045815;

	setVarFcFilter(obj, f,obj->q);
}

void runVarFcFilter(VarFcFilter *obj, float in, float &hpf_out) {
	//  Variable cut-off frequency high-pass filter reference from DAFX.Chap2-State variable ﬁlter

	// f = 2 * sin( pi * f_c / f_s)
	// q = 1 /  Q_factor is fixed 0.707
	//hpf_out = in - state[1] - q * state[0];
	//state[1] = state[0] * f + state[1];			// lpf out
	//state[0] = hpf_out * f + state[0];			// bpf out
	// brf = hpf + lpf;

	hpf_out = in - obj->state1 - obj->q * obj->state0;
	obj->state1 = obj->state0 * obj->f + obj->state1;		// lpf out
	obj->state0 = hpf_out * obj->f + obj->state0;				// bpf out
}
VarFcFilter* createVarFcFilter(float samplerate,float f, float q) {
	VarFcFilter * obj = newVarFcFilter(samplerate);
	// set coefficients
	setVarFcFilter(obj,f, q);
	return obj;
}

int freeVariFcFilter(VarFcFilter * obj) {
	if (!obj) {
		return -1;
	}
	free(obj);
	return 0;
}


typedef struct  {
	// These initialized values of variables are for reference.
	// public variable
	float attack_time;		// 0.05;	second (s)
	float release_time;		// 0.1;		second (s)
	float sample_rate;		// 48e3;

	// private variable
	float alpha_a = exp(-log(9) / (sample_rate * attack_time));
	float alpha_r = exp(-log(9) / (sample_rate * release_time));
	float y1 = 0;
	float e_ave = 0;

}
EnvelopeDetector;
void getAverEnv(EnvelopeDetector *obj, float in, float &envelope) {
	//Running agerage, get the agerage envelope, reference from the maxLimiter.
	//float x_abs = abs(in);
	//y1 = max(x_abs(n), alpha_r*y1);
	//e_ave(n) = alpha_a * e_ave(n - 1) + (1 - alpha_a) * y1;
	obj->y1 = MAX(X_ABS(in), obj->alpha_r * obj->y1);
	obj->e_ave = obj->alpha_a * obj->e_ave + (1 - obj->alpha_a) * obj->y1;
	envelope = obj->e_ave;
}
void setEnvelopeDetector(EnvelopeDetector*obj, float attack_time, float release_time, float sample_rate) {
	obj->attack_time = attack_time;
	obj->release_time = release_time;
	obj->sample_rate = sample_rate;
	obj->alpha_a = exp(-log(9) / (sample_rate * attack_time));
	obj->alpha_r = exp(-log(9) / (sample_rate * release_time));
}
EnvelopeDetector* createEnvelopeDetector(float attack_time, float release_time, float sample_rate) {
	EnvelopeDetector* obj = (EnvelopeDetector*)malloc(sizeof(EnvelopeDetector));
	memset(obj, 0, sizeof(*obj));						// initialize the private variables.
	setEnvelopeDetector(obj, attack_time, release_time, sample_rate);
	return obj;
}
void freeEnvelopeDetector(EnvelopeDetector * obj) {
	free(obj);
}

typedef struct {
	VarFcFilter* hpf;
	Biquad* boostFilter;
	EnvelopeDetector* envelopeDetector;
	// 
	float f = 0;
	float q = 1.414;	// q = 1 / Q = 1/0.707 is fixed in DBB
	float boostFilterCoeffs[2][3];
	// variables for test to find the correct control parameters
	float A1;
	float A2;
	float f1;
	float f2;
	float slove;
	//
	float fs_;
	float boostFreq_;
	float boostGain_;
	float boostBw_;
	// variable fc high-pass filter
	float state[2];
}
DBB2;


void setBoostFilter(DBB2* obj,float boostFreq, float boostGain, float boostBw, float fs) {
	// calculate the coefficients of boost filter
	// boostBw: octave

	float w0 = 2.0 * M_PI * boostFreq / fs;
	float A = pow(10, (boostGain / 40.0));
	float alpha = sin(w0)*sinh(log(2.0) / 2.0 * boostBw * w0 / sin(w0));

	obj->boostFilterCoeffs[0][0] = 1 + alpha * A;	// b0
	obj->boostFilterCoeffs[0][1] = -2 * cos(w0);	// b1
	obj->boostFilterCoeffs[0][2] = 1 - alpha * A;	// b2
	obj->boostFilterCoeffs[1][0] = 1 + alpha / A;	// a0
	obj->boostFilterCoeffs[1][1] = -2 * cos(w0);	// a1
	obj->boostFilterCoeffs[1][2] = 1 - alpha / A;	// a2

	// Normalize
	float a0 = obj->boostFilterCoeffs[1][0];
	for (size_t i = 0; i < 2; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			obj->boostFilterCoeffs[i][j] = float(obj->boostFilterCoeffs[i][j]) / float(a0);
		}
	}
	// set boost filter coefficients
	setBiquad(obj->boostFilter,obj->fs_, obj->boostFilterCoeffs[0], obj->boostFilterCoeffs[1]);
}
DBB2* createDbb2(float boostFreq, float boostGain, float boostBw, float fs) {
	DBB2* obj = (DBB2*)malloc(sizeof(DBB2));
	if (!obj)
	{
		return NULL;
	}
	// record the coefficients for check.
	obj->boostFreq_ = boostFreq;
	obj->fs_ = fs;
	obj->boostGain_ = boostGain;
	obj->boostBw_ = boostBw;

	obj->envelopeDetector = createEnvelopeDetector(0.05, 0.1, fs);// envelope detector's default at is 0.05 s, rt is 0.1 s.
	obj->hpf = createVarFcFilter(fs, 0.5, 1.414); // the |f| parameter is controled by the output of envelop detector 
	obj->boostFilter = newBiquad(fs);
	setBoostFilter(obj, boostFreq, boostGain, boostBw, fs);
	return obj;
}
float runDbb2(DBB2* obj,float in,float &out) {
	float averEnvelope=0;
	float boostFilterOut = 0;
	getAverEnv(obj->envelopeDetector,in, averEnvelope);
	getControlFactor(obj->hpf, averEnvelope); // change the |f| factor of variable cut-off frequency hpf

	runBiquad(obj->boostFilter, in, boostFilterOut);
	runVarFcFilter(obj->hpf, boostFilterOut, out);
	return out;
}

int freeDbb2(DBB2* obj) {
	if (!obj)
	{
		return -1;
	}
	free(obj->hpf);
	free(obj->boostFilter);
	free(obj->envelopeDetector);
	free(obj);
	return 0;
}