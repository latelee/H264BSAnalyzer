#include "stdafx.h" // for mfc

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "bs.h"
#include "h265_stream.h"
#include "h265_sei.h"

FILE* h265_dbgfile = NULL;

#define printf(...) fprintf((h265_dbgfile == NULL ? stdout : h265_dbgfile), __VA_ARGS__)

/**
 Create a new H265 stream object.  Allocates all structures contained within it.
 @return    the stream object
 */
h265_stream_t* h265_new()
{
    h265_stream_t* h = (h265_stream_t*)calloc(1, sizeof(h265_stream_t));

    h->nal = (h265_nal_t*)calloc(1, sizeof(h265_nal_t));

    // initialize tables
    for ( int i = 0; i < 16; i++ ) { h->vps_table[i] = (h265_vps_t*)calloc(1, sizeof(h265_vps_t)); }
    for ( int i = 0; i < 32; i++ ) { h->sps_table[i] = (h265_sps_t*)calloc(1, sizeof(h265_sps_t)); }
    for ( int i = 0; i < 256; i++ ) { h->pps_table[i] = (h265_pps_t*)calloc(1, sizeof(h265_pps_t)); }

    h->vps = h->vps_table[0];
    h->sps = h->sps_table[0];
    h->pps = h->pps_table[0];
    h->aud = (h265_aud_t*)calloc(1, sizeof(h265_aud_t));
    h->num_seis = 0;
    h->seis = NULL;
    h->sei = NULL;  //This is a TEMP pointer at whats in h->seis...
    h->sh = (h265_slice_header_t*)calloc(1, sizeof(h265_slice_header_t));

    h->info = (videoinfo_t*)calloc(1, sizeof(videoinfo_t));
    h->info->type = 1;
    return h;
}

/**
 Free an existing H265 stream object.  Frees all contained structures.
 @param[in,out] h   the stream object
 */
void h265_free(h265_stream_t* h)
{
    free(h->nal);
    for ( int i = 0; i < 16; i++ ) { if (h->vps_table[i]!=NULL) free( h->vps_table[i] ); }
    for ( int i = 0; i < 32; i++ ) { if (h->sps_table[i]!=NULL) free( h->sps_table[i] ); }
    for ( int i = 0; i < 256; i++ ) { if (h->pps_table[i]!=NULL) free( h->pps_table[i] ); }

    if (h->aud != NULL)
    {
        free(h->aud);
    }

    if(h->seis != NULL)
    {
        for( int i = 0; i < h->num_seis; i++ )
        {
            h265_sei_t* sei = h->seis[i];
            h265_sei_free(sei);
        }
        free(h->seis);
    }
    free(h->sh);
    free(h->info);
    free(h);
}

/**
 Read a NAL unit from a byte buffer.
 The buffer must start exactly at the beginning of the nal (after the start prefix).
 The NAL is read into h->nal and into other fields within h depending on its type (check h->nal->nal_unit_type after reading).
 @param[in,out] h          the stream object
 @param[in]     buf        the buffer
 @param[in]     size       the size of the buffer
 @return                   the length of data actually read, or -1 on error
 */
//7.3.1 NAL unit syntax
int h265_read_nal_unit(h265_stream_t* h, uint8_t* buf, int size)
{
    h265_nal_t* nal = h->nal;

    bs_t* b = bs_new(buf, size);
    // nal header
    nal->forbidden_zero_bit = bs_read_f(b,1);
    nal->nal_unit_type = bs_read_u(b,6);
    nal->nuh_layer_id = bs_read_u(b,6);
    nal->nuh_temporal_id_plus1 = bs_read_u(b,3);
    nal->parsed = NULL;
    nal->sizeof_parsed = 0;
    bs_free(b);

    int nal_size = size;
    int rbsp_size = size;
    uint8_t* rbsp_buf = (uint8_t*)malloc(rbsp_size);

    int rc = nal_to_rbsp(2, buf, &nal_size, rbsp_buf, &rbsp_size);

    if (rc < 0) { free(rbsp_buf); return -1; } // handle conversion error

    b = bs_new(rbsp_buf, rbsp_size);

    // nal data
    switch ( nal->nal_unit_type )
    {
        case NAL_UNIT_VPS:
            h265_read_vps_rbsp(h,b);
            break;
        case NAL_UNIT_SPS:
            h265_read_sps_rbsp(h, b);
            nal->parsed = h->sps;
            nal->sizeof_parsed = sizeof(h265_sps_t);
            break;
        case NAL_UNIT_PPS:
            h265_read_pps_rbsp(h, b);
            nal->parsed = h->pps;
            nal->sizeof_parsed = sizeof(h265_pps_t);
            break;
        case NAL_UNIT_PREFIX_SEI:
            h265_read_sei_rbsp(h, b);
            nal->parsed = h->sei;
            nal->sizeof_parsed = sizeof(h265_sei_t);
            break;
        case NAL_UNIT_SUFFIX_SEI: 
            h265_read_sei_rbsp(h, b);
            nal->parsed = h->sei;
            nal->sizeof_parsed = sizeof(h265_sei_t);
            break;
        case NAL_UNIT_AUD:
            h265_read_aud_rbsp(h, b);
            nal->parsed = h->aud;
            nal->sizeof_parsed = sizeof(h265_aud_t);
            break;
        case NAL_UNIT_EOS:
            h265_read_end_of_seq_rbsp(h, b);
            break;
        case NAL_UNIT_EOB:
            h265_read_end_of_stream_rbsp(h, b);
            break;
        case NAL_UNIT_CODED_SLICE_TRAIL_N:
        case NAL_UNIT_CODED_SLICE_TRAIL_R:
        case NAL_UNIT_CODED_SLICE_TSA_N:
        case NAL_UNIT_CODED_SLICE_TSA_R:
        case NAL_UNIT_CODED_SLICE_STSA_N:
        case NAL_UNIT_CODED_SLICE_STSA_R:
        case NAL_UNIT_CODED_SLICE_RADL_N:
        case NAL_UNIT_CODED_SLICE_RADL_R:
        case NAL_UNIT_CODED_SLICE_RASL_N:
        case NAL_UNIT_CODED_SLICE_RASL_R:
        case NAL_UNIT_CODED_SLICE_BLA_W_LP:
        case NAL_UNIT_CODED_SLICE_BLA_W_RADL:
        case NAL_UNIT_CODED_SLICE_BLA_N_LP:
        case NAL_UNIT_CODED_SLICE_IDR_W_RADL:
        case NAL_UNIT_CODED_SLICE_IDR_N_LP:
        case NAL_UNIT_CODED_SLICE_CRA:
            h265_read_slice_layer_rbsp(h, b);
            nal->parsed = h->sh;
            nal->sizeof_parsed = sizeof(h265_slice_header_t);
            break;
        case NAL_UNIT_RESERVED_VCL_N10:
        case NAL_UNIT_RESERVED_VCL_R11:
        case NAL_UNIT_RESERVED_VCL_N12:
        case NAL_UNIT_RESERVED_VCL_R13:
        case NAL_UNIT_RESERVED_VCL_N14:
        case NAL_UNIT_RESERVED_VCL_R15:

        case NAL_UNIT_RESERVED_IRAP_VCL22:
        case NAL_UNIT_RESERVED_IRAP_VCL23:

        case NAL_UNIT_RESERVED_VCL24:
        case NAL_UNIT_RESERVED_VCL25:
        case NAL_UNIT_RESERVED_VCL26:
        case NAL_UNIT_RESERVED_VCL27:
        case NAL_UNIT_RESERVED_VCL28:
        case NAL_UNIT_RESERVED_VCL29:
        case NAL_UNIT_RESERVED_VCL30:
        case NAL_UNIT_RESERVED_VCL31:
            printf ("Note: found reserved VCL NAL unit.\n");
            nal->parsed = NULL;
            nal->sizeof_parsed = 0;
            break;
        case NAL_UNIT_RESERVED_NVCL41:
        case NAL_UNIT_RESERVED_NVCL42:
        case NAL_UNIT_RESERVED_NVCL43:
        case NAL_UNIT_RESERVED_NVCL44:
        case NAL_UNIT_RESERVED_NVCL45:
        case NAL_UNIT_RESERVED_NVCL46:
        case NAL_UNIT_RESERVED_NVCL47:
            printf ("Note: found reserved NAL unit.\n");
            nal->parsed = NULL;
            nal->sizeof_parsed = 0;
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
            printf ("Note: found unspecified NAL unit.\n");
            nal->parsed = NULL;
            nal->sizeof_parsed = 0;
            break;
        default:
            // here comes the reserved/unspecified/ignored stuff
            nal->parsed = NULL;
            nal->sizeof_parsed = 0;
            return 0;
    }

    if (bs_overrun(b)) { bs_free(b); free(rbsp_buf); return -1; }

    bs_free(b);
    free(rbsp_buf);

    return nal_size;
}

void h265_read_ptl(profile_tier_level_t* ptl, bs_t* b, int profilePresentFlag, int max_sub_layers_minus1)
{
    int i = 0;
    if (profilePresentFlag)
    {
        ptl->general_profile_space = bs_read_u(b, 2);
        ptl->general_tier_flag     =  bs_read_u1(b);
        ptl->general_profile_idc   = bs_read_u(b, 5);
        for (i = 0; i < 32; i++)
        {
            ptl->general_profile_compatibility_flag[i] = bs_read_u1(b);
        }
        ptl->general_progressive_source_flag    = bs_read_u1(b);
        ptl->general_interlaced_source_flag     = bs_read_u1(b);
        ptl->general_non_packed_constraint_flag = bs_read_u1(b);
        ptl->general_frame_only_constraint_flag = bs_read_u1(b);
        if (ptl->general_profile_idc==4 || ptl->general_profile_compatibility_flag[4] ||
            ptl->general_profile_idc==5 || ptl->general_profile_compatibility_flag[5] ||
            ptl->general_profile_idc==6 || ptl->general_profile_compatibility_flag[6] ||
            ptl->general_profile_idc==7 || ptl->general_profile_compatibility_flag[7])
        {
            ptl->general_max_12bit_constraint_flag      = bs_read_u1(b);
            ptl->general_max_10bit_constraint_flag      = bs_read_u1(b);
            ptl->general_max_8bit_constraint_flag       = bs_read_u1(b);
            ptl->general_max_422chroma_constraint_flag  = bs_read_u1(b);
            ptl->general_max_420chroma_constraint_flag  = bs_read_u1(b);
            ptl->general_max_monochrome_constraint_flag = bs_read_u1(b);
            ptl->general_intra_constraint_flag          = bs_read_u1(b);
            ptl->general_one_picture_only_constraint_flag = bs_read_u1(b);
            ptl->general_lower_bit_rate_constraint_flag = bs_read_u1(b);
            uint64_t tmp1 = bs_read_u(b, 32);
            uint64_t tmp2 = bs_read_u(b, 2);
            ptl->general_reserved_zero_34bits = tmp1+tmp2;
        }
        else
        {
            uint64_t tmp1 = bs_read_u(b, 32);
            uint64_t tmp2 = bs_read_u(b, 11);
            ptl->general_reserved_zero_43bits = tmp1+tmp2;
        }
        if ((ptl->general_profile_idc>=1 && ptl->general_profile_idc<=5) ||
            ptl->general_profile_compatibility_flag[1] || ptl->general_profile_compatibility_flag[2] ||
            ptl->general_profile_compatibility_flag[3] || ptl->general_profile_compatibility_flag[4] ||
            ptl->general_profile_compatibility_flag[5])
        {
            ptl->general_inbld_flag = bs_read_u1(b);
        }
        else
        {
            ptl->general_reserved_zero_bit = bs_read_u1(b);
        }
    }
    ptl->general_level_idc = bs_read_u8(b);
    ptl->sub_layer_profile_present_flag.resize(max_sub_layers_minus1);
    ptl->sub_layer_level_present_flag.resize(max_sub_layers_minus1);
    for (i = 0; i < max_sub_layers_minus1; i++)
    {
        ptl->sub_layer_profile_present_flag[i] = bs_read_u1(b);
        ptl->sub_layer_level_present_flag[i]   = bs_read_u1(b);
    }
    if (max_sub_layers_minus1 > 0)
    {
        for (i = max_sub_layers_minus1; i < 8; i++)
        {
            ptl->reserved_zero_2bits[i] = bs_read_u(b, 2);
        }
    }
    ptl->sub_layer_profile_space.resize(max_sub_layers_minus1);
    ptl->sub_layer_tier_flag.resize(max_sub_layers_minus1);
    ptl->sub_layer_profile_idc.resize(max_sub_layers_minus1);
    ptl->sub_layer_profile_compatibility_flag.resize(max_sub_layers_minus1);
    for (int j = 0; j < max_sub_layers_minus1; j++)
    {
        ptl->sub_layer_profile_compatibility_flag[j].resize(32);
    }
    ptl->sub_layer_progressive_source_flag.resize(max_sub_layers_minus1);
    ptl->sub_layer_interlaced_source_flag.resize(max_sub_layers_minus1);
    ptl->sub_layer_non_packed_constraint_flag.resize(max_sub_layers_minus1);
    ptl->sub_layer_frame_only_constraint_flag.resize(max_sub_layers_minus1);
    ptl->sub_layer_max_12bit_constraint_flag.resize(max_sub_layers_minus1);
    ptl->sub_layer_max_10bit_constraint_flag.resize(max_sub_layers_minus1);
    ptl->sub_layer_max_8bit_constraint_flag.resize(max_sub_layers_minus1);
    ptl->sub_layer_max_422chroma_constraint_flag.resize(max_sub_layers_minus1);
    ptl->sub_layer_max_420chroma_constraint_flag.resize(max_sub_layers_minus1);
    ptl->sub_layer_max_monochrome_constraint_flag.resize(max_sub_layers_minus1);
    ptl->sub_layer_intra_constraint_flag.resize(max_sub_layers_minus1);
    ptl->sub_layer_one_picture_only_constraint_flag.resize(max_sub_layers_minus1);
    ptl->sub_layer_lower_bit_rate_constraint_flag.resize(max_sub_layers_minus1);
    ptl->sub_layer_reserved_zero_34bits.resize(max_sub_layers_minus1);
    ptl->sub_layer_reserved_zero_43bits.resize(max_sub_layers_minus1);
    ptl->sub_layer_inbld_flag.resize(max_sub_layers_minus1);
    ptl->sub_layer_reserved_zero_bit.resize(max_sub_layers_minus1);
    ptl->sub_layer_level_idc.resize(max_sub_layers_minus1);
    for (i = 0; i < max_sub_layers_minus1; i++)
    {
        if (ptl->sub_layer_profile_present_flag[i])
        {
            ptl->sub_layer_profile_space[i] = bs_read_u(b, 2);
            ptl->sub_layer_tier_flag[i]     = bs_read_u1(b);
            ptl->sub_layer_profile_idc[i]   = bs_read_u(b, 5);
            for (int j = 0; j < 32; j++)
            {
                ptl->sub_layer_profile_compatibility_flag[i][j] = bs_read_u1(b);
            }
            ptl->sub_layer_progressive_source_flag[i]    = bs_read_u1(b);
            ptl->sub_layer_interlaced_source_flag[i]     = bs_read_u1(b);
            ptl->sub_layer_non_packed_constraint_flag[i] = bs_read_u1(b);
            ptl->sub_layer_frame_only_constraint_flag[i] = bs_read_u1(b);
            if (ptl->sub_layer_profile_idc[i]==4 || ptl->sub_layer_profile_compatibility_flag[i][4] ||
                ptl->sub_layer_profile_idc[i]==5 || ptl->sub_layer_profile_compatibility_flag[i][5] ||
                ptl->sub_layer_profile_idc[i]==6 || ptl->sub_layer_profile_compatibility_flag[i][6] ||
                ptl->sub_layer_profile_idc[i]==7 || ptl->sub_layer_profile_compatibility_flag[i][7])
            {
                ptl->sub_layer_max_12bit_constraint_flag[i]        = bs_read_u1(b);
                ptl->sub_layer_max_10bit_constraint_flag[i]        = bs_read_u1(b);
                ptl->sub_layer_max_8bit_constraint_flag[i]         = bs_read_u1(b);
                ptl->sub_layer_max_422chroma_constraint_flag[i]    = bs_read_u1(b);
                ptl->sub_layer_max_420chroma_constraint_flag[i]    = bs_read_u1(b);
                ptl->sub_layer_max_monochrome_constraint_flag[i]   = bs_read_u1(b);
                ptl->sub_layer_intra_constraint_flag[i] = bs_read_u1(b);
                ptl->sub_layer_one_picture_only_constraint_flag[i] = bs_read_u1(b);
                ptl->sub_layer_lower_bit_rate_constraint_flag[i]   = bs_read_u1(b);
                uint64_t tmp1 = bs_read_u(b, 32);
                uint64_t tmp2 = bs_read_u(b, 2);
                ptl->sub_layer_reserved_zero_34bits[i] = tmp1+tmp2;
            }
            else
            {
                uint64_t tmp1 = bs_read_u(b, 32);
                uint64_t tmp2 = bs_read_u(b, 12);
                ptl->sub_layer_reserved_zero_43bits[i] = tmp1+tmp2;
            }
            // to check
            if ((ptl->sub_layer_profile_idc[i]>=1 && ptl->sub_layer_profile_idc[i]<=5) ||
                ptl->sub_layer_profile_compatibility_flag[i][1] ||
                ptl->sub_layer_profile_compatibility_flag[i][2] ||
                ptl->sub_layer_profile_compatibility_flag[i][3] ||
                ptl->sub_layer_profile_compatibility_flag[i][4] ||
                ptl->sub_layer_profile_compatibility_flag[i][5])
            {
                ptl->sub_layer_inbld_flag[i] = bs_read_u1(b);
            }
            else
            {
                ptl->sub_layer_reserved_zero_bit[i] = bs_read_u1(b);
            }
        }
        if (ptl->sub_layer_level_present_flag[i])
        {
            ptl->sub_layer_level_idc[i] = bs_read_u8(b);
        }
    }
}

// E.2.3  Sub-layer HRD parameters syntax
// E.3.3  The variable CpbCnt is set equal to cpb_cnt_minus1[ subLayerId ].
void h265_read_sub_layer_hrd_parameters(sub_layer_hrd_parameters_t* subhrd, bs_t* b, int sub_pic_hrd_params_present_flag, int CpbCnt)
{
    subhrd->bit_rate_value_minus1.resize(CpbCnt+1);
    subhrd->cpb_size_value_minus1.resize(CpbCnt+1);
    subhrd->cpb_size_du_value_minus1.resize(CpbCnt+1);
    subhrd->bit_rate_du_value_minus1.resize(CpbCnt+1);
    subhrd->cbr_flag.resize(CpbCnt+1);
    for (int i = 0; i <= CpbCnt; i++)
    {
        subhrd->bit_rate_value_minus1[i] = bs_read_ue(b);
        subhrd->cpb_size_value_minus1[i] = bs_read_ue(b);
        if (sub_pic_hrd_params_present_flag)
        {
            subhrd->cpb_size_du_value_minus1[i] = bs_read_ue(b);
            subhrd->bit_rate_du_value_minus1[i] = bs_read_ue(b);
        }
        subhrd->cbr_flag[i] = bs_read_u1(b);
    }
}

// E.2.2  HRD parameters syntax
void h265_read_hrd_parameters(hrd_parameters_t* hrd, bs_t* b, int commonInfPresentFlag, int maxNumSubLayersMinus1)
{
    if(commonInfPresentFlag)
    {
         hrd->nal_hrd_parameters_present_flag = bs_read_u1(b);
         hrd->vcl_hrd_parameters_present_flag = bs_read_u1(b);
         if (hrd->nal_hrd_parameters_present_flag ||
             hrd->vcl_hrd_parameters_present_flag)
         {
            hrd->sub_pic_hrd_params_present_flag = bs_read_u1(b);
            if (hrd->sub_pic_hrd_params_present_flag)
            {
                hrd->tick_divisor_minus2 = bs_read_u8(b);
                hrd->du_cpb_removal_delay_increment_length_minus1 = bs_read_u(b, 5);
                hrd->sub_pic_cpb_params_in_pic_timing_sei_flag    = bs_read_u1(b);
                hrd->dpb_output_delay_du_length_minus1            = bs_read_u(b, 5);
            }
            hrd->bit_rate_scale  = bs_read_u(b, 4);
            hrd->cpb_size_scale  = bs_read_u(b, 4);
            if (hrd->sub_pic_hrd_params_present_flag)
            {
                hrd->cpb_size_du_scale = bs_read_u(b, 4);
            }
            hrd->initial_cpb_removal_delay_length_minus1 = bs_read_u(b, 5);
            hrd->au_cpb_removal_delay_length_minus1      = bs_read_u(b, 5);
            hrd->dpb_output_delay_length_minus1          = bs_read_u(b, 5);
         }
    }
    hrd->fixed_pic_rate_general_flag.resize(maxNumSubLayersMinus1+1);
    hrd->fixed_pic_rate_within_cvs_flag.resize(maxNumSubLayersMinus1+1);
    hrd->elemental_duration_in_tc_minus1.resize(maxNumSubLayersMinus1+1);
    hrd->low_delay_hrd_flag.resize(maxNumSubLayersMinus1+1);
    hrd->cpb_cnt_minus1.resize(maxNumSubLayersMinus1+1);
    for (int i = 0; i <= maxNumSubLayersMinus1; i++)
    {
        hrd->fixed_pic_rate_general_flag[i] = bs_read_u1(b);
        if (!hrd->fixed_pic_rate_general_flag[i])
        {
            hrd->fixed_pic_rate_within_cvs_flag[i] = bs_read_u1(b);
        }
        if (hrd->fixed_pic_rate_within_cvs_flag[i])
        {
            hrd->elemental_duration_in_tc_minus1[i] = bs_read_ue(b);
        }
        else
        {
            hrd->low_delay_hrd_flag[i] = bs_read_u1(b);
        }
        if (!hrd->low_delay_hrd_flag[i])
        {
            hrd->cpb_cnt_minus1[i] = bs_read_u1(b);
        }
        if(hrd->nal_hrd_parameters_present_flag)
        {
            h265_read_sub_layer_hrd_parameters(&(hrd->sub_layer_hrd_parameters), b, hrd->sub_pic_hrd_params_present_flag, hrd->cpb_cnt_minus1[i]);
        }
        if(hrd->vcl_hrd_parameters_present_flag)
        {
            h265_read_sub_layer_hrd_parameters(&(hrd->sub_layer_hrd_parameters_v), b, hrd->sub_pic_hrd_params_present_flag, hrd->cpb_cnt_minus1[i]);
        }
    }
}

// E.2.1  VUI parameters syntax
void h265_read_vui_parameters(vui_parameters_t* vui, bs_t* b, int maxNumSubLayersMinus1)
{
    vui->aspect_ratio_info_present_flag = bs_read_u1(b);
    if (vui->aspect_ratio_info_present_flag)
    {
        vui->aspect_ratio_idc  = bs_read_u8(b);
        if (vui->aspect_ratio_idc == H265_SAR_Extended)
        {
            vui->sar_width  = bs_read_u(b, 16);
            vui->sar_height = bs_read_u(b, 16);
        }
    }
    vui->overscan_info_present_flag = bs_read_u1(b);
    if (vui->overscan_info_present_flag)
    {
        vui->overscan_appropriate_flag  = bs_read_u1(b);
    }
    vui->video_signal_type_present_flag = bs_read_u1(b);
    if (vui->video_signal_type_present_flag)
    {
        vui->video_format          = bs_read_u(b, 3);
        vui->video_full_range_flag = bs_read_u1(b);
        vui->colour_description_present_flag = bs_read_u1(b);
        if (vui->colour_description_present_flag)
        {
            vui->colour_primaries  = bs_read_u8(b);
            vui->transfer_characteristics = bs_read_u8(b);
            vui->matrix_coeffs     = bs_read_u8(b);
        }
    }
    vui->chroma_loc_info_present_flag = bs_read_u1(b);
    if (vui->chroma_loc_info_present_flag)
    {
        vui->chroma_sample_loc_type_top_field    = bs_read_ue(b);
        vui->chroma_sample_loc_type_bottom_field = bs_read_ue(b);
    }
    vui->neutral_chroma_indication_flag = bs_read_u1(b);
    vui->field_seq_flag                 = bs_read_u1(b);
    vui->frame_field_info_present_flag  = bs_read_u1(b);
    vui->default_display_window_flag    = bs_read_u1(b);
    if (vui->default_display_window_flag)
    {
        vui->def_disp_win_left_offset   = bs_read_ue(b);
        vui->def_disp_win_right_offset  = bs_read_ue(b);
        vui->def_disp_win_top_offset    = bs_read_ue(b);
        vui->def_disp_win_bottom_offset = bs_read_ue(b);
    }
    vui->vui_timing_info_present_flag   = bs_read_u1(b);
    if (vui->vui_timing_info_present_flag)
    {
        vui->vui_num_units_in_tick     = bs_read_u(b, 32);
        vui->vui_time_scale            = bs_read_u(b, 32);
        vui->vui_poc_proportional_to_timing_flag = bs_read_u1(b);
        if (vui->vui_poc_proportional_to_timing_flag)
        {
            vui->vui_num_ticks_poc_diff_one_minus1 = bs_read_ue(b);
        }
        vui->vui_hrd_parameters_present_flag = bs_read_u1(b);
        if (vui->vui_hrd_parameters_present_flag)
        {
            h265_read_hrd_parameters(&vui->hrd_parameters, b, 1, maxNumSubLayersMinus1);
        }
    }
    vui->bitstream_restriction_flag = bs_read_u1(b);
    if (vui->bitstream_restriction_flag)
    {
        vui->tiles_fixed_structure_flag     = bs_read_u1(b);
        vui->motion_vectors_over_pic_boundaries_flag = bs_read_u1(b);
        vui->restricted_ref_pic_lists_flag  = bs_read_u1(b);
        vui->min_spatial_segmentation_idc   = bs_read_ue(b);
        vui->max_bytes_per_pic_denom        = bs_read_ue(b);
        vui->max_bits_per_min_cu_denom      = bs_read_ue(b);
        vui->log2_max_mv_length_horizontal  = bs_read_ue(b);
        vui->log2_max_mv_length_vertical    = bs_read_ue(b);
    }
}

// 7.3.4  Scaling list data syntax
void h265_read_scaling_list(scaling_list_data_t* sld, bs_t* b)
{
    for(int sizeId = 0; sizeId < 4; sizeId++)
    {
        for(int matrixId = 0; matrixId < 6; matrixId += ( sizeId == 3 ) ? 3 : 1)
        {
            sld->scaling_list_pred_mode_flag[sizeId][matrixId] = bs_read_u1(b);
            if (!sld->scaling_list_pred_mode_flag[sizeId][matrixId])
            {
                sld->scaling_list_pred_matrix_id_delta[sizeId][matrixId] = bs_read_ue(b);
            }
            else
            {
                int nextCoef = 8;
                int coefNum = min(64, (1 << (4 + (sizeId << 1))));
                sld->coefNum = coefNum; // tmp store
                if (sizeId > 1)
                {
                    sld->scaling_list_dc_coef_minus8[sizeId - 2][matrixId] = bs_read_se(b);
                    nextCoef = sld->scaling_list_dc_coef_minus8[sizeId - 2][matrixId] + 8;
                }
                for (int i = 0; i < sld->coefNum; i++)
                {
                    int scaling_list_delta_coef = bs_read_se(b);
                    nextCoef = (nextCoef + scaling_list_delta_coef + 256) % 256;
                    sld->ScalingList[sizeId][matrixId][i] = nextCoef;
                }
            }
        }
    }
}

// st_ref_pic_set
// 7.3.7  Short-term reference picture set syntax
void h265_read_short_term_ref_pic_set(bs_t* b, h265_sps_t* sps, st_ref_pic_set_t*st, referencePictureSets_t* rps, int stRpsIdx)
{
    st->inter_ref_pic_set_prediction_flag = 0;
    if (stRpsIdx != 0)
    {
        st->inter_ref_pic_set_prediction_flag = bs_read_u1(b);
    }
    if (st->inter_ref_pic_set_prediction_flag)
    {
        st->delta_idx_minus1 = 0;
        if (stRpsIdx == sps->m_RPSList.size())
        {
            st->delta_idx_minus1 = bs_read_ue(b);
        }
        int rIdx = stRpsIdx - 1 - st->delta_idx_minus1;
        referencePictureSets_t* rpsRef = &sps->m_RPSList[rIdx];

        st->delta_rps_sign       = bs_read_u1(b);
        st->abs_delta_rps_minus1 = bs_read_ue(b);
        int deltaRPS = (1 - 2 * st->delta_rps_sign) * (st->abs_delta_rps_minus1 + 1); // delta_RPS
        st->used_by_curr_pic_flag.resize(rpsRef->m_numberOfPictures+1);
        st->use_delta_flag.resize(rpsRef->m_numberOfPictures+1);
        for (int j = 0; j <= rpsRef->m_numberOfPictures; j++)
        {
            st->used_by_curr_pic_flag[j] = bs_read_u1(b);
            int refIdc = st->used_by_curr_pic_flag[j];
            if (!st->used_by_curr_pic_flag[j])
            {
                st->use_delta_flag[j] = bs_read_u1(b);
                refIdc = st->use_delta_flag[j] << 1; //second bit is "1" if refIdc is 2, "0" if refIdc = 0.
            }
            // todo furture
            if (refIdc == 1 || refIdc == 2)
            {

            }
        }
    }
    else
    {
        st->num_negative_pics = bs_read_ue(b);
        st->num_positive_pics = bs_read_ue(b);

        rps->m_numberOfNegativePictures = st->num_negative_pics;
        rps->m_numberOfPositivePictures = st->num_positive_pics;

        // to check...
        st->delta_poc_s0_minus1.resize(st->num_negative_pics);
        st->used_by_curr_pic_s0_flag.resize(st->num_negative_pics);
        for (int i = 0; i < st->num_negative_pics; i++)
        {
            st->delta_poc_s0_minus1[i] = bs_read_ue(b);
            st->used_by_curr_pic_s0_flag[i] = bs_read_u1(b);
            rps->m_used[i] = st->used_by_curr_pic_s0_flag[i];
        }
        st->delta_poc_s1_minus1.resize(st->num_positive_pics);
        st->used_by_curr_pic_s1_flag.resize(st->num_positive_pics);
        for (int i = 0; i < st->num_positive_pics; i++)
        {
            st->delta_poc_s1_minus1[i] = bs_read_ue(b);
            st->used_by_curr_pic_s1_flag[i] = bs_read_u1(b);
            rps->m_used[i + st->num_negative_pics] = st->used_by_curr_pic_s1_flag[i];
        }
        rps->m_numberOfPictures = rps->m_numberOfNegativePictures + rps->m_numberOfPositivePictures;
    }
}


static int getNumRpsCurrTempList(h265_slice_header_t *hrd)
{
    int numRpsCurrTempList = 0;

    if (hrd->slice_type == H265_SH_SLICE_TYPE_I)
    {
        return 0;
    }
    if (hrd->m_pRPS == NULL) return 0; // tmp...
    // todo error
    for (int i = 0;
        i < hrd->m_pRPS->m_numberOfNegativePictures + hrd->m_pRPS->m_numberOfPositivePictures + hrd->m_pRPS->m_numberOfLongtermPictures;
        i++)
    {
        if (hrd->m_pRPS->m_used[i])
        {
            numRpsCurrTempList++;
        }
    }

    return numRpsCurrTempList;
}

// ref_pic_lists_modification
// 7.3.6.2  Reference picture list modification syntax
void h265_read_ref_pic_lists_modification(bs_t* b, h265_slice_header_t* hrd)
{
    hrd->ref_pic_lists_modification.ref_pic_list_modification_flag_l0 = bs_read_u1(b);
    if (hrd->ref_pic_lists_modification.ref_pic_list_modification_flag_l0)
    {
        int numRpsCurrTempList0 = getNumRpsCurrTempList(hrd);
        if (numRpsCurrTempList0 > 1)
        {
            int length = 1;
            numRpsCurrTempList0--;
            while (numRpsCurrTempList0 >>= 1)
            {
                length++;
            }
            for (int i = 0; i <= hrd->num_ref_idx_l0_active_minus1; i++) // 注意有等号，要注意边界
            {
                hrd->ref_pic_lists_modification.list_entry_l0[i] = bs_read_u(b, length);
            }
        }
        else
        {
            for (int i = 0; i <= hrd->num_ref_idx_l0_active_minus1; i ++)
            {
                hrd->ref_pic_lists_modification.list_entry_l0[i] = 0;
            }
        }
    }
    if (hrd->slice_type == H265_SH_SLICE_TYPE_B)
    {
        hrd->ref_pic_lists_modification.ref_pic_list_modification_flag_l1 = bs_read_u1(b);
        if (hrd->ref_pic_lists_modification.ref_pic_list_modification_flag_l1)
        {
            int numRpsCurrTempList1 = getNumRpsCurrTempList(hrd);
            if (numRpsCurrTempList1 > 1)
            {
                int length = 1;
                numRpsCurrTempList1--;
                while (numRpsCurrTempList1 >>= 1)
                {
                    length++;
                }
                for (int i = 0; i <= hrd->num_ref_idx_l1_active_minus1; i++) // 注意有等号，要注意边界
                {
                    hrd->ref_pic_lists_modification.list_entry_l1[i] = bs_read_u(b, length);
                }
            }
            else
            {
                for (int i = 0; i <= hrd->num_ref_idx_l1_active_minus1; i ++)
                {
                    hrd->ref_pic_lists_modification.list_entry_l1[i] = 0;
                }
            }
        }
    }
}

// 7.3.6.3  Weighted prediction parameters syntax
void h265_read_pred_weight_table(h265_stream_t* h, bs_t* b)
{
    pred_weight_table_t* pwt = &h->sh->pred_weight_table;

    int l0_size = h->sh->num_ref_idx_l0_active_minus1+1;
    int l1_size = h->sh->num_ref_idx_l1_active_minus1+1;

    pwt->luma_weight_l0_flag.resize(l0_size);
    pwt->chroma_weight_l0_flag.resize(l0_size);
    pwt->delta_luma_weight_l0.resize(l0_size);
    pwt->luma_offset_l0.resize(l0_size);
    pwt->delta_chroma_weight_l0.resize(l0_size);
    pwt->delta_chroma_offset_l0.resize(l0_size);
    for (int j = 0; j < l0_size; j++)
    {
        pwt->delta_chroma_weight_l0[j].resize(2);
        pwt->delta_chroma_offset_l0[j].resize(2);
    }
    pwt->luma_weight_l1_flag.resize(l1_size);
    pwt->chroma_weight_l1_flag.resize(l1_size);
    pwt->delta_luma_weight_l1.resize(l1_size);
    pwt->luma_offset_l1.resize(l1_size);
    pwt->delta_chroma_weight_l1.resize(l1_size);
    pwt->delta_chroma_offset_l1.resize(l1_size);
    for (int j = 0; j < l1_size; j++)
    {
        pwt->delta_chroma_weight_l1[j].resize(2);
        pwt->delta_chroma_offset_l1[j].resize(2);
    }

    pwt->luma_log2_weight_denom = bs_read_ue(b);
    if (h->sps->chroma_format_idc != 0)
    {
        pwt->delta_chroma_log2_weight_denom = bs_read_se(b);
    }

    for (int i = 0; i <= h->sh->num_ref_idx_l0_active_minus1; i++)
    {
        pwt->luma_weight_l0_flag[i] = bs_read_u1(b);
    }
    if (h->sps->chroma_format_idc != 0)
    {
        for (int i = 0; i <= h->sh->num_ref_idx_l0_active_minus1; i++)
        {
            pwt->chroma_weight_l0_flag[i] = bs_read_u1(b);
        }
    }

    for (int i = 0; i <= h->sh->num_ref_idx_l0_active_minus1; i++)
    {
        if (pwt->luma_weight_l0_flag[i])
        {
            pwt->delta_luma_weight_l0[i] = bs_read_se(b);
            pwt->luma_offset_l0[i] = bs_read_se(b);
        }
        if (pwt->chroma_weight_l0_flag[i])
        {
            for (int j = 0; j < 2; j++)
            {
                pwt->delta_chroma_weight_l0[i][j] = bs_read_se(b);
                pwt->delta_chroma_offset_l0[i][j] = bs_read_se(b);
            }
        }
    }

    if (h->sh->slice_type == H265_SH_SLICE_TYPE_B)
    {
        for (int i = 0; i <= h->sh->num_ref_idx_l1_active_minus1; i++)
        {
            pwt->luma_weight_l1_flag[i] = bs_read_u1(b);
        }
        if (h->sps->chroma_format_idc != 0)
        {
            for (int i = 0; i <= h->sh->num_ref_idx_l1_active_minus1; i++)
            {
                pwt->chroma_weight_l1_flag[i] = bs_read_u1(b);
            }
        }
        for (int i = 0; i <= h->sh->num_ref_idx_l1_active_minus1; i++)
        {
            if (pwt->luma_weight_l1_flag[i])
            {
                pwt->delta_luma_weight_l1[i] = bs_read_se(b);
                pwt->luma_offset_l1[i] = bs_read_se(b);
            }
            if (pwt->chroma_weight_l1_flag[i])
            {
                for (int j = 0; j < 2; j++)
                {
                    pwt->delta_chroma_weight_l1[i][j] = bs_read_se(b);
                    pwt->delta_chroma_offset_l1[i][j] = bs_read_se(b);
                }
            }
        }
    }
}

void  h265_read_vps_rbsp(h265_stream_t* h, bs_t* b)
{
    int i = 0;
    int j = 0;

    int vps_video_parameter_set_id  = bs_read_u(b, 4);
    // 选择正确的sps表
    h->vps = h->vps_table[vps_video_parameter_set_id];
    h265_vps_t* vps = h->vps;
    memset(vps, 0, sizeof(h265_vps_t));

    vps->vps_video_parameter_set_id    = vps_video_parameter_set_id;
    vps->vps_base_layer_internal_flag  = bs_read_u1(b);
    vps->vps_base_layer_available_flag = bs_read_u1(b);
    vps->vps_max_layers_minus1         = bs_read_u(b, 6);
    vps->vps_max_sub_layers_minus1     = bs_read_u(b, 3);
    vps->vps_temporal_id_nesting_flag  = bs_read_u1(b);
    vps->vps_reserved_0xffff_16bits    = bs_read_u(b, 16);

    // profile tier level...
    h265_read_ptl(&vps->ptl, b, 1, vps->vps_max_sub_layers_minus1);
    h->info->profile_idc = vps->ptl.general_profile_idc;
    h->info->level_idc   = vps->ptl.general_level_idc;
    h->info->tier_idc    = vps->ptl.general_tier_flag;

    vps->vps_sub_layer_ordering_info_present_flag = bs_read_u1(b);
    for (i = (vps->vps_sub_layer_ordering_info_present_flag ? 0 : vps->vps_max_sub_layers_minus1);
         i <= vps->vps_max_sub_layers_minus1; i++ )
    {
        vps->vps_max_dec_pic_buffering_minus1[i] = bs_read_ue(b);
        vps->vps_max_num_reorder_pics[i]         = bs_read_ue(b);
        vps->vps_max_latency_increase_plus1[i]   = bs_read_ue(b);
    }
    vps->vps_max_layer_id           = bs_read_u(b, 6);
    vps->vps_num_layer_sets_minus1  = bs_read_ue(b);
    vps->layer_id_included_flag.resize(vps->vps_num_layer_sets_minus1+1);
    for (unsigned int k = 0; k < vps->layer_id_included_flag.size(); k++)
    {
        vps->layer_id_included_flag[k].resize(vps->vps_max_layer_id);
    }
    for (i = 1; i <= vps->vps_num_layer_sets_minus1; i++)
    {
        vps->layer_id_included_flag[i].resize(vps->vps_num_layer_sets_minus1+1);
    }
    for (i = 1; i <= vps->vps_num_layer_sets_minus1; i++)
    {
        for (j = 0; j <= vps->vps_max_layer_id; j++)
        {
            vps->layer_id_included_flag[i][j] = bs_read_u1(b);
        }
    }
    vps->vps_timing_info_present_flag = bs_read_u1(b);
    if (vps->vps_timing_info_present_flag)
    {
        vps->vps_num_units_in_tick = bs_read_u(b, 32);
        vps->vps_time_scale = bs_read_u(b, 32);
        vps->vps_poc_proportional_to_timing_flag = bs_read_u1(b);
        if (vps->vps_poc_proportional_to_timing_flag)
        {
            vps->vps_num_ticks_poc_diff_one_minus1 = bs_read_ue(b);
        }
        vps->vps_num_hrd_parameters  = bs_read_ue(b);
        vps->hrd_layer_set_idx.resize(vps->vps_num_hrd_parameters);
        vps->cprms_present_flag.resize(vps->vps_num_hrd_parameters);
        vps->hrd_layer_set_idx.resize(vps->vps_num_hrd_parameters);
        vps->cprms_present_flag.resize(vps->vps_num_hrd_parameters);
        for (i = 0; i < vps->vps_num_hrd_parameters; i++)
        {
            vps->hrd_layer_set_idx[i] = bs_read_ue(b);
            if (i > 0)
            {
                vps->cprms_present_flag[i] = bs_read_u1(b);
            }
            //  hrd_parameters()
            h265_read_hrd_parameters(&(vps->hrd_parameters), b, vps->cprms_present_flag[i], vps->vps_max_sub_layers_minus1);
        }
    }
    vps->vps_extension_flag  = bs_read_u1(b);
    if (vps->vps_extension_flag)
    {
        while (h265_more_rbsp_trailing_data(b))
        {
            int sps_extension_data_flag = bs_read_u1(b);
        }
    }
    h265_read_rbsp_trailing_bits(b);
}

//7.3.2.1 Sequence parameter set RBSP syntax
void  h265_read_sps_rbsp(h265_stream_t* h, bs_t* b)
{
    // NOTE 不能直接赋值给sps，因为还未知是哪一个sps。

    int sps_video_parameter_set_id = 0;
    int sps_max_sub_layers_minus1 = 0;
    int sps_temporal_id_nesting_flag = 0;
    int sps_seq_parameter_set_id = 0;
    profile_tier_level_t profile_tier_level;

    sps_video_parameter_set_id = bs_read_u(b, 4);
    sps_max_sub_layers_minus1 = bs_read_u(b, 3);
    sps_temporal_id_nesting_flag = bs_read_u1(b);

    // profile tier level...
    memset(&profile_tier_level, '\0', sizeof(profile_tier_level_t));

    h265_read_ptl(&profile_tier_level, b, 1, sps_max_sub_layers_minus1);

    sps_seq_parameter_set_id = bs_read_ue(b);
    // 选择正确的sps表
    h->sps = h->sps_table[sps_seq_parameter_set_id];
    h265_sps_t* sps = h->sps;
    memset(sps, 0, sizeof(h265_sps_t));

    sps->sps_video_parameter_set_id   = sps_video_parameter_set_id;
    sps->sps_max_sub_layers_minus1    = sps_max_sub_layers_minus1;
    sps->sps_temporal_id_nesting_flag = sps_temporal_id_nesting_flag;
    
    memcpy(&(sps->ptl), &profile_tier_level, sizeof(profile_tier_level_t)); // ptl

    sps->sps_seq_parameter_set_id     = sps_seq_parameter_set_id;
    sps->chroma_format_idc     = bs_read_ue(b);
    h->info->chroma_format_idc = sps->chroma_format_idc;
    if (sps->chroma_format_idc == 3)
    {
        sps->separate_colour_plane_flag = bs_read_u1(b);
    }
    sps->pic_width_in_luma_samples  = bs_read_ue(b);
    sps->pic_height_in_luma_samples = bs_read_ue(b);

    h->info->width  = sps->pic_width_in_luma_samples;
    h->info->height = sps->pic_height_in_luma_samples;

    sps->conformance_window_flag    = bs_read_u1(b);
    if (sps->conformance_window_flag)
    {
        sps->conf_win_left_offset   = bs_read_ue(b);
        sps->conf_win_right_offset  = bs_read_ue(b);
        sps->conf_win_top_offset    = bs_read_ue(b);
        sps->conf_win_bottom_offset = bs_read_ue(b);

        // calc width & height again...
        h->info->crop_left = sps->conf_win_left_offset;
        h->info->crop_right = sps->conf_win_right_offset;
        h->info->crop_top = sps->conf_win_top_offset;
        h->info->crop_bottom = sps->conf_win_bottom_offset;

        // 根据Table6-1及7.4.3.2.1重新计算宽、高
        // 注意：手册里加1，实际上不用
        // 参考：https://github.com/mbunkus/mkvtoolnix/issues/1152
        int sub_width_c  = ((1 == sps->chroma_format_idc) || (2 == sps->chroma_format_idc)) && (0 == sps->separate_colour_plane_flag) ? 2 : 1;
        int sub_height_c =  (1 == sps->chroma_format_idc)                                   && (0 == sps->separate_colour_plane_flag) ? 2 : 1;
        h->info->width  -= (sub_width_c*sps->conf_win_right_offset + sub_width_c*sps->conf_win_left_offset);
        h->info->height -= (sub_height_c*sps->conf_win_bottom_offset + sub_height_c*sps->conf_win_top_offset);
    }

    sps->bit_depth_luma_minus8   = bs_read_ue(b);
    sps->bit_depth_chroma_minus8 = bs_read_ue(b);

    // bit depth
    h->info->bit_depth_luma = sps->bit_depth_luma_minus8 + 8;
    h->info->bit_depth_chroma = sps->bit_depth_chroma_minus8 + 8;

    sps->log2_max_pic_order_cnt_lsb_minus4 = bs_read_ue(b);

    sps->sps_sub_layer_ordering_info_present_flag = bs_read_u1(b);
    for (int i = (sps->sps_sub_layer_ordering_info_present_flag ? 0 : sps->sps_max_sub_layers_minus1);
        i <= sps->sps_max_sub_layers_minus1; i++ )
    {
        sps->sps_max_dec_pic_buffering_minus1[i] = bs_read_ue(b);
        sps->sps_max_num_reorder_pics[i]         = bs_read_ue(b);
        sps->sps_max_latency_increase_plus1[i]   = bs_read_ue(b);
    }

    sps->log2_min_luma_coding_block_size_minus3      = bs_read_ue(b);
    sps->log2_diff_max_min_luma_coding_block_size    = bs_read_ue(b);
    sps->log2_min_luma_transform_block_size_minus2   = bs_read_ue(b);
    sps->log2_diff_max_min_luma_transform_block_size = bs_read_ue(b);
    sps->max_transform_hierarchy_depth_inter         = bs_read_ue(b);
    sps->max_transform_hierarchy_depth_intra         = bs_read_ue(b);

    sps->scaling_list_enabled_flag = bs_read_u1(b);
    if (sps->scaling_list_enabled_flag)
    {
        sps->sps_scaling_list_data_present_flag = bs_read_u1(b);
        if (sps->sps_scaling_list_data_present_flag)
        {
            // scaling_list_data()
            h265_read_scaling_list(&(sps->scaling_list_data), b);
        }
    }

    sps->amp_enabled_flag = bs_read_u1(b);
    sps->sample_adaptive_offset_enabled_flag = bs_read_u1(b);
    sps->pcm_enabled_flag = bs_read_u1(b);
    if (sps->pcm_enabled_flag)
    {
        sps->pcm_sample_bit_depth_luma_minus1   = bs_read_u(b, 4);
        sps->pcm_sample_bit_depth_chroma_minus1 = bs_read_u(b, 4);
        sps->log2_min_pcm_luma_coding_block_size_minus3   = bs_read_ue(b);
        sps->log2_diff_max_min_pcm_luma_coding_block_size = bs_read_ue(b);
        sps->pcm_loop_filter_disabled_flag      = bs_read_u1(b);
    }

    sps->num_short_term_ref_pic_sets = bs_read_ue(b);
    // 根据num_short_term_ref_pic_sets创建数组
    sps->st_ref_pic_set.resize(sps->num_short_term_ref_pic_sets);
    sps->m_RPSList.resize(sps->num_short_term_ref_pic_sets); // 确定一共有多少个RPS列表
    referencePictureSets_t* rps = NULL;
    st_ref_pic_set_t* st = NULL;
    for (int i = 0; i < sps->num_short_term_ref_pic_sets; i++)
    {
        st = &sps->st_ref_pic_set[i];
        rps = &sps->m_RPSList[i];
        h265_read_short_term_ref_pic_set(b, sps, st, rps, i);
    }

    sps->long_term_ref_pics_present_flag = bs_read_u1(b);
    if (sps->long_term_ref_pics_present_flag)
    {
        sps->num_long_term_ref_pics_sps = bs_read_ue(b);
        sps->lt_ref_pic_poc_lsb_sps.resize(sps->num_long_term_ref_pics_sps);
        sps->used_by_curr_pic_lt_sps_flag.resize(sps->num_long_term_ref_pics_sps);
        for (int i = 0; i < sps->num_long_term_ref_pics_sps; i++)
        {
            sps->lt_ref_pic_poc_lsb_sps_bytes = sps->log2_max_pic_order_cnt_lsb_minus4 + 4;
            sps->lt_ref_pic_poc_lsb_sps[i] = bs_read_u(b, sps->log2_max_pic_order_cnt_lsb_minus4 + 4); // u(v)
            sps->used_by_curr_pic_lt_sps_flag[i] = bs_read_u1(b);
        }
    }

    sps->sps_temporal_mvp_enabled_flag = bs_read_u1(b);
    sps->strong_intra_smoothing_enabled_flag = bs_read_u1(b);
    sps->vui_parameters_present_flag = bs_read_u1(b);
    if (sps->vui_parameters_present_flag)
    {
        h265_read_vui_parameters(&(sps->vui), b, sps->sps_max_sub_layers_minus1);
        // calc fps
        if (sps->vui.vui_num_units_in_tick != 0)
            h->info->max_framerate = (float)(sps->vui.vui_time_scale) / (float)(sps->vui.vui_num_units_in_tick);
    }

    sps->sps_extension_present_flag = bs_read_u1(b);
    if (sps->sps_extension_present_flag)
    {
        sps->sps_range_extension_flag      = bs_read_u1(b);
        sps->sps_multilayer_extension_flag = bs_read_u1(b);
        sps->sps_3d_extension_flag         = bs_read_u1(b);
        sps->sps_extension_5bits           = bs_read_u(b, 5);
    }

    if (sps->sps_range_extension_flag)
    {
        sps->sps_range_extension.transform_skip_rotation_enabled_flag    = bs_read_u1(b);
        sps->sps_range_extension.transform_skip_context_enabled_flag     = bs_read_u1(b);
        sps->sps_range_extension.implicit_rdpcm_enabled_flag             = bs_read_u1(b);
        sps->sps_range_extension.explicit_rdpcm_enabled_flag             = bs_read_u1(b);
        sps->sps_range_extension.extended_precision_processing_flag      = bs_read_u1(b);
        sps->sps_range_extension.intra_smoothing_disabled_flag           = bs_read_u1(b);
        sps->sps_range_extension.high_precision_offsets_enabled_flag     = bs_read_u1(b);
        sps->sps_range_extension.persistent_rice_adaptation_enabled_flag = bs_read_u1(b);
        sps->sps_range_extension.cabac_bypass_alignment_enabled_flag     = bs_read_u1(b);
    }
    if (sps->sps_multilayer_extension_flag)
    {
        // sps_multilayer_extension()
        sps->inter_view_mv_vert_constraint_flag = bs_read_u1(b);
    }
    if (sps->sps_3d_extension_flag)
    {
        // todo sps_3d_extension( )
    }
    if (sps->sps_extension_5bits)
    {
        while (h265_more_rbsp_trailing_data(b))
        {
            int sps_extension_data_flag = bs_read_u1(b);
        }
    }
    h265_read_rbsp_trailing_bits(b);
}


//7.3.2.2 Picture parameter set RBSP syntax
void h265_read_pps_rbsp(h265_stream_t* h, bs_t* b)
{
    int pps_pic_parameter_set_id = bs_read_ue(b); // get id

    h265_pps_t* pps = h->pps = h->pps_table[pps_pic_parameter_set_id] ;

    memset(pps, 0, sizeof(h265_pps_t));

    pps->pps_pic_parameter_set_id      = pps_pic_parameter_set_id;
    pps->pps_seq_parameter_set_id      = bs_read_ue(b);
    pps->dependent_slice_segments_enabled_flag  = bs_read_u1(b);
    pps->output_flag_present_flag      = bs_read_u1(b);
    pps->num_extra_slice_header_bits   = bs_read_u(b, 3);
    pps->sign_data_hiding_enabled_flag = bs_read_u1(b);
    pps->cabac_init_present_flag       = bs_read_u1(b);
    pps->num_ref_idx_l0_default_active_minus1   = bs_read_ue(b);
    pps->num_ref_idx_l1_default_active_minus1   = bs_read_ue(b);
    pps->init_qp_minus26               = bs_read_se(b);
    pps->constrained_intra_pred_flag   = bs_read_u1(b);
    pps->transform_skip_enabled_flag   = bs_read_u1(b);
    pps->cu_qp_delta_enabled_flag      = bs_read_u1(b);
    if (pps->cu_qp_delta_enabled_flag)
    {
        pps->diff_cu_qp_delta_depth    = bs_read_ue(b);
    }

    pps->pps_cb_qp_offset   = bs_read_se(b);
    pps->pps_cr_qp_offset   = bs_read_se(b);
    pps->pps_slice_chroma_qp_offsets_present_flag = bs_read_u1(b);
    pps->weighted_pred_flag               = bs_read_u1(b);
    pps->weighted_bipred_flag             = bs_read_u1(b);
    pps->transquant_bypass_enabled_flag   = bs_read_u1(b);
    pps->tiles_enabled_flag               = bs_read_u1(b);
    pps->entropy_coding_sync_enabled_flag = bs_read_u1(b);
    h->info->encoding_type = pps->entropy_coding_sync_enabled_flag;

    if (pps->tiles_enabled_flag)
    {
        pps->num_tile_columns_minus1 = bs_read_ue(b);
        pps->num_tile_rows_minus1    = bs_read_ue(b);
        pps->uniform_spacing_flag    = bs_read_u1(b);
        if (!pps->uniform_spacing_flag)
        {
            pps->column_width_minus1.resize(pps->num_tile_columns_minus1);
            pps->row_height_minus1.resize(pps->num_tile_rows_minus1);
            for (int i = 0; i < pps->num_tile_columns_minus1; i++)
            {
                pps->column_width_minus1[i] = bs_read_ue(b);
            }
            for (int i = 0; i < pps->num_tile_rows_minus1; i++)
            {
                pps->row_height_minus1[i]   = bs_read_ue(b);
            }
        }
        pps->loop_filter_across_tiles_enabled_flag  = bs_read_u1(b);
    }

    pps->pps_loop_filter_across_slices_enabled_flag = bs_read_u1(b);
    pps->deblocking_filter_control_present_flag     = bs_read_u1(b);
    if (pps->deblocking_filter_control_present_flag)
    {
        pps->deblocking_filter_override_enabled_flag = bs_read_u1(b);
        pps->pps_deblocking_filter_disabled_flag     = bs_read_u1(b);
        if (pps->pps_deblocking_filter_disabled_flag)
        {
            pps->pps_beta_offset_div2 = bs_read_se(b);
            pps->pps_tc_offset_div2   = bs_read_se(b);
        }
    }

    pps->pps_scaling_list_data_present_flag = bs_read_u1(b);
    if (pps->pps_scaling_list_data_present_flag)
    {
        // scaling_list_data()
        h265_read_scaling_list(&(pps->scaling_list_data), b);
    }

    pps->lists_modification_present_flag  = bs_read_u1(b);
    pps->log2_parallel_merge_level_minus2 = bs_read_ue(b);
    pps->slice_segment_header_extension_present_flag = bs_read_u1(b);
    pps->pps_extension_present_flag       = bs_read_u1(b);
    if (pps->pps_extension_present_flag)
    {
        pps->pps_range_extension_flag      = bs_read_u1(b);
        pps->pps_multilayer_extension_flag = bs_read_u1(b);
        pps->pps_3d_extension_flag         = bs_read_u1(b);
        pps->pps_extension_5bits           = bs_read_u(b, 5);
    }
    if (pps->pps_range_extension_flag)
    {
        if (pps->transform_skip_enabled_flag)
        {
            pps->pps_range_extension.log2_max_transform_skip_block_size_minus2 = bs_read_ue(b);
        }
        pps->pps_range_extension.cross_component_prediction_enabled_flag = bs_read_u1(b);
        pps->pps_range_extension.chroma_qp_offset_list_enabled_flag      = bs_read_u1(b);
        if (pps->pps_range_extension.chroma_qp_offset_list_enabled_flag)
        {
            pps->pps_range_extension.diff_cu_chroma_qp_offset_depth   = bs_read_ue(b);
            pps->pps_range_extension.chroma_qp_offset_list_len_minus1 = bs_read_ue(b);
            pps->pps_range_extension.cb_qp_offset_list.resize(pps->pps_range_extension.chroma_qp_offset_list_len_minus1);
            pps->pps_range_extension.cr_qp_offset_list.resize(pps->pps_range_extension.chroma_qp_offset_list_len_minus1);
            for (int i = 0; i < pps->pps_range_extension.chroma_qp_offset_list_len_minus1; i++)
            {
                 pps->pps_range_extension.cb_qp_offset_list[i] = bs_read_se(b);
                 pps->pps_range_extension.cr_qp_offset_list[i] = bs_read_se(b);
            }
        }
        pps->pps_range_extension.log2_sao_offset_scale_luma    = bs_read_ue(b);
        pps->pps_range_extension.log2_sao_offset_scale_chroma  = bs_read_ue(b);
    }
    if (pps->pps_multilayer_extension_flag)
    {
        // todo sps_multilayer_extension( )
    }
    if (pps->pps_3d_extension_flag)
    {
        // todo sps_3d_extension( )
    }
    if (pps->pps_extension_5bits)
    {
        while (h265_more_rbsp_trailing_data(b))
        {
            int pps_extension_data_flag = bs_read_u1(b);
        }
    }
    h265_read_rbsp_trailing_bits(b);
}

//7.3.6.1  General slice segment header syntax
void h265_read_slice_header(h265_stream_t* h, bs_t* b)
{
    h265_slice_header_t* hrd = h->sh;
    h265_sps_t* sps = NULL;
    h265_pps_t* pps = NULL;
    int nal_unit_type = h->nal->nal_unit_type;
    int read_slice_type = hrd->read_slice_type;

    memset(hrd, 0, sizeof(h265_slice_header_t));

    hrd->first_slice_segment_in_pic_flag  = bs_read_u1(b);

    if (nal_unit_type >= NAL_UNIT_CODED_SLICE_BLA_W_LP && nal_unit_type <= NAL_UNIT_RESERVED_IRAP_VCL23)
    {
        hrd->no_output_of_prior_pics_flag = bs_read_u1(b);
    }
    hrd->slice_pic_parameter_set_id       = bs_read_ue(b);

    pps = h->pps = h->pps_table[hrd->slice_pic_parameter_set_id];
    sps = h->sps = h->sps_table[pps->pps_seq_parameter_set_id];

    hrd->dependent_slice_segment_flag = 0;
    if (!hrd->first_slice_segment_in_pic_flag)
    {
        if (pps->dependent_slice_segments_enabled_flag)
        {
            hrd->dependent_slice_segment_flag = bs_read_u1(b);
        }
        int maxCUWidth = 1<<(sps->log2_min_luma_coding_block_size_minus3+3 + sps->log2_diff_max_min_luma_coding_block_size);
        int maxCUHeight = maxCUWidth;// to check
        int numCTUs = ((sps->pic_width_in_luma_samples+maxCUWidth-1)/maxCUWidth)*((sps->pic_height_in_luma_samples+maxCUHeight-1)/maxCUHeight);;
        int bitsSliceSegmentAddress = 0;
        while(numCTUs>(1<<bitsSliceSegmentAddress))
        {
            bitsSliceSegmentAddress++;
        }
        hrd->slice_segment_address_bytes = bitsSliceSegmentAddress;
        hrd->slice_segment_address = bs_read_u(b, bitsSliceSegmentAddress); // u(v)
    }
    if (!hrd->dependent_slice_segment_flag)
    {
        hrd->slice_reserved_flag.resize(pps->num_extra_slice_header_bits);
        for (int i = 0; i < pps->num_extra_slice_header_bits; i++)
        {
            hrd->slice_reserved_flag[i] = bs_read_u1(b);
        }
        hrd->slice_type = bs_read_ue(b);

        // we need slice type only
        if (read_slice_type) return;

        if (pps->output_flag_present_flag)
        {
            hrd->pic_output_flag = bs_read_u1(b);
        }
        if (sps->separate_colour_plane_flag == 1)
        {
            hrd->colour_plane_id = bs_read_u(b, 2);
        }
        // IDR
        if (nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL || nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP)
        {
            referencePictureSets_t* rps = &hrd->m_localRPS;
            memset(rps, '\0', sizeof(referencePictureSets_t));
            rps->m_numberOfPictures = 0;
            rps->m_numberOfNegativePictures = 0;
            rps->m_numberOfPositivePictures = 0;
            hrd->m_pRPS = rps;
        }
        //if (nal_unit_type != NAL_UNIT_CODED_SLICE_IDR_W_RADL && nal_unit_type != NAL_UNIT_CODED_SLICE_IDR_N_LP)
        else
        {
            hrd->slice_pic_order_cnt_lsb_bytes = sps->log2_max_pic_order_cnt_lsb_minus4 + 4;
            hrd->slice_pic_order_cnt_lsb = bs_read_u(b, hrd->slice_pic_order_cnt_lsb_bytes); // poc v(u)
            hrd->short_term_ref_pic_set_sps_flag = bs_read_u1(b);
            if (!hrd->short_term_ref_pic_set_sps_flag)
            {
                referencePictureSets_t* rps = &hrd->m_localRPS;
                hrd->m_pRPS = &hrd->m_localRPS;
                // error here
                // st_ref_pic_set(num_short_term_ref_pic_sets)
                h265_read_short_term_ref_pic_set(b, sps, &hrd->st_ref_pic_set, rps, sps->num_short_term_ref_pic_sets);
            }
            // sps->num_short_term_ref_pic_set实际等于ssps->m_RPSList.size() 下同
            else if (sps->num_short_term_ref_pic_sets > 1)
            {
                uint32_t numBits = 0;
                while ((1 << numBits) < sps->num_short_term_ref_pic_sets)
                {
                    numBits++;
                }
                if (numBits)
                {
                    hrd->short_term_ref_pic_set_idx_bytes = numBits;
                    hrd->short_term_ref_pic_set_idx = bs_read_u(b, numBits);
                }
                else
                {
                    hrd->short_term_ref_pic_set_idx = 0;
                }
            }
            if (sps->long_term_ref_pics_present_flag)
            {
                if (sps->num_long_term_ref_pics_sps > 0)
                {
                    hrd->num_long_term_sps = bs_read_ue(b);
                }
                uint32_t numLtrpInSPS = 0;
                while (sps->num_long_term_ref_pics_sps > (1 << numLtrpInSPS))
                {
                    numLtrpInSPS++;
                }
                hrd->num_long_term_pics = bs_read_ue(b);

                int cnt = hrd->num_long_term_sps + hrd->num_long_term_pics;
                hrd->lt_idx_sps.resize(cnt);
                hrd->poc_lsb_lt.resize(cnt);
                hrd->used_by_curr_pic_lt_flag.resize(cnt);
                hrd->delta_poc_msb_present_flag.resize(cnt);
                hrd->delta_poc_msb_cycle_lt.resize(cnt);
                for (int i = 0; i < cnt; i++)
                {
                    if (i < hrd->num_long_term_sps)
                    {
                        //if (sps->num_long_term_ref_pics_sps > 1)
                        // to confirm...
                        if (numLtrpInSPS > 0)
                        {
                            hrd->lt_idx_sps[i] = bs_read_u(b, numLtrpInSPS); // u(v)
                        }
                    }
                    else
                    {
                        hrd->poc_lsb_lt[i] = bs_read_u(b, sps->log2_max_pic_order_cnt_lsb_minus4+4);
                        hrd->used_by_curr_pic_lt_flag[i] = bs_read_u1(b);
                    }
                    hrd->delta_poc_msb_present_flag[i] = bs_read_u1(b);
                    if(hrd->delta_poc_msb_present_flag[i])
                    {
                        hrd->delta_poc_msb_cycle_lt[i] = bs_read_ue(b);
                    }
                }
            }
            if(sps->sps_temporal_mvp_enabled_flag)
            {
                hrd->slice_temporal_mvp_enabled_flag = bs_read_u1(b);
            }
        }
        if(sps->sample_adaptive_offset_enabled_flag)
        {
            hrd->slice_sao_luma_flag = bs_read_u1(b);
            bool ChromaArrayType = (sps->chroma_format_idc != CHROMA_400);
            if (ChromaArrayType != 0)
            {
                hrd->slice_sao_chroma_flag = bs_read_u1(b);
            }
        }
        if (hrd->slice_type == H265_SH_SLICE_TYPE_P || hrd->slice_type == H265_SH_SLICE_TYPE_B)
        {
            hrd->num_ref_idx_active_override_flag = bs_read_u1(b);
            if (hrd->num_ref_idx_active_override_flag)
            {
                hrd->num_ref_idx_l0_active_minus1 = bs_read_ue(b);
                if (hrd->slice_type == H265_SH_SLICE_TYPE_B)
                {
                    hrd->num_ref_idx_l1_active_minus1 = bs_read_ue(b);
                }
            }
            // to confirm... 
            int tmp = 0;
            int NumPicTotalCurr = getNumRpsCurrTempList(hrd);
            if(pps->lists_modification_present_flag  &&  NumPicTotalCurr > 1)
            {
                h265_read_ref_pic_lists_modification(b, hrd);
            }
            if (hrd->slice_type == H265_SH_SLICE_TYPE_B)
            {
                hrd->mvd_l1_zero_flag = bs_read_u1(b);
            }
            if (pps->cabac_init_present_flag)
            {
                hrd->cabac_init_flag = bs_read_u1(b);
            }
            if (hrd->slice_temporal_mvp_enabled_flag)
            {
                if (hrd->slice_type == H265_SH_SLICE_TYPE_B)
                {
                    hrd->collocated_from_l0_flag = bs_read_u1(b);
                }
                if ((hrd->collocated_from_l0_flag && hrd->num_ref_idx_l0_active_minus1 > 0) ||
                    (!hrd->collocated_from_l0_flag && hrd->num_ref_idx_l1_active_minus1 > 0))
                {
                    hrd->collocated_ref_idx = bs_read_ue(b);
                }
            }
            if ((pps->weighted_pred_flag && hrd->slice_type == H265_SH_SLICE_TYPE_P) ||
                (pps->weighted_bipred_flag && hrd->slice_type == H265_SH_SLICE_TYPE_B))
            {
                h265_read_pred_weight_table(h, b);
            }
            hrd->five_minus_max_num_merge_cand = bs_read_ue(b);
        } // end of slice_type P || B
        hrd->slice_qp_delta = bs_read_se(b);
        if (pps->pps_slice_chroma_qp_offsets_present_flag)
        {
            hrd->slice_cb_qp_offset = bs_read_se(b);
            hrd->slice_cr_qp_offset = bs_read_se(b);
        }
        if (pps->pps_range_extension.chroma_qp_offset_list_enabled_flag)
        {
            hrd->cu_chroma_qp_offset_enabled_flag = bs_read_u1(b);
        }
        if (pps->deblocking_filter_override_enabled_flag)
        {
            hrd->deblocking_filter_override_flag  = bs_read_u1(b);
        }
        if (hrd->deblocking_filter_override_flag)
        {
            hrd->slice_deblocking_filter_disabled_flag = bs_read_u1(b);
            if (!hrd->slice_deblocking_filter_disabled_flag)
            {
                hrd->slice_beta_offset_div2 = bs_read_se(b);
                hrd->slice_tc_offset_div2   = bs_read_se(b);
            }
        }
        if (pps-> pps_loop_filter_across_slices_enabled_flag &&
            (hrd->slice_sao_luma_flag || hrd->slice_sao_chroma_flag ||
            !hrd->slice_deblocking_filter_disabled_flag))
        {
            hrd->slice_loop_filter_across_slices_enabled_flag = bs_read_u1(b);
        }
    } // end of dependent_slice_segment_flag

    if (pps->tiles_enabled_flag || pps->entropy_coding_sync_enabled_flag)
    {
        hrd->num_entry_point_offsets = bs_read_ue(b);
        if (hrd->num_entry_point_offsets > 0)
        {
            hrd->offset_len_minus1   = bs_read_ue(b);
            hrd->entry_point_offset_minus1_bytes = hrd->offset_len_minus1+1;
            hrd->entry_point_offset_minus1.resize(hrd->num_entry_point_offsets);
            // error
            for (int i = 0; i < hrd->num_entry_point_offsets; i++)
            {
                // to confirm
                // entry_point_offset_minus1为u(u),长度在offset_len_minus1的值加上1，见上
                hrd->entry_point_offset_minus1[i] = bs_read_u(b, hrd->entry_point_offset_minus1_bytes); // u(v) 
            }
        }
    }
    if (pps->slice_segment_header_extension_present_flag)
    {
        hrd->slice_segment_header_extension_length = bs_read_ue(b);
        hrd->slice_segment_header_extension_data_byte.resize(hrd->slice_segment_header_extension_length);
        for (int i = 0; i < hrd->slice_segment_header_extension_length; i++)
        {
            hrd->slice_segment_header_extension_data_byte[i] = bs_read_u8(b);
        }
    }
    // byte_alignment()
}

void h265_read_slice_layer_rbsp(h265_stream_t* h, bs_t* b)
{
    h265_read_slice_header(h, b);
#if 0

    slice_data_rbsp_t* slice_data = h->slice_data;

    if ( slice_data != NULL )
    {
        if ( slice_data->rbsp_buf != NULL ) free( slice_data->rbsp_buf );
        uint8_t *sptr = b->p + (!!b->bits_left); // CABAC-specific: skip alignment bits, if there are any
        slice_data->rbsp_size = b->end - sptr;

        slice_data->rbsp_buf = (uint8_t*)malloc(slice_data->rbsp_size);
        memcpy( slice_data->rbsp_buf, sptr, slice_data->rbsp_size );
        // ugly hack: since next NALU starts at byte border, we are going to be padded by trailing_bits;
        return;
    }

    // FIXME should read or skip data
    //slice_data( ); /* all categories of slice_data( ) syntax */
    read_rbsp_slice_trailing_bits(h, b);
#endif
}

//7.3.2.5 Access unit delimiter RBSP syntax
void h265_read_aud_rbsp(h265_stream_t* h, bs_t* b)
{
    h->aud->pic_type = bs_read_u(b,3);
     h265_read_rbsp_trailing_bits(b);
}

//7.3.2.6 End of sequence RBSP syntax
void h265_read_end_of_seq_rbsp(h265_stream_t* h, bs_t* b)
{
}

//7.3.2.7 End of stream RBSP syntax
void h265_read_end_of_stream_rbsp(h265_stream_t* h, bs_t* b)
{
}

int h265_more_rbsp_data(bs_t* b) 
{
    if ( bs_eof(b) ) { return 0; }
    if ( bs_peek_u1(b) == 1 ) { return 0; } // if next bit is 1, we've reached the stop bit
    return 1;
}

int h265_more_rbsp_trailing_data(bs_t* b) { return !bs_eof(b); }

int __read_ff_coded_number(bs_t* b)
{
    int n1 = 0;
    int n2;
    do
    {
        n2 = bs_read_u8(b);
        n1 += n2;
    } while (n2 == 0xff);
    return n1;
}

void h265_read_sei(h265_stream_t* h, bs_t* b)
{
    h->sei->payloadType = __read_ff_coded_number(b);
    h->sei->payloadSize = __read_ff_coded_number(b);
    h265_read_sei_payload(h, b, h->sei->payloadType, h->sei->payloadSize);
    h265_read_rbsp_trailing_bits(b);
}

//7.3.2.4 Supplemental enhancement information RBSP syntax
void h265_read_sei_rbsp(h265_stream_t* h, bs_t* b)
{
    //return;
    for (int i = 0; i < h->num_seis; i++)
    {
        h265_sei_free(h->seis[i]);
    }

    h->num_seis = 0;
    do {
        h->num_seis++;
        h->seis = (h265_sei_t**)realloc(h->seis, h->num_seis * sizeof(sei_t*));
        h->seis[h->num_seis - 1] = h265_sei_new();
        h->sei = h->seis[h->num_seis - 1];
        h265_read_sei(h, b);
    } while(h265_more_rbsp_data(b));

    h265_more_rbsp_trailing_data(b);
}

//7.3.2.10 RBSP slice trailing bits syntax
// 与h.264略有不同
void h265_read_rbsp_slice_trailing_bits(bs_t* b)
{
    h265_read_rbsp_trailing_bits(b);
    while( h265_more_rbsp_trailing_data(b) )
    {
        int cabac_zero_word = bs_read_f(b,16); // equal to 0x0000
    }
}

//7.3.2.11 RBSP trailing bits syntax
void h265_read_rbsp_trailing_bits(bs_t* b)
{
    int rbsp_stop_one_bit = bs_read_u1( b ); // equal to 1

    while( !bs_byte_aligned(b) )
    {
        int rbsp_alignment_zero_bit = bs_read_u1( b ); // equal to 0 7 bits
    }
}
