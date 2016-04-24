#include "stdafx.h" // for MFC

#include <stdio.h>
#ifdef linux
#include <unistd.h>
#endif

#include "H264ToVideo.h"

#define IO_BUFFER_SIZE (32768)
static unsigned char g_szIOBuffer[IO_BUFFER_SIZE];

H264BS2Video::H264BS2Video()
    :m_infctx(NULL),
    m_outfctx(NULL),
    m_instream(NULL),
    m_outstream(NULL),
    m_avio(NULL),
    m_videoidx(-1),
    m_isfile(0)
{
    m_avbuffer.ptr = NULL;
}

H264BS2Video::~H264BS2Video()
{
    if(m_avbuffer.ptr != NULL)
    {
        free(m_avbuffer.ptr);
        m_avbuffer.ptr = NULL;
    }
}

int H264BS2Video::openBSFile(const char* rawfile)
{
    int ret = 0;
    
    av_register_all();
    
    // 从文件判断视频格式
    ret = avformat_open_input(&m_infctx, rawfile, NULL, NULL);
    if (ret != 0)
    {
        debug("open input file failed. ret: %d\n", ret);
        return -1;
    }

    ret = avformat_find_stream_info(m_infctx, NULL);
    if (ret < 0)
    {
        debug("find stream info failed. ret: %d\n", ret);
        return -1;
    }

    // 找到视频流
    for (unsigned int i = 0; i < m_infctx->nb_streams; i++)
    {
        if (m_infctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            m_videoidx = i;
            m_instream = m_infctx->streams[i];
            break;
        }
    }
    if (m_videoidx == -1)
    {
        debug("no video stream.\n");
        return -1;
    }
    
    debug("video index: %d\n", m_videoidx);
    return 0;
}

int H264BS2Video::openVideoFile(const char* videofile, int width, int height, int fps, int gop, int bitrate)
{
    int ret = 0;

    av_register_all(); // 注册协议，等

    avformat_alloc_output_context2(&m_outfctx, NULL, NULL, videofile);

    m_outstream = avformat_new_stream(m_outfctx, m_instream->codec->codec);
    if (!m_outstream)
    {
        debug("avformat_new_stream failed.\n");
        return -1;
    }

    // 复制
    avcodec_copy_context(m_outstream->codec, m_instream->codec);

#if 0
    m_outstream->codec->bit_rate = bitrate;

    m_outstream->time_base.num = 1;
    m_outstream->time_base.den = fps;
    //m_outstream->codec->time_base.num = 1;
    //m_outstream->codec->time_base.den = fps;
    m_outstream->r_frame_rate.den = 1;
    m_outstream->r_frame_rate.num = fps;
#endif
    if (m_outfctx->oformat->flags & AVFMT_GLOBALHEADER)
    m_outstream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

#if 0
    m_outstream->codec->codec_id = m_instream->codec->codec_id;
    m_outstream->codec->codec_type = m_instream->codec->codec_type;
    m_outstream->codec->bit_rate = bitrate;
    m_outstream->codec->width = m_instream->codec->width;
    m_outstream->codec->height = m_instream->codec->height;

    m_outstream->time_base.num = 1;
    m_outstream->time_base.den = fps;
    //m_outstream->codec->time_base.num = 1;
    //m_outstream->codec->time_base.den = fps;
    m_outstream->codec->gop_size = m_instream->codec->gop_size;
    m_outstream->codec->pix_fmt = m_instream->codec->pix_fmt;
    m_outstream->codec->max_b_frames = 0;
    // for mp4...
    m_outstream->codec->extradata = m_instream->codec->extradata;
    m_outstream->codec->extradata_size  = m_instream->codec->extradata_size ;

    m_outstream->codec->flags = m_instream->codec->flags;
    m_outstream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
    m_outstream->codec->me_range = m_instream->codec->me_range;
    m_outstream->codec->max_qdiff = m_instream->codec->max_qdiff;
    m_outstream->codec->qmin = m_instream->codec->qmin;
    m_outstream->codec->qmax = m_instream->codec->qmax;
    m_outstream->codec->qcompress = m_instream->codec->qcompress;

    m_outstream->r_frame_rate.den = 1;
    m_outstream->r_frame_rate.num = fps;
#endif

    if (!(m_outfctx->flags & AVFMT_NOFILE))
    {
        if (avio_open(&m_outfctx->pb,videofile,AVIO_FLAG_WRITE)<0)
        {
            debug("avio_open failed.\n");
            return -1;
        }
    }
    if (!m_outfctx->nb_streams)
    {
        debug("nb_streams failed.\n");
        return -1;
    }

    ret = avformat_write_header(m_outfctx, NULL);
    if (ret < 0)
    {
        debug("avformat_write_header failed %d\n", ret);
        return -1;
    }

    m_isfile = 1;

    return 0;
}

static int writeBuffer(void *opaque, unsigned char *buf, int size)
{
    AVIOBufferContext* pIO = (AVIOBufferContext*)opaque;

    if (pIO->pos + size > pIO->totalSize)
    {
        // 重新申请 根据数值逐步加大
        int totalSize = pIO->totalSize*sizeof(char) * 3 / 2;
        unsigned char* ptr = (unsigned char*)realloc(pIO->ptr, totalSize);
        if (ptr == NULL)
        {
            return -1;
        }
        //debug("org ptr: %p new ptr: %p size: %d(%0.fMB) ", pIO->ptr, ptr,
        //            totalSize, totalSize/1024.0/1024.0);
        pIO->totalSize = totalSize;
        pIO->ptr = ptr;
        debug(" realloc for: %d!!!!!!!!!!!!!!!!!!!!!!!\n", totalSize);
    }
    memcpy(pIO->ptr + pIO->pos, buf, size);

    if (pIO->pos + size >= pIO->realSize)
        pIO->realSize += size;

    pIO->pos += size;

    return 0;
}

static int64_t seekBuffer(void *opaque, int64_t offset, int whence)
{
    AVIOBufferContext* pIO = (AVIOBufferContext*)opaque;
    int64_t new_pos = 0;
    int64_t fake_pos = 0;

    switch (whence)
    {
        case SEEK_SET:
            new_pos = offset;
            break;
        case SEEK_CUR:
            new_pos = pIO->pos + offset;
            break;
        case SEEK_END:
            new_pos = pIO->totalSize + offset;
            break;
        default:
            return -1;
    }

    fake_pos = min(new_pos, pIO->totalSize);
    if (fake_pos != pIO->pos)
    {
        pIO->pos = (int)fake_pos;
    }

    return new_pos;
}

int H264BS2Video::allocBuffer(int size)
{
    memset((void*)&m_avbuffer, '\0', sizeof(AVIOBufferContext));
    m_avbuffer.totalSize = size;
    m_avbuffer.ptr = (unsigned char*)malloc(m_avbuffer.totalSize*sizeof(char));
    if (m_avbuffer.ptr == NULL)
    {
        debug("alloc mem failed.\n");
        return -1;
    }
    debug("alloc ptr: %p total len: %d\n", m_avbuffer.ptr, m_avbuffer.totalSize);
    memset(m_avbuffer.ptr, '\0', m_avbuffer.totalSize);

    return 0;
}

int H264BS2Video::openVideoMem(const char* fmt, int width, int height, int fps, int gop, int bitrate)
{
    int ret = 0;

    if (m_isfile) return 0;

    av_register_all();

    m_avio =avio_alloc_context((unsigned char *)g_szIOBuffer, IO_BUFFER_SIZE, 1,
                &m_avbuffer, NULL, writeBuffer, seekBuffer);

    // 根据传递的fmt来确定是何种封装格式
    avformat_alloc_output_context2(&m_outfctx, NULL, fmt, NULL);
    m_outfctx->pb=m_avio;
    m_outfctx->flags=AVFMT_FLAG_CUSTOM_IO;
    debug("guess format: %s(%s) flag: %d\n", m_outfctx->oformat->name, m_outfctx->oformat->long_name, m_outfctx->oformat->flags);

    m_outstream = avformat_new_stream(m_outfctx, NULL);
    if (!m_outstream)
    {
        debug("avformat_new_stream failed.\n");
        return -1;
    }
    m_outstream->codec->codec_id = AV_CODEC_ID_H264;
    m_outstream->codec->codec_type = AVMEDIA_TYPE_VIDEO;
    m_outstream->codec->bit_rate = bitrate;
    m_outstream->codec->width = width;
    m_outstream->codec->height = height;

    m_outstream->time_base.num = 1;
    m_outstream->time_base.den = fps;
    m_outstream->codec->gop_size = gop;
    m_outstream->codec->pix_fmt = AV_PIX_FMT_YUV420P;
    m_outstream->codec->max_b_frames = 0;

    m_outstream->r_frame_rate.den = 1;
    m_outstream->r_frame_rate.num = fps;

    ret = avformat_write_header(m_outfctx, NULL);
    if (ret < 0)
    {
        debug("avformat_write_header failed %d\n", ret);
        return -1;
    }

    return 0;
}

int H264BS2Video::writeFrame(char* bitstream, int size, int keyframe)
{
    AVPacket pkt;
    int ret = 0;

    memset(&pkt, '\0', sizeof(AVPacket));
    av_init_packet(&pkt);

    if (keyframe)
    {
        pkt.flags |= AV_PKT_FLAG_KEY;
    }

    pkt.stream_index = m_outstream->index;
    pkt.data = (unsigned char*)bitstream;
    pkt.size = size;

    ret = av_write_frame(m_outfctx, &pkt);

    return ret;
}

int H264BS2Video::writeFrame(void)
{
    AVPacket avpkt = { 0 };
    int idx = 0;

#if 0
    char* filename = "../ffmpeg_log.txt";
    FILE* fp = NULL;
    char buffer[128] = {0};

    fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("open file %s failed.\n", filename);
        return -1;
    }
#endif

    av_init_packet(&avpkt);

    // av_read_fram返回下一帧，发生错误或文件结束返回<0
    while (av_read_frame(m_infctx, &avpkt) >= 0)
    {
        // 解码视频流
        if (avpkt.stream_index == m_videoidx)
        {
            // note：从formatcontext中获取
            AVStream* in_stream  = m_infctx->streams[avpkt.stream_index];
            AVStream* out_stream = m_outfctx->streams[m_videoidx];

            //debug("write %d, size: %d\n", idx++, avpkt.size);

            if (avpkt.pts == AV_NOPTS_VALUE)
            {
                // 计算PTS/DTS
                AVRational time_base = in_stream->time_base;
                int64_t duration=(int64_t)((double)AV_TIME_BASE/(double)av_q2d(in_stream->r_frame_rate));
                avpkt.pts=(int64_t)((double)(idx*duration)/(double)(av_q2d(time_base)*AV_TIME_BASE));
                avpkt.dts=avpkt.pts;
                avpkt.duration=(int)((double)duration/(double)(av_q2d(time_base)*AV_TIME_BASE));

                // 转换 PTS/DTS
                avpkt.pts = av_rescale_q_rnd(avpkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
                avpkt.dts = av_rescale_q_rnd(avpkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
                avpkt.duration = (int)av_rescale_q(avpkt.duration, in_stream->time_base, out_stream->time_base);

                idx++;
#if 0
                sprintf(buffer, "111 %d pts: %d dts: %d duration: %d\n", idx, avpkt.pts, avpkt.dts, avpkt.duration);
                fwrite(buffer, 1, strlen(buffer), fp);
#endif
            }

            int ret = av_interleaved_write_frame(m_outfctx, &avpkt);
        }

        av_packet_unref(&avpkt);
        //av_init_packet(&avpkt);

        avpkt.data = NULL;
        avpkt.size = 0;
    }

    //fclose(fp);

    return 0;
}

int H264BS2Video::close()
{
    if ( m_outfctx != NULL )
    {
        //printf("write trailer....\n");
        av_write_trailer(m_outfctx);
        for (unsigned int i = 0; i < m_outfctx->nb_streams; ++i)
        {
            av_freep(&m_outfctx->streams[i]->codec);
            av_freep(&m_outfctx->streams[i]);
        }
        if (m_isfile)
        {
            //printf("close...\n");
            avio_close(m_outfctx->pb);
        }

        av_free(m_outfctx);
        m_outfctx = NULL;
    }

    return 0;
}

void H264BS2Video::getBuffer(unsigned char** buffer, int* size)
{
    *buffer = m_avbuffer.ptr; 
    *size = m_avbuffer.realSize;
}

void H264BS2Video::freeBuffer(void)
{
    if(m_avbuffer.ptr != NULL)
    {
        free(m_avbuffer.ptr); 
        m_avbuffer.ptr = NULL;
    }
}

void H264BS2Video::resetBuffer(void)
{
    m_avbuffer.realSize = 0;
    m_avbuffer.pos = 0;
}
