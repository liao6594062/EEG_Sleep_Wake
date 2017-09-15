/*******************************************************************
Describes: 	routines for detection/score of wake, sleep and REM
   Author:	Jango
   Verson: 	v0.1
     Date: 	2017-9-11
Copyright:	EEGSmart
********************************************************************/
#include "sleepWakeDetection.h"

void removeDC(float *rawdata, int totalSecond)
{
	int i;
	int j;
	int numEpoch;
	int numHour;
	int offset;
	int numEpochLeft;
	double sum0[MAXSECOND] = {0};
	double sum1[SECONDS_OF_EPOCH] = {0};
	double sum2[EPOCH_OF_HOUR] = {0};
	double sum3;
	double sum4;
	float sum5;

	numEpoch = totalSecond/SECONDS_OF_EPOCH;
	numHour = numEpoch/EPOCH_OF_HOUR;
	numEpochLeft = numEpoch - numHour*EPOCH_OF_HOUR;

	for(i=0; i<totalSecond; i++)
	{
		for(j=0; j<NFFT; j++)
			sum0[i] += rawdata[i*NFFT+j];
		sum0[i] /= NFFT;
	}

	for(i=0; i<numEpoch; i++)
	{
		for(j=0; j<SECONDS_OF_EPOCH; j++)
			sum1[i] += sum0[i*SECONDS_OF_EPOCH+j];
		sum1[i] /= SECONDS_OF_EPOCH;
	}

	for(i=0; i<numHour; i++)
	{
		for(j=0; j<EPOCH_OF_HOUR; j++)
			sum2[i] += sum1[EPOCH_OF_HOUR*i+j];
		sum2[i] /= EPOCH_OF_HOUR;
	}

	sum3 = 0;
	for(i=0; i<numHour; i++)
		sum3 += sum2[i];

	sum4 = 0;
	for(i=0; i<numEpochLeft; i++)
	{
		offset = EPOCH_OF_HOUR*numHour + i;
		sum4 += sum1[offset];
	}
	sum4 /= numEpochLeft;
	sum5 = (float)(sum3+sum4)/(numHour+1);

	for(i=0; i<totalSecond; i++)
		rawdata[i] -= sum5;

	return;
}

void judge(float *epochFilted2, float *th, float *epoch2Filted2, float *th2, int *epochValid, int numEpoch, char *score)
{
	int i;
	int haveSlept;

	haveSlept = 0;
	for(i=0; i<numEpoch; i++)
	{
		if(epochValid[i]==NaN)
			score[i] = NaN;
		else
		{
			if(epochFilted2[i]<=th[i])
			{
				if(haveSlept==0)
					score[i] = WAKE;
				else
					score[i] = REM;
			}
			else
			{
				if(epoch2Filted2[i]<=th2[i])
					score[i] = DEEP;
				else
					score[i] = LIGHT;

				haveSlept = 1;
			}
		}
	}

	while(score[i-1]==REM)
		score[i---1] = WAKE;

	return;
}

int arrage(float *fftdata, int totalSecond, int *indClear)
{
	int i;
	int j;
	int second;

	second = 0;
	for(i=0; i<totalSecond; i++)
	{
		if(indClear[i]!=NaN)
		{
			for(j=0; j<NFFT; j++)
				fftdata[NFFT*second+j] = fftdata[NFFT*i+j];
			second++;
		}
	}
	return second;
}

void clearRaw(float *rawdata, int totalSecond, float threshold, char *indClear)
{
	int i;
	int j;
	int offset;
	int index;

	index = 0;
	for(i=0; i<totalSecond; i++)
	{
		indClear[index] = 1;
		for(j=0; j<NFFT; j++)
		{
			offset = i*NFFT + j;
			if((rawdata[offset]>threshold)||(rawdata[offset]<-1*threshold))
			{
				indClear[index] = NaN;
				break;
			}
		}
		index++;
	}

	return;
}

float mean(float *data, int num)
{
	int i;
	float sum;
	float result;

	sum = 0;
	for(i=0; i<num; i++)
		sum += data[i];
	result = sum/num;

	return result;
}

float ratioFreqBand(float *absFft)
{
	int numAlpha;
	int numBeta;
	const int alphaLow = 9;
	const int alphaHigh = 14;
	const int betaLow = 17;
	const int betaHigh = 22;
	float meanAlpha;
	float meanBeta;
	float ratioBand;

	numAlpha = alphaHigh - alphaLow + 1;
	numBeta = betaHigh - betaLow + 1;
	meanAlpha = mean(&absFft[alphaLow],numAlpha);
	meanBeta = mean(&absFft[betaLow],numBeta);
	ratioBand = meanAlpha/meanBeta;

	return ratioBand;
}

float ratioFreqBand2(float *absFft)
{
	int numAlpha;
	int numDelta;
	const int alphaLow = 9;
	const int alphaHigh = 14;
	const int deltaLow = 1;
	const int deltaHigh = 3;
	float meanAlpha;
	float meanDelta;
	float ratioBand;

	numAlpha = alphaHigh - alphaLow + 1;
	numDelta = deltaHigh - deltaLow + 1;
	meanAlpha = mean(&absFft[alphaLow],numAlpha);
	meanDelta = mean(&absFft[deltaLow],numDelta);
	ratioBand = meanAlpha/meanDelta;

	return ratioBand;
}

void averagingFilter(float *in, int length, int window, float *out)
{
	int i;
	int j;
	int midpoint;
	float tmp;

	midpoint = window/2 + 1;

	for(i=0; i<midpoint-1; i++)
		out[i] = in[i];

	for(i=midpoint-1; i<length-midpoint+1; i++)
	{
		tmp = 0;
		for(j=i-midpoint; j<i-midpoint+window; j++)
			tmp += in[j];
		out[i] = tmp/window;
	}

	for(i=length-midpoint+1; i<length; i++)
		out[i] = in[i];

	return;
}


void thSleepWake(float *ratio, int length, float *th)
{
	int i;
	const int window = 5;
	const int startPoint = 5;
	int node[2] = {0,startPoint-1};

	const float gain = ORIGINAL_THRESHOLD2;
	const float coef1 = 0.5;
	const float coef2 = 0.5;
	//int flag = 0;

	for(i=0; i<length; i++)
		th[i] = gain;

	for(i=startPoint; i<length; i++)
	{
		if(((th[i-1]>ratio[i-1])&&(th[i-1]<ratio[i]))||((th[i-1]<ratio[i-1])&&(th[i-1]>ratio[i])))
		{
			node[0] = node[1];
			node[1] = i;
			//flag = 1;

			if(node[1]-node[0]>=window)
				th[i] = coef1*mean(&ratio[node[0]],node[1]-node[0]) + coef2*ratio[i];
			else
				th[i] = th[i-1];
		}
		else if (((th[i-1]<=ratio[i-1])&&(th[i-1]<=ratio[i]))||((th[i-1]>=ratio[i-1])&&(th[i-1]>=ratio[i])))
		{
			//if(flag == 0)
			//{
			   th[i] = th[i-1];
		    //}
			//else
			//{
				//th[i] = coef1*mean(&ratio[node[0]],node[1]-node[0]) + coef2*mean(&ratio[node[1]],i-node[1]+1);
			//}
				
		}
		//else
		//{
			//th[i] = coef1*mean(&ratio[node[0]],node[1]-node[0]) + coef2*mean(&ratio[node[1]],i-node[1]+1);
		//}
	}

	return;
}


void detect(float *absFft, int totalSecond, char *indClear, char *score)
{
	int i;
	int j;
	int offset;
	int totalEpoch;
	int tmp;
	int numValid;
	int numEpoch;
	int epochValid[MAXENPOCH];
	const int window = 5;
	float th[MAXENPOCH];
	float epoch[MAXENPOCH] = {0};
	float epochFilted[MAXENPOCH] = {0};
	float epochFilted2[MAXENPOCH] = {0};
	float th2[MAXENPOCH];
	float epoch2[MAXENPOCH] = {0};
	float epoch2Filted[MAXENPOCH] = {0};
	float epoch2Filted2[MAXENPOCH] = {0};

	totalEpoch = totalSecond/SECONDS_OF_EPOCH;
	numEpoch = totalEpoch;

	for(i=0; i<totalEpoch; i++)
	{
		tmp = 0;
		numValid = 0;
		for(j=0; j<SECONDS_OF_EPOCH; j++)
		{
			offset = (i*SECONDS_OF_EPOCH+j);
			tmp += indClear[offset];
			if(indClear[offset]!=NaN)
			{
				epoch[i] += ratioFreqBand(absFft+offset*NFFT);
				epoch2[i] += ratioFreqBand2(absFft+offset*NFFT);
				numValid++;
			}
		}

		if(tmp==SECONDS_OF_EPOCH*NaN)
			epochValid[i] = NaN;
		else
			epochValid[i] = 1;

		if(numValid==0)
		{
			if(i>0)
				epoch[i] = epoch[i-1];
			else
				epoch[i] = ORIGINAL_THRESHOLD;
		}
		else
			epoch[i] /= numValid;
	}

	averagingFilter(epoch, numEpoch, window, epochFilted);
	averagingFilter(epochFilted, numEpoch, window, epochFilted2);
	thSleepWake(epochFilted2, numEpoch, th);
	averagingFilter(epoch2, numEpoch, window, epoch2Filted);
	averagingFilter(epoch2Filted, numEpoch, window, epoch2Filted2);
	thSleepWake(epoch2Filted2, numEpoch, th2);
	judge(epochFilted2, th, epoch2Filted2, th2, epochValid, numEpoch, score);

	return;
}

void aasm(float* rawdata, float *fftdata, int totalSecond, char *score)
{
	char 		*indClear;
	const int 	rawTh = 500;
	indClear = (char*)malloc(totalSecond*sizeof(char));

	removeDC(rawdata, totalSecond);
	clearRaw(rawdata, totalSecond, rawTh, indClear);
	detect(fftdata, totalSecond, indClear, score);

	free(indClear);
	return;
}

static void combine_deal(int *bp_style_vector, int *bp_move_vector, int style_thirty_seconds_len, int raw_thirty_seconds_len, char *score)
{
	int index = 0;
	const int bp_style_num = 0;
	const int bp_move_num = 0;
	
	for(index=0; index<raw_thirty_seconds_len; index++)
	{
		if(index<=style_thirty_seconds_len && bp_style_vector[index]>bp_style_num && score[index]!= NaN)
		{
			score[index] = WAKE;
		}
		
		if(index<=style_thirty_seconds_len && bp_style_vector[index]==bp_style_num && bp_move_vector[index]==bp_move_num && score[index]== LIGHT)
		{
			score[index] = LIGHT;
		}
		
		if(index<=style_thirty_seconds_len && bp_style_vector[index]==bp_style_num && bp_move_vector[index]>bp_move_num && (score[index]== LIGHT || score[index]== DEEP))
		{
			score[index] = LIGHT;
		}
		
		if(index<=style_thirty_seconds_len && score[index]== NaN) 
		{
			score[index] = NaN;
		} 
	
	}
	
	return;
}

static void get_precise_body_position_style(float* bodyposition, float* bodymove, int *bodypositionVector, int *bodymoveVector,  int style_totalSecond)
{
	
	int style_thirty_seconds_num = style_totalSecond/(2*SECONDS_OF_EPOCH);
	int index = 0;
	int i = 0, j = 0;
	
	memset(bodypositionVector, 0, sizeof(int)*style_thirty_seconds_num);
	memset(bodymoveVector, 0, sizeof(int)*style_thirty_seconds_num);
	
	for(index=0; index<style_totalSecond; index++)
	{
		if(bodyposition[index]==0 || bodyposition[index]==7)
		{
			if(index==0)
			{
				bodyposition[index] = 3;
			}
			else
			{
				bodyposition[index] =(int)bodyposition[index-1];
			}
			
		} 
		
		bodyposition[index] = (int)bodyposition[index]; 
	}
	
	for(i=0; i<style_thirty_seconds_num; i++)
	{
		for(j=0; j<2*SECONDS_OF_EPOCH; j++)
		{
			if(bodyposition[i*2*SECONDS_OF_EPOCH+j]==5 || bodyposition[i*2*SECONDS_OF_EPOCH+j]==6)
			{
				bodypositionVector[i]+=1;
			}
			if((int)bodymove[i*2*SECONDS_OF_EPOCH+j]>0)
			{
				bodymoveVector[i]+=1;
			}
	    }	 
	}
	
}



void  eeg_detect_algorithm(float* rawdata, float *fftdata, float *bodyposition, float *bodymove, int style_totalSecond, int raw_totalSecond, char *score)
{																				
	 int style_thirty_seconds_len = style_totalSecond/(2*SECONDS_OF_EPOCH);
	 int raw_thirty_seconds_len = raw_totalSecond/SECONDS_OF_EPOCH;
	 int *bodypositionVector;
	 int *bodymoveVector;
	 
	 bodypositionVector = (int*)malloc(sizeof(int)*style_thirty_seconds_len);
	 bodymoveVector = (int*)malloc(sizeof(int)*style_thirty_seconds_len);
	 
	 get_precise_body_position_style(bodyposition, bodymove, bodypositionVector, bodymoveVector, style_totalSecond);
	 aasm(rawdata, fftdata, raw_totalSecond, score);
	 
	 combine_deal(bodypositionVector, bodymoveVector, style_thirty_seconds_len, raw_thirty_seconds_len, score);
	 
	 return;
}
