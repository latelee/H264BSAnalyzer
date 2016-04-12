/**

H.264码流转可播放视频

note 使用ffmpeg版本为2.6.3，附带的库为静态库

码流仅限于H.264

*/

#ifndef H264TOVIDEO_H
#define H264TOVIDEO_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
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


#define _LL_DEBUG_

// low level debug
#ifdef _LL_DEBUG_
    #ifndef debug
    #define debug(fmt, ...) printf(fmt, ##__VA_ARGS__)
    #endif
    #define LL_DEBUG(fmt, ...) printf("[DEBUG %s().%d @ %s]: " fmt, __func__, __LINE__, P_SRC, ##__VA_ARGS__)
#else
     #define debug(fmt, ...)
    #define LL_DEBUG(fmt, ...)
#endif


#ifndef min
#define min(a,b) ((a) > (b) ? (b) : (a))
#endif

/**
 @brief 视频缓冲区结构体
*/
typedef struct AVIOBufferContext {
    unsigned char* ptr;
    int pos;
    int totalSize;
    int realSize;
}AVIOBufferContext;

class H264BS2Video
{
public:
    H264BS2Video();
    ~H264BS2Video();

public:
    /**
     * 打开H.264视频文件并初始化
     * 
     * @param rawfile 祼视频文件路径全称(包括目录和视频文件名称)
     *
     * @return =< 0 成功：= 0， 失败 = -1
     */
    int openBSFile(const char* rawfile);

    /**
     * 打开H.264视频文件并初始化
     *
     * @param videofile 视频文件路径全称(包括目录和视频文件名称)
     * @param fps 帧率
     * @param gop GOP大小，如果视频没有B帧，则GOP大小为I帧间隔
     * @param width 视频宽
     * @param height 视频高 
     * @param bitrate 视频码率，默认为2048kbps
     *
     * @return =< 0 成功：= 0， 失败 = -1
     *
     * @note 当前只测试封装成avi格式的视频文件
     */
    int openVideoFile(const char* videofile,
                    int fps = 25,
                    int gop = 10,
                    int width=1920,
                    int height=1080,
                    int bitrate = 2097152);
    /**
     * 申请内部缓冲区
     *
     * @note 用于存储转码后的视频数据，如使用文件，无须调用本函数
     */
    int allocBuffer(int size);

    /**
     * 初始化H.264视频参数
     *
     * @param fmt 封装格式，avi、mp4、mkv，默认avi
     * @param fps 帧率
     * @param gop GOP大小，如果视频没有B帧，则GOP大小为I帧间隔
     * @param width 视频宽
     * @param height 视频高 
     * @param bitrate 视频码率，默认为2048kbps
     *
     * @return =< 0 成功：= 0， 失败 = -1
     *
     * @note 当前只测试封装成avi格式的视频文件
     */
    int openVideoMem(const char* fmt = "avi", int fps = 25, int gop = 10, int width=1920, int height=1080, int bitrate = 2097152);

    /**
     * 保存一帧H.264视频
     * @param bitstream H.264码流缓冲区
     * @param size  H.264码流缓冲区大小
     * @param keyframe 关键帧标志(I帧)，关键帧为1，否则为0
     *
     */
    int writeFrame(char* bitstream, int size, int keyframe);
   
    /**
     * 保存H.264视频
     *
     * @return =< 0 成功：= 0， 失败 = -1
     */
    int writeFrame(void);

    /**
     * 释放资源，写视频文件尾部数据
     *
     * @note 如果不调用此函数，像AVI这类的视频，有的播放器无法播放，
     *       因为尾部数据为其帧索引，是必须的
     */
    int close(void);

    /**
     * 获取转码后的视频数据，缓冲区为类内部使用
     * @param buffer 缓冲区
     * @param size  缓冲区大小
     *
     */
    void getBuffer(unsigned char** buffer, int* size);
    
    /**
     * 释放资源内部缓冲区
     *
     * @note 如不调用，在本类析构函数中也会自行释放
     */
    void freeBuffer(void);
    
    /**
     * 复位内部缓冲区
     *
     * @note 此函数会复原缓冲区参数，这样不用频繁申请、释放资源
     */
    void resetBuffer(void);


private:
    AVFormatContext *m_infctx;
    AVFormatContext *m_outfctx; 
    AVStream *m_stream;
    AVIOContext *m_avio;
    AVIOBufferContext m_avbuffer;
    int m_videoidx;
    int m_isfile;
};

#endif // H264TOVIDEO_H
