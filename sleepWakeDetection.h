/*******************************************************************
Describes: 	header for detection/score of wake, sleep and REM
   Author:	Jango
   Verson: 	v0.1
     Date: 	2017-9-15
Copyright:	EEGSmart
********************************************************************/
#ifndef __SLEEPWAKEDETECTION_H__
#define __SLEEPWAKEDETECTION_H__
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "readcsv.h"

#define MAXHOUR				(20)
#define SECONDS_OF_EPOCH 	(30)
#define EPOCH_OF_HOUR		(120)
#define NFFT				(256)
#define NaN					(-1)
#define ORIGINAL_THRESHOLD	(3) 
#define ORIGINAL_THRESHOLD2	(3)
#define WAKE				(1)
#define LIGHT				(2)
#define DEEP				(3)
#define REM					(5)
#define MAXENPOCH 			(MAXHOUR*EPOCH_OF_HOUR)
#define MAXSECOND			(SECONDS_OF_EPOCH*EPOCH_OF_HOUR*MAXHOUR)

#define PRINT_MAIN	 		(1)
#define TEST_MAIN			(1)

 void  eeg_detect_algorithm(float* rawdata, float *fftdata, float *bodyposition, float *bodymove, int style_totalSecond, int raw_totalSecond, char *score); 
 /* @�������
 * 			rawdata: ���飬�����ܳ���Ϊ��256*3600��*10Сʱ����ԭʼʱ���źţ�������256Hz
 * 			fftdata: ���飬������rawdata��ͬ�������ܳ���Ϊ��256*3600��*10Сʱ����ԭʼʱ���ź�ÿ��256��FFT�����ľ���ֵƽ��
 *          bodyposition: һά���������������ҹ����λֵ��ÿ�����ݵ�ʱ����Ϊ���� 
            bodymove: һά������ �������ҹ���嶯ֵ��ÿ�����ݵ�ʱ����Ϊ����
            style_totalSecond����ʱ��������λ������. 
 *			raw_totalSecond: ��ʱ��������λ����.
 * *
 * @�������
 * 			score���ַ����飬�����ܳ���Ϊ1200. �������������ҹ���ڽ���������ڵ�ÿ��Ԫ�ض�Ӧÿ30��ķ��ڽ����
 *
 * @return
 * 			��
 */
 
#endif
