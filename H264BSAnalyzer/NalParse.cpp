#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "NaLParse.h"

#include "H264BSAnalyzerDlg.h"

static void h264_debug_nal(h264_stream_t* h, nal_t* nal);
static void h265_debug_nal(h265_stream_t* h, h265_nal_t* nal);

// todo：不使用这种写死空间的做法
//存放解析出来的字符串
static char g_tmpStore[1024]={0};
static char g_outputInfo[512*1024]={'\0'};

CNalParser::CNalParser()
{
    m_nType = FILE_H264; // default
    m_hH264 = NULL;
    m_hH265 = NULL;
}

CNalParser::~CNalParser()
{
    release();
}

FileType CNalParser::judeVideoFile(const char* filename)
{
    char szExt[16] = {0};
    FileType type = FILE_H264; // default

    _splitpath(filename, NULL, NULL, NULL, szExt);
    if (!strcmp(&szExt[1], "h265") || !strcmp(&szExt[1], "265") ||
        !strcmp(&szExt[1], "hevc"))
    {
        type = FILE_H265;
    }
    else if (!strcmp(&szExt[1], "h264") || !strcmp(&szExt[1], "264") ||
        !strcmp(&szExt[1], "avc"))
    {
        type = FILE_H264;
    }
    else
    {
        // read file content
        FILE* fp = NULL;
        int offset = 0;
        int startcode = 0;
        unsigned char nalHader = 0;
        unsigned char nalType = 0;
        fp = fopen(filename, "r+b");
        offset = find_first_nal(fp, startcode);
        fseek(fp, offset+startcode, SEEK_SET);
        fread((void*)&nalHader,1,1,fp);
        // check h264 first...
        nalType = nalHader & 0x1f; // 5 bit
        if (nalType > 0 && nalType < 22) // ok
        {
            type = FILE_H264;
        }
        else
        {
            // not h264, then check h265...
            nalType = (nalHader>>1) & 0x3f; // 6 bit
            if (nalType >= 0 && nalType <= 47) // ok
            {
                type = FILE_H265;
            }
        }
    }

    return type;
}

int CNalParser::init(const char* filename)
{
    m_filename = filename;

    // judge file 
    m_nType = judeVideoFile(m_filename);

    // init handle
    if (m_nType == FILE_H265)
    {
        if (m_hH265 != NULL)
        {
            h265_free(m_hH265);
        }
        m_hH265 = h265_new();
    }
    else
    {
        if (m_hH264 != NULL)
        {
            h264_free(m_hH264);
        }
        m_hH264 = h264_new();
    }
    
    return 0;
}

int CNalParser::release(void)
{
    if (m_hH264 != NULL)
    {
        h264_free(m_hH264);
        m_hH264 = NULL;
    }
    if (m_hH265 != NULL)
    {
        h265_free(m_hH265);
        m_hH265 = NULL;
    }
    return 0;
}

int CNalParser::h264_nal_probe(char *fileurl, vector<NALU_t>& vNal, int num)
{
    NALU_t n;
    int nal_num=0;
    int data_offset=0;
    int data_lenth;
    FILE* fp = NULL;

    fp=fopen(fileurl, "r+b");
    if (fp == NULL)
    {
        return -1;
    }

    memset(&n, '\0', sizeof(NALU_t));

    n.type = m_nType; // h.265
    int startcode_len = 0;

    int tmp = find_first_nal(fp, startcode_len);

    data_offset = tmp;

    while(!feof(fp))
    {
        if (num > 0 && nal_num == num)
        {
            break;
        }
        data_lenth=GetAnnexbNALU(fp, &n);//每执行一次，文件的指针指向本次找到的NALU的末尾，下一个位置即为下个NALU的起始码0x000001
        n.data_offset=data_offset;
        n.num = nal_num;
        data_offset=data_offset+data_lenth;

        vNal.push_back(n);

        nal_num++;
    }
    return 0;
}

int CNalParser::h264_nal_parse(char* filename,int data_offset,int data_lenth,LPVOID lparam)
{
    int nal_start,nal_end;
    //static h264_stream_t* h = NULL;
    //static h265_stream_t* h1 = NULL;

    //清空字符串-----------------
    memset(g_outputInfo,'\0',100000);

    if (data_lenth == 0)
        return 0;
    //句柄
    CH264BSAnalyzerDlg *dlg=(CH264BSAnalyzerDlg *)lparam;
    //tempstr=(char *)malloc(10000);
    //g_outputInfo=(char *)malloc(100000);
    //内存用于存放NAL（包含起始码）
    uint8_t *nal_temp=(uint8_t *)malloc(data_lenth);

    //从文件读取
    FILE *fp=fopen(filename,"rb");
    if (fp == NULL)
    {
        return -1;
    }

    fseek(fp,data_offset,SEEK_SET);
    fread(nal_temp,data_lenth,1,fp);

    printf("test\n");
    find_nal_unit(nal_temp, data_lenth, &nal_start, &nal_end);
    if (m_nType == 1)
    {
        h265_read_nal_unit(m_hH265, &nal_temp[nal_start], nal_end - nal_start);
        h265_debug_nal(m_hH265,m_hH265->nal);    // 打印到g_outputInfo中
    }
    else
    {
        read_nal_unit(m_hH264, &nal_temp[nal_start], nal_end - nal_start);
        h264_debug_nal(m_hH264, m_hH264->nal);  // 打印到g_outputInfo中
    }
    dlg->m_h264NalInfo.SetWindowText(g_outputInfo);    // 把NAL详细信息显示到界面上

    // 使用新的十六进制显示控件
    dlg->m_edHexInfo.SetData((LPBYTE)nal_temp, data_lenth);

    // 不要控件焦点
    ::SendMessage(dlg->GetDlgItem(IDC_EDIT_HEX)-> m_hWnd,WM_KILLFOCUS,-1,0);

    if (nal_temp != NULL)
    {
        free(nal_temp);
        nal_temp = NULL;
    }

    // 不再释放
#if 0
    // 必须调用，否则不释放内存
    if (m_nType == 1)
        h265_free(h1);
    else
        h264_free(h);
#endif
    fclose(fp);
    return 0;
}

// 解析SPS，得到视频宽高、yuv空间等信息
int CNalParser::h264_sps_parse(char* filename,int data_offset,int data_lenth, SPSInfo_t& info)
{
    int nal_start,nal_end;

    //内存用于存放NAL（包含起始码）
    uint8_t *nal_temp=(uint8_t *)malloc(data_lenth);

    //从文件读取
    FILE *fp=fopen(filename,"rb");
    if (fp == NULL)
    {
        return -1;
    }

    fseek(fp,data_offset,SEEK_SET);
    fread(nal_temp,data_lenth,1,fp);
    // read some H264 data into buf

    find_nal_unit(nal_temp, data_lenth, &nal_start, &nal_end);
    read_nal_unit(m_hH264, &nal_temp[nal_start], nal_end - nal_start);

    int pic_width_in_mbs_minus1;
    int pic_height_in_map_units_minus1;
    int frame_crop_left_offset, frame_crop_right_offset;
    int frame_mbs_only_flag, frame_crop_top_offset, frame_crop_bottom_offset;

    pic_width_in_mbs_minus1 =  m_hH264->sps->pic_width_in_mbs_minus1;
    pic_height_in_map_units_minus1 =  m_hH264->sps->pic_height_in_map_units_minus1;

    frame_mbs_only_flag =  m_hH264->sps->frame_mbs_only_flag;
    info.crop_left = frame_crop_left_offset = m_hH264->sps->frame_crop_left_offset;
    info.crop_right = frame_crop_right_offset = m_hH264->sps->frame_crop_right_offset;
    info.crop_top = frame_crop_top_offset = m_hH264->sps->frame_crop_top_offset;
    info.crop_bottom = frame_crop_bottom_offset = m_hH264->sps->frame_crop_bottom_offset;

    // 宽高计算公式
    info.width = ((pic_width_in_mbs_minus1 +1)*16) - frame_crop_left_offset*2 - frame_crop_right_offset*2;
    info.height= ((2 - frame_mbs_only_flag)* (pic_height_in_map_units_minus1 +1) * 16) - (frame_crop_top_offset * 2) - (frame_crop_bottom_offset * 2);

    info.profile_idc = m_hH264->sps->profile_idc;
    info.level_idc = m_hH264->sps->level_idc;

    // YUV空间
    info.chroma_format_idc = m_hH264->sps->chroma_format_idc;

    // 注：这里的帧率计算还有疑问
    if (m_hH264->sps->vui_parameters_present_flag)
    {
        info.max_framerate = (float)(m_hH264->sps->vui.time_scale) / (float)(m_hH264->sps->vui.num_units_in_tick);
    }
    if (nal_temp != NULL)
    {
        free(nal_temp);
        nal_temp = NULL;
    }

    fclose(fp);
    return 0;
}

int CNalParser::h264_pps_parse(char* filename,int data_offset,int data_lenth, PPSInfo_t& info)
{
    int nal_start,nal_end;

    //内存用于存放NAL（包含起始码）
    uint8_t *nal_temp=(uint8_t *)malloc(data_lenth);

    //从文件读取
    FILE *fp=fopen(filename,"rb");
    if (fp == NULL)
    {
        return -1;
    }

    fseek(fp,data_offset,SEEK_SET);
    fread(nal_temp,data_lenth,1,fp);
    // read some H264 data into buf
    find_nal_unit(nal_temp, data_lenth, &nal_start, &nal_end);
    read_nal_unit(m_hH264, &nal_temp[nal_start], nal_end - nal_start);

    info.encoding_type = m_hH264->pps->entropy_coding_mode_flag;

    if (nal_temp != NULL)
    {
        free(nal_temp);
        nal_temp = NULL;
    }

    fclose(fp);
    return 0;
}

//FILE *g_fpBitStream = NULL;                //!< the bit stream file
#define MAX_NAL_SIZE (1*1024*1024)

static int ue(char *buff, int len, int &start_bit)
{
    int zero_num = 0;
    int ret = 0;

    while (start_bit < len * 8)
    {
        if (buff[start_bit / 8] & (0x80 >> (start_bit % 8)))
        {
            break;
        }
        zero_num++;
        start_bit++;
    }
    start_bit++;

    for (int i=0; i<zero_num; i++)
    {
        ret <<= 1;
        if (buff[start_bit / 8] & (0x80 >> (start_bit % 8)))
        {
            ret += 1;
        }
        start_bit++;
    }
    return (1 << zero_num) - 1 + ret;
}


//这个函数输入为一个NAL结构体，主要功能为得到一个完整的NALU并保存在NALU_t的buf中，获取他的长度，填充F,IDC,TYPE位。
//并且返回两个开始字符之间间隔的字节数，即包含有前缀的NALU的长度
// todo:每次读一个字节，较慢，有无好的方法？
int CNalParser::GetAnnexbNALU (FILE* fp, NALU_t *nalu)
{
    int pos = 0;
    int found_startcode, rewind;
    unsigned char *Buf;
    int info2=0, info3=0;
    int eof = 0;
    int startcodeprefix_len = 3; // 码流序列的开始字符为3个字节

    if ((Buf = (unsigned char*)calloc (MAX_NAL_SIZE, sizeof(char))) == NULL)
        printf ("GetAnnexbNALU: Could not allocate Buf memory\n");

    //nalu->startcodeprefix_len=3;

    if (3 != fread (Buf, 1, 3, fp))//从码流中读3个字节
    {
        free(Buf);
        return 0;
    }
    info2 = findStartcode3 (Buf);//判断是否为0x000001
    if(info2 != 1)
    {
        //如果不是，再读一个字节
        if(1 != fread(Buf+3, 1, 1, fp))//读一个字节
        {
            free(Buf);
            return 0;
        }
        info3 = findStartcode4 (Buf);//判断是否为0x00000001
        if (info3 != 1)//如果不是，返回-1
        {
            free(Buf);
            return -1;
        }
        else
        {
            //如果是0x00000001,得到开始前缀为4个字节
            //pos = 4;
            startcodeprefix_len = 4;
        }
    }
    else
    {
        //如果是0x000001,得到开始前缀为3个字节
        startcodeprefix_len = 3;
        //pos = 3;
    }

    pos = startcodeprefix_len;
    //查找下一个开始字符的标志位
    found_startcode = 0;
    info2 = 0;
    info3 = 0;

    while (!found_startcode)
    {
        if (feof(fp))//判断是否到了文件尾
        {
            eof = 1;
            goto got_nal;
        }
        Buf[pos++] = fgetc(fp);//读一个字节到BUF中

        info3 = findStartcode4(&Buf[pos-4]);//判断是否为0x00000001
        if(info3 != 1)
            info2 = findStartcode3(&Buf[pos-3]);//判断是否为0x000001

        found_startcode = (info2 == 1 || info3 == 1);
    }

    // Here, we have found another start code (and read length of startcode bytes more than we should
    // have.  Hence, go back in the file
    rewind = (info3 == 1)? -4 : -3;

    if (0 != fseek (fp, rewind, SEEK_CUR))//把文件指针指向前一个NALU的末尾
    {
        free(Buf);
        printf("GetAnnexbNALU: Cannot fseek in the bit stream file");
    }

got_nal:
    // 当达到文件末尾时，回退1个位置
    if (eof)
    {
        rewind = -1;
    }
    // Here the Start code, the complete NALU, and the next start code is in the Buf.
    // The size of Buf is pos, pos+rewind are the number of bytes excluding the next
    // start code, and (pos+rewind)-startcodeprefix_len is the size of the NALU excluding the start code
    // 不包含起始码的数据的长度
    //nalu->len = (pos+rewind)-nalu->startcodeprefix_len;
    // 有什么用？
    //memcpy (nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);//拷贝一个完整NALU，不拷贝起始前缀0x000001或0x00000001
    //nalu->forbidden_bit = nalu->buf[0] & 0x80; //1 bit
    //nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit
    //nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;// 5 bit

    // 包括起始码在内的5个字节
    sprintf(nalu->startcode_buf, "%02x%02x%02x%02x%02x", Buf[0], Buf[1], Buf[2], Buf[3], Buf[4]);
    nalu->len = pos+rewind;   //nalu->len + nalu->startcodeprefix_len;
    uint16_t nal_header = 0; // two bytes for h.265
    if (nalu->type)
    {
        //nal_header = (Buf[startcodeprefix_len]<<8) | Buf[startcodeprefix_len+1];
        //nalu->nal_unit_type = h265_get_nal_type((uint8_t*)&nal_header, 2);
        nal_header = Buf[startcodeprefix_len];
        nalu->nal_unit_type = h265_get_nal_type((uint8_t*)&nal_header, 1); // ugly

        // todo 读slice_type

    }
    else
    {
        //char nal_header = 0;
        nal_header = Buf[startcodeprefix_len];
        nalu->nal_unit_type = nal_header & 0x1f;// 5 bit

        // 获取slice类型：I帧、P帧、B帧
        // 注：在nal类型为1~5时获取
        if (nalu->nal_unit_type <= 5 && nalu->nal_unit_type >= 1)
        {
            int start_bit = 0;
            int first_mb_in_slice = ue((char*)Buf+startcodeprefix_len+1, 8, start_bit);
            nalu->slice_type = ue((char*)Buf+startcodeprefix_len+1, 8, start_bit);
        }
    }


    free(Buf);

    return (pos+rewind);//返回两个开始字符之间间隔的字节数，即包含有前缀的NALU的长度
}

int CNalParser::find_first_nal(FILE* fp, int& startcodeLenght)
{
    int found_startcode = 0;
    int info2 = 0;
    int info3 = 0;
    int eof = 0;
    int pos = 0;
    int startcode_len = 0;
    unsigned char *Buf = NULL;

    if ((Buf = (unsigned char*)calloc(MAX_NAL_SIZE, sizeof(char))) == NULL)
        printf ("find_first_nal: Could not allocate Buf memory\n");

    while (!found_startcode && !feof(fp))
    {
        Buf[pos++] = fgetc(fp);//读一个字节到BUF中

        info3 = findStartcode4(&Buf[pos-4]);//判断是否为0x00000001
        if(info3 != 1)
        {
            info2 = findStartcode3(&Buf[pos-3]);//判断是否为0x000001
            if (info2)
            {
                startcode_len = 3;
            }
        }
        else
        {
            startcode_len = 4;
        }

        found_startcode = (info2 == 1 || info3 == 1);
    }

    // 文件指针要恢复
    fseek (fp, -startcode_len, SEEK_CUR);

    free(Buf);
    startcodeLenght = startcode_len;

    return pos - startcode_len;
}

//自己写的，解析NAL数据的函数

// 以下代码来自h264_stream.c，单独出来
/***************************** debug ******************************/

#define my_printf(...) do { \
    sprintf(g_tmpStore, __VA_ARGS__);\
    strcat(g_tmpStore, "\r\n"); \
    strcat(g_outputInfo, g_tmpStore);} while(0)

static void h264_debug_sps(sps_t* sps)
{
    my_printf("======= SPS =======");
    my_printf(" profile_idc : %d", sps->profile_idc );
    my_printf(" constraint_set0_flag : %d", sps->constraint_set0_flag );
    my_printf(" constraint_set1_flag : %d", sps->constraint_set1_flag );
    my_printf(" constraint_set2_flag : %d", sps->constraint_set2_flag );
    my_printf(" constraint_set3_flag : %d", sps->constraint_set3_flag );
    my_printf(" constraint_set4_flag : %d", sps->constraint_set4_flag );
    my_printf(" constraint_set5_flag : %d", sps->constraint_set5_flag );
    my_printf(" reserved_zero_2bits : %d", sps->reserved_zero_2bits );
    my_printf(" level_idc : %d", sps->level_idc );
    my_printf(" seq_parameter_set_id : %d", sps->seq_parameter_set_id );
    my_printf(" chroma_format_idc : %d", sps->chroma_format_idc );
    my_printf(" residual_colour_transform_flag : %d", sps->residual_colour_transform_flag );
    my_printf(" bit_depth_luma_minus8 : %d", sps->bit_depth_luma_minus8 );
    my_printf(" bit_depth_chroma_minus8 : %d", sps->bit_depth_chroma_minus8 );
    my_printf(" qpprime_y_zero_transform_bypass_flag : %d", sps->qpprime_y_zero_transform_bypass_flag );
    my_printf(" seq_scaling_matrix_present_flag : %d", sps->seq_scaling_matrix_present_flag );
    if (sps->seq_scaling_matrix_present_flag)
    {
        for (int i = 0; i < ((sps->chroma_format_idc!=3) ? 8 : 12); i++)
        {
            my_printf("   seq_scaling_list_present_flag[%d] : %d", i, sps->seq_scaling_list_present_flag[i]);
        }
    }
    //  int seq_scaling_list_present_flag[8];
    //  void* ScalingList4x4[6];
    //  int UseDefaultScalingMatrix4x4Flag[6];
    //  void* ScalingList8x8[2];
    //  int UseDefaultScalingMatrix8x8Flag[2];
    my_printf(" log2_max_frame_num_minus4 : %d", sps->log2_max_frame_num_minus4 );
    my_printf(" pic_order_cnt_type : %d", sps->pic_order_cnt_type );
    my_printf("   log2_max_pic_order_cnt_lsb_minus4 : %d", sps->log2_max_pic_order_cnt_lsb_minus4 );
    my_printf("   delta_pic_order_always_zero_flag : %d", sps->delta_pic_order_always_zero_flag );
    my_printf("   offset_for_non_ref_pic : %d", sps->offset_for_non_ref_pic );
    my_printf("   offset_for_top_to_bottom_field : %d", sps->offset_for_top_to_bottom_field );
    my_printf("   num_ref_frames_in_pic_order_cnt_cycle : %d", sps->num_ref_frames_in_pic_order_cnt_cycle );
    //  int offset_for_ref_frame[256];
    my_printf(" num_ref_frames : %d", sps->num_ref_frames );
    my_printf(" gaps_in_frame_num_value_allowed_flag : %d", sps->gaps_in_frame_num_value_allowed_flag );
    my_printf(" pic_width_in_mbs_minus1 : %d", sps->pic_width_in_mbs_minus1 );
    my_printf(" pic_height_in_map_units_minus1 : %d", sps->pic_height_in_map_units_minus1 );
    my_printf(" frame_mbs_only_flag : %d", sps->frame_mbs_only_flag );
    my_printf(" mb_adaptive_frame_field_flag : %d", sps->mb_adaptive_frame_field_flag );
    my_printf(" direct_8x8_inference_flag : %d", sps->direct_8x8_inference_flag );
    my_printf(" frame_cropping_flag : %d", sps->frame_cropping_flag );
    if (sps->frame_cropping_flag)
    {
        my_printf("   frame_crop_left_offset : %d", sps->frame_crop_left_offset );
        my_printf("   frame_crop_right_offset : %d", sps->frame_crop_right_offset );
        my_printf("   frame_crop_top_offset : %d", sps->frame_crop_top_offset );
        my_printf("   frame_crop_bottom_offset : %d", sps->frame_crop_bottom_offset );
    }
    my_printf(" vui_parameters_present_flag : %d", sps->vui_parameters_present_flag );
    if (sps->vui_parameters_present_flag)
    {
        my_printf("=== VUI ===");
        my_printf(" aspect_ratio_info_present_flag : %d", sps->vui.aspect_ratio_info_present_flag );
        my_printf("   aspect_ratio_idc : %d", sps->vui.aspect_ratio_idc );
        my_printf("     sar_width : %d", sps->vui.sar_width );
        my_printf("     sar_height : %d", sps->vui.sar_height );
        my_printf(" overscan_info_present_flag : %d", sps->vui.overscan_info_present_flag );
        my_printf("   overscan_appropriate_flag : %d", sps->vui.overscan_appropriate_flag );
        my_printf(" video_signal_type_present_flag : %d", sps->vui.video_signal_type_present_flag );
        my_printf("   video_format : %d", sps->vui.video_format );
        my_printf("   video_full_range_flag : %d", sps->vui.video_full_range_flag );
        my_printf("   colour_description_present_flag : %d", sps->vui.colour_description_present_flag );
        my_printf("     colour_primaries : %d", sps->vui.colour_primaries );
        my_printf("   transfer_characteristics : %d", sps->vui.transfer_characteristics );
        my_printf("   matrix_coefficients : %d", sps->vui.matrix_coefficients );
        my_printf(" chroma_loc_info_present_flag : %d", sps->vui.chroma_loc_info_present_flag );
        my_printf("   chroma_sample_loc_type_top_field : %d", sps->vui.chroma_sample_loc_type_top_field );
        my_printf("   chroma_sample_loc_type_bottom_field : %d", sps->vui.chroma_sample_loc_type_bottom_field );
        my_printf(" timing_info_present_flag : %d", sps->vui.timing_info_present_flag );
        my_printf("   num_units_in_tick : %d", sps->vui.num_units_in_tick );
        my_printf("   time_scale : %d", sps->vui.time_scale );
        my_printf("   fixed_frame_rate_flag : %d", sps->vui.fixed_frame_rate_flag );
        my_printf(" nal_hrd_parameters_present_flag : %d", sps->vui.nal_hrd_parameters_present_flag );
        my_printf(" vcl_hrd_parameters_present_flag : %d", sps->vui.vcl_hrd_parameters_present_flag );
        my_printf("   low_delay_hrd_flag : %d", sps->vui.low_delay_hrd_flag );
        my_printf(" pic_struct_present_flag : %d", sps->vui.pic_struct_present_flag );
        my_printf(" bitstream_restriction_flag : %d", sps->vui.bitstream_restriction_flag );
        my_printf("   motion_vectors_over_pic_boundaries_flag : %d", sps->vui.motion_vectors_over_pic_boundaries_flag );
        my_printf("   max_bytes_per_pic_denom : %d", sps->vui.max_bytes_per_pic_denom );
        my_printf("   max_bits_per_mb_denom : %d", sps->vui.max_bits_per_mb_denom );
        my_printf("   log2_max_mv_length_horizontal : %d", sps->vui.log2_max_mv_length_horizontal );
        my_printf("   log2_max_mv_length_vertical : %d", sps->vui.log2_max_mv_length_vertical );
        my_printf("   num_reorder_frames : %d", sps->vui.num_reorder_frames );
        my_printf("   max_dec_frame_buffering : %d", sps->vui.max_dec_frame_buffering );
    }
    my_printf("=== HRD ===");
    my_printf(" cpb_cnt_minus1 : %d", sps->hrd.cpb_cnt_minus1 );
    my_printf(" bit_rate_scale : %d", sps->hrd.bit_rate_scale );
    my_printf(" cpb_size_scale : %d", sps->hrd.cpb_size_scale );
    int SchedSelIdx;
    for( SchedSelIdx = 0; SchedSelIdx <= sps->hrd.cpb_cnt_minus1; SchedSelIdx++ )
    {
        my_printf("   bit_rate_value_minus1[%d] : %d", SchedSelIdx, sps->hrd.bit_rate_value_minus1[SchedSelIdx] ); // up to cpb_cnt_minus1, which is <= 31
        my_printf("   cpb_size_value_minus1[%d] : %d", SchedSelIdx, sps->hrd.cpb_size_value_minus1[SchedSelIdx] );
        my_printf("   cbr_flag[%d] : %d", SchedSelIdx, sps->hrd.cbr_flag[SchedSelIdx] );
    }
    my_printf(" initial_cpb_removal_delay_length_minus1 : %d", sps->hrd.initial_cpb_removal_delay_length_minus1 );
    my_printf(" cpb_removal_delay_length_minus1 : %d", sps->hrd.cpb_removal_delay_length_minus1 );
    my_printf(" dpb_output_delay_length_minus1 : %d", sps->hrd.dpb_output_delay_length_minus1 );
    my_printf(" time_offset_length : %d", sps->hrd.time_offset_length );
}


static void h264_debug_pps(pps_t* pps)
{
    my_printf("======= PPS =======");
    my_printf(" pic_parameter_set_id : %d", pps->pic_parameter_set_id );
    my_printf(" seq_parameter_set_id : %d", pps->seq_parameter_set_id );
    my_printf(" entropy_coding_mode_flag : %d", pps->entropy_coding_mode_flag );
    my_printf(" pic_order_present_flag : %d", pps->pic_order_present_flag );
    my_printf(" num_slice_groups_minus1 : %d", pps->num_slice_groups_minus1 );
    my_printf(" slice_group_map_type : %d", pps->slice_group_map_type );
    //  int run_length_minus1[8]; // up to num_slice_groups_minus1, which is <= 7 in Baseline and Extended, 0 otheriwse
    //  int top_left[8];
    //  int bottom_right[8];
    //  int slice_group_change_direction_flag;
    //  int slice_group_change_rate_minus1;
    //  int pic_size_in_map_units_minus1;
    //  int slice_group_id[256]; // FIXME what size?
    my_printf(" num_ref_idx_l0_active_minus1 : %d", pps->num_ref_idx_l0_active_minus1 );
    my_printf(" num_ref_idx_l1_active_minus1 : %d", pps->num_ref_idx_l1_active_minus1 );
    my_printf(" weighted_pred_flag : %d", pps->weighted_pred_flag );
    my_printf(" weighted_bipred_idc : %d", pps->weighted_bipred_idc );
    my_printf(" pic_init_qp_minus26 : %d", pps->pic_init_qp_minus26 );
    my_printf(" pic_init_qs_minus26 : %d", pps->pic_init_qs_minus26 );
    my_printf(" chroma_qp_index_offset : %d", pps->chroma_qp_index_offset );
    my_printf(" deblocking_filter_control_present_flag : %d", pps->deblocking_filter_control_present_flag );
    my_printf(" constrained_intra_pred_flag : %d", pps->constrained_intra_pred_flag );
    my_printf(" redundant_pic_cnt_present_flag : %d", pps->redundant_pic_cnt_present_flag );
    my_printf(" transform_8x8_mode_flag : %d", pps->transform_8x8_mode_flag );
    my_printf(" pic_scaling_matrix_present_flag : %d", pps->pic_scaling_matrix_present_flag );
    //  int pic_scaling_list_present_flag[8];
    //  void* ScalingList4x4[6];
    //  int UseDefaultScalingMatrix4x4Flag[6];
    //  void* ScalingList8x8[2];
    //  int UseDefaultScalingMatrix8x8Flag[2];
    my_printf(" second_chroma_qp_index_offset : %d", pps->second_chroma_qp_index_offset );
}

static void h264_debug_slice_header(slice_header_t* sh)
{
    my_printf("======= Slice Header =======");
    my_printf(" first_mb_in_slice : %d", sh->first_mb_in_slice );
    const char* slice_type_name;
    switch(sh->slice_type)
    {
    case SH_SLICE_TYPE_P :       slice_type_name = "P slice"; break;
    case SH_SLICE_TYPE_B :       slice_type_name = "B slice"; break;
    case SH_SLICE_TYPE_I :       slice_type_name = "I slice"; break;
    case SH_SLICE_TYPE_SP :      slice_type_name = "SP slice"; break;
    case SH_SLICE_TYPE_SI :      slice_type_name = "SI slice"; break;
    case SH_SLICE_TYPE_P_ONLY :  slice_type_name = "P slice only"; break;
    case SH_SLICE_TYPE_B_ONLY :  slice_type_name = "B slice only"; break;
    case SH_SLICE_TYPE_I_ONLY :  slice_type_name = "I slice only"; break;
    case SH_SLICE_TYPE_SP_ONLY : slice_type_name = "SP slice only"; break;
    case SH_SLICE_TYPE_SI_ONLY : slice_type_name = "SI slice only"; break;
    default :                    slice_type_name = "Unknown"; break;
    }
    my_printf(" slice_type : %d ( %s )", sh->slice_type, slice_type_name );

    my_printf(" pic_parameter_set_id : %d", sh->pic_parameter_set_id );
    my_printf(" frame_num : %d", sh->frame_num );
    my_printf(" field_pic_flag : %d", sh->field_pic_flag );
      my_printf(" bottom_field_flag : %d", sh->bottom_field_flag );
    my_printf(" idr_pic_id : %d", sh->idr_pic_id );
    my_printf(" pic_order_cnt_lsb : %d", sh->pic_order_cnt_lsb );
    my_printf(" delta_pic_order_cnt_bottom : %d", sh->delta_pic_order_cnt_bottom );
    // int delta_pic_order_cnt[ 2 ];
    my_printf(" redundant_pic_cnt : %d", sh->redundant_pic_cnt );
    my_printf(" direct_spatial_mv_pred_flag : %d", sh->direct_spatial_mv_pred_flag );
    my_printf(" num_ref_idx_active_override_flag : %d", sh->num_ref_idx_active_override_flag );
    my_printf(" num_ref_idx_l0_active_minus1 : %d", sh->num_ref_idx_l0_active_minus1 );
    my_printf(" num_ref_idx_l1_active_minus1 : %d", sh->num_ref_idx_l1_active_minus1 );
    my_printf(" cabac_init_idc : %d", sh->cabac_init_idc );
    my_printf(" slice_qp_delta : %d", sh->slice_qp_delta );
    my_printf(" sp_for_switch_flag : %d", sh->sp_for_switch_flag );
    my_printf(" slice_qs_delta : %d", sh->slice_qs_delta );
    my_printf(" disable_deblocking_filter_idc : %d", sh->disable_deblocking_filter_idc );
    my_printf(" slice_alpha_c0_offset_div2 : %d", sh->slice_alpha_c0_offset_div2 );
    my_printf(" slice_beta_offset_div2 : %d", sh->slice_beta_offset_div2 );
    my_printf(" slice_group_change_cycle : %d", sh->slice_group_change_cycle );

    my_printf("=== Prediction Weight Table ===");
    my_printf(" luma_log2_weight_denom : %d", sh->pwt.luma_log2_weight_denom );
    my_printf(" chroma_log2_weight_denom : %d", sh->pwt.chroma_log2_weight_denom );
     //   my_printf(" luma_weight_l0_flag : %d", sh->pwt.luma_weight_l0_flag );
        // int luma_weight_l0[64];
        // int luma_offset_l0[64];
    //    my_printf(" chroma_weight_l0_flag : %d", sh->pwt.chroma_weight_l0_flag );
        // int chroma_weight_l0[64][2];
        // int chroma_offset_l0[64][2];
     //   my_printf(" luma_weight_l1_flag : %d", sh->pwt.luma_weight_l1_flag );
        // int luma_weight_l1[64];
        // int luma_offset_l1[64];
    //    my_printf(" chroma_weight_l1_flag : %d", sh->pwt.chroma_weight_l1_flag );
        // int chroma_weight_l1[64][2];
        // int chroma_offset_l1[64][2];

    my_printf("=== Ref Pic List Reordering ===");
    my_printf(" ref_pic_list_reordering_flag_l0 : %d", sh->rplr.ref_pic_list_reordering_flag_l0 );
    my_printf(" ref_pic_list_reordering_flag_l1 : %d", sh->rplr.ref_pic_list_reordering_flag_l1 );
        // int reordering_of_pic_nums_idc;
        // int abs_diff_pic_num_minus1;
        // int long_term_pic_num;

    my_printf("=== Decoded Ref Pic Marking ===");
    my_printf(" no_output_of_prior_pics_flag : %d", sh->drpm.no_output_of_prior_pics_flag );
    my_printf(" long_term_reference_flag : %d", sh->drpm.long_term_reference_flag );
    my_printf(" adaptive_ref_pic_marking_mode_flag : %d", sh->drpm.adaptive_ref_pic_marking_mode_flag );
        // int memory_management_control_operation;
        // int difference_of_pic_nums_minus1;
        // int long_term_pic_num;
        // int long_term_frame_idx;
        // int max_long_term_frame_idx_plus1;

}

static void h264_debug_aud(aud_t* aud)
{
    my_printf("======= Access Unit Delimiter =======");
    const char* primary_pic_type_name;
    switch (aud->primary_pic_type)
    {
    case AUD_PRIMARY_PIC_TYPE_I :       primary_pic_type_name = "I"; break;
    case AUD_PRIMARY_PIC_TYPE_IP :      primary_pic_type_name = "I, P"; break;
    case AUD_PRIMARY_PIC_TYPE_IPB :     primary_pic_type_name = "I, P, B"; break;
    case AUD_PRIMARY_PIC_TYPE_SI :      primary_pic_type_name = "SI"; break;
    case AUD_PRIMARY_PIC_TYPE_SISP :    primary_pic_type_name = "SI, SP"; break;
    case AUD_PRIMARY_PIC_TYPE_ISI :     primary_pic_type_name = "I, SI"; break;
    case AUD_PRIMARY_PIC_TYPE_ISIPSP :  primary_pic_type_name = "I, SI, P, SP"; break;
    case AUD_PRIMARY_PIC_TYPE_ISIPSPB : primary_pic_type_name = "I, SI, P, SP, B"; break;
    default : primary_pic_type_name = "Unknown"; break;
    }
    my_printf(" primary_pic_type : %d ( %s )", aud->primary_pic_type, primary_pic_type_name );
}

static void h264_debug_seis( h264_stream_t* h)
{
    sei_t** seis = h->seis;
    int num_seis = h->num_seis;

    my_printf("======= SEI =======");
    const char* sei_type_name;
    int i;
    for (i = 0; i < num_seis; i++)
    {
        sei_t* s = seis[i];
        switch(s->payloadType)
        {
        case SEI_TYPE_BUFFERING_PERIOD :          sei_type_name = "Buffering period"; break;
        case SEI_TYPE_PIC_TIMING :                sei_type_name = "Pic timing"; break;
        case SEI_TYPE_PAN_SCAN_RECT :             sei_type_name = "Pan scan rect"; break;
        case SEI_TYPE_FILLER_PAYLOAD :            sei_type_name = "Filler payload"; break;
        case SEI_TYPE_USER_DATA_REGISTERED_ITU_T_T35 : sei_type_name = "User data registered ITU-T T35"; break;
        case SEI_TYPE_USER_DATA_UNREGISTERED :    sei_type_name = "User data unregistered"; break;
        case SEI_TYPE_RECOVERY_POINT :            sei_type_name = "Recovery point"; break;
        case SEI_TYPE_DEC_REF_PIC_MARKING_REPETITION : sei_type_name = "Dec ref pic marking repetition"; break;
        case SEI_TYPE_SPARE_PIC :                 sei_type_name = "Spare pic"; break;
        case SEI_TYPE_SCENE_INFO :                sei_type_name = "Scene info"; break;
        case SEI_TYPE_SUB_SEQ_INFO :              sei_type_name = "Sub seq info"; break;
        case SEI_TYPE_SUB_SEQ_LAYER_CHARACTERISTICS : sei_type_name = "Sub seq layer characteristics"; break;
        case SEI_TYPE_SUB_SEQ_CHARACTERISTICS :   sei_type_name = "Sub seq characteristics"; break;
        case SEI_TYPE_FULL_FRAME_FREEZE :         sei_type_name = "Full frame freeze"; break;
        case SEI_TYPE_FULL_FRAME_FREEZE_RELEASE : sei_type_name = "Full frame freeze release"; break;
        case SEI_TYPE_FULL_FRAME_SNAPSHOT :       sei_type_name = "Full frame snapshot"; break;
        case SEI_TYPE_PROGRESSIVE_REFINEMENT_SEGMENT_START : sei_type_name = "Progressive refinement segment start"; break;
        case SEI_TYPE_PROGRESSIVE_REFINEMENT_SEGMENT_END : sei_type_name = "Progressive refinement segment end"; break;
        case SEI_TYPE_MOTION_CONSTRAINED_SLICE_GROUP_SET : sei_type_name = "Motion constrained slice group set"; break;
        case SEI_TYPE_FILM_GRAIN_CHARACTERISTICS : sei_type_name = "Film grain characteristics"; break;
        case SEI_TYPE_DEBLOCKING_FILTER_DISPLAY_PREFERENCE : sei_type_name = "Deblocking filter display preference"; break;
        case SEI_TYPE_STEREO_VIDEO_INFO :         sei_type_name = "Stereo video info"; break;
        default: sei_type_name = "Unknown"; break;
        }
        my_printf("=== %s ===", sei_type_name);
        my_printf(" payloadType : %d", s->payloadType );
        my_printf(" payloadSize : %d", s->payloadSize );

        my_printf(" payload : " );
        unsigned char* p = s->payload;
        while (*p > 0x7f || *p < 0x20)
        {
            p++;
        }
        //my_printf("%s", s->payload);
        my_printf("%s", p);
        //debug_bytes(s->payload, s->payloadSize);
        //for (i = 0; i < s->payloadSize; i++)
        //{
        //    my_printf("%c ", s->payload[i]);
        //    //if ((i+1) % 16 == 0) { my_printf ("\n"); }
        //}
    }
}

/**
 Print the contents of a NAL unit to standard output.
 The NAL which is printed out has a type determined by nal and data which comes from other fields within h depending on its type.
 @param[in]      h          the stream object
 @param[in]      nal        the nal unit
 */
static void h264_debug_nal(h264_stream_t* h, nal_t* nal)
{
    my_printf("==================== NAL ====================");
    my_printf(" forbidden_zero_bit : %d", nal->forbidden_zero_bit );
    my_printf(" nal_ref_idc : %d", nal->nal_ref_idc );
    // TODO make into subroutine
    const char* nal_unit_type_name;
    switch (nal->nal_unit_type)
    {
    case  NAL_UNIT_TYPE_UNSPECIFIED :                   nal_unit_type_name = "Unspecified"; break;
    case  NAL_UNIT_TYPE_CODED_SLICE_NON_IDR :           nal_unit_type_name = "Coded slice of a non-IDR picture"; break;
    case  NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_A :  nal_unit_type_name = "Coded slice data partition A"; break;
    case  NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_B :  nal_unit_type_name = "Coded slice data partition B"; break;
    case  NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_C :  nal_unit_type_name = "Coded slice data partition C"; break;
    case  NAL_UNIT_TYPE_CODED_SLICE_IDR :               nal_unit_type_name = "Coded slice of an IDR picture"; break;
    case  NAL_UNIT_TYPE_SEI :                           nal_unit_type_name = "Supplemental enhancement information (SEI)"; break;
    case  NAL_UNIT_TYPE_SPS :                           nal_unit_type_name = "Sequence parameter set"; break;
    case  NAL_UNIT_TYPE_PPS :                           nal_unit_type_name = "Picture parameter set"; break;
    case  NAL_UNIT_TYPE_AUD :                           nal_unit_type_name = "Access unit delimiter"; break;
    case  NAL_UNIT_TYPE_END_OF_SEQUENCE :               nal_unit_type_name = "End of sequence"; break;
    case  NAL_UNIT_TYPE_END_OF_STREAM :                 nal_unit_type_name = "End of stream"; break;
    case  NAL_UNIT_TYPE_FILLER :                        nal_unit_type_name = "Filler data"; break;
    case  NAL_UNIT_TYPE_SPS_EXT :                       nal_unit_type_name = "Sequence parameter set extension"; break;
        // 14..18    // Reserved
    case  NAL_UNIT_TYPE_CODED_SLICE_AUX :               nal_unit_type_name = "Coded slice of an auxiliary coded picture without partitioning"; break;
        // 20..23    // Reserved
        // 24..31    // Unspecified
    default :                                           nal_unit_type_name = "Unknown"; break;
    }
    my_printf(" nal_unit_type : %d ( %s )", nal->nal_unit_type, nal_unit_type_name );

    if( nal->nal_unit_type == NAL_UNIT_TYPE_CODED_SLICE_NON_IDR) { h264_debug_slice_header(h->sh); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_CODED_SLICE_IDR) { h264_debug_slice_header(h->sh); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_SPS) { h264_debug_sps(h->sps); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_PPS) { h264_debug_pps(h->pps); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_AUD) { h264_debug_aud(h->aud); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_SEI) { h264_debug_seis( h ); }
}

static void debug_bytes(uint8_t* buf, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        my_printf("%02X ", buf[i]);
        if ((i+1) % 16 == 0) { my_printf ("\n"); }
    }
    my_printf("\n");
}

////////////////////////////////////////////////////////
static void h265_debug_ptl(profile_tier_level_t* ptl, int profilePresentFlag, int max_sub_layers_minus1)
{
    if (profilePresentFlag)
    {
        my_printf(" general_profile_space: %d", ptl->general_profile_space);
        my_printf(" general_tier_flag: %d", ptl->general_tier_flag);
        my_printf(" general_profile_idc: %d", ptl->general_profile_idc);
        for (int i = 0; i < 32; i++)
        {
            my_printf(" general_profile_compatibility_flag[%d]: %d", i, ptl->general_profile_compatibility_flag[i]);
        }
        my_printf(" general_progressive_source_flag: %d", ptl->general_progressive_source_flag);
        my_printf(" general_interlaced_source_flag: %d", ptl->general_interlaced_source_flag);
        my_printf(" general_non_packed_constraint_flag: %d", ptl->general_non_packed_constraint_flag);
        my_printf(" general_frame_only_constraint_flag: %d", ptl->general_frame_only_constraint_flag);
        if (ptl->general_profile_idc==4 || ptl->general_profile_compatibility_flag[4] ||
            ptl->general_profile_idc==5 || ptl->general_profile_compatibility_flag[5] ||
            ptl->general_profile_idc==6 || ptl->general_profile_compatibility_flag[6] ||
            ptl->general_profile_idc==7 || ptl->general_profile_compatibility_flag[7])
        {
            my_printf(" general_max_12bit_constraint_flag: %d", ptl->general_max_12bit_constraint_flag);
            my_printf(" general_max_10bit_constraint_flag: %d", ptl->general_max_10bit_constraint_flag);
            my_printf(" general_max_8bit_constraint_flag: %d", ptl->general_max_8bit_constraint_flag);
            my_printf(" general_max_422chroma_constraint_flag: %d", ptl->general_max_422chroma_constraint_flag);
            my_printf(" general_max_420chroma_constraint_flag: %d", ptl->general_max_420chroma_constraint_flag);
            my_printf(" general_max_monochrome_constraint_flag: %d", ptl->general_max_monochrome_constraint_flag);
            my_printf(" general_intra_constraint_flag: %d", ptl->general_intra_constraint_flag);
            my_printf(" general_one_picture_only_constraint_flag: %d", ptl->general_one_picture_only_constraint_flag);
            my_printf(" general_lower_bit_rate_constraint_flag: %d", ptl->general_lower_bit_rate_constraint_flag);
            my_printf(" general_reserved_zero_34bits: %u", ptl->general_reserved_zero_34bits);// tocheck
        }
        else
        {
            my_printf(" general_reserved_zero_43bits: %u", ptl->general_reserved_zero_43bits);// tocheck
        }
        if ((ptl->general_profile_idc>=1 && ptl->general_profile_idc<=5) ||
            ptl->general_profile_compatibility_flag[1] || ptl->general_profile_compatibility_flag[2] ||
            ptl->general_profile_compatibility_flag[3] || ptl->general_profile_compatibility_flag[4] ||
            ptl->general_profile_compatibility_flag[5])
        {
            my_printf(" general_inbld_flag: %d", ptl->general_inbld_flag);
        }
        else
        {
            my_printf(" general_reserved_zero_bit: %d", ptl->general_reserved_zero_bit);
        }
    }
        
    my_printf(" general_level_idc: %d", ptl->general_level_idc);
    for (int i = 0; i < max_sub_layers_minus1; i++)
    {
        my_printf(" sub_layer_profile_present_flag[%d]: %d", i, ptl->sub_layer_profile_present_flag[i]);
        my_printf(" sub_layer_level_present_flag[%d]: %d", i, ptl->sub_layer_level_present_flag[i]);
    }
    if (max_sub_layers_minus1 > 0)
    {
        for (int i = max_sub_layers_minus1; i < 8; i++)
        {
            my_printf(" reserved_zero_2bits[%d]: %d", i, ptl->reserved_zero_2bits[i]);
        }
    }
    for (int i = 0; i < max_sub_layers_minus1; i++)
    {
        if (ptl->sub_layer_profile_present_flag[i])
        {
            my_printf("  sub_layer_profile_space[%d]: %d", i, ptl->sub_layer_profile_space[i]);
            my_printf("  sub_layer_tier_flag[%d]: %d", i, ptl->sub_layer_tier_flag[i]);
            my_printf("  sub_layer_profile_idc[%d]: %d", i, ptl->sub_layer_profile_idc[i]);
            for (int j = 0; j < 32; j++)
            {
                my_printf("  sub_layer_profile_compatibility_flag[%d][%d]: %d", i, j, ptl->sub_layer_profile_compatibility_flag[i][j]);
            }
            my_printf("  sub_layer_progressive_source_flag[%d]: %d", i, ptl->sub_layer_progressive_source_flag[i]);
            my_printf("  sub_layer_interlaced_source_flag[%d]: %d", i, ptl->sub_layer_interlaced_source_flag[i]);
            my_printf("  sub_layer_non_packed_constraint_flag[%d]: %d", i, ptl->sub_layer_non_packed_constraint_flag[i]);
            my_printf("  sub_layer_frame_only_constraint_flag[%d]: %d", i, ptl->sub_layer_frame_only_constraint_flag[i]);
            if (ptl->sub_layer_profile_idc[i]==4 || ptl->sub_layer_profile_compatibility_flag[i][4] ||
                ptl->sub_layer_profile_idc[i]==5 || ptl->sub_layer_profile_compatibility_flag[i][5] ||
                ptl->sub_layer_profile_idc[i]==6 || ptl->sub_layer_profile_compatibility_flag[i][6] ||
                ptl->sub_layer_profile_idc[i]==7 || ptl->sub_layer_profile_compatibility_flag[i][7])
            {
                my_printf("  sub_layer_max_12bit_constraint_flag[%d]: %d", i, ptl->sub_layer_max_12bit_constraint_flag[i]);
                my_printf("  sub_layer_max_10bit_constraint_flag[%d]: %d", i, ptl->sub_layer_max_10bit_constraint_flag[i]);
                my_printf("  sub_layer_max_8bit_constraint_flag[%d]: %d", i, ptl->sub_layer_max_8bit_constraint_flag[i]);
                my_printf("  sub_layer_max_422chroma_constraint_flag[%d]: %d", i, ptl->sub_layer_max_422chroma_constraint_flag[i]);
                my_printf("  sub_layer_max_420chroma_constraint_flag[%d]: %d", i, ptl->sub_layer_max_420chroma_constraint_flag[i]);
                my_printf("  sub_layer_max_monochrome_constraint_flag[%d]: %d", i, ptl->sub_layer_max_monochrome_constraint_flag[i]);
                my_printf("  sub_layer_intra_constraint_flag[%d]: %d", i, ptl->sub_layer_intra_constraint_flag[i]);
                my_printf("  sub_layer_one_picture_only_constraint_flag[%d]: %d", i, ptl->sub_layer_one_picture_only_constraint_flag[i]);
                my_printf("  sub_layer_lower_bit_rate_constraint_flag[%d]: %d", i, ptl->sub_layer_lower_bit_rate_constraint_flag[i]);
                my_printf("  sub_layer_reserved_zero_34bits[%d]: %ul", i, ptl->sub_layer_reserved_zero_34bits[i]);// todo
            }
            else
            {
                my_printf("  sub_layer_reserved_zero_43bits: %ul", ptl->sub_layer_reserved_zero_43bits);// todo
            }
            // to check
            if ((ptl->sub_layer_profile_idc[i]>=1 && ptl->sub_layer_profile_idc[i]<=5) ||
                ptl->sub_layer_profile_compatibility_flag[i][1] ||
                ptl->sub_layer_profile_compatibility_flag[i][2] ||
                ptl->sub_layer_profile_compatibility_flag[i][3] ||
                ptl->sub_layer_profile_compatibility_flag[i][4] ||
                ptl->sub_layer_profile_compatibility_flag[i][5])
            {
                my_printf("  sub_layer_inbld_flag[%d]: %d", i, ptl->sub_layer_inbld_flag[i]);
            }
            else
            {
                my_printf("  sub_layer_reserved_zero_bit[%d]: %d", i, ptl->sub_layer_reserved_zero_bit[i]);
            }
        }
        if (ptl->sub_layer_level_present_flag[i])
        {
            my_printf("  sub_layer_level_idc[%d]: %d", i, ptl->sub_layer_level_idc[i]);
        }
    }
    
}

void h265_debug_sub_layer_hrd_parameters(sub_layer_hrd_parameters_t* subhrd, int sub_pic_hrd_params_present_flag, int CpbCnt, const char* p)
{
    my_printf("  [%s] sub_layer_hrd_parameters(%d)", p);
    for (int i = 0; i <= CpbCnt; i++)
    {
        my_printf("   bit_rate_value_minus1[%d]: %d", i, subhrd->bit_rate_value_minus1[i]);
        my_printf("   cpb_size_value_minus1[%d]: %d", i, subhrd->cpb_size_value_minus1[i]);
        if (sub_pic_hrd_params_present_flag)
        {
            my_printf("    cpb_size_du_value_minus1[%d]: %d", i, subhrd->cpb_size_du_value_minus1[i]);
            my_printf("    bit_rate_du_value_minus1[%d]: %d", i, subhrd->bit_rate_du_value_minus1[i]);
        }
        my_printf("   cbr_flag[%d]: %d", i, subhrd->cbr_flag[i]);
    }
}
static void h265_debug_hrd_parameters(hrd_parameters_t* hrd, int commonInfPresentFlag, int maxNumSubLayersMinus1)
{
    if(commonInfPresentFlag)
    {
        my_printf("  nal_hrd_parameters_present_flag: %d", hrd->nal_hrd_parameters_present_flag);
        my_printf("  vcl_hrd_parameters_present_flag: %d", hrd->vcl_hrd_parameters_present_flag);
        if (hrd->nal_hrd_parameters_present_flag ||
            hrd->vcl_hrd_parameters_present_flag)
        {
            
            my_printf("   sub_pic_hrd_params_present_flag: %d", hrd->sub_pic_hrd_params_present_flag);
            if (hrd->sub_pic_hrd_params_present_flag)
            {
                my_printf("    tick_divisor_minus2: %d", hrd->tick_divisor_minus2);
                my_printf("    du_cpb_removal_delay_increment_length_minus1: %d", hrd->du_cpb_removal_delay_increment_length_minus1);
                my_printf("    sub_pic_cpb_params_in_pic_timing_sei_flag: %d", hrd->sub_pic_cpb_params_in_pic_timing_sei_flag);
                my_printf("    dpb_output_delay_du_length_minus1: %d", hrd->dpb_output_delay_du_length_minus1);
            }
            my_printf("   bit_rate_scale: %d", hrd->bit_rate_scale);
            my_printf("   cpb_size_scale: %d", hrd->cpb_size_scale);
            if (hrd->sub_pic_hrd_params_present_flag)
                my_printf("    cpb_size_du_scale: %d", hrd->cpb_size_du_scale);
            my_printf("   initial_cpb_removal_delay_length_minus1: %d", hrd->initial_cpb_removal_delay_length_minus1);
            my_printf("   au_cpb_removal_delay_length_minus1: %d", hrd->au_cpb_removal_delay_length_minus1);
            my_printf("   dpb_output_delay_length_minus1: %d", hrd->dpb_output_delay_length_minus1);
        }
    }
    for (int i = 0; i <= maxNumSubLayersMinus1; i++)
    {
        my_printf("  fixed_pic_rate_general_flag[%d]: %d", i, hrd->fixed_pic_rate_general_flag[i]);
        if (!hrd->fixed_pic_rate_general_flag[i])
            my_printf("  fixed_pic_rate_general_flag[%d]: %d", i, hrd->fixed_pic_rate_general_flag[i]);
        if (hrd->fixed_pic_rate_within_cvs_flag[i])
        {
            my_printf("   elemental_duration_in_tc_minus1[%d]: %d", i, hrd->elemental_duration_in_tc_minus1[i]);
        }
        else
        {
            my_printf("   low_delay_hrd_flag[%d]: %d", i, hrd->low_delay_hrd_flag[i]);
        }
        if (!hrd->low_delay_hrd_flag[i])
            my_printf("   cpb_cnt_minus1[%d]: %d", i, hrd->cpb_cnt_minus1[i]);
            
        if(hrd->nal_hrd_parameters_present_flag)
        {
            h265_debug_sub_layer_hrd_parameters(&(hrd->sub_layer_hrd_parameters), hrd->sub_pic_hrd_params_present_flag, hrd->cpb_cnt_minus1[i], "nal");
        }
        if(hrd->vcl_hrd_parameters_present_flag)
        {
            h265_debug_sub_layer_hrd_parameters(&(hrd->sub_layer_hrd_parameters), hrd->sub_pic_hrd_params_present_flag, hrd->cpb_cnt_minus1[i], "vcl");
        }
    }
}
// vps
static void h265_debug_vps(h265_vps_t* vps)
{
    int i, j;
    my_printf("======= HEVC VPS =======");
    my_printf("vps_video_parameter_set_id: %d", vps->vps_video_parameter_set_id);
    my_printf("vps_base_layer_internal_flag: %d", vps->vps_base_layer_internal_flag);
    my_printf("vps_base_layer_available_flag: %d", vps->vps_base_layer_available_flag);
    my_printf("vps_max_layers_minus1: %d", vps->vps_max_layers_minus1);
    my_printf("vps_max_sub_layers_minus1: %d", vps->vps_max_sub_layers_minus1);
    my_printf("vps_temporal_id_nesting_flag: %d", vps->vps_temporal_id_nesting_flag);
    my_printf("vps_reserved_0xffff_16bits: %d", vps->vps_reserved_0xffff_16bits);
    // ptl
    my_printf("profile_tier_level()");
    h265_debug_ptl(&vps->ptl, 1, vps->vps_max_layers_minus1);
    
    my_printf("vps_sub_layer_ordering_info_present_flag: %d", vps->vps_sub_layer_ordering_info_present_flag);
    my_printf("SubLayers");
    for (i = (vps->vps_sub_layer_ordering_info_present_flag ? 0 : vps->vps_max_sub_layers_minus1);
        i <= vps->vps_max_sub_layers_minus1; i++ )
    {
        my_printf(" vps_max_dec_pic_buffering_minus1[%d]: %d", i, vps->vps_max_dec_pic_buffering_minus1[i]);
        my_printf(" vps_max_num_reorder_pics[%d]: %d", i, vps->vps_max_num_reorder_pics[i]);
        my_printf(" vps_max_latency_increase_plus1[%d]: %d", i, vps->vps_max_latency_increase_plus1[i]);
    }
    my_printf("vps_max_layer_id: %d", vps->vps_max_layer_id);
    my_printf("vps_num_layer_sets_minus1: %d", vps->vps_num_layer_sets_minus1);
    for (i = 1; i <= vps->vps_num_layer_sets_minus1; i++)
    {
        for (j = 0; j <= vps->vps_max_layer_id; j++)
        {
            my_printf(" layer_id_included_flag[%d][%d]: %d", i, j, vps->layer_id_included_flag[i][j]);
        }
    }
    my_printf("vps_timing_info_present_flag: %d", vps->vps_timing_info_present_flag);
    if (vps->vps_timing_info_present_flag)
    {
        my_printf(" vps_num_units_in_tick: %d", vps->vps_num_units_in_tick);
        my_printf(" vps_time_scale: %d", vps->vps_time_scale);
        my_printf(" vps_poc_proportional_to_timing_flag: %d", vps->vps_poc_proportional_to_timing_flag);
        if (vps->vps_poc_proportional_to_timing_flag)
        {
            my_printf("  vps_num_ticks_poc_diff_one_minus1: %d", vps->vps_num_ticks_poc_diff_one_minus1);
        }
        for (i = 0; i < vps->vps_num_hrd_parameters; i++)
        {
            my_printf("  hrd_layer_set_idx[%d]: %d", i, vps->hrd_layer_set_idx[i]);
            if (i > 0)
            {
                my_printf("   cprms_present_flag[%d]: %d", i, vps->cprms_present_flag[i]);
            }
            //  hrd_parameters()
            h265_debug_hrd_parameters(&(vps->hrd_parameters), vps->cprms_present_flag[i], vps->vps_max_sub_layers_minus1);
        }
    }
    my_printf("vps_extension_flag: %d", vps->vps_extension_flag);
    if (vps->vps_extension_flag)
    {
        // do nothing...
    }
}

static void h265_debug_scaling_list(scaling_list_data_t* sld)
{
    for(int sizeId = 0; sizeId < 4; sizeId++)
    {
        for(int matrixId = 0; matrixId < 6; matrixId += ( sizeId == 3 ) ? 3 : 1)
        {
            my_printf("  scaling_list_pred_mode_flag[%d][%d]: %d", sizeId, matrixId, sld->scaling_list_pred_mode_flag[sizeId][matrixId]);
            if (!sld->scaling_list_pred_mode_flag[sizeId][matrixId])
            {
                my_printf("   scaling_list_pred_mode_flag[%d][%d]: %d", sizeId, matrixId, sld->scaling_list_pred_matrix_id_delta[sizeId][matrixId]);
            }
            else
            {
                if (sizeId > 1)
                {
                    my_printf("   scaling_list_dc_coef_minus8[%d][%d]: %d", sizeId, matrixId, sld->scaling_list_dc_coef_minus8[sizeId - 2][matrixId]);
                }
                for (int i = 0; i < sld->coefNum; i++)
                {

                    my_printf("   ScalingList[%d][%d][%d]: %d", sizeId, matrixId, i, sld->ScalingList[sizeId][matrixId][i]);
                }
            }
        }
    }
}
void h265_debug_short_term_ref_pic_set(h265_sps_t* sps, st_ref_pic_set_t*st, referencePictureSets_t* rps, int stRpsIdx)
{
    my_printf(" st_ref_pic_set[%d]", stRpsIdx);

    my_printf(" inter_ref_pic_set_prediction_flag: %d", st->inter_ref_pic_set_prediction_flag);
    if (st->inter_ref_pic_set_prediction_flag)
    {
        my_printf("  delta_idx_minus1: %d", st->delta_idx_minus1);
        my_printf("  delta_rps_sign: %d", st->delta_rps_sign);
        my_printf("  abs_delta_rps_minus1: %d", st->abs_delta_rps_minus1);
        int rIdx = stRpsIdx - 1 - st->delta_idx_minus1;
        referencePictureSets_t* rpsRef = &sps->m_RPSList[rIdx];
        for (int j = 0; j <= rpsRef->m_numberOfPictures; j++)
        {
            my_printf("  used_by_curr_pic_flag[%d]: %d", j, st->used_by_curr_pic_flag[j]);
            if (!st->used_by_curr_pic_flag[j])
            {
                my_printf("  use_delta_flag[%d]: %d", j, st->use_delta_flag[j]);
            }
        }
    }
    else
    {
        my_printf("  num_negative_pics: %d", st->num_negative_pics);
        my_printf("  num_positive_pics: %d", st->num_positive_pics);
        for (int i = 0; i < st->num_negative_pics; i++)
        {
            my_printf("  delta_poc_s0_minus1[%d]: %d", i, st->delta_poc_s0_minus1[i]);
            my_printf("  used_by_curr_pic_s0_flag[%d]: %d", i, st->used_by_curr_pic_s0_flag[i]);
        }
        for (int i = 0; i < st->num_positive_pics; i++)
        {
            my_printf("  delta_poc_s1_minus1[%d]: %d", i, st->delta_poc_s1_minus1[i]);
            my_printf("  used_by_curr_pic_s1_flag[%d]: %d", i, st->used_by_curr_pic_s1_flag[i]);
        }
    }
}
static void h265_debug_vui_parameters(vui_parameters_t* vui, int maxNumSubLayersMinus1)
{
    my_printf(" aspect_ratio_info_present_flag: %d", vui->aspect_ratio_info_present_flag);
    if (vui->aspect_ratio_info_present_flag)
    {
        my_printf(" aspect_ratio_idc: %d", vui->aspect_ratio_idc);
        if (vui->aspect_ratio_idc == H265_SAR_Extended)
        {
            my_printf("  sar_width: %d", vui->sar_width);
            my_printf("  sar_height: %d", vui->sar_height);
        }
    }
    my_printf(" overscan_info_present_flag: %d", vui->overscan_info_present_flag);
    if (vui->overscan_info_present_flag)
    {
        my_printf(" overscan_appropriate_flag: %d", vui->overscan_appropriate_flag);
    }
    my_printf(" video_signal_type_present_flag: %d", vui->video_signal_type_present_flag);
    if (vui->video_signal_type_present_flag)
    {
        my_printf("  video_format: %d", vui->video_format);
        my_printf("  video_full_range_flag: %d", vui->video_full_range_flag);
        my_printf("  colour_description_present_flag: %d", vui->colour_description_present_flag);
        if (vui->colour_description_present_flag)
        {
            my_printf("   colour_primaries: %d", vui->colour_primaries);
            my_printf("   transfer_characteristics: %d", vui->transfer_characteristics);
            my_printf("   matrix_coeffs: %d", vui->matrix_coeffs);
        }
    }
    my_printf(" chroma_loc_info_present_flag: %d", vui->chroma_loc_info_present_flag);

    if (vui->chroma_loc_info_present_flag)
    {
        my_printf("  chroma_sample_loc_type_top_field: %d", vui->chroma_sample_loc_type_top_field);
        my_printf("  chroma_sample_loc_type_bottom_field: %d", vui->chroma_sample_loc_type_bottom_field);
    }
    my_printf(" neutral_chroma_indication_flag: %d", vui->neutral_chroma_indication_flag);
    my_printf(" field_seq_flag: %d", vui->field_seq_flag);
    my_printf(" frame_field_info_present_flag: %d", vui->frame_field_info_present_flag);
    my_printf(" default_display_window_flag: %d", vui->default_display_window_flag);
    if (vui->default_display_window_flag)
    {
        my_printf("  def_disp_win_left_offset: %d", vui->def_disp_win_left_offset);
        my_printf("  def_disp_win_left_offset: %d", vui->def_disp_win_left_offset);
        my_printf("  def_disp_win_right_offset: %d", vui->def_disp_win_right_offset);
        my_printf("  def_disp_win_bottom_offset: %d", vui->def_disp_win_bottom_offset);
    }
    my_printf(" vui_timing_info_present_flag: %d", vui->vui_timing_info_present_flag);
    if (vui->vui_timing_info_present_flag)
    {
        my_printf("  vui_num_units_in_tick: %d", vui->vui_num_units_in_tick);
        my_printf("  vui_time_scale: %d", vui->vui_time_scale);
        my_printf("  vui_poc_proportional_to_timing_flag: %d", vui->vui_poc_proportional_to_timing_flag);
        if (vui->vui_poc_proportional_to_timing_flag)
        {
            my_printf("   vui_num_ticks_poc_diff_one_minus1: %d", vui->vui_num_ticks_poc_diff_one_minus1);
        }
        my_printf("  vui_hrd_parameters_present_flag: %d", vui->vui_hrd_parameters_present_flag);
        if (vui->vui_hrd_parameters_present_flag)
        {
            h265_debug_hrd_parameters(&vui->hrd_parameters, 1, maxNumSubLayersMinus1);
        }
    }
    my_printf(" bitstream_restriction_flag: %d", vui->bitstream_restriction_flag);
    if (vui->bitstream_restriction_flag)
    {
        my_printf("  tiles_fixed_structure_flag: %d", vui->tiles_fixed_structure_flag);
        my_printf("  motion_vectors_over_pic_boundaries_flag: %d", vui->motion_vectors_over_pic_boundaries_flag);
        my_printf("  restricted_ref_pic_lists_flag: %d", vui->restricted_ref_pic_lists_flag);
        my_printf("  min_spatial_segmentation_idc: %d", vui->min_spatial_segmentation_idc);
        my_printf("  max_bytes_per_pic_denom: %d", vui->max_bytes_per_pic_denom);
        my_printf("  max_bits_per_min_cu_denom: %d", vui->max_bits_per_min_cu_denom);
        my_printf("  log2_max_mv_length_horizontal: %d", vui->log2_max_mv_length_horizontal);
        my_printf("  log2_max_mv_length_vertical: %d", vui->bitstream_restriction_flag);
    }
}
// sps
static void h265_debug_sps(h265_sps_t* sps)
{
    my_printf("======= HEVC SPS =======");
    my_printf("sps_video_parameter_set_id: %d", sps->sps_video_parameter_set_id);
    my_printf("sps_max_sub_layers_minus1: %d", sps->sps_max_sub_layers_minus1);
    my_printf("sps_temporal_id_nesting_flag: %d", sps->sps_temporal_id_nesting_flag);
    // ptl
    my_printf("profile_tier_level()");
    h265_debug_ptl(&sps->ptl, 1, sps->sps_max_sub_layers_minus1);

    my_printf("sps_seq_parameter_set_id: %d", sps->sps_seq_parameter_set_id);
    my_printf("chroma_format_idc: %d", sps->chroma_format_idc);
    if (sps->chroma_format_idc == 3)
    {
        my_printf(" separate_colour_plane_flag: %d", sps->separate_colour_plane_flag);
    }
    my_printf("pic_width_in_luma_samples: %d", sps->pic_width_in_luma_samples);
    my_printf("pic_height_in_luma_samples: %d", sps->pic_height_in_luma_samples);
    my_printf("conformance_window_flag: %d", sps->conformance_window_flag);
    if (sps->conformance_window_flag)
    {
        my_printf(" conf_win_left_offset: %d", sps->conf_win_left_offset);
        my_printf(" conf_win_right_offset: %d", sps->conf_win_right_offset);
        my_printf(" conf_win_top_offset: %d", sps->conf_win_top_offset);
        my_printf(" conf_win_bottom_offset: %d", sps->conf_win_bottom_offset);
    }
    my_printf("bit_depth_luma_minus8: %d", sps->bit_depth_luma_minus8);
    my_printf("bit_depth_chroma_minus8: %d", sps->bit_depth_chroma_minus8);
    my_printf("log2_max_pic_order_cnt_lsb_minus4: %d", sps->log2_max_pic_order_cnt_lsb_minus4);
    my_printf("sps_sub_layer_ordering_info_present_flag: %d", sps->sps_sub_layer_ordering_info_present_flag);
    for (int i = (sps->sps_sub_layer_ordering_info_present_flag ? 0 : sps->sps_max_sub_layers_minus1);
        i <= sps->sps_max_sub_layers_minus1; i++ )
    {
        my_printf(" sps_max_dec_pic_buffering_minus1[%d]: %d", i, sps->sps_max_dec_pic_buffering_minus1[i]);
        my_printf(" sps_max_num_reorder_pics[%d]: %d", i, sps->sps_max_num_reorder_pics[i]);
        my_printf(" sps_max_latency_increase_plus1[%d]: %d", i, sps->sps_max_latency_increase_plus1[i]);
    }
    my_printf("log2_min_luma_coding_block_size_minus3: %d", sps->log2_min_luma_coding_block_size_minus3);
    my_printf("log2_diff_max_min_luma_coding_block_size: %d", sps->log2_diff_max_min_luma_coding_block_size);
    my_printf("log2_min_luma_transform_block_size_minus2: %d", sps->log2_min_luma_transform_block_size_minus2);
    my_printf("log2_diff_max_min_luma_transform_block_size: %d", sps->log2_diff_max_min_luma_transform_block_size);
    my_printf("max_transform_hierarchy_depth_inter: %d", sps->max_transform_hierarchy_depth_inter);
    my_printf("max_transform_hierarchy_depth_intra: %d", sps->max_transform_hierarchy_depth_intra);
    my_printf("scaling_list_enabled_flag: %d", sps->scaling_list_enabled_flag);
    if (sps->scaling_list_enabled_flag)
    {
        my_printf(" sps_scaling_list_data_present_flag: %d", sps->sps_scaling_list_data_present_flag);
        {
            if (sps->sps_scaling_list_data_present_flag)
            {
                h265_debug_scaling_list(&sps->scaling_list_data);
            }
        }
    }

    my_printf("amp_enabled_flag: %d", sps->amp_enabled_flag);
    my_printf("sample_adaptive_offset_enabled_flag: %d", sps->sample_adaptive_offset_enabled_flag);
    my_printf("pcm_enabled_flag: %d", sps->pcm_enabled_flag);
    if (sps->pcm_enabled_flag)
    {
        my_printf(" pcm_sample_bit_depth_luma_minus1: %d", sps->pcm_sample_bit_depth_luma_minus1);
        my_printf(" pcm_sample_bit_depth_chroma_minus1: %d", sps->pcm_sample_bit_depth_chroma_minus1);
        my_printf(" log2_min_pcm_luma_coding_block_size_minus3: %d", sps->log2_min_pcm_luma_coding_block_size_minus3);
        my_printf(" log2_diff_max_min_pcm_luma_coding_block_size: %d", sps->log2_diff_max_min_pcm_luma_coding_block_size);
        my_printf(" pcm_loop_filter_disabled_flag: %d", sps->pcm_loop_filter_disabled_flag);
    }
    my_printf("num_short_term_ref_pic_sets: %d", sps->num_short_term_ref_pic_sets);
    referencePictureSets_t* rps = NULL;
    st_ref_pic_set_t* st = NULL;
    for (int i = 0; i < sps->num_short_term_ref_pic_sets; i++)
    {
        st = &sps->st_ref_pic_set[i];
        rps = &sps->m_RPSList[i];
        h265_debug_short_term_ref_pic_set(sps, st, rps, i);
    }
    my_printf("long_term_ref_pics_present_flag: %d", sps->long_term_ref_pics_present_flag);
    if (sps->long_term_ref_pics_present_flag)
    {
        my_printf(" num_long_term_ref_pics_sps: %d", sps->num_long_term_ref_pics_sps);
        for (int i = 0; i < sps->num_long_term_ref_pics_sps; i++)
        {
            my_printf(" lt_ref_pic_poc_lsb_sps[%d]: %d", i, sps->lt_ref_pic_poc_lsb_sps[i]);
            my_printf(" used_by_curr_pic_lt_sps_flag[%d]: %d", i, sps->used_by_curr_pic_lt_sps_flag[i]);
        }
    }
    my_printf("sps_temporal_mvp_enabled_flag: %d", sps->sps_temporal_mvp_enabled_flag);
    my_printf("strong_intra_smoothing_enabled_flag: %d", sps->strong_intra_smoothing_enabled_flag);
    my_printf("vui_parameters_present_flag: %d", sps->vui_parameters_present_flag);
    if (sps->vui_parameters_present_flag)
    {
        // vui
        h265_debug_vui_parameters(&sps->vui_parameters, sps->sps_max_sub_layers_minus1);
    }
    my_printf("sps_extension_present_flag: %d", sps->sps_extension_present_flag);
    if (sps->sps_extension_present_flag)
    {
        my_printf(" sps_range_extension_flag: %d", sps->sps_range_extension_flag);
        my_printf(" sps_multilayer_extension_flag: %d", sps->sps_multilayer_extension_flag);
        my_printf(" sps_3d_extension_flag: %d", sps->sps_3d_extension_flag);
        my_printf(" sps_extension_5bits: %d", sps->sps_extension_5bits);
    }
    if (sps->sps_range_extension_flag)
    {
        my_printf(" transform_skip_rotation_enabled_flag: %d", sps->sps_range_extension.transform_skip_rotation_enabled_flag);
        my_printf(" transform_skip_context_enabled_flag: %d", sps->sps_range_extension.transform_skip_context_enabled_flag);
        my_printf(" implicit_rdpcm_enabled_flag: %d", sps->sps_range_extension.implicit_rdpcm_enabled_flag);
        my_printf(" explicit_rdpcm_enabled_flag: %d", sps->sps_range_extension.explicit_rdpcm_enabled_flag);
        my_printf(" extended_precision_processing_flag: %d", sps->sps_range_extension.extended_precision_processing_flag);
        my_printf(" intra_smoothing_disabled_flag: %d", sps->sps_range_extension.intra_smoothing_disabled_flag);
        my_printf(" high_precision_offsets_enabled_flag: %d", sps->sps_range_extension.high_precision_offsets_enabled_flag);
        my_printf(" persistent_rice_adaptation_enabled_flag: %d", sps->sps_range_extension.persistent_rice_adaptation_enabled_flag);
        my_printf(" cabac_bypass_alignment_enabled_flag: %d", sps->sps_range_extension.cabac_bypass_alignment_enabled_flag);
    }
    if (sps->sps_multilayer_extension_flag)
    {
        my_printf(" inter_view_mv_vert_constraint_flag: %d", sps->inter_view_mv_vert_constraint_flag);
    }
    // todo sps_3d_extension_flag

}

// pps
static void h265_debug_pps(h265_pps_t* pps)
{
    my_printf("======= HEVC PPS =======");
    my_printf("pps_pic_parameter_set_id: %d", pps->pps_pic_parameter_set_id);
    my_printf("pps_seq_parameter_set_id: %d", pps->pps_seq_parameter_set_id);
    my_printf("dependent_slice_segments_enabled_flag: %d", pps->dependent_slice_segments_enabled_flag);
    my_printf("output_flag_present_flag: %d", pps->output_flag_present_flag);
    my_printf("num_extra_slice_header_bits: %d", pps->num_extra_slice_header_bits);
    my_printf("sign_data_hiding_enabled_flag: %d", pps->sign_data_hiding_enabled_flag);
    my_printf("cabac_init_present_flag: %d", pps->cabac_init_present_flag);
    my_printf("num_ref_idx_l0_default_active_minus1: %d", pps->num_ref_idx_l0_default_active_minus1);
    my_printf("num_ref_idx_l1_default_active_minus1: %d", pps->num_ref_idx_l1_default_active_minus1);
    my_printf("init_qp_minus26: %d", pps->init_qp_minus26);
    my_printf("constrained_intra_pred_flag: %d", pps->constrained_intra_pred_flag);
    my_printf("transform_skip_enabled_flag: %d", pps->transform_skip_enabled_flag);
    my_printf("cu_qp_delta_enabled_flag: %d", pps->cu_qp_delta_enabled_flag);
    if (pps->cu_qp_delta_enabled_flag)
        my_printf("diff_cu_qp_delta_depth: %d", pps->diff_cu_qp_delta_depth);
    my_printf("pps_cb_qp_offset: %d", pps->pps_cb_qp_offset);
    my_printf("pps_cr_qp_offset: %d", pps->pps_cr_qp_offset);
    my_printf("pps_slice_chroma_qp_offsets_present_flag: %d", pps->pps_slice_chroma_qp_offsets_present_flag);
    my_printf("weighted_pred_flag: %d", pps->weighted_pred_flag);
    my_printf("weighted_bipred_flag: %d", pps->weighted_bipred_flag);
    my_printf("transquant_bypass_enabled_flag: %d", pps->transquant_bypass_enabled_flag);
    my_printf("tiles_enabled_flag: %d", pps->tiles_enabled_flag);
    my_printf("entropy_coding_sync_enabled_flag: %d", pps->entropy_coding_sync_enabled_flag);
    if (pps->tiles_enabled_flag)
    {
        my_printf("num_tile_columns_minus1: %d", pps->num_tile_columns_minus1);
        my_printf("num_tile_rows_minus1: %d", pps->num_tile_rows_minus1);
        my_printf("uniform_spacing_flag: %d", pps->uniform_spacing_flag);
        if (!pps->uniform_spacing_flag)
        {
            for (int i = 0; i < pps->num_tile_columns_minus1; i++)
                my_printf(" column_width_minus1[%d]: %d", i, pps->column_width_minus1[i]);
            for (int i = 0; i < pps->num_tile_rows_minus1; i++)
                my_printf(" row_height_minus1[%d]: %d", i, pps->row_height_minus1[i]);
        }
        my_printf(" loop_filter_across_tiles_enabled_flag: %d", pps->loop_filter_across_tiles_enabled_flag); // to check
    }
    my_printf("pps_loop_filter_across_slices_enabled_flag: %d", pps->pps_loop_filter_across_slices_enabled_flag); // to check
    my_printf("deblocking_filter_control_present_flag: %d", pps->deblocking_filter_control_present_flag);
    if (pps->deblocking_filter_control_present_flag)
    {
        my_printf(" deblocking_filter_override_enabled_flag: %d", pps->deblocking_filter_override_enabled_flag);
        my_printf(" pps_deblocking_filter_disabled_flag: %d", pps->pps_deblocking_filter_disabled_flag);
        if (pps->pps_deblocking_filter_disabled_flag)
        {
            my_printf("  pps_beta_offset_div2: %d", pps->pps_beta_offset_div2);
            my_printf("  pps_tc_offset_div2: %d", pps->pps_tc_offset_div2);
        }
    }
    my_printf("pps_scaling_list_data_present_flag: %d", pps->pps_scaling_list_data_present_flag);
    if (pps->pps_scaling_list_data_present_flag)
    {
        // scaling_list_data()
        h265_debug_scaling_list(&pps->scaling_list_data);
    }
    my_printf("lists_modification_present_flag: %d", pps->lists_modification_present_flag);
    my_printf("log2_parallel_merge_level_minus2: %d", pps->log2_parallel_merge_level_minus2);
    my_printf("slice_segment_header_extension_present_flag: %d", pps->slice_segment_header_extension_present_flag);
    my_printf("pps_extension_present_flag: %d", pps->pps_extension_present_flag);
    if (pps->pps_extension_present_flag)
    {
        my_printf(" pps_range_extension_flag: %d", pps->pps_range_extension_flag);
        my_printf(" pps_multilayer_extension_flag: %d", pps->pps_multilayer_extension_flag);
        my_printf(" pps_3d_extension_flag: %d", pps->pps_3d_extension_flag);
        my_printf(" pps_extension_5bits: %d", pps->pps_extension_5bits);
    }
    if (pps->pps_range_extension_flag)
    {
        if (pps->transform_skip_enabled_flag)
            my_printf(" pps_extension_5bits: %d", pps->pps_range_extension.log2_max_transform_skip_block_size_minus2);
        my_printf(" cross_component_prediction_enabled_flag: %d", pps->pps_range_extension.cross_component_prediction_enabled_flag);
        my_printf(" chroma_qp_offset_list_enabled_flag: %d", pps->pps_range_extension.chroma_qp_offset_list_enabled_flag);
        if (pps->pps_range_extension.chroma_qp_offset_list_enabled_flag)
        {
            my_printf(" diff_cu_chroma_qp_offset_depth: %d", pps->pps_range_extension.diff_cu_chroma_qp_offset_depth);
            my_printf(" chroma_qp_offset_list_len_minus1: %d", pps->pps_range_extension.chroma_qp_offset_list_len_minus1);
            for (int i = 0; i < pps->pps_range_extension.chroma_qp_offset_list_len_minus1; i++)
            {
                my_printf(" cb_qp_offset_list[%d]: %d", i, pps->pps_range_extension.cb_qp_offset_list[i]);
                my_printf(" cr_qp_offset_list[%d]: %d", i, pps->pps_range_extension.cb_qp_offset_list[i]);
            }
        }
        my_printf(" log2_sao_offset_scale_luma: %d", pps->pps_range_extension.log2_sao_offset_scale_luma);
        my_printf(" log2_sao_offset_scale_chroma: %d", pps->pps_range_extension.log2_sao_offset_scale_chroma);
    }
    if (pps->pps_multilayer_extension_flag)
    {
        // todo...
    }
    if (pps->pps_3d_extension_flag)
    {
        // todo...
    }
}

// aud
static void h265_debug_aud(h265_aud_t* aud)
{
    my_printf("======= HEVC AUD =======");
    const char* pic_type;
    switch (aud->pic_type)
    {
    case H265_AUD_PRIMARY_PIC_TYPE_I :    pic_type = "I"; break;
    case H265_AUD_PRIMARY_PIC_TYPE_IP :   pic_type = "P, I"; break;
    case H265_AUD_PRIMARY_PIC_TYPE_IPB :  pic_type = "B, P, I"; break;
    default : pic_type = "Unknown"; break;
    }
    my_printf("pic_type: %d ( %s ) ", aud->pic_type, pic_type );
}

// sei
static void h265_debug_sei(h265_stream_t* h)
{
    my_printf("======= HEVC SEI =======");
}

static void h265_debug_slice_header(h265_stream_t* h)
{
    h265_slice_header_t* hrd = h->sh;
    h265_sps_t* sps = NULL;
    h265_pps_t* pps = NULL;
    int nal_unit_type = h->nal->nal_unit_type;
    pps = h->pps = h->pps_table[hrd->slice_pic_parameter_set_id];
    sps = h->sps = h->sps_table[pps->pps_seq_parameter_set_id];

    my_printf("======= HEVC Slice Header =======");
    my_printf("first_slice_segment_in_pic_flag: %d", hrd->first_slice_segment_in_pic_flag);
    my_printf("no_output_of_prior_pics_flag: %d", hrd->no_output_of_prior_pics_flag);
    my_printf("slice_pic_parameter_set_id: %d", hrd->slice_pic_parameter_set_id);
    my_printf("dependent_slice_segment_flag: %d", hrd->dependent_slice_segment_flag);
    my_printf("slice_segment_address: %d", hrd->slice_segment_address);
    if (!hrd->dependent_slice_segment_flag)
    {
        my_printf("dependent_slice_segment_flag");
        for (int i = 0; i < pps->num_extra_slice_header_bits; i++)
            my_printf(" slice_reserved_flag[%d]: %d", i, hrd->slice_reserved_flag[i]);
        const char* slice_type_name;
        switch(hrd->slice_type)
        {
            case H265_SH_SLICE_TYPE_P:  slice_type_name = "P slice"; break;
            case H265_SH_SLICE_TYPE_B:  slice_type_name = "B slice"; break;
            case H265_SH_SLICE_TYPE_I:  slice_type_name = "I slice"; break;
            default :                   slice_type_name = "Unknown"; break;
        }
        my_printf(" slice_type: %d (%s)", hrd->slice_type, slice_type_name);
        if (pps->output_flag_present_flag)
            my_printf("  pic_output_flag: %d", hrd->pic_output_flag);
        if (sps->separate_colour_plane_flag == 1)
            my_printf("  colour_plane_id: %d", hrd->colour_plane_id);
        my_printf(" slice_pic_order_cnt_lsb: %d", hrd->slice_pic_order_cnt_lsb);
        my_printf(" short_term_ref_pic_set_sps_flag: %d", hrd->short_term_ref_pic_set_sps_flag);
        if (!hrd->short_term_ref_pic_set_sps_flag)
        {
            referencePictureSets_t* rps = &hrd->m_localRPS;
            h265_debug_short_term_ref_pic_set(sps, &hrd->st_ref_pic_set, rps, sps->num_short_term_ref_pic_sets);
        }
        else if (sps->num_short_term_ref_pic_sets > 1)
        {
            my_printf("  short_term_ref_pic_set_idx: %d", hrd->short_term_ref_pic_set_idx);
        }
        if (sps->long_term_ref_pics_present_flag)
        {
            my_printf("  num_long_term_sps: %d", hrd->num_long_term_sps);
            my_printf("  num_long_term_pics: %d", hrd->num_long_term_pics);
            for (int i = 0; i < (int)hrd->lt_idx_sps.size(); i++)
            {
                if (i < hrd->num_long_term_sps)
                    my_printf("   hrd->lt_idx_sps[%d]: %d", i, hrd->lt_idx_sps[i]);
                else
                {
                    my_printf("   hrd->poc_lsb_lt[%d]: %d", i, hrd->poc_lsb_lt[i]);
                    my_printf("   hrd->used_by_curr_pic_lt_flag[%d]: %d", i, hrd->used_by_curr_pic_lt_flag[i]);
                }
                my_printf("  hrd->delta_poc_msb_present_flag[%d]: %d", i, hrd->delta_poc_msb_present_flag[i]);
                if (hrd->delta_poc_msb_present_flag[i])
                    my_printf("  hrd->delta_poc_msb_cycle_lt[%d]: %d", i, hrd->delta_poc_msb_cycle_lt[i]);
            }
        }
        if(sps->sps_temporal_mvp_enabled_flag)
        {
            my_printf(" slice_temporal_mvp_enabled_flag: %d", hrd->slice_temporal_mvp_enabled_flag);
        }
        if(sps->sample_adaptive_offset_enabled_flag)
        {
            my_printf(" slice_sao_luma_flag: %d", hrd->slice_sao_luma_flag);
            my_printf(" slice_sao_chroma_flag: %d", hrd->slice_sao_chroma_flag);
        }
        if (hrd->slice_type == H265_SH_SLICE_TYPE_P || hrd->slice_type == H265_SH_SLICE_TYPE_B)
        {
            my_printf("  num_ref_idx_active_override_flag: %d", hrd->num_ref_idx_active_override_flag);
            if (hrd->num_ref_idx_active_override_flag)
            {
                my_printf("  num_ref_idx_l0_active_minus1: %d", hrd->num_ref_idx_l0_active_minus1);
                my_printf("  num_ref_idx_l1_active_minus1: %d", hrd->num_ref_idx_l1_active_minus1);
            }
            if(pps->lists_modification_present_flag)
            {
                // h265_read_ref_pic_lists_modification
            }
            my_printf("  mvd_l1_zero_flag: %d", hrd->mvd_l1_zero_flag);
            my_printf("  cabac_init_flag: %d", hrd->cabac_init_flag);
            my_printf("  collocated_from_l0_flag: %d", hrd->collocated_from_l0_flag);
            my_printf("  collocated_ref_idx: %d", hrd->collocated_ref_idx);
            // h265_read_pred_weight_table
            my_printf("  five_minus_max_num_merge_cand: %d", hrd->five_minus_max_num_merge_cand);
        }
        my_printf(" slice_qp_delta: %d", hrd->slice_qp_delta);
        if (pps->pps_slice_chroma_qp_offsets_present_flag)
        {
            my_printf("  slice_cb_qp_offset: %d", hrd->slice_cb_qp_offset);
            my_printf("  slice_cr_qp_offset: %d", hrd->slice_cr_qp_offset);
        }
        if (pps->pps_range_extension.chroma_qp_offset_list_enabled_flag)
        {
            my_printf("  cu_chroma_qp_offset_enabled_flag: %d", hrd->cu_chroma_qp_offset_enabled_flag);
        }
        if (pps->deblocking_filter_override_enabled_flag)
        {
            my_printf("  deblocking_filter_override_flag: %d", hrd->deblocking_filter_override_flag);
        }
        if (hrd->deblocking_filter_override_flag)
        {
            my_printf("  slice_deblocking_filter_disabled_flag: %d", hrd->slice_deblocking_filter_disabled_flag);
            if (!hrd->slice_deblocking_filter_disabled_flag)
            {
                my_printf("   slice_beta_offset_div2: %d", hrd->slice_beta_offset_div2);
                my_printf("   slice_tc_offset_div2: %d", hrd->slice_tc_offset_div2);
            }
        }
        my_printf(" slice_loop_filter_across_slices_enabled_flag: %d", hrd->slice_loop_filter_across_slices_enabled_flag);
        
    }
    if (pps->tiles_enabled_flag || pps->entropy_coding_sync_enabled_flag)
    {
        my_printf(" num_entry_point_offsets: %d", hrd->num_entry_point_offsets);
        if (hrd->num_entry_point_offsets > 0)
        {
            my_printf("  offset_len_minus1: %d", hrd->offset_len_minus1);
            my_printf(" NumEntryPointOffsets");
            for (int i = 0; i < hrd->num_entry_point_offsets; i++)
                my_printf("  entry_point_offset_minus1[%d]: %d", i, hrd->entry_point_offset_minus1[i]);
        }
    }
    if (pps->slice_segment_header_extension_present_flag)
    {
        my_printf("slice_segment_header_extension_length: %d", hrd->slice_segment_header_extension_length);
        for (int i = 0; i < hrd->slice_segment_header_extension_length; i++)
            my_printf("slice_segment_header_extension_data_byte[%d]: %d", hrd->slice_segment_header_extension_data_byte[i]);
    }
    // no need to debug...
    my_printf("slice_segment_data()");
    my_printf("rbsp_slice_segment_trailing_bits()");
}

static void h265_debug_nal(h265_stream_t* h, h265_nal_t* nal)
{
    int my_nal_type = -1;

    const char* nal_unit_type_name;
    switch (nal->nal_unit_type)
    {
    case NAL_UNIT_VPS:
        nal_unit_type_name = "Video parameter set";
        my_nal_type = 0;
        break;
    case NAL_UNIT_SPS:
        nal_unit_type_name = "Sequence parameter set";
        my_nal_type = 1;
        break;
    case NAL_UNIT_PPS:
        nal_unit_type_name = "Picture parameter set";
        my_nal_type = 2;
        break;
    case NAL_UNIT_AUD:
        nal_unit_type_name = "Access unit delimiter";
        my_nal_type = 3;
        break;
    case NAL_UNIT_EOS:
        nal_unit_type_name = "End of sequence";
        break;
    case NAL_UNIT_EOB:
        nal_unit_type_name = "End of bitstream";
        break;
    case NAL_UNIT_FILLER_DATA:
        nal_unit_type_name = "Filler data";
        break;
    case NAL_UNIT_PREFIX_SEI:
    case NAL_UNIT_SUFFIX_SEI:
        nal_unit_type_name = "Supplemental enhancement information";
        my_nal_type = 4;
        break;
    case NAL_UNIT_CODED_SLICE_TRAIL_N:
    case NAL_UNIT_CODED_SLICE_TRAIL_R:
        nal_unit_type_name = "Coded slice segment of a non-TSA, non-STSA trailing picture";
        my_nal_type = 5;
        break;
    case NAL_UNIT_CODED_SLICE_TSA_N:
    case NAL_UNIT_CODED_SLICE_TSA_R:
        nal_unit_type_name = "Coded slice segment of a TSA picture";
        my_nal_type = 5;
        break;
    case NAL_UNIT_CODED_SLICE_STSA_N:
    case NAL_UNIT_CODED_SLICE_STSA_R:
        nal_unit_type_name = "Coded slice segment of an STSA picture";
        my_nal_type = 5;
        break;
    case NAL_UNIT_CODED_SLICE_RADL_N:
    case NAL_UNIT_CODED_SLICE_RADL_R:
        nal_unit_type_name = "Coded slice segment of a RADL picture";
        my_nal_type = 5;
        break;
    case NAL_UNIT_CODED_SLICE_RASL_N:
    case NAL_UNIT_CODED_SLICE_RASL_R:
        nal_unit_type_name = "Coded slice segment of a RASL picture";
        my_nal_type = 5;
        break;
    case NAL_UNIT_RESERVED_VCL_N10:
    case NAL_UNIT_RESERVED_VCL_N12:
    case NAL_UNIT_RESERVED_VCL_N14:
        nal_unit_type_name = "Reserved non-IRAP SLNR VCL NAL unit types";
        my_nal_type = 5;
        break;
    case NAL_UNIT_RESERVED_VCL_R11:
    case NAL_UNIT_RESERVED_VCL_R13:
    case NAL_UNIT_RESERVED_VCL_R15:
        nal_unit_type_name = "Reserved non-IRAP sub-layer reference VCL NAL unit types";
        break;
    case NAL_UNIT_CODED_SLICE_BLA_W_LP:
    case NAL_UNIT_CODED_SLICE_BLA_W_RADL:
    case NAL_UNIT_CODED_SLICE_BLA_N_LP:
        nal_unit_type_name = "Coded slice segment of a BLA picture";
        my_nal_type = 5;
        break;
    case NAL_UNIT_CODED_SLICE_IDR_W_RADL:
    case NAL_UNIT_CODED_SLICE_IDR_N_LP:
        nal_unit_type_name = "Coded slice segment of an IDR picture";
        my_nal_type = 5;
        break;
    case NAL_UNIT_CODED_SLICE_CRA:
        nal_unit_type_name = "Coded slice segment of a CRA picture";
        break;

    case NAL_UNIT_RESERVED_IRAP_VCL22:
    case NAL_UNIT_RESERVED_IRAP_VCL23:
        nal_unit_type_name = "Reserved IRAP VCL NAL unit types";
        break;
    case NAL_UNIT_RESERVED_VCL24:
    case NAL_UNIT_RESERVED_VCL25:
    case NAL_UNIT_RESERVED_VCL26:
    case NAL_UNIT_RESERVED_VCL27:
    case NAL_UNIT_RESERVED_VCL28:
    case NAL_UNIT_RESERVED_VCL29:
    case NAL_UNIT_RESERVED_VCL30:
    case NAL_UNIT_RESERVED_VCL31:
        nal_unit_type_name = "Reserved non-IRAP VCL NAL unit types";
        break;
    case NAL_UNIT_RESERVED_NVCL41:
    case NAL_UNIT_RESERVED_NVCL42:
    case NAL_UNIT_RESERVED_NVCL43:
    case NAL_UNIT_RESERVED_NVCL44:
    case NAL_UNIT_RESERVED_NVCL45:
    case NAL_UNIT_RESERVED_NVCL46:
    case NAL_UNIT_RESERVED_NVCL47:
        nal_unit_type_name = "Reserved";
        break;
    case NAL_UNIT_UNSPECIFIED_48:
    case NAL_UNIT_UNSPECIFIED_49:
    case NAL_UNIT_UNSPECIFIED_50:
    case NAL_UNIT_UNSPECIFIED_51:
    case NAL_UNIT_UNSPECIFIED_52:
    case NAL_UNIT_UNSPECIFIED_53:
    case NAL_UNIT_UNSPECIFIED_54:
    case NAL_UNIT_UNSPECIFIED_55:
    case NAL_UNIT_UNSPECIFIED_56:
    case NAL_UNIT_UNSPECIFIED_57:
    case NAL_UNIT_UNSPECIFIED_58:
    case NAL_UNIT_UNSPECIFIED_59:
    case NAL_UNIT_UNSPECIFIED_60:
    case NAL_UNIT_UNSPECIFIED_61:
    case NAL_UNIT_UNSPECIFIED_62:
    case NAL_UNIT_UNSPECIFIED_63:
        nal_unit_type_name = "Unspecified";
        break;
    default :
        nal_unit_type_name = "Unknown";
        break;
    }
    // nal header
    my_printf("==================== HEVC NAL ====================");
    my_printf(" forbidden_zero_bit : %d", nal->forbidden_zero_bit);
    my_printf(" nal_unit_type : %d ( %s )", nal->nal_unit_type, nal_unit_type_name);
    my_printf(" nal_ref_idc : %d", nal->nuh_layer_id);
    my_printf(" nal_ref_idc : %d", nal->nuh_temporal_id_plus1);
    
    // nal unit
    if(my_nal_type == 0)
        h265_debug_vps(h->vps);
    else if(my_nal_type == 1)
        h265_debug_sps(h->sps);
    else if(my_nal_type == 2)
        h265_debug_pps(h->pps);
    else if(my_nal_type == 3)
        h265_debug_aud(h->aud);
    else if(my_nal_type == 4)
        h265_debug_sei(h);
    else if(my_nal_type == 5)
        h265_debug_slice_header(h);
}