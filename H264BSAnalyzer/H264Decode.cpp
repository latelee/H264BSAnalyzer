#include "stdafx.h" // for MFC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "H264Decode.h"

#ifdef _DEBUG_
int debug(const char* fmt, ...)
{
    char buffer[512] = {0};
    va_list ap;
    int ret;
    va_start(ap, fmt);
    ret = vsprintf(buffer, fmt, ap);
    va_end(ap);
    printf("%s", buffer);
    return ret;
}
#else
int debug(const char* fmt, ...)
{
    return 0;
}
#endif

CH264Decode::CH264Decode()
{
    m_skippedFrame = 0;
    m_picWidth = 0;
    m_picHeight = 0;
    m_fmtctx  = NULL;
    m_avctx   = NULL;
    //m_codec   = NULL;
    m_picture = NULL;
    m_frameRGB = NULL;
    m_imgctx = NULL;
    m_videoStream = -1;           // 视频流，注：不能初始化为0
}

CH264Decode::~CH264Decode()
{
    closeVideoFile();
#if 0
    if (m_picture)
    {
        av_free(m_picture);
        m_picture = NULL;
    }
    if (m_avctx)
    {
        avcodec_close(m_avctx);
        m_avctx = NULL;
    }
    if (m_fmtctx)
    {
        avformat_close_input(&m_fmtctx);
        m_fmtctx = NULL;
    }
    if (m_picBuffer)
    {
        av_free(m_picBuffer);
        m_picBuffer = NULL;
    }
    if (m_frameRGB)
    {
        av_free(m_frameRGB);
        m_frameRGB = NULL;
    }
    if (m_imgctx)
    {
        sws_freeContext(m_imgctx);
        m_imgctx = NULL;
    }
#endif
}

int CH264Decode::openVideoFile(const char* avifile)
{
    int ret = 0;
    int size = 0;
    AVCodec* codec = NULL;

    PixelFormat fmtRGB = PIX_FMT_BGR24;     // !!! 如果是PIX_FMT_RGB24，图像颜色有偏差(即r、b互换)

    if (avifile == NULL)
    {
        debug("file invalid.\n");
        return -1;
    }
    av_register_all();

    // 从视频文件检测出视频格式等信息
    ret = avformat_open_input(&m_fmtctx, avifile, NULL, NULL);
    if (ret != 0)
    {
        debug("open input file failed. ret: %d\n", ret);
        return -1;
    }
    ret = avformat_find_stream_info(m_fmtctx, NULL);
    if (ret < 0)
    {
        debug("find stream info failed. ret: %d\n", ret);
        return -1;
    }

#ifdef _DEBUG_
    // TODO: 测试视频格式，标题、作者、时间长度，文件长度...
    av_dump_format(m_fmtctx, 0, avifile, 0);
#endif

    // 找到视频流
    for (unsigned int i = 0; i < m_fmtctx->nb_streams; i++)
    {
        if (m_fmtctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            m_videoStream = i;
            break;
        }
    }
    if (m_videoStream == -1)
    {
        debug("no video stream.\n");
        return -1;
    }

    // 查找并打开解码器
    m_avctx = m_fmtctx->streams[m_videoStream]->codec;
    codec = avcodec_find_decoder(m_avctx->codec_id);
    if (codec == NULL)
    {
        debug("unsupoorted codec\n");
        return -1;
    }

    ret = avcodec_open2(m_avctx, codec, NULL);
    if (ret < 0)
    {
        debug("open codec failed.\n");
        return -1;
    }
    // 分配frame
    m_picture = av_frame_alloc();
    if (!m_picture)
    {
        return -1;
    }
    // 分配转换成RGB后的frame
    m_frameRGB = av_frame_alloc();
    if (!m_frameRGB)
    {
        av_free(m_picture);
        return -1;
    }
    size = avpicture_get_size(fmtRGB, m_avctx->width, m_avctx->height);

    // m_picBuffer要到最后释放
    m_picBuffer = (unsigned char *)av_malloc(size);
    if (!m_picBuffer)
    {
        av_free(m_picture);
        av_free(m_frameRGB);
        return -1;
    }
    avpicture_fill((AVPicture *)m_frameRGB, m_picBuffer, fmtRGB, m_avctx->width, m_avctx->height);

    // 创建转换上下文
    m_imgctx = sws_getContext(m_avctx->width, m_avctx->height, m_avctx->pix_fmt, m_avctx->width, m_avctx->height, 
        fmtRGB, SWS_BICUBIC, NULL, NULL, NULL);
    if (m_imgctx == NULL)
    {
        av_free(m_picture);
        av_free(m_frameRGB);
        av_free(m_picBuffer);
        return -1;
    }
    //debug("---%s %d fmt: %d %d\n", __func__, __LINE__, m_avctx->pix_fmt, fmtRGB);
    return 0;
}

void CH264Decode::closeVideoFile(void)
{
    if (m_picture)
    {
        av_free(m_picture);
        m_picture = NULL;
    }
    if (m_avctx)
    {
        avcodec_close(m_avctx);
        m_avctx = NULL;
    }
    if (m_fmtctx)
    {
        avformat_close_input(&m_fmtctx);
        m_fmtctx = NULL;
    }
    if (m_picBuffer)
    {
        av_free(m_picBuffer);
        m_picBuffer = NULL;
    }
    if (m_frameRGB)
    {
        av_free(m_frameRGB);
        m_frameRGB = NULL;
    }
    if (m_imgctx)
    {
        sws_freeContext(m_imgctx);
        m_imgctx = NULL;
    }
}

int CH264Decode::jumpToTime(int64_t time)
{
    int64_t seekTarget = 0;
    AVRational AV_TIME = {1, AV_TIME_BASE};
    int ret = 0;

    // 跳到指定时间
    seekTarget = time * AV_TIME_BASE;
    if (seekTarget > m_fmtctx->duration || seekTarget < 0)
    {
        debug("time beyond the video or less than zero.\n");
        return -1;
    }
    seekTarget = av_rescale_q(seekTarget, AV_TIME, m_fmtctx->streams[m_videoStream]->time_base);
    ret = av_seek_frame(m_fmtctx, m_videoStream, seekTarget,0);   //  AVSEEK_FLAG_ANY AVSEEK_FLAG_BACKWARD
    if (ret < 0)
    {
        return -1;
    }
    return 0;
}

int CH264Decode::getFrame(unsigned char** yuvBuffer, unsigned char** rgbBuffer, int* size, int* width, int* height)
{
    int got_picture = 0;    // 找到帧标志
    int len = 0;
    AVPacket avpkt;

    av_init_packet(&avpkt);
    //int frame = 0;
    // av_read_fram返回下一帧，发生错误或文件结束返回<0
    while (av_read_frame(m_fmtctx, &avpkt) >= 0)
    {
        // 解码视频流
        if (avpkt.stream_index == m_videoStream)
        {
            len = avcodec_decode_video2(m_avctx, m_picture, &got_picture, &avpkt);
            if (len < 0)
            {
                debug("error while decoding frame.\n");
                return -1;
            }
            if (got_picture)
            {
                m_picWidth  = m_avctx->width;
                m_picHeight = m_avctx->height;
                // 传出原始数据指针，由于内部已经申请了，不用再开辟数据
                if (yuvBuffer != NULL)
                {
                    *yuvBuffer = m_picture->data[0];
                    if (size != NULL)
                    {
                        *size = len; // to check
                    }
                }
                if (rgbBuffer != NULL)
                {
                    *rgbBuffer = convertToRgb();
                    if (size != NULL)
                    {
                        *size = m_picWidth * m_picHeight * 3; // 上面指定了rgb24，所以是w*h*3
                    }
                }
                //printf("frame fmt: %d\n", m_picture->format);


                if (width != NULL)
                {
                    *width = m_picWidth;
                }
                if (height != NULL)
                {
                    *height = m_picHeight;
                }
                //printf("bit_rate: %d width: %d height:%d\n", m_avctx->bit_rate, m_avctx->width, m_avctx->height);
                return 1;
            } // end of got picture
            else
            {
                m_skippedFrame++;
                //debug("skipped count: %d\n", m_skippedFrame);
            }
        } // end of video stream

        av_free_packet(&avpkt);
    } // end of read frame

    return 0;
}

int CH264Decode::getSkippedFrame(unsigned char** yuvBuffer, unsigned char** rgbBuffer, int* size, int* width, int* height)
{
    int got_picture = 0;    // 找到帧标志
    int len = 0;
    AVPacket avpkt;

    av_init_packet(&avpkt);

    // 是否还有缓存的帧
    while (m_skippedFrame-- > 0)
    {
        // 解码视频流
        len = avcodec_decode_video2(m_avctx, m_picture, &got_picture, &avpkt);
        if (len < 0)
        {
            debug("error while decoding frame.\n");
            return -1;
        }
        if (got_picture)
        {
            // 传出原始数据指针，由于内部已经申请了，不用再开辟数据
            if (yuvBuffer != NULL)
            {
                *yuvBuffer = m_picture->data[0];
            }
            if (rgbBuffer != NULL)
            {
                *rgbBuffer = convertToRgb();
            }
            //printf("frame fmt: %d\n", m_picture->format);
            if (size != NULL)
            {
                *size = len;
            }
            m_picWidth  = m_avctx->width;
            m_picHeight = m_avctx->height;
            if (width != NULL)
            {
                *width = m_picWidth;
            }
            if (height != NULL)
            {
                *height = m_picHeight;
            }
            //printf("bit_rate: %d width: %d height:%d\n", m_avctx->bit_rate, m_avctx->width, m_avctx->height);
            return 1;
        } // end of got picture

        av_free_packet(&avpkt);
    } // end of read frame

    return 0;
}

unsigned char* CH264Decode::convertToRgb()
{
    sws_scale(m_imgctx, m_picture->data, m_picture->linesize, 0, m_avctx->height, 
        m_frameRGB->data, m_frameRGB->linesize);

    return m_frameRGB->data[0];
}

int CH264Decode::writeBmpFile(const char* filename)
{
    MYBITMAPFILEHEADER bmpHeader;
    MYBITMAPINFOHEADER bmpInfo;
    FILE* fp = NULL;
    int stride = 0;
    unsigned char* rgbBuffer = NULL;
    int width = m_picWidth;
    int height = m_picHeight;
    stride = (((width * 24 + 31) >> 5) << 2);
    bmpHeader.bfType = ('M' << 8) | 'B';
    bmpHeader.bfReserved1 = 0;
    bmpHeader.bfReserved2 = 0;
    bmpHeader.bfOffBits = sizeof(MYBITMAPINFOHEADER) + sizeof(MYBITMAPFILEHEADER);
    bmpHeader.bfSize = bmpHeader.bfOffBits + stride * height;

    bmpInfo.biSize = sizeof(MYBITMAPINFOHEADER);
    bmpInfo.biWidth  = width;
    bmpInfo.biHeight = -height; // !!!rgb图像本来是倒过来的，加上负号才正常
    bmpInfo.biPlanes = 1;
    bmpInfo.biBitCount = 24;
    bmpInfo.biCompression = 0;
    bmpInfo.biSizeImage = 0;
    bmpInfo.biXPelsPerMeter = 0;
    bmpInfo.biYPelsPerMeter = 0;
    bmpInfo.biClrUsed = 0;
    bmpInfo.biClrImportant = 0;

    rgbBuffer = convertToRgb();

    fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        debug("open file %s failed.\n", filename);
        return -1;
    }

    // 是否需要填充，BMP要求每一行数据必须4字节对齐，不足够以0补。
    int padding = (4 - width * 3 % 4) % 4;
    char pad = '\0';
    int rgbSize = stride * height;
    unsigned char* tmp_buf = NULL;
    if (padding)
    {
        debug("The bmp file need pad: %d bytes.\n", padding);
        tmp_buf = (unsigned char *)malloc(sizeof(char) * rgbSize); // 对齐后的RGB数据总大小
        for (int i = 0; i < height; i++)
        {
            memcpy(tmp_buf+i * stride, rgbBuffer+i*width*3, width*3);
            memcpy(tmp_buf+i*width*3, &pad, padding);
        }
    }
    fwrite(&bmpHeader, 1, sizeof(MYBITMAPFILEHEADER), fp);
    fwrite(&bmpInfo, 1, sizeof(MYBITMAPINFOHEADER), fp);

    if(padding)
    {
        fwrite(tmp_buf, 1, rgbSize, fp);
        free(tmp_buf);
    }
    else
    {
        fwrite(rgbBuffer, 1, width * height * bmpInfo.biBitCount / 8, fp);
    }
    fclose(fp);

    return 0;
}

int CH264Decode::writeJPGFile(const char* filename)
{
    //char filename[64] = {0};
    int ret = -1;
    FILE* fp = NULL;
    int sizeJPG = 0;
    PixelFormat fmtJPG = AV_PIX_FMT_YUVJ420P;//AV_PIX_FMT_YUVJ420P;
    struct SwsContext* imgctx = NULL;
    // jpeg
    AVCodecContext* JPGCodecCtx;
    AVCodec* JPGCodec;
    AVFrame* frameJPG;
    AVCodecContext* avctx = m_avctx;
    AVPacket avpkt;

    int got_picture = 0;

    int size = 0;
    unsigned char* picture_buf = NULL;
    unsigned char* buffer = NULL;

    //printf("%s bit_rate: %d width: %d height:%d\n", __func__, avctx->bit_rate, avctx->width, avctx->height);


    JPGCodec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    if (!JPGCodec)
    {
        debug("find mjpeg encoder failed.\n");
        return -1;
    }

    JPGCodecCtx = avcodec_alloc_context3(JPGCodec);
    if (!JPGCodecCtx)
    {
        debug("alloc context for mjpeg codec failded.\n");
        return -1;
    }


    JPGCodecCtx->bit_rate      = avctx->bit_rate;
    JPGCodecCtx->width         = avctx->width;
    JPGCodecCtx->height        = avctx->height;
    JPGCodecCtx->pix_fmt       = fmtJPG;
    JPGCodecCtx->codec_id      = AV_CODEC_ID_MJPEG;
    JPGCodecCtx->codec_type    = AVMEDIA_TYPE_VIDEO;
    JPGCodecCtx->time_base.num = avctx->time_base.num;
    JPGCodecCtx->time_base.den = avctx->time_base.den;
    JPGCodecCtx->mb_lmin = JPGCodecCtx->qmin * FF_QP2LAMBDA;
    JPGCodecCtx->mb_lmax = JPGCodecCtx->qmax * FF_QP2LAMBDA;
    JPGCodecCtx->flags   = CODEC_FLAG_QSCALE;
    JPGCodecCtx->global_quality = JPGCodecCtx->qmin * FF_QP2LAMBDA;

    if (avcodec_open2(JPGCodecCtx, JPGCodec, NULL) < 0)
    {
        debug("open mjpeg codec failed.\n");
        return -1;
    }

    frameJPG = av_frame_alloc();
    if (!frameJPG)
    {
        debug("alloc for jpeg frame failed.\n");
        return -1;
    }

    size = avpicture_get_size(fmtJPG, avctx->width, avctx->height); // PIX_FMT_YUVJ420P
    picture_buf = (unsigned char *)av_malloc(size);
    if (!picture_buf)
    {
        av_free(frameJPG);
        return -1;
    }
    avpicture_fill((AVPicture *)frameJPG, picture_buf, fmtJPG, avctx->width, avctx->height);

#if 0
    buffer = (unsigned char *)av_malloc(size);
    if (!buffer)
    {
        return -1;
    }
    memset(buffer, 0, size);
#endif

    // 转换成JPEG格式
    // 目标格式写 AV_PIX_FMT_YUV420P 就正常，写m_picture->format就不正常。
    imgctx = sws_getContext(JPGCodecCtx->width, JPGCodecCtx->height, (AVPixelFormat)m_picture->format, JPGCodecCtx->width,
        JPGCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); // JPGCodecCtx->pix_fmt fmtJPG

    debug("%s %d fmt: %d %d\n", __FUNCTION__, __LINE__, m_picture->format, AV_PIX_FMT_YUVJ420P);
    if (imgctx)
    {
        sws_scale(imgctx, m_picture->data, m_picture->linesize, 0, JPGCodecCtx->height, 
            frameJPG->data, frameJPG->linesize);
        frameJPG->pts = 1;
        frameJPG->quality = JPGCodecCtx->global_quality;
        frameJPG->width = JPGCodecCtx->width;
        frameJPG->height = JPGCodecCtx->height;
        frameJPG->format = JPGCodecCtx->pix_fmt;
        //debug("----%s %d \n", __func__, __LINE__);
        //sizeJPG = avcodec_encode_video(JPGCodecCtx, buffer, size, frameJPG);    // 编码，buffer为jpeg数据
        av_init_packet(&avpkt);
        ret = avcodec_encode_video2(JPGCodecCtx, &avpkt, frameJPG, &got_picture);    // 编码，buffer为jpeg数据
        if (ret < 0)
        {
            debug("encode to jpeg failed.\n");
            return -1;
        }
        //debug("----%s %d \n", __func__, __LINE__);
        if (got_picture)
        {
            buffer = avpkt.data;
            sizeJPG = avpkt.size;
            //debug("----%s %d \n", __func__, __LINE__);
            debug("size: %d\n", sizeJPG);
            fp = fopen(filename, "wb");
            if (fp == NULL)
            {
                debug("open directory or file failed.\n");
                return -1;
            }
            fwrite(buffer, 1, sizeJPG, fp);
            fclose(fp);
        } // end of got picture
        sws_freeContext(imgctx);    // 必须释放，否则内存泄漏
        imgctx = NULL;
    }

    avcodec_close(JPGCodecCtx);
    JPGCodecCtx = NULL;
    av_free(picture_buf);
    picture_buf = NULL;
    av_free(buffer);
    buffer = NULL;
    av_free(frameJPG);
    frameJPG = NULL;
    return 0;
}
