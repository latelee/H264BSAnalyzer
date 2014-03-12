#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "NaLParse.h"

#include "h264_stream.h"

#include "H264BSAnalyzerDlg.h"

FILE *g_fpBitStream = NULL;                //!< the bit stream file
//static bool flag = true;
//static int info2=0, info3=0;

//判断是否为0x000001,如果是返回1
static int FindStartCode2 (unsigned char *Buf)
{
    if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=1)
        return 0;
    else
        return 1;
}

//判断是否为0x00000001,如果是返回1
static int FindStartCode3 (unsigned char *Buf)
{
    if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=0 || Buf[3] !=1)
        return 0;
    else
        return 1;
}

//为NALU_t结构体分配内存空间
NALU_t *AllocNALU(int buffersize)
{
    NALU_t *n;

    if ((n = (NALU_t*)calloc (1, sizeof (NALU_t))) == NULL)
    {
        printf("AllocNALU: n");
        return NULL;
    }

    n->max_size=buffersize;
#if 0
    if ((n->buf = (char*)calloc (buffersize, sizeof (char))) == NULL)
    {
        free (n);
        printf ("AllocNALU: n->buf");
        return NULL;
    }
#endif
    return n;
}
//释放
void FreeNALU(NALU_t *n)
{
    if (n)
    {
#if 0
        if (n->buf)
        {
            free(n->buf);
            n->buf=NULL;
        }
#endif
        free (n);
    }
}

void OpenBitstreamFile (char *fn)
{
    if (NULL == (g_fpBitStream=fopen(fn, "r+b")))
    {
        printf("open file error\n");
        exit(0);
    }
}

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
int GetAnnexbNALU (NALU_t *nalu)
{
    int pos = 0;
    int found_startcode, rewind;
    unsigned char *Buf;
    int info2=0, info3=0;
    int eof = 0;
    
    if ((Buf = (unsigned char*)calloc (nalu->max_size , sizeof(char))) == NULL) 
        printf ("GetAnnexbNALU: Could not allocate Buf memory\n");

    nalu->startcodeprefix_len=3;//初始化码流序列的开始字符为3个字节

    if (3 != fread (Buf, 1, 3, g_fpBitStream))//从码流中读3个字节
    {
        free(Buf);
        return 0;
    }
    info2 = FindStartCode2 (Buf);//判断是否为0x000001 
    if(info2 != 1) 
    {
        //如果不是，再读一个字节
        if(1 != fread(Buf+3, 1, 1, g_fpBitStream))//读一个字节
        {
            free(Buf);
            return 0;
        }
        info3 = FindStartCode3 (Buf);//判断是否为0x00000001
        if (info3 != 1)//如果不是，返回-1
        { 
            free(Buf);
            return -1;
        }
        else 
        {
            //如果是0x00000001,得到开始前缀为4个字节
            pos = 4;
            nalu->startcodeprefix_len = 4;
        }
    }
   
    else
    {
        //如果是0x000001,得到开始前缀为3个字节
        nalu->startcodeprefix_len = 3;
        pos = 3;
    }
    //查找下一个开始字符的标志位
    found_startcode = 0;
    info2 = 0;
    info3 = 0;
  
    while (!found_startcode)
    {
        if (feof (g_fpBitStream))//判断是否到了文件尾
        {
            eof = 1;
            goto got_nal;
        }
        Buf[pos++] = fgetc (g_fpBitStream);//读一个字节到BUF中

        info3 = FindStartCode3(&Buf[pos-4]);//判断是否为0x00000001
        if(info3 != 1)
            info2 = FindStartCode2(&Buf[pos-3]);//判断是否为0x000001

        found_startcode = (info2 == 1 || info3 == 1);
    }

    // Here, we have found another start code (and read length of startcode bytes more than we should
    // have.  Hence, go back in the file
    rewind = (info3 == 1)? -4 : -3;

    if (0 != fseek (g_fpBitStream, rewind, SEEK_CUR))//把文件指针指向前一个NALU的末尾
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
    nalu->len = (pos+rewind)-nalu->startcodeprefix_len;
    // 有什么用？
    //memcpy (nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);//拷贝一个完整NALU，不拷贝起始前缀0x000001或0x00000001
    //nalu->forbidden_bit = nalu->buf[0] & 0x80; //1 bit
    //nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit
    //nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;// 5 bit
    
    char nal_header = 0;
    nal_header = Buf[nalu->startcodeprefix_len];
    nalu->forbidden_bit = nal_header & 0x80; //1 bit
    nalu->nal_reference_idc = nal_header & 0x60; // 2 bit
    nalu->nal_unit_type = nal_header & 0x1f;// 5 bit

    nalu->total_len = nalu->len + nalu->startcodeprefix_len;
    // 包括起始码在内的5个字节
    sprintf(nalu->startcode_buf, "%02x%02x%02x%02x%02x", Buf[0], Buf[1], Buf[2], Buf[3], Buf[4]);

    // 获取slice类型：I帧、P帧、B帧
    // 注：在nal类型为1~5时获取
    if (nalu->nal_unit_type <= 5 && nalu->nal_unit_type >= 1)
    {
        int start_bit = 0;
        int first_mb_in_slice = ue((char*)Buf+5, 8, start_bit);
        nalu->slice_type = ue((char*)Buf+5, 8, start_bit);
    }
    free(Buf);
 
    return (pos+rewind);//返回两个开始字符之间间隔的字节数，即包含有前缀的NALU的长度
}

// 获取NAL类型
// todo: 不能写死空间
int h264_nal_parse(LPVOID lparam,char *fileurl)
{
    CH264BSAnalyzerDlg *dlg;
    NALU_t n;
    char* nalu_payload;  
    char sendbuf[1500];
    
    unsigned short seq_num =0;
    int    bytes=0;
    int nal_num=0;
    int data_offset=0;

    OpenBitstreamFile(fileurl);

    //n = AllocNALU(8000000);//为结构体nalu_t及其成员buf分配空间。返回值为指向nalu_t存储空间的指针

    memset(&n, '\0', sizeof(NALU_t));
    n.max_size = 8*1024*1024;   // 假设一个nal包最大为8MB

    dlg=(CH264BSAnalyzerDlg *)lparam;

    while(!feof(g_fpBitStream)) 
    {
        int data_lenth;
        data_lenth=GetAnnexbNALU(&n);//每执行一次，文件的指针指向本次找到的NALU的末尾，下一个位置即为下个NALU的起始码0x000001
        n.data_offset=data_offset;
        data_offset=data_offset+data_lenth;
        //n->total_len = n->len+n->startcodeprefix_len;
        //输出NALU长度和TYPE
        // 简洁的方式
        dlg->ShowNLInfo(&n);
        //判断是否选择了“只分析5000条”，如果选择了就不再分析了
        //if(dlg->m_h264Nallistmaxnum.GetCheck()==1&&nal_num>5000){
        //    break;
        //}
        nal_num++;
    }
    //FreeNALU(n);
    return 0;
}

int h264_nal_parse_1(char *fileurl, handle_nalu_info p)
{
    NALU_t *n;
    char* nalu_payload;  
    char sendbuf[1500];
    
    unsigned short seq_num =0;
    int    bytes=0;
    int nal_num=0;
    int data_offset=0;

    OpenBitstreamFile(fileurl);
    n = AllocNALU(8000000);//为结构体nalu_t及其成员buf分配空间。返回值为指向nalu_t存储空间的指针

    while(!feof(g_fpBitStream)) 
    {
        int data_lenth;
        data_lenth=GetAnnexbNALU(n);//每执行一次，文件的指针指向本次找到的NALU的末尾，下一个位置即为下个NALU的起始码0x000001
        n->data_offset=data_offset;
        data_offset=data_offset+data_lenth;
        n->total_len = n->len+n->startcodeprefix_len;
        //输出NALU长度和TYPE
        if (p)
            p(n);
        //判断是否选择了“只分析5000条”，如果选择了就不再分析了
        //if(dlg->m_h264Nallistmaxnum.GetCheck()==1&&nal_num>5000){
        //    break;
        //}
        nal_num++;
    }
    FreeNALU(n);
    return 0;
}

static void debug_nal(h264_stream_t* h, nal_t* nal);
void dump_hex(const char *buffer, int offset, int len);

// todo：不使用这种写死空间的做法
//存放解析出来的字符串
char tempstr[1000]={0};
//char* outputstr=(char *)malloc(100000);
char outputstr[100000]={'\0'};

//自己写的，解析NAL数据的函数
int probe_nal_unit(char* filename,int data_offset,int data_lenth,LPVOID lparam)
{
    //清空字符串-----------------
    memset(outputstr,'\0',100000);
    //句柄
    CH264BSAnalyzerDlg *dlg=(CH264BSAnalyzerDlg *)lparam;
    //tempstr=(char *)malloc(10000);
    //outputstr=(char *)malloc(100000);
    //内存用于存放NAL（包含起始码）
    uint8_t *nal_temp=(uint8_t *)malloc(data_lenth);

    //uint8_t nal_temp[16*1024] = {0};

    //从文件读取
    FILE *fp=fopen(filename,"rb");
    if (fp == NULL)
    {
        return -1;
    }

    fseek(fp,data_offset,SEEK_SET);
    fread(nal_temp,data_lenth,1,fp);
    // read some H264 data into buf
    int nal_start,nal_end;
    h264_stream_t* h = h264_new();
    find_nal_unit(nal_temp, data_lenth, &nal_start, &nal_end);
    read_nal_unit(h, &nal_temp[nal_start], nal_end - nal_start);

    debug_nal(h,h->nal);    // 打印到outputstr中
    dlg->m_h264NalInfo.SetWindowText(outputstr);    // 把NAL详细信息显示到界面上

    //dump_hex((char*)nal_temp, data_offset, data_lenth);

    // 使用新的十六进制显示控件
    dlg->m_edHexInfo.SetData((LPBYTE)nal_temp, data_lenth);

    // 不要控件焦点
    ::SendMessage(dlg->GetDlgItem(IDC_EDIT_HEX)-> m_hWnd,WM_KILLFOCUS,-1,0);

    free(nal_temp);
    nal_temp = NULL;

    fclose(fp);
    return 0;
}

// 以下代码来自h264_stream.c，单独出来
/***************************** debug ******************************/

#define my_printf(...) sprintf( tempstr,__VA_ARGS__);\
    strcat(tempstr,"\r\n");                        \
    strcat(outputstr,tempstr);

static void debug_sps(sps_t* sps)
{
    my_printf("======= SPS =======\n");
    my_printf(" profile_idc : %d \n", sps->profile_idc );
    my_printf(" constraint_set0_flag : %d \n", sps->constraint_set0_flag );
    my_printf(" constraint_set1_flag : %d \n", sps->constraint_set1_flag );
    my_printf(" constraint_set2_flag : %d \n", sps->constraint_set2_flag );
    my_printf(" constraint_set3_flag : %d \n", sps->constraint_set3_flag );
    my_printf(" constraint_set4_flag : %d \n", sps->constraint_set4_flag );
    my_printf(" constraint_set5_flag : %d \n", sps->constraint_set5_flag );
    my_printf(" reserved_zero_2bits : %d \n", sps->reserved_zero_2bits );
    my_printf(" level_idc : %d \n", sps->level_idc );
    my_printf(" seq_parameter_set_id : %d \n", sps->seq_parameter_set_id );
    my_printf(" chroma_format_idc : %d \n", sps->chroma_format_idc );
    my_printf(" residual_colour_transform_flag : %d \n", sps->residual_colour_transform_flag );
    my_printf(" bit_depth_luma_minus8 : %d \n", sps->bit_depth_luma_minus8 );
    my_printf(" bit_depth_chroma_minus8 : %d \n", sps->bit_depth_chroma_minus8 );
    my_printf(" qpprime_y_zero_transform_bypass_flag : %d \n", sps->qpprime_y_zero_transform_bypass_flag );
    my_printf(" seq_scaling_matrix_present_flag : %d \n", sps->seq_scaling_matrix_present_flag );
    //  int seq_scaling_list_present_flag[8];
    //  void* ScalingList4x4[6];
    //  int UseDefaultScalingMatrix4x4Flag[6];
    //  void* ScalingList8x8[2];
    //  int UseDefaultScalingMatrix8x8Flag[2];
    my_printf(" log2_max_frame_num_minus4 : %d \n", sps->log2_max_frame_num_minus4 );
    my_printf(" pic_order_cnt_type : %d \n", sps->pic_order_cnt_type );
    my_printf("   log2_max_pic_order_cnt_lsb_minus4 : %d \n", sps->log2_max_pic_order_cnt_lsb_minus4 );
    my_printf("   delta_pic_order_always_zero_flag : %d \n", sps->delta_pic_order_always_zero_flag );
    my_printf("   offset_for_non_ref_pic : %d \n", sps->offset_for_non_ref_pic );
    my_printf("   offset_for_top_to_bottom_field : %d \n", sps->offset_for_top_to_bottom_field );
    my_printf("   num_ref_frames_in_pic_order_cnt_cycle : %d \n", sps->num_ref_frames_in_pic_order_cnt_cycle );
    //  int offset_for_ref_frame[256];
    my_printf(" num_ref_frames : %d \n", sps->num_ref_frames );
    my_printf(" gaps_in_frame_num_value_allowed_flag : %d \n", sps->gaps_in_frame_num_value_allowed_flag );
    my_printf(" pic_width_in_mbs_minus1 : %d \n", sps->pic_width_in_mbs_minus1 );
    my_printf(" pic_height_in_map_units_minus1 : %d \n", sps->pic_height_in_map_units_minus1 );
    my_printf(" frame_mbs_only_flag : %d \n", sps->frame_mbs_only_flag );
    my_printf(" mb_adaptive_frame_field_flag : %d \n", sps->mb_adaptive_frame_field_flag );
    my_printf(" direct_8x8_inference_flag : %d \n", sps->direct_8x8_inference_flag );
    my_printf(" frame_cropping_flag : %d \n", sps->frame_cropping_flag );
    my_printf("   frame_crop_left_offset : %d \n", sps->frame_crop_left_offset );
    my_printf("   frame_crop_right_offset : %d \n", sps->frame_crop_right_offset );
    my_printf("   frame_crop_top_offset : %d \n", sps->frame_crop_top_offset );
    my_printf("   frame_crop_bottom_offset : %d \n", sps->frame_crop_bottom_offset );
    my_printf(" vui_parameters_present_flag : %d \n", sps->vui_parameters_present_flag );

    my_printf("=== VUI ===\n");
    my_printf(" aspect_ratio_info_present_flag : %d \n", sps->vui.aspect_ratio_info_present_flag );
    my_printf("   aspect_ratio_idc : %d \n", sps->vui.aspect_ratio_idc );
    my_printf("     sar_width : %d \n", sps->vui.sar_width );
    my_printf("     sar_height : %d \n", sps->vui.sar_height );
    my_printf(" overscan_info_present_flag : %d \n", sps->vui.overscan_info_present_flag );
    my_printf("   overscan_appropriate_flag : %d \n", sps->vui.overscan_appropriate_flag );
    my_printf(" video_signal_type_present_flag : %d \n", sps->vui.video_signal_type_present_flag );
    my_printf("   video_format : %d \n", sps->vui.video_format );
    my_printf("   video_full_range_flag : %d \n", sps->vui.video_full_range_flag );
    my_printf("   colour_description_present_flag : %d \n", sps->vui.colour_description_present_flag );
    my_printf("     colour_primaries : %d \n", sps->vui.colour_primaries );
    my_printf("   transfer_characteristics : %d \n", sps->vui.transfer_characteristics );
    my_printf("   matrix_coefficients : %d \n", sps->vui.matrix_coefficients );
    my_printf(" chroma_loc_info_present_flag : %d \n", sps->vui.chroma_loc_info_present_flag );
    my_printf("   chroma_sample_loc_type_top_field : %d \n", sps->vui.chroma_sample_loc_type_top_field );
    my_printf("   chroma_sample_loc_type_bottom_field : %d \n", sps->vui.chroma_sample_loc_type_bottom_field );
    my_printf(" timing_info_present_flag : %d \n", sps->vui.timing_info_present_flag );
    my_printf("   num_units_in_tick : %d \n", sps->vui.num_units_in_tick );
    my_printf("   time_scale : %d \n", sps->vui.time_scale );
    my_printf("   fixed_frame_rate_flag : %d \n", sps->vui.fixed_frame_rate_flag );
    my_printf(" nal_hrd_parameters_present_flag : %d \n", sps->vui.nal_hrd_parameters_present_flag );
    my_printf(" vcl_hrd_parameters_present_flag : %d \n", sps->vui.vcl_hrd_parameters_present_flag );
    my_printf("   low_delay_hrd_flag : %d \n", sps->vui.low_delay_hrd_flag );
    my_printf(" pic_struct_present_flag : %d \n", sps->vui.pic_struct_present_flag );
    my_printf(" bitstream_restriction_flag : %d \n", sps->vui.bitstream_restriction_flag );
    my_printf("   motion_vectors_over_pic_boundaries_flag : %d \n", sps->vui.motion_vectors_over_pic_boundaries_flag );
    my_printf("   max_bytes_per_pic_denom : %d \n", sps->vui.max_bytes_per_pic_denom );
    my_printf("   max_bits_per_mb_denom : %d \n", sps->vui.max_bits_per_mb_denom );
    my_printf("   log2_max_mv_length_horizontal : %d \n", sps->vui.log2_max_mv_length_horizontal );
    my_printf("   log2_max_mv_length_vertical : %d \n", sps->vui.log2_max_mv_length_vertical );
    my_printf("   num_reorder_frames : %d \n", sps->vui.num_reorder_frames );
    my_printf("   max_dec_frame_buffering : %d \n", sps->vui.max_dec_frame_buffering );

    my_printf("=== HRD ===\n");
    my_printf(" cpb_cnt_minus1 : %d \n", sps->hrd.cpb_cnt_minus1 );
    my_printf(" bit_rate_scale : %d \n", sps->hrd.bit_rate_scale );
    my_printf(" cpb_size_scale : %d \n", sps->hrd.cpb_size_scale );
    int SchedSelIdx;
    for( SchedSelIdx = 0; SchedSelIdx <= sps->hrd.cpb_cnt_minus1; SchedSelIdx++ )
    {
        my_printf("   bit_rate_value_minus1[%d] : %d \n", SchedSelIdx, sps->hrd.bit_rate_value_minus1[SchedSelIdx] ); // up to cpb_cnt_minus1, which is <= 31
        my_printf("   cpb_size_value_minus1[%d] : %d \n", SchedSelIdx, sps->hrd.cpb_size_value_minus1[SchedSelIdx] );
        my_printf("   cbr_flag[%d] : %d \n", SchedSelIdx, sps->hrd.cbr_flag[SchedSelIdx] );
    }
    my_printf(" initial_cpb_removal_delay_length_minus1 : %d \n", sps->hrd.initial_cpb_removal_delay_length_minus1 );
    my_printf(" cpb_removal_delay_length_minus1 : %d \n", sps->hrd.cpb_removal_delay_length_minus1 );
    my_printf(" dpb_output_delay_length_minus1 : %d \n", sps->hrd.dpb_output_delay_length_minus1 );
    my_printf(" time_offset_length : %d \n", sps->hrd.time_offset_length );
}


static void debug_pps(pps_t* pps)
{
    my_printf("======= PPS =======\n");
    my_printf(" pic_parameter_set_id : %d \n", pps->pic_parameter_set_id );
    my_printf(" seq_parameter_set_id : %d \n", pps->seq_parameter_set_id );
    my_printf(" entropy_coding_mode_flag : %d \n", pps->entropy_coding_mode_flag );
    my_printf(" pic_order_present_flag : %d \n", pps->pic_order_present_flag );
    my_printf(" num_slice_groups_minus1 : %d \n", pps->num_slice_groups_minus1 );
    my_printf(" slice_group_map_type : %d \n", pps->slice_group_map_type );
    //  int run_length_minus1[8]; // up to num_slice_groups_minus1, which is <= 7 in Baseline and Extended, 0 otheriwse
    //  int top_left[8];
    //  int bottom_right[8];
    //  int slice_group_change_direction_flag;
    //  int slice_group_change_rate_minus1;
    //  int pic_size_in_map_units_minus1;
    //  int slice_group_id[256]; // FIXME what size?
    my_printf(" num_ref_idx_l0_active_minus1 : %d \n", pps->num_ref_idx_l0_active_minus1 );
    my_printf(" num_ref_idx_l1_active_minus1 : %d \n", pps->num_ref_idx_l1_active_minus1 );
    my_printf(" weighted_pred_flag : %d \n", pps->weighted_pred_flag );
    my_printf(" weighted_bipred_idc : %d \n", pps->weighted_bipred_idc );
    my_printf(" pic_init_qp_minus26 : %d \n", pps->pic_init_qp_minus26 );
    my_printf(" pic_init_qs_minus26 : %d \n", pps->pic_init_qs_minus26 );
    my_printf(" chroma_qp_index_offset : %d \n", pps->chroma_qp_index_offset );
    my_printf(" deblocking_filter_control_present_flag : %d \n", pps->deblocking_filter_control_present_flag );
    my_printf(" constrained_intra_pred_flag : %d \n", pps->constrained_intra_pred_flag );
    my_printf(" redundant_pic_cnt_present_flag : %d \n", pps->redundant_pic_cnt_present_flag );
    my_printf(" transform_8x8_mode_flag : %d \n", pps->transform_8x8_mode_flag );
    my_printf(" pic_scaling_matrix_present_flag : %d \n", pps->pic_scaling_matrix_present_flag );
    //  int pic_scaling_list_present_flag[8];
    //  void* ScalingList4x4[6];
    //  int UseDefaultScalingMatrix4x4Flag[6];
    //  void* ScalingList8x8[2];
    //  int UseDefaultScalingMatrix8x8Flag[2];
    my_printf(" second_chroma_qp_index_offset : %d \n", pps->second_chroma_qp_index_offset );
}

static void debug_slice_header(slice_header_t* sh)
{
    my_printf("======= Slice Header =======\n");
    my_printf(" first_mb_in_slice : %d \n", sh->first_mb_in_slice );
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
    my_printf(" slice_type : %d ( %s ) \n", sh->slice_type, slice_type_name );

    my_printf(" pic_parameter_set_id : %d \n", sh->pic_parameter_set_id );
    my_printf(" frame_num : %d \n", sh->frame_num );
    my_printf(" field_pic_flag : %d \n", sh->field_pic_flag );
      my_printf(" bottom_field_flag : %d \n", sh->bottom_field_flag );
    my_printf(" idr_pic_id : %d \n", sh->idr_pic_id );
    my_printf(" pic_order_cnt_lsb : %d \n", sh->pic_order_cnt_lsb );
    my_printf(" delta_pic_order_cnt_bottom : %d \n", sh->delta_pic_order_cnt_bottom );
    // int delta_pic_order_cnt[ 2 ];
    my_printf(" redundant_pic_cnt : %d \n", sh->redundant_pic_cnt );
    my_printf(" direct_spatial_mv_pred_flag : %d \n", sh->direct_spatial_mv_pred_flag );
    my_printf(" num_ref_idx_active_override_flag : %d \n", sh->num_ref_idx_active_override_flag );
    my_printf(" num_ref_idx_l0_active_minus1 : %d \n", sh->num_ref_idx_l0_active_minus1 );
    my_printf(" num_ref_idx_l1_active_minus1 : %d \n", sh->num_ref_idx_l1_active_minus1 );
    my_printf(" cabac_init_idc : %d \n", sh->cabac_init_idc );
    my_printf(" slice_qp_delta : %d \n", sh->slice_qp_delta );
    my_printf(" sp_for_switch_flag : %d \n", sh->sp_for_switch_flag );
    my_printf(" slice_qs_delta : %d \n", sh->slice_qs_delta );
    my_printf(" disable_deblocking_filter_idc : %d \n", sh->disable_deblocking_filter_idc );
    my_printf(" slice_alpha_c0_offset_div2 : %d \n", sh->slice_alpha_c0_offset_div2 );
    my_printf(" slice_beta_offset_div2 : %d \n", sh->slice_beta_offset_div2 );
    my_printf(" slice_group_change_cycle : %d \n", sh->slice_group_change_cycle );

    my_printf("=== Prediction Weight Table ===\n");
    my_printf(" luma_log2_weight_denom : %d \n", sh->pwt.luma_log2_weight_denom );
    my_printf(" chroma_log2_weight_denom : %d \n", sh->pwt.chroma_log2_weight_denom );
     //   my_printf(" luma_weight_l0_flag : %d \n", sh->pwt.luma_weight_l0_flag );
        // int luma_weight_l0[64];
        // int luma_offset_l0[64];
    //    my_printf(" chroma_weight_l0_flag : %d \n", sh->pwt.chroma_weight_l0_flag );
        // int chroma_weight_l0[64][2];
        // int chroma_offset_l0[64][2];
     //   my_printf(" luma_weight_l1_flag : %d \n", sh->pwt.luma_weight_l1_flag );
        // int luma_weight_l1[64];
        // int luma_offset_l1[64];
    //    my_printf(" chroma_weight_l1_flag : %d \n", sh->pwt.chroma_weight_l1_flag );
        // int chroma_weight_l1[64][2];
        // int chroma_offset_l1[64][2];

    my_printf("=== Ref Pic List Reordering ===\n");
    my_printf(" ref_pic_list_reordering_flag_l0 : %d \n", sh->rplr.ref_pic_list_reordering_flag_l0 );
    my_printf(" ref_pic_list_reordering_flag_l1 : %d \n", sh->rplr.ref_pic_list_reordering_flag_l1 );
        // int reordering_of_pic_nums_idc;
        // int abs_diff_pic_num_minus1;
        // int long_term_pic_num;

    my_printf("=== Decoded Ref Pic Marking ===\n");
    my_printf(" no_output_of_prior_pics_flag : %d \n", sh->drpm.no_output_of_prior_pics_flag );
    my_printf(" long_term_reference_flag : %d \n", sh->drpm.long_term_reference_flag );
    my_printf(" adaptive_ref_pic_marking_mode_flag : %d \n", sh->drpm.adaptive_ref_pic_marking_mode_flag );
        // int memory_management_control_operation;
        // int difference_of_pic_nums_minus1;
        // int long_term_pic_num;
        // int long_term_frame_idx;
        // int max_long_term_frame_idx_plus1;

}

static void debug_aud(aud_t* aud)
{
    my_printf("======= Access Unit Delimiter =======\n");
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
    my_printf(" primary_pic_type : %d ( %s ) \n", aud->primary_pic_type, primary_pic_type_name );
}

static void debug_seis( h264_stream_t* h)
{
    sei_t** seis = h->seis;
    int num_seis = h->num_seis;

    my_printf("======= SEI =======\n");
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
        my_printf("=== %s ===\n", sei_type_name);
        my_printf(" payloadType : %d \n", s->payloadType );
        my_printf(" payloadSize : %d \n", s->payloadSize );

        my_printf(" payload : " );
        my_printf("%s", s->payload);
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
static void debug_nal(h264_stream_t* h, nal_t* nal)
{
    my_printf("==================== NAL ====================\n");
    my_printf(" forbidden_zero_bit : %d \n", nal->forbidden_zero_bit );
    my_printf(" nal_ref_idc : %d \n", nal->nal_ref_idc );
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
    my_printf(" nal_unit_type : %d ( %s ) \n", nal->nal_unit_type, nal_unit_type_name );

    if( nal->nal_unit_type == NAL_UNIT_TYPE_CODED_SLICE_NON_IDR) { debug_slice_header(h->sh); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_CODED_SLICE_IDR) { debug_slice_header(h->sh); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_SPS) { debug_sps(h->sps); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_PPS) { debug_pps(h->pps); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_AUD) { debug_aud(h->aud); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_SEI) { debug_seis( h ); }
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
