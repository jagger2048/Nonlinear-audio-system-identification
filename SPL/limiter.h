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
#define M_ABS(x) (x)<0?-(x):(x)
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
	float levelDetected ;
	float gc;				// gain computer
	float gs;				// gain smoothing
	float alphaA;			// attack time factor
	float alphaR;			// release time factor

	float x_sc;				// the static characteristic of input x_dB
	float rms;
}
RmsLimiter;
static inline void RmsLimiter_runGainSmoothing(RmsLimiter* self, float gain) {
	if (gain <self->gs)
	{
		self->gs = self->alphaA * self->gs + (1.0f - self->alphaA)*gain;
	}
	else
	{
		self->gs = self->alphaR * self->gs + (1.0f - self->alphaR)*gain;
	}
}
static inline float RmsLimiter_runGainComputer(RmsLimiter* self,float x_dB) {
	float gc = 0;
	float x_sc = 0;
	float tmp2 = x_dB - self->threshold + self->kneeWidth / 2.0f;
	if (x_dB < self->threshold - self->kneeWidth / 2.0f)
	{
		x_sc = x_dB;
	}
	else if (x_dB <= self->threshold - self->kneeWidth / 2.0f  )
	{
		x_sc = x_dB + (1.0f / self->ratio - 1)*(tmp2*tmp2) / (2.0f*self->kneeWidth);
	}
	else 
	{
		x_sc = self->threshold + (x_dB - self->threshold) / self->ratio;
	}
	return x_sc - x_dB;	// return gc
}
RmsLimiter* RmsLimiter_create(float threshold, float ratio, float kneeWidth, float attackTime, float releaseTime, float makeUpGain, float samplerate) {
	RmsLimiter* self = (RmsLimiter*)malloc(sizeof(RmsLimiter));
	if (!self)
	{
		return NULL;
	}
	self->threshold = threshold;
	self->ratio = ratio;
	self->kneeWidth = kneeWidth;
	self->attackTime = attackTime;
	self->releaseTime = releaseTime;
	self->makeUpGain = makeUpGain;
	self->samplerate = samplerate;

	//
	self->alphaA = exp( -log(9.0f) / (double)(self->samplerate * self->attackTime) );
	self->alphaR = exp( -log(9.0f) / (double)(self->samplerate * self->releaseTime) );

	self->levelDetected = 0;
	self->gc = 0;
	self->gs = 0;
	self->x_sc = 0;
	self->rms = 0.01;
	return self;
}
//int RmsLimiter_set(int threshold,int ratio,int kneeWidth,int attackTime,int releaseTime,int makeUpGain,size_t samplerate);
float RmsLimiter_process(RmsLimiter* self, float input, float& output) {
	//if (M_ABS(input - 1e-10) < 1e-10)
	//{
	//	output = input;
	//	return output;
	//}
	//------- Convert input to dB -------//
	//float inputGain = 20 * log10( M_ABS(input) );	// the gain of input sample in dB |x_dB|
	self->rms = (1- 0.0001) * self->rms + 0.0001 * input*input;
	float inputGain = 10*log10(self->rms);

	//------- Running gain computer -------//
	//self->gc = RmsLimiter_runGainComputer(self,inputGain);
	float tmp2 = inputGain - self->threshold + self->kneeWidth / 2.0f;
	if (inputGain < self->threshold - self->kneeWidth / 2.0f)
	{
		self->x_sc = inputGain;
	}
	else if (inputGain >= self->threshold - self->kneeWidth / 2.0f)
	{
		self->x_sc = inputGain + (1.0f / self->ratio - 1)*(tmp2*tmp2) / (2.0f*self->kneeWidth);
	}
	else
	{
		self->x_sc = self->threshold + (inputGain - self->threshold) / self->ratio;
	}
	self->gc = self->x_sc - inputGain;


	//------- Running gain smoothing -------//
	if (self->gc < self->gs)
	{
		self->gs = self->alphaA * self->gs + (1.0f - self->alphaA)*self->gc;
	}
	else
	{
		self->gs = self->alphaR * self->gs + (1.0f - self->alphaR)*self->gc;
	}


	//------- Running Makeup Gain -------//
	// to be completed.
	//self->makeUpGain = (-x_sc)_{ x_dB = 0 }
	float gainMadeUp = self->gs + self->makeUpGain;


	//------- Applying gain -------//
	float linearGain = pow(10.0, gainMadeUp / 20.0f);
	output = input * linearGain;


	return output;

}
//int RmsLimiter_free();



