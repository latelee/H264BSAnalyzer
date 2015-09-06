#ifndef PALPARSE_H
#define PALPARSE_H

#include "stdafx.h"

#include "h264_stream.h"
#include "h265_stream.h"

#include <vector>
using std::vector;

typedef struct
{
    int type;                       // 0 -- h.264; 1 -- h.265
    unsigned int num;               // 序号
    unsigned int len;               // 含起始码的总的长度
    char slice_type;               // 帧类型
    char nal_unit_type;            // NAL类型
    unsigned int data_offset;       // nal包在文件中的偏移
    char startcode_buf[14];         // 起始码，字符串形式
} NALU_t;

typedef struct 
{
    int profile_idc;
    int level_idc;
    int width;
    int height;
    int crop_left;
    int crop_right;
    int crop_top;
    int crop_bottom;
    float max_framerate;  // 由SPS计算得到的帧率，为0表示SPS中没有相应的字段计算
    int chroma_format_idc;  // YUV颜色空间 0: monochrome 1:420 2:422 3:444
}SPSInfo_t;

typedef struct 
{
    int encoding_type;  // 为1表示CABAC 0表示CAVLC

}PPSInfo_t;

typedef int handle_nalu_info(NALU_t* nalu);

int h264_nal_probe(char *fileurl, vector<NALU_t>& vNal, int num);

int h264_nal_parse(char* filename,int data_offset,int data_lenth,LPVOID lparam);;

int h264_sps_parse(char* filename,int data_offset,int data_lenth, SPSInfo_t& info);

int h264_pps_parse(char* filename,int data_offset,int data_lenth, PPSInfo_t& info);

#endif
