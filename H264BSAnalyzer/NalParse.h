#ifndef PALPARSE_H
#define PALPARSE_H

#include "stdafx.h"
#include <vector>
using std::vector;

typedef struct
{
    int num;
    int startcodeprefix_len;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
    unsigned int len;                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
    unsigned int total_len;                // 含起始码的总的长度
    unsigned int max_size;            //! Nal Unit Buffer size
    //  int forbidden_bit;            //! should be always FALSE
    //  int nal_reference_idc;        //! NALU_PRIORITY_xxxx
    //char *buf;                    //! contains the first byte followed by the EBSP
    //unsigned short lost_packets;  //! true, if packet loss is detected
    unsigned int data_offset;
    char startcode_buf[14];       // 起始码，字符串形式
    char slice_type;               // 帧类型
    char nal_unit_type;            //! NALU_TYPE_xxxx 
} NALU_t;

typedef int handle_nalu_info(NALU_t* nalu);

int h264_nal_parse(LPVOID lparam,char *fileurl);

int h264_nal_parse_1(char *fileurl, vector<NALU_t>& vNal);

int probe_nal_unit(char* filename,int data_offset,int data_lenth,LPVOID lparam);;

int parse_sps(char* filename,int data_offset,int data_lenth);

int parse_pps(char* filename,int data_offset,int data_lenth);

#endif
