#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sleepWakeDetection.h"
#include "readcsv.h"
/*run this program using the console pauser or add your own getch, system("pause") or input loop*/


int main()
{
	char *bodypositionname = "C:\\Users\\EEG\\Desktop\\android_trackbodyposi_liaolm_163_20170914_152336.csv"; //the address of test_File
	char *bodymovename = "C:\\Users\\EEG\\Desktop\\android_trackbodymove_liaolm_163_20170914_152336.csv";
	char *fftname = "C:\\Users\\EEG\\Desktop\\fftdata.csv";
	char *rawname = "C:\\Users\\EEG\\Desktop\\android_trackrawdata_liaolm_163_20170914_152336.csv";
	int row, col; // the row and col of data
	float *rawdata; // save data as vector
	float *fftdata;
	float *bodyposition;
	float *bodymove;
	char *score;  
    int style_totalSecond = 0;
    int raw_totalSecond = 0;
    
	col = get_col_of_raw(rawname); //get the col of raw 
	row = get_row_of_raw(rawname, col); // get the row of raw 
	rawdata = (float*)malloc(sizeof(float) * row * col);  // get the data named raw 
	get_raw_from_csv(rawname, rawdata, row, col); // get raw from csv
	raw_totalSecond = row;
	printf("size of matrix: %d x %d\n", row, col); // print row and col
	score = (char*)malloc(sizeof(char)*(raw_totalSecond/SECONDS_OF_EPOCH));
	
	col = get_col_of_raw(fftname); //get the col of raw 
	row = get_row_of_raw(fftname, col); // get the row of raw 
	fftdata = (float*)malloc(sizeof(float) * row * col);  // get the data named raw 
	get_raw_from_csv(fftname, fftdata, row, col); // get raw from csv
	printf("size of matrix: %d x %d\n", row, col); // print row and col
	
	col = get_col_of_raw(bodypositionname); //get the col of raw 
	row = get_row_of_raw(bodypositionname, col); // get the row of raw 
	bodyposition = (float*)malloc(sizeof(float) * row * col);  // get the data named raw 
	get_raw_from_csv(bodypositionname, bodyposition, row, col); // get raw from csv
	style_totalSecond = row;
	printf("size of matrix: %d x %d\n", row, col); // print row and col
	
	col = get_col_of_raw(bodymovename); //get the col of raw 
	row = get_row_of_raw(bodymovename, col); // get the row of raw 
	bodymove = (float*)malloc(sizeof(float) * row * col);  // get the data named raw 
	get_raw_from_csv(bodymovename, bodymove, row, col); // get raw from csv
	printf("size of matrix: %d x %d\n", row, col); // print row and col
	
	eeg_detect_algorithm(rawdata, fftdata, bodyposition, bodymove, style_totalSecond, raw_totalSecond, score);
	printf("%d\n", raw_totalSecond/SECONDS_OF_EPOCH);
	
	for(row=0; row<raw_totalSecond/SECONDS_OF_EPOCH; row++)
	{
		printf("%d\n", score[row]); 
	}
	
    return 0;		    
}
