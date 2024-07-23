#include "sorrylinus/modules/v1/ir/core.h"


void ir_get_opts(char* result){

	char optsdir[1024] = {0};

	DIR* d;

	struct dirent *dir;

	strcat(optsdir, "./conf/");

	strcat(optsdir, SOLIMODCFG->mod_ir_opts_dir);

	d = opendir(optsdir);

	if(d){

		while((dir = readdir(d)) != NULL){

			if(strcmp(dir->d_name, ".") == 0){
				continue;
			}

			if(strcmp(dir->d_name, "..") == 0){
				continue;
			}

			strcat(result, dir->d_name);

			strcat(result, "\n");
			
		}

		closedir(d);
	}


}


void ir_send(char* result, char* opt){

	char optfile[1024] = {0};

	strcat(optfile, "./conf/");

	strcat(optfile, SOLIMODCFG->mod_ir_opts_dir);

	strcat(optfile, "/");

	strcat(optfile, opt);

	uint8_t buff[MAX_OPT_SIZE] = {0};

	int check = read_file_to_buffer(buff, MAX_OPT_SIZE, optfile);

	if (check < 0){

		strcpy(result, "ir_send: failed to read opt");

		return;
	}

	int* signal;	

	int signalc = 0;

    char** lines;

    int line_count = 0;

    char* token;

    char* delim = "\n";

	char* delim2 = " ";

    token = strtok(buff, delim);

    while(token != NULL){

        int linelen = 0;

        if (line_count == 0){

            lines = (char**)malloc(sizeof(char*) * (line_count + 1));

        } else {

            lines = (char**)realloc(lines, sizeof(char*) * (line_count + 1));

        }

        linelen = strlen(token) + 1;

        lines[line_count] = (char*)malloc(sizeof(char) * linelen);

        memset(lines[line_count], 0 , sizeof(char) * linelen);

        strcpy(lines[line_count], token);

        line_count += 1;

        token = strtok(NULL, delim);
    }

    for (int i = 0 ; i < line_count; i++) {

    	token = strtok(lines[i], delim2);

		while(token != NULL){

			if(signalc == 0){

				signal = (int*)malloc(sizeof(int) * (signalc + 1));

				sscanf(token, "%d", &(signal[signalc]));

			} else {

				signal = (int*)realloc(signal, sizeof(int) * (signalc + 1));

				sscanf(token, "%d", &(signal[signalc]));

			}

			token = strtok(NULL, delim2);

			signalc += 1;
		}


    }

	double duty_cycle = (double)(SOLIMODCFG->mod_ir_gap / 1000000);


	/*


		int check = irSlingRaw(
						SOLIMODCFG->mod_ir_outpin,
						SOLIMODCFG->mod_ir_frequency,
						duty_cycle,
						signal,
						sizeof(signal) / sizeof(signal));
	*/

	char testout[512] = {0};

	stringify_array_int(testout, signalc, signal);

	strcpy(result, testout);

	free(signal);

    for(int i = 0 ; i < line_count; i++){

        free(lines[i]);

    }

    if (line_count != 0){

        free(lines);
    }


}





void addPulse(uint32_t onPins, uint32_t offPins, uint32_t duration, gpioPulse_t *irSignal, int *pulseCount){
	int index = *pulseCount;

	irSignal[index].gpioOn = onPins;
	irSignal[index].gpioOff = offPins;
	irSignal[index].usDelay = duration;

	(*pulseCount)++;
}


void carrierFrequency(uint32_t outPin, double frequency, double dutyCycle, double duration, gpioPulse_t *irSignal, int *pulseCount){
	double oneCycleTime = 1000000.0 / frequency; // 1000000 microseconds in a second
	int onDuration = (int)round(oneCycleTime * dutyCycle);
	int offDuration = (int)round(oneCycleTime * (1.0 - dutyCycle));

	int totalCycles = (int)round(duration / oneCycleTime);
	int totalPulses = totalCycles * 2;

	int i;
	for (i = 0; i < totalPulses; i++)
	{
		if (i % 2 == 0)
		{
			// High pulse
			addPulse(1 << outPin, 0, onDuration, irSignal, pulseCount);
		}
		else
		{
			// Low pulse
			addPulse(0, 1 << outPin, offDuration, irSignal, pulseCount);
		}
	}
}


void gap(uint32_t outPin, double duration, gpioPulse_t *irSignal, int *pulseCount){
	addPulse(0, 0, duration, irSignal, pulseCount);
}


int transmitWave(uint32_t outPin, gpioPulse_t *irSignal, unsigned int *pulseCount){
	// Init pigpio
	if (gpioInitialise() < 0)
	{
		// Initialization failed
		printf("GPIO Initialization failed\n");
		return 1;
	}

	// Setup the GPIO pin as an output pin
	gpioSetMode(outPin, PI_OUTPUT);

	// Start a new wave
	gpioWaveClear();

	gpioWaveAddGeneric(*pulseCount, irSignal);
	int waveID = gpioWaveCreate();

	if (waveID >= 0)
	{
		int result = gpioWaveTxSend(waveID, PI_WAVE_MODE_ONE_SHOT);

		printf("Result: %i\n", result);
	}
	else
	{
		printf("Wave creation failure!\n %i", waveID);
	}

	// Wait for the wave to finish transmitting
	while (gpioWaveTxBusy())
	{
		time_sleep(0.1);
	}

	// Delete the wave if it exists
	if (waveID >= 0)
	{
		gpioWaveDelete(waveID);
	}

	// Cleanup
	gpioTerminate();
	return 0;
}



int irSlingRC5(uint32_t outPin,
	int frequency,
	double dutyCycle,
	int pulseDuration,
	const char *code){

	if (outPin > 31)
	{
		// Invalid pin number
		return 1;
	}

	size_t codeLen = strlen(code);

	printf("code size is %zu\n", codeLen);

	if (codeLen > MAX_COMMAND_SIZE)
	{
		// Command is too big
		return 1;
	}

	gpioPulse_t irSignal[MAX_PULSES];
	int pulseCount = 0;

	// Generate Code
	int i;
	for (i = 0; i < codeLen; i++)
	{
		if (code[i] == '0')
		{
			carrierFrequency(outPin, frequency, dutyCycle, pulseDuration, irSignal, &pulseCount);
			gap(outPin, pulseDuration, irSignal, &pulseCount);
		}
		else if (code[i] == '1')
		{
			gap(outPin, pulseDuration, irSignal, &pulseCount);
			carrierFrequency(outPin, frequency, dutyCycle, pulseDuration, irSignal, &pulseCount);
		}
		else
		{
			printf("Warning: Non-binary digit in command\n");
		}
	}

	printf("pulse count is %i\n", pulseCount);
	// End Generate Code

	return transmitWave(outPin, irSignal, &pulseCount);
}

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
	const char *code){
	if (outPin > 31)
	{
		// Invalid pin number
		return 1;
	}

	size_t codeLen = strlen(code);

	printf("code size is %zu\n", codeLen);

	if (codeLen > MAX_COMMAND_SIZE)
	{
		// Command is too big
		return 1;
	}

	gpioPulse_t irSignal[MAX_PULSES];
	int pulseCount = 0;

	// Generate Code
	carrierFrequency(outPin, frequency, dutyCycle, leadingPulseDuration, irSignal, &pulseCount);
	gap(outPin, leadingGapDuration, irSignal, &pulseCount);

	int i;
	for (i = 0; i < codeLen; i++)
	{
		if (code[i] == '0')
		{
			carrierFrequency(outPin, frequency, dutyCycle, zeroPulse, irSignal, &pulseCount);
			gap(outPin, zeroGap, irSignal, &pulseCount);
		}
		else if (code[i] == '1')
		{
			carrierFrequency(outPin, frequency, dutyCycle, onePulse, irSignal, &pulseCount);
			gap(outPin, oneGap, irSignal, &pulseCount);
		}
		else
		{
			printf("Warning: Non-binary digit in command\n");
		}
	}

	if (sendTrailingPulse)
	{
		carrierFrequency(outPin, frequency, dutyCycle, onePulse, irSignal, &pulseCount);
	}

	printf("pulse count is %i\n", pulseCount);
	// End Generate Code

	return transmitWave(outPin, irSignal, &pulseCount);
}

int irSlingRaw(uint32_t outPin,
	int frequency,
	double dutyCycle,
	const int *pulses,
	int numPulses){
	if (outPin > 31)
	{
		// Invalid pin number
		return 1;
	}

	// Generate Code
	gpioPulse_t irSignal[MAX_PULSES];
	int pulseCount = 0;

	int i;
	for (i = 0; i < numPulses; i++)
	{
		if (i % 2 == 0) {
			carrierFrequency(outPin, frequency, dutyCycle, pulses[i], irSignal, &pulseCount);
		} else {
			gap(outPin, pulses[i], irSignal, &pulseCount);
		}
	}

	printf("pulse count is %i\n", pulseCount);
	// End Generate Code

	return transmitWave(outPin, irSignal, &pulseCount);
}