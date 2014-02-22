
#ifndef PALPARSE_H
#define PALPARSE_H

#include "stdafx.h"


//typedef struct {
//    //byte 0
//    unsigned char TYPE:5;
//    unsigned char NRI:2;
//    unsigned char F:1;
//} NALU_HEADER; /**//* 1 BYTES */
//
//typedef struct {
//    //byte 0
//    unsigned char TYPE:5;
//    unsigned char NRI:2;
//    unsigned char F:1;
//} FU_INDICATOR; /**//* 1 BYTES */
//
//typedef struct {
//    //byte 0
//    unsigned char TYPE:5;
//    unsigned char R:1;
//    unsigned char E:1;
//    unsigned char S:1;    
//} FU_HEADER; /**//* 1 BYTES */

typedef struct
{
  int startcodeprefix_len;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
  unsigned len;                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
  unsigned max_size;            //! Nal Unit Buffer size
  int forbidden_bit;            //! should be always FALSE
  int nal_reference_idc;        //! NALU_PRIORITY_xxxx
  int nal_unit_type;            //! NALU_TYPE_xxxx    
  char *buf;                    //! contains the first byte followed by the EBSP
  unsigned short lost_packets;  //! true, if packet loss is detected
  int data_offset;
  int startcode;
} NALU_t;

int h264_nal_parse(LPVOID lparam,char *fileurl);

int probe_nal_unit(char* filename,int data_offset,int data_lenth,LPVOID lparam);;

#endif
