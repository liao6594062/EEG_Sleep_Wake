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
 /* @输入参数
 * 			rawdata: 数组，例如总长度为（256*3600秒*10小时），原始时域信号，采样率256Hz
 * 			fftdata: 数组，长度与rawdata相同，例如总长度为（256*3600秒*10小时），原始时域信号每秒256点FFT后结果的绝对值平方
 *          bodyposition: 一维向量，计算出的整夜的体位值，每个数据点时间间隔为半秒 
            bodymove: 一维向量， 算出的整夜的体动值，每个数据点时间间隔为半秒
            style_totalSecond：总时间量，单位：半秒. 
 *			raw_totalSecond: 总时间量，单位：秒.
 * *
 * @输出参数
 * 			score：字符数组，例如总长度为1200. 整个数组标书整夜分期结果，数组内的每个元素对应每30秒的分期结果。
 *
 * @return
 * 			无
 */
 
#endif
