
#ifndef PALPARSE_H
#define PALPARSE_H

#include "stdafx.h"

typedef struct {
    //byte 0
	unsigned char TYPE:5;
    unsigned char NRI:2;
	unsigned char F:1;    
         
} NALU_HEADER; /**//* 1 BYTES */

typedef struct {
    //byte 0
    unsigned char TYPE:5;
	unsigned char NRI:2; 
	unsigned char F:1;    
            
             
} FU_INDICATOR; /**//* 1 BYTES */

typedef struct {
    //byte 0
    unsigned char TYPE:5;
	unsigned char R:1;
	unsigned char E:1;
	unsigned char S:1;    
} FU_HEADER; /**//* 1 BYTES */


int h264_nal_parse(LPVOID lparam,char *fileurl);

int probe_nal_unit(char* filename,int data_offset,int data_lenth,LPVOID lparam);;

#endif
