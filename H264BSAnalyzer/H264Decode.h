#ifndef _H264DECODE_H
#define _H264DECODE_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}

#ifdef WIN32
// 静态库
#pragma comment(lib, "libgcc.a") // divdi3(), etc.
#pragma comment(lib, "libmingwex.a") // snprintf()....
#pragma comment(lib, "libiconv.a") // libiconv_open(), etc.

#pragma comment(lib, "libavcodec.a")
#pragma comment(lib, "libavformat.a")
#pragma comment(lib, "libavutil.a")
#pragma comment(lib, "libswscale.a")
#pragma comment(lib, "libswresample.a")

#endif

#define _DEBUG_

typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef long            LONG;

// 2字节对齐
#pragma pack(2)
typedef struct tagMYBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} MYBITMAPFILEHEADER;

// 40
// 上面更改为2字节对齐，现在要恢复为原来默认8字节对齐
#pragma pack(8)
typedef struct tagMYBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} MYBITMAPINFOHEADER;

class CH264Decoder {
public:
    CH264Decoder();
    ~CH264Decoder();

    /**
    * openVideoFile - 打开H264视频
    * 
    * @param  avifile H264格式的AVI视频文件名称
    *
    * @return 0 - 成功   -1 - 失败(包括找不到解码器、分配内存不成功)
    */  
    int openVideoFile(const char* avifile);

    /**
    * closeVideoFile - 关闭视频并释放资源
    * 
    */
    void closeVideoFile(void);

    /**
    * jumpToTime - 跳到指定时间
    *
    * @param     time：时间，以秒为单位
    *
    * @return    0 - 成功   -1 －跳转不成功或指定时间超出视频总时间或小于0
    */
    int jumpToTime(int64_t time);

    /**
    * getFrame - 从视频中获取一帧数据
    * 
    * @param[OUT] yuvBuffer 传出参数，保存原始数据指针
    * @param[OUT] rgbBuffer 传出参数，RGB数据指针
    * @param[OUT] size      传出参数，原始数据大小
    * @param[OUT] width     传出参数，宽
    * @param[OUT] height    传出参数，高
    *
    * @return  1 - 成功   0 - 没有读到帧(视频结束了)  -1 - 解码失败
    *
    * @note  传出参数中的宽和高指视频数据的宽和高
    *        原始数据为YUV420P格式
    *        有些视频不以I帧开始，调用jumpToTime(0)可跳到开始的I帧
    *        该函数调用av_read_frame，会返回一下帧，即>0表明视频中还有数据，可以继续读取
    */
    int getFrame(unsigned char** yuvBuffer = NULL, unsigned char** rgbBuffer = NULL, int* size = NULL, int* width = NULL, int* height = NULL);

    /**
    * getSkippedFrame - 获取ffmpeg缓冲的帧
    * 
    * @param[OUT] yuvBuffer 传出参数，保存原始数据指针
    * @param[OUT] rgbBuffer 传出参数，RGB数据指针
    * @param[OUT] size      传出参数，原始数据大小
    * @param[OUT] width     传出参数，宽
    * @param[OUT] height    传出参数，高
    *
    * @return  1 - 成功   0 - 没有缓冲帧(视频结束了)  -1 - 解码失败
    *
    * @note  有的帧A需要参考后面的帧B，但按顺序，ffmpeg还没有解到B帧，没有能力解A，会缓存起来。
    *        这个函数读取ffmpeg缓冲区中还有的帧。
    *        疑问：如果是开头几帧跳过不解，到最后才调用这个函数，得到的帧是开始的还是最后的？——测试结果表明，是最后的
    */
    int getSkippedFrame(unsigned char** yuvBuffer = NULL, unsigned char** rgbBuffer = NULL, int* size = NULL, int* width = NULL, int* height = NULL);

    /**
    * writeYUVFile - 写YUV文件到指定文件
    *
    * @param   filename  图片文件名称
    *
    * @return  0 - 成功  -1 - 写文件失败
    *
    */
    int writeYUVFile(const char* filename);

    /**
    * writeBmpFile - 写BMP图片到指定文件
    *
    * @param   filename  图片文件名称
    *
    * @return  0 - 成功  -1 - 写文件失败
    *
    */
    int writeBMPFile(const char* filename);

    int writeBMPFile2(const char* filename);

    /**
    * writeBmpFile - 写JPEG图片到指定文件
    *
    * @param   filename  图片文件名称
    *
    * @return  0 - 成功  -1 - 写文件失败
    *
    */
    int writeJPGFile(const char* filename);

    int writeJPGFile2(const char* filename);

private:
    /**
    *  convertToRgb - 将解出来的数据转换为RGB格式(实际为BGR24)
    *
    * @return 返回RGB数据指针
    */
    unsigned char* convertToRgb();

private:
    int m_skippedFrame;
    int m_picWidth;
    int m_picHeight;

    int m_videoStream;    // 视频流索引
    unsigned char* m_picBuffer;
    AVFormatContext* m_fmtctx;
    AVCodecContext* m_avctx;
    AVFrame* m_picture;
    AVFrame* m_frameRGB;
    unsigned char* m_bufferYUV;
    AVFrame* m_frameYUV;
    struct SwsContext* m_imgctx;
    struct SwsContext* m_imgctxyuv;
};
#endif