#pragma once
/**
*	The Implement of the Limiter
*
*/
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14.592654
#endif // !M_PI
#ifndef M_ABS
#define M_ABS(x) (x)<0?(-(x)):(x)
#endif // !M_ABS

typedef struct {
	// interface properties
	float threshold	;			// Threshold	dB		[50, 0]
	float ratio		;			// ratio		none	[1, 50]
	float kneeWidth	;			// KneeWidth	dB		[0, 20]
	float attackTime	;		// attack time	seconds	[0,	4]
	float releaseTime ;		// release time	seconds	[0, 4]
	float makeUpGain	;			// makeup gain	dB		[-10,24]

	// private parameters
	size_t samplerate;
	float gc;				// gain computer
	float gs;				// gain smoothing
	float alphaA;			// attack time factor
	float alphaR;			// release time factor

	float x_sc;				// the static characteristic of input x_dB
	float rms;
}
RmsLimiter;
static inline void RmsLimiter_runGainSmoothing(RmsLimiter* obj, float gain) {
	if (gain <obj->gs)
	{
		obj->gs = obj->alphaA * obj->gs + (1.0f - obj->alphaA)*gain;
	}
	else
	{
		obj->gs = obj->alphaR * obj->gs + (1.0f - obj->alphaR)*gain;
	}
}
static inline float RmsLimiter_runGainComputer(RmsLimiter* obj,float x_dB) {
	float gc = 0;
	float x_sc = 0;
	float tmp2 = x_dB - obj->threshold + obj->kneeWidth / 2.0f;
	if (x_dB < obj->threshold - obj->kneeWidth / 2.0f)
	{
		x_sc = x_dB;
	}
	else if (x_dB <= obj->threshold - obj->kneeWidth / 2.0f  )
	{
		x_sc = x_dB + (1.0f / obj->ratio - 1)*(tmp2*tmp2) / (2.0f*obj->kneeWidth);
	}
	else 
	{
		x_sc = obj->threshold + (x_dB - obj->threshold) / obj->ratio;
	}
	return x_sc - x_dB;	// return gc
}
RmsLimiter* RmsLimiter_create(float threshold, float ratio, float kneeWidth, float attackTime, float releaseTime, float makeUpGain, float samplerate) {
	RmsLimiter* obj = (RmsLimiter*)malloc(sizeof(RmsLimiter));
	if (!obj)
	{
		return NULL;
	}
	obj->threshold = threshold;
	obj->ratio = ratio;
	obj->kneeWidth = kneeWidth;
	obj->attackTime = attackTime;
	obj->releaseTime = releaseTime;
	obj->makeUpGain = makeUpGain;
	obj->samplerate = samplerate;

	//
	obj->alphaA = expf( -log(9.0f) / (double)(obj->samplerate * obj->attackTime) );
	obj->alphaR = expf( -log(9.0f) / (double)(obj->samplerate * obj->releaseTime) );

	obj->gc = 0;
	obj->gs = 0;
	obj->x_sc = 0;
	obj->rms = 0.01;
	return obj;
}
//int RmsLimiter_set(int threshold,int ratio,int kneeWidth,int attackTime,int releaseTime,int makeUpGain,size_t samplerate);
float RmsLimiter_process(RmsLimiter* obj, float input, float& output) {
	//	input:	linear input data
	//	output:	linear output data

	//------- Convert input to dB -------//
	//float inputGain = 20 * log10( M_ABS(input) + 0.00001);	// the gain of input sample in dB |x_dB|
	float inputGain = 20 * log10( fabsf(input) + 0.00001);	// the gain of input sample in dB |x_dB|

	//obj->rms = (1- 0.0001) * obj->rms + 0.0001 * input*input;// y(n) = (1-alpha)*y(n-1) + alpha*x(n)^2
	//float inputGain = 10*log10(obj->rms); // RMS version

	//------- Running gain computer -------//
	float tmp2 = inputGain - obj->threshold + obj->kneeWidth / 2.0f;
	if (inputGain < obj->threshold - obj->kneeWidth / 2.0f)
	{
		obj->x_sc = inputGain;
	}
	else if (inputGain > obj->threshold + obj->kneeWidth / 2.0f)
	{
		obj->x_sc = obj->threshold + (inputGain - obj->threshold) / obj->ratio;

	}
	else
	{
		obj->x_sc = inputGain + (1.0f / obj->ratio - 1)*(tmp2*tmp2) / (2.0f*obj->kneeWidth);
	}
	obj->gc = obj->x_sc - inputGain;


	//------- Running gain smoothing -------//
	if (obj->gc < obj->gs)
	{
		obj->gs = obj->alphaA * obj->gs + (1.0f - obj->alphaA)*obj->gc;
	}
	else
	{
		obj->gs = obj->alphaR * obj->gs + (1.0f - obj->alphaR)*obj->gc;
	}


	//------- Running Makeup Gain -------//
	// TBD.
	//obj->makeUpGain = (-x_sc)_{ x_dB = 0 }
	float gainMadeUp = obj->gs + obj->makeUpGain;


	//------- Applying gain -------//
	float linearGain = powf(10.0, gainMadeUp / 20.0f);
	output = input * linearGain;

	return output;

}
//int RmsLimiter_free();



