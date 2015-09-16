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
    unsigned int data_offset;       // nal包在文件中的偏移
    char slice_type;               // 帧类型
    char nal_unit_type;            // NAL类型
    char startcode_len;             // start code长度
    char startcode_buf[16];         // 起始码，字符串形式
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

enum FileType
{
    FILE_H264 = 0,
    FILE_H265 = 1,
};

class CNalParser
{
public:
    CNalParser();
    ~CNalParser();
    
    int init(const char* filename);
    int release(void);

    int h264_nal_probe(char *fileurl, vector<NALU_t>& vNal, int num);

    int h264_nal_parse(char* filename,int data_offset,int data_lenth,LPVOID lparam);;

    int h264_sps_parse(char* filename,int data_offset,int data_lenth, SPSInfo_t& info);

    int h264_pps_parse(char* filename,int data_offset,int data_lenth, PPSInfo_t& info);

private:
    //判断是否为0x000001,如果是返回1
    inline int findStartcode3(unsigned char *Buf)
    {
        return (Buf[0]==0 && Buf[1]==0 && Buf[2]==1);
    }

    //判断是否为0x00000001,如果是返回1
    inline int findStartcode4(unsigned char *Buf)
    {
        return (Buf[0]==0 && Buf[1]==0 && Buf[2]==0 && Buf[3]==1);
    }
    int GetAnnexbNALU (FILE* fp, NALU_t *nalu);
    int find_first_nal(FILE* fp, int& startcodeLenght);

    FileType judeVideoFile(const char* filename);

private:
    h264_stream_t* m_hH264;
    h265_stream_t* m_hH265;
    FileType m_nType; // 0:264 1:265
    const char* m_filename;
};
#endif
