#include "../headerfiles/qsr.h"
#include <stdio.h>
#include <stdlib.h>
#include "../headerfiles/arrayFunctions.h"

void peakDetection(QRS_params *params)
{
	int result;
	isThereAPeak(params->pArrayWithPeak, &result);
	if (result == 0) {return;}

	isItRPeak(params->pArrayWithPeak, params->THRESHOLD1, &result);
	if (result == 0) {
		params->NPKF = noicePeakDetected(params->THRESHOLD1, params->THRESHOLD2, params->NPKF, params->SPKF, params->pArrayWithPeak[1]);
		return;
	}
	int average2;
	calculateAverage2(params->pRRInterval, params->sizeOfRRInterval, params->THRESHOLD1, &average2);
	isRRIntervalBetweenLowAndHigh(params->pRRInterval[0], average2, &result);

	if (result == 1) {
		regularRPeakDetected(&params, params->pArrayWithPeak[1]);
		return;
	}

	isRRIntervalLargerThanMiss(params->pRRInterval[0], average2, &result);
	if (result == 0) {return;}


}

void isThereAPeak(int *arrayWithPeak, int *result) {
	if (arrayWithPeak[1] > arrayWithPeak[0] && arrayWithPeak[1] > arrayWithPeak[2]) {
		*result = 1;
	} else {
		*result = 0;
	}

}

void isItRPeak(int peak, int *threshold1, int *result) {
	if (peak > threshold1) {
		*result = 1;
	} else {
		*result = 0;
	}
}

void insertRR(int *pRRInterval, int *sizeOfRRInterval, int *RRIntervalCounter) {
	rotateArrayOnce(pRRInterval,sizeOfRRInterval);
	pRRInterval[0] = RRIntervalCounter;
	*RRIntervalCounter = 0;

}

void calculateAverage2(int *pRRInterval, int sizeOfRRInterval, int *threshold1, int *result) {
	*result = 0;
	int runUntil8 = 0;
	int arrayCounter = 0;
	while (runUntil8 != 8 && arrayCounter != sizeOfRRInterval-1) {
		if (pRRInterval[arrayCounter] > threshold1) {
			*result = *result + pRRInterval[arrayCounter];
			runUntil8++;
		}
		arrayCounter++;
	}

	*result = *result/8;
}

void isRRIntervalBetweenLowAndHigh(int rrInterval, int average2,int *result) {
	int low = 0.92*(average2);
	int high = 1.16*(average2);
	if (rrInterval > low && rrInterval < high) {
		*result = 1;
	} else {
		*result = 2;
	}

}

void isRRIntervalLargerThanMiss(int rrInterval, int average, int *result) {
	int miss = 1.66*average;
	if (rrInterval > miss) {
		*result = 1;
	} else {
		*result = 2;
	}
}


void noicePeakDetected(int *threshold1, int *threshold2, int *NPKF, int *SPKF, int peak) {
	*threshold1 = NPKF + 0.25*((*SPKF)-(*NPKF));
	*threshold2 = (*threshold1)/2;
	*NPKF = 0.125*peak+0.875*(*NPKF);
}

void regularRPeakDetected(QRS_params *params, int peak) {
	rotateArrayOnce(params->pRPeakArray, params->sizeOfRPeakArray);
	params->pRPeakArray[0] = peak;
	params->SPKF = 0.125*peak+0.875*(params->SPKF);
	insertRR(params->pRRInterval, params->sizeOfRRInterval, params->RRIntervalCounter);
	insertRR(params->pRRIntervalAll, params->sizeOfRRIntervalAll, params->RRIntervalAllCounter);
	params->THRESHOLD1 = params->NPKF + 0.25*(params->SPKF-params->NPKF);
	params->THRESHOLD2 = (params->THRESHOLD1)/2;

}

