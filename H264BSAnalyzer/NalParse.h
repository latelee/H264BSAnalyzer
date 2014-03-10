
#ifndef PALPARSE_H
#define PALPARSE_H

#include "stdafx.h"

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
  int total_len;                // 含起始码的总的长度
  char startcode_buf[16];       // 起始码，字符串形式
  int is_b_slice;
} NALU_t;

typedef int handle_nalu_info(NALU_t* nalu);

int h264_nal_parse(LPVOID lparam,char *fileurl);

int h264_nal_parse_1(char *fileurl, handle_nalu_info p);

int probe_nal_unit(char* filename,int data_offset,int data_lenth,LPVOID lparam);;

#endif
