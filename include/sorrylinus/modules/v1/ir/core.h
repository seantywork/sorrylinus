#ifndef _SORRYLINUS_MOD_IRSEND_H_
#define _SORRYLINUS_MOD_IRSEND_H_

#ifdef __cplusplus
extern "C" {
#endif



#include "sorrylinus/modules/v1/core.h"

#include <pigpio.h>

#define MAX_COMMAND_SIZE 512
#define MAX_OPT_SIZE 2048
#define MAX_PULSES 12000




void ir_get_opts(char* result);

void ir_send(char* result, char* opt);

void addPulse(uint32_t onPins, uint32_t offPins, uint32_t duration, gpioPulse_t *irSignal, int *pulseCount);


// Generates a square wave for duration (microseconds) at frequency (Hz)
// on GPIO pin outPin. dutyCycle is a floating value between 0 and 1.
void carrierFrequency(uint32_t outPin, double frequency, double dutyCycle, double duration, gpioPulse_t *irSignal, int *pulseCount);

// Generates a low signal gap for duration, in microseconds, on GPIO pin outPin
void gap(uint32_t outPin, double duration, gpioPulse_t *irSignal, int *pulseCount);


// Transmit generated wave
int transmitWave(uint32_t outPin, gpioPulse_t *irSignal, unsigned int *pulseCount);

int irSlingRC5(uint32_t outPin,
	int frequency,
	double dutyCycle,
	int pulseDuration,
	const char *code);

int irSling(uint32_t outPin,
	int frequency,
	double dutyCycle,
	int leadingPulseDuration,
	int leadingGapDuration,
	int onePulse,
	int zeroPulse,
	int oneGap,
	int zeroGap,
	int sendTrailingPulse,
	const char *code);

int irSlingRaw(uint32_t outPin,
	int frequency,
	double dutyCycle,
	const int *pulses,
	int numPulses);

#ifdef __cplusplus
}
#endif // __cplusplus


#endif