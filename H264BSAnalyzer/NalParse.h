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
    unsigned int offset;       // nal包在文件中的偏移
    int sliceType;               // 帧类型
    int nalType;            // NAL类型
    int startcodeLen;             // start code长度
    char startcodeBuffer[16];         // 起始码，字符串形式
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

const int MAX_NAL_SIZE = 1*1024*1024;
const int OUTPUT_SIZE = 512*1024;

class CNalParser
{
public:
    CNalParser();
    ~CNalParser();
    
    int init(const char* filename);
    int release(void);

    // 搜索视频文件的ANL单元，记录偏移及长度
    int probeNALU(vector<NALU_t>& vNal, int num);

    // 解析offset处大小为length的数据，十六进制数据传递到naluData，NAL信息传递到naluInfo
    int parseNALU(NALU_t& vNal, char** naluData, char** naluInfo);

    void getVideoInfo(videoinfo_t* videoInfo)
    {
        if (m_nType) memcpy(videoInfo, m_hH265->info, sizeof(videoinfo_t));
        else memcpy(videoInfo, m_hH264->info, sizeof(videoinfo_t));
    }
    int h264_sps_parse(char* filename,int offset,int data_lenth, SPSInfo_t& info);

    int h264_pps_parse(char* filename,int offset,int data_lenth, PPSInfo_t& info);

private:
    inline int findStartcode3(unsigned char *buffer)
    {
        return (buffer[0]==0 && buffer[1]==0 && buffer[2]==1);
    }
    inline int findStartcode4(unsigned char *buffer)
    {
        return (buffer[0]==0 && buffer[1]==0 && buffer[2]==0 && buffer[3]==1);
    }

    int getAnnexbNALU (FILE* fp, NALU_t *nalu);

    int findFirstNALU(FILE* fp, int* startcodeLenght);

    FileType judeVideoFile(const char* filename);

private:
    h264_stream_t* m_hH264;
    h265_stream_t* m_hH265;
    FileType m_nType; // 0:264 1:265
    const char* m_filename;
    uint8_t* m_naluData;
};
#endif
