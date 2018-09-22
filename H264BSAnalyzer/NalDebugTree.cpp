#include "stdafx.h"

#include "NaLParse.h"

#define my_printf(...) do { \
    sprintf(m_outputInfo, __VA_ARGS__);\
    } while(0)

#define my_printf_flag(buffer, value) do { \
    sprintf(m_tmpStore, "%s: %d  [%s] (1 bit)", buffer,value,value?"True":"False");\
    sprintf(m_outputInfo, m_tmpStore);\
    } while(0)

#define my_printf_flag2(buffer, idx, value) do { \
    sprintf(m_tmpStore, "%s[%d]: %d  [%s] (1 bit)", buffer,idx, value,value?"True":"False");\
    sprintf(m_outputInfo, m_tmpStore);\
    } while(0)

#define my_printf_flag3(buffer, idx1, idx2, value) do { \
    sprintf(m_tmpStore, "%s[%d][%d]: %d  [%s] (1 bit)", buffer,idx1,idx2,value,value?"True":"False");\
    sprintf(m_outputInfo, m_tmpStore);\
    } while(0)

// 树形项
#define AddTreeItem(_item) m_pTree->InsertItem(m_outputInfo, _item)

// 以下代码来自h264_stream.c，单独出来
/***************************** debug ******************************/

void CNalParser::h264_debug_sps(sps_t* sps, HTREEITEM root)
{
    my_printf("seq_parameter_set_data()");
    HTREEITEM isps = AddTreeItem(root);
    my_printf("profile_idc: %d  (8 bits)", sps->profile_idc ); AddTreeItem(isps);
    my_printf_flag("constraint_set0_flag", sps->constraint_set0_flag ); AddTreeItem(isps);
    my_printf_flag("constraint_set1_flag", sps->constraint_set1_flag ); AddTreeItem(isps);
    my_printf_flag("constraint_set2_flag", sps->constraint_set2_flag ); AddTreeItem(isps);
    my_printf_flag("constraint_set3_flag", sps->constraint_set3_flag ); AddTreeItem(isps);
    my_printf_flag("constraint_set4_flag", sps->constraint_set4_flag ); AddTreeItem(isps);
    my_printf_flag("constraint_set5_flag", sps->constraint_set5_flag ); AddTreeItem(isps);
    my_printf("reserved_zero_2bits: %d  (2 bits)", sps->reserved_zero_2bits ); AddTreeItem(isps);
    my_printf("level_idc: %d  (8 bits)", sps->level_idc ); AddTreeItem(isps);
    my_printf("seq_parameter_set_id: %d  (v bits)", sps->seq_parameter_set_id ); AddTreeItem(isps);
    if( sps->profile_idc == 100 || sps->profile_idc == 110 ||
        sps->profile_idc == 122 || sps->profile_idc == 144 )
    {
        my_printf("chroma_format_idc: %d  (8 bits)", sps->chroma_format_idc ); AddTreeItem(isps);
        if (sps->chroma_format_idc == 3)
        {
            my_printf_flag("separate_colour_plane_flag", sps->separate_colour_plane_flag ); AddTreeItem(isps);
        }
        my_printf("bit_depth_luma_minus8: %d  (v bits)", sps->bit_depth_luma_minus8 ); AddTreeItem(isps);
        my_printf("bit_depth_chroma_minus8: %d  (v bits)", sps->bit_depth_chroma_minus8 ); AddTreeItem(isps);
        my_printf_flag("qpprime_y_zero_transform_bypass_flag", sps->qpprime_y_zero_transform_bypass_flag ); AddTreeItem(isps);
        my_printf_flag("seq_scaling_matrix_present_flag", sps->seq_scaling_matrix_present_flag );
        HTREEITEM ssmpf = AddTreeItem(isps);
        if (sps->seq_scaling_matrix_present_flag)
        {
            for (int i = 0; i < ((sps->chroma_format_idc!=3) ? 8 : 12); i++)
            {
                my_printf_flag2("seq_scaling_list_present_flag", i, sps->seq_scaling_list_present_flag[i]);
                AddTreeItem(ssmpf);
                if( sps->seq_scaling_list_present_flag[ i ] )
                {
                    if( i < 6 )
                    {
                        my_printf("ScalingList4x4[%d]: %d  (v bits)", i, sps->ScalingList4x4[i] );
                        AddTreeItem(ssmpf);
                    }
                    else
                    {
                        my_printf("ScalingList8x8[%d]: %d  (v bits)", i, sps->ScalingList8x8[i] );
                        AddTreeItem(ssmpf);
                    }
                }
            }
        }
    }
    my_printf("log2_max_frame_num_minus4: %d  (v bits)", sps->log2_max_frame_num_minus4 ); AddTreeItem(isps);
    my_printf("pic_order_cnt_type: %d  (v bits)", sps->pic_order_cnt_type ); AddTreeItem(isps);
    if( sps->pic_order_cnt_type == 0 )
    {
        my_printf("log2_max_pic_order_cnt_lsb_minus4: %d  (v bits)", sps->log2_max_pic_order_cnt_lsb_minus4 );
        AddTreeItem(isps);
    }
    else if( sps->pic_order_cnt_type == 1 )
    {
        my_printf_flag("delta_pic_order_always_zero_flag", sps->delta_pic_order_always_zero_flag );AddTreeItem(isps);
        my_printf("offset_for_non_ref_pic: %d  (v bits)", sps->offset_for_non_ref_pic );AddTreeItem(isps);
        my_printf("offset_for_top_to_bottom_field: %d  (v bits)", sps->offset_for_top_to_bottom_field );AddTreeItem(isps);
        my_printf("num_ref_frames_in_pic_order_cnt_cycle: %d  (v bits)", sps->num_ref_frames_in_pic_order_cnt_cycle );AddTreeItem(isps);
        for( int i = 0; i < sps->num_ref_frames_in_pic_order_cnt_cycle; i++ )
        {
            my_printf("offset_for_ref_frame[%d]: %d  (v bits)", i, sps->offset_for_ref_frame[i] );AddTreeItem(isps);
        }
    }
    my_printf("max_num_ref_frames: %d  (v bits)", sps->max_num_ref_frames );AddTreeItem(isps);
    my_printf_flag("gaps_in_frame_num_value_allowed_flag", sps->gaps_in_frame_num_value_allowed_flag );AddTreeItem(isps);
    my_printf("pic_width_in_mbs_minus1: %d  (v bits)", sps->pic_width_in_mbs_minus1 );AddTreeItem(isps);
    my_printf("pic_height_in_map_units_minus1: %d  (v bits)", sps->pic_height_in_map_units_minus1 );AddTreeItem(isps);
    my_printf_flag("frame_mbs_only_flag", sps->frame_mbs_only_flag );
    HTREEITEM fmof = AddTreeItem(isps);
    if( !sps->frame_mbs_only_flag )
    {
        my_printf_flag("mb_adaptive_frame_field_flag", sps->mb_adaptive_frame_field_flag );AddTreeItem(fmof);
    }
    my_printf_flag("direct_8x8_inference_flag", sps->direct_8x8_inference_flag );AddTreeItem(isps);
    my_printf_flag("frame_cropping_flag", sps->frame_cropping_flag );
    HTREEITEM fcf = AddTreeItem(isps);
    if (sps->frame_cropping_flag)
    {
        my_printf("frame_crop_left_offset: %d  (v bits)", sps->frame_crop_left_offset );AddTreeItem(fcf);
        my_printf("frame_crop_right_offset: %d  (v bits)", sps->frame_crop_right_offset );AddTreeItem(fcf);
        my_printf("frame_crop_top_offset: %d  (v bits)", sps->frame_crop_top_offset );AddTreeItem(fcf);
        my_printf("frame_crop_bottom_offset: %d  (v bits)", sps->frame_crop_bottom_offset );AddTreeItem(fcf);
    }
    my_printf_flag("vui_parameters_present_flag", sps->vui_parameters_present_flag );
    HTREEITEM vppf = AddTreeItem(isps);
    if (sps->vui_parameters_present_flag)
    {
        my_printf("vui_parameters()");
        HTREEITEM vp = AddTreeItem(vppf);
        my_printf_flag("aspect_ratio_info_present_flag", sps->vui.aspect_ratio_info_present_flag );
        HTREEITEM aripf = AddTreeItem(vp);
        if( sps->vui.aspect_ratio_info_present_flag )
        {
            my_printf("aspect_ratio_idc: %d  (8 bits)", sps->vui.aspect_ratio_idc );AddTreeItem(aripf);
            if( sps->vui.aspect_ratio_idc == SAR_Extended )
            {
                my_printf("sar_width: %d   (16 bits)", sps->vui.sar_width );
                my_printf("sar_height: %d  (16 bits)", sps->vui.sar_height );
            }
        }
        my_printf_flag("overscan_info_present_flag", sps->vui.overscan_info_present_flag );
        HTREEITEM oipf = AddTreeItem(vp);
        if( sps->vui.overscan_info_present_flag )
        {
            my_printf_flag("overscan_appropriate_flag", sps->vui.overscan_appropriate_flag );AddTreeItem(oipf);
        }
        my_printf_flag("video_signal_type_present_flag", sps->vui.video_signal_type_present_flag );
        HTREEITEM vstpf = AddTreeItem(vp);
        if( sps->vui.video_signal_type_present_flag )
        {
            my_printf("video_format: %d  (3 bits)", sps->vui.video_format );AddTreeItem(vstpf);
            my_printf_flag("video_full_range_flag", sps->vui.video_full_range_flag );AddTreeItem(vstpf);
            my_printf_flag("colour_description_present_flag", sps->vui.colour_description_present_flag );
            HTREEITEM cdpf = AddTreeItem(vstpf);
            if( sps->vui.colour_description_present_flag )
            {
                my_printf("colour_primaries: %d  (8 bits)", sps->vui.colour_primaries );AddTreeItem(cdpf);
                my_printf("transfer_characteristics: %d  (8 bits)", sps->vui.transfer_characteristics );AddTreeItem(cdpf);
                my_printf("matrix_coefficients: %d  (8 bits)", sps->vui.matrix_coefficients );AddTreeItem(cdpf);
            }
        }
        my_printf_flag("chroma_loc_info_present_flag", sps->vui.chroma_loc_info_present_flag );
        HTREEITEM clipf = AddTreeItem(vp);
        if( sps->vui.chroma_loc_info_present_flag )
        {
            my_printf("chroma_sample_loc_type_top_field: %d  (v bits)", sps->vui.chroma_sample_loc_type_top_field );AddTreeItem(clipf);
            my_printf("chroma_sample_loc_type_bottom_field: %d  (v bits)", sps->vui.chroma_sample_loc_type_bottom_field );AddTreeItem(clipf);
        }
        my_printf_flag("timing_info_present_flag", sps->vui.timing_info_present_flag );
        HTREEITEM tipf = AddTreeItem(vp);
        if( sps->vui.timing_info_present_flag )
        {
            my_printf("num_units_in_tick: %d  (32 bits)", sps->vui.num_units_in_tick );AddTreeItem(tipf);
            my_printf("time_scale: %d  (32 bits)", sps->vui.time_scale );AddTreeItem(tipf);
            my_printf_flag("fixed_frame_rate_flag", sps->vui.fixed_frame_rate_flag );AddTreeItem(tipf);
        }
        my_printf_flag("nal_hrd_parameters_present_flag", sps->vui.nal_hrd_parameters_present_flag );
        HTREEITEM nhppf = AddTreeItem(vp);
        if( sps->vui.nal_hrd_parameters_present_flag )
        {
            my_printf("hrd_parameters()");
            HTREEITEM hp = AddTreeItem(nhppf);
            my_printf("cpb_cnt_minus1: %d  (v bits)", sps->hrd.cpb_cnt_minus1 );AddTreeItem(hp);
            my_printf("bit_rate_scale: %d  (4 bits)", sps->hrd.bit_rate_scale );AddTreeItem(hp);
            my_printf("cpb_size_scale: %d  (4 bits)", sps->hrd.cpb_size_scale );AddTreeItem(hp);
            int SchedSelIdx;
            for( SchedSelIdx = 0; SchedSelIdx <= sps->hrd.cpb_cnt_minus1; SchedSelIdx++ )
            {
                my_printf("bit_rate_value_minus1[%d]: %d  (v bits)", SchedSelIdx, sps->hrd.bit_rate_value_minus1[SchedSelIdx] ); // up to cpb_cnt_minus1, which is <= 31
                AddTreeItem(hp);
                my_printf("cpb_size_value_minus1[%d]: %d  (v bits)", SchedSelIdx, sps->hrd.cpb_size_value_minus1[SchedSelIdx] );
                AddTreeItem(hp);
                my_printf_flag2("cbr_flag", SchedSelIdx, sps->hrd.cbr_flag[SchedSelIdx] );
                AddTreeItem(hp);
            }
            my_printf("initial_cpb_removal_delay_length_minus1: %d  (5 bits)", sps->hrd.initial_cpb_removal_delay_length_minus1 );AddTreeItem(nhppf);
            my_printf("cpb_removal_delay_length_minus1: %d  (5 bits)", sps->hrd.cpb_removal_delay_length_minus1 );AddTreeItem(nhppf);
            my_printf("dpb_output_delay_length_minus1: %d  (5 bits)", sps->hrd.dpb_output_delay_length_minus1 );AddTreeItem(nhppf);
            my_printf("time_offset_length: %d  (5 bits)", sps->hrd.time_offset_length );AddTreeItem(nhppf);
        }
        my_printf_flag("vcl_hrd_parameters_present_flag", sps->vui.vcl_hrd_parameters_present_flag );
        HTREEITEM vhppf = AddTreeItem(vp);
        if( sps->vui.vcl_hrd_parameters_present_flag )
        {
            my_printf("hrd_parameters()");
            HTREEITEM hp = AddTreeItem(vhppf);
            my_printf("cpb_cnt_minus1: %d  (v bits)", sps->hrd.cpb_cnt_minus1 );AddTreeItem(hp);
            my_printf("bit_rate_scale: %d  (4 bits)", sps->hrd.bit_rate_scale );AddTreeItem(hp);
            my_printf("cpb_size_scale: %d  (4 bits)", sps->hrd.cpb_size_scale );AddTreeItem(hp);
            int SchedSelIdx;
            for( SchedSelIdx = 0; SchedSelIdx <= sps->hrd.cpb_cnt_minus1; SchedSelIdx++ )
            {
                my_printf("bit_rate_value_minus1[%d]: %d  (v bits)", SchedSelIdx, sps->hrd.bit_rate_value_minus1[SchedSelIdx] ); // up to cpb_cnt_minus1, which is <= 31
                AddTreeItem(hp);
                my_printf("cpb_size_value_minus1[%d]: %d  (v bits)", SchedSelIdx, sps->hrd.cpb_size_value_minus1[SchedSelIdx] );
                AddTreeItem(hp);
                my_printf_flag2("cbr_flag", SchedSelIdx, sps->hrd.cbr_flag[SchedSelIdx] );AddTreeItem(hp);
            }
            my_printf("initial_cpb_removal_delay_length_minus1: %d  (5 bits)", sps->hrd.initial_cpb_removal_delay_length_minus1 );AddTreeItem(nhppf);
            my_printf("cpb_removal_delay_length_minus1: %d  (5 bits)", sps->hrd.cpb_removal_delay_length_minus1 );AddTreeItem(nhppf);
            my_printf("dpb_output_delay_length_minus1: %d  (5 bits)", sps->hrd.dpb_output_delay_length_minus1 );AddTreeItem(nhppf);
            my_printf("time_offset_length: %d  (5 bits)", sps->hrd.time_offset_length );AddTreeItem(nhppf);
        }
        if( sps->vui.nal_hrd_parameters_present_flag || sps->vui.vcl_hrd_parameters_present_flag )
        {
            my_printf_flag("low_delay_hrd_flag", sps->vui.low_delay_hrd_flag );AddTreeItem(vp); // tocheck
        }
        my_printf_flag("pic_struct_present_flag", sps->vui.pic_struct_present_flag );AddTreeItem(vp);
        my_printf_flag("bitstream_restriction_flag", sps->vui.bitstream_restriction_flag );
        HTREEITEM brf = AddTreeItem(vp);
        if( sps->vui.bitstream_restriction_flag )
        {
            my_printf_flag("motion_vectors_over_pic_boundaries_flag", sps->vui.motion_vectors_over_pic_boundaries_flag );AddTreeItem(brf);
            my_printf("max_bytes_per_pic_denom: %d  (v bits)", sps->vui.max_bytes_per_pic_denom );AddTreeItem(brf);
            my_printf("max_bits_per_mb_denom: %d  (v bits)", sps->vui.max_bits_per_mb_denom );AddTreeItem(brf);
            my_printf("log2_max_mv_length_horizontal: %d  (v bits)", sps->vui.log2_max_mv_length_horizontal );AddTreeItem(brf);
            my_printf("log2_max_mv_length_vertical: %d  (v bits)", sps->vui.log2_max_mv_length_vertical );AddTreeItem(brf);
            my_printf("num_reorder_frames: %d  (v bits)", sps->vui.num_reorder_frames );AddTreeItem(brf);
            my_printf("max_dec_frame_buffering: %d  (v bits)", sps->vui.max_dec_frame_buffering );AddTreeItem(brf);
        }
    }
}

void CNalParser::h264_debug_pps(pps_t* pps, HTREEITEM root)
{
    my_printf("pic_parameter_set_rbsp()");
    HTREEITEM ipps = AddTreeItem(root);

    my_printf("pic_parameter_set_id: %d  (v bits)", pps->pic_parameter_set_id ); AddTreeItem(ipps);
    my_printf("seq_parameter_set_id: %d  (v bits)", pps->seq_parameter_set_id ); AddTreeItem(ipps);
    my_printf_flag("entropy_coding_mode_flag", pps->entropy_coding_mode_flag ); AddTreeItem(ipps);
    my_printf_flag("bottom_field_pic_order_in_frame_present_flag", pps->bottom_field_pic_order_in_frame_present_flag); AddTreeItem(ipps);
    my_printf("num_slice_groups_minus1: %d  (v bits)", pps->num_slice_groups_minus1 ); AddTreeItem(ipps);
    if( pps->num_slice_groups_minus1 > 0 )
    {
        my_printf("slice_group_map_type: %d  (v bits)", pps->slice_group_map_type ); AddTreeItem(ipps);
        if( pps->slice_group_map_type == 0 )
        {
            for( int i_group = 0; i_group <= pps->num_slice_groups_minus1; i_group++ )
                my_printf("run_length_minus1[%d]: %d  (v bits)", i_group, pps->run_length_minus1[i_group] );
        }
        else if( pps->slice_group_map_type == 2 )
        {
            for( int i_group = 0; i_group <= pps->num_slice_groups_minus1; i_group++ )
            {
                my_printf("top_left[%d]: %d  (v bits)", i_group, pps->top_left[i_group] );
                my_printf("bottom_right[%d]: %d  (v bits)", i_group, pps->bottom_right[i_group] );
            }
        }
        else if( pps->slice_group_map_type == 3 ||
            pps->slice_group_map_type == 4 ||
            pps->slice_group_map_type == 5 )
        {
            my_printf_flag("slice_group_change_direction_flag", pps->slice_group_change_direction_flag );
            my_printf("slice_group_change_rate_minus1: %d  (v bits)", pps->slice_group_change_rate_minus1 );
        }
        else if( pps->slice_group_map_type == 6 )
        {            
            my_printf("pic_size_in_map_units_minus1: %d  (v bits)", pps->pic_size_in_map_units_minus1 );
            for( int i = 0; i <= pps->pic_size_in_map_units_minus1; i++ )
                my_printf("slice_group_id[%d]: %d  (%d bits)", i, pps->slice_group_id[i], pps->slice_group_id_bytes);
        }
    }
    my_printf("num_ref_idx_l0_active_minus1: %d  (v bits)", pps->num_ref_idx_l0_active_minus1 ); AddTreeItem(ipps);
    my_printf("num_ref_idx_l1_active_minus1: %d  (v bits)", pps->num_ref_idx_l1_active_minus1 ); AddTreeItem(ipps);
    my_printf_flag("weighted_pred_flag", pps->weighted_pred_flag ); AddTreeItem(ipps);
    const char* weighted_pre = NULL;
    if (pps->weighted_bipred_idc == 0)
    {
        weighted_pre = "Default";
    }
    if (pps->weighted_bipred_idc == 1)
    {
        weighted_pre = "Explicit";
    }
    if (pps->weighted_bipred_idc == 2)
    {
        weighted_pre = "Implicit";
    }
    my_printf("weighted_bipred_idc: %d  [%s]  (2 bits)", pps->weighted_bipred_idc, weighted_pre); AddTreeItem(ipps);
    my_printf("pic_init_qp_minus26: %d  (v bits)", pps->pic_init_qp_minus26 ); AddTreeItem(ipps);
    my_printf("pic_init_qs_minus26: %d  (v bits)", pps->pic_init_qs_minus26 ); AddTreeItem(ipps);
    my_printf("chroma_qp_index_offset: %d  (v bits)", pps->chroma_qp_index_offset ); AddTreeItem(ipps);
    my_printf_flag("deblocking_filter_control_present_flag", pps->deblocking_filter_control_present_flag ); AddTreeItem(ipps);
    my_printf_flag("constrained_intra_pred_flag", pps->constrained_intra_pred_flag ); AddTreeItem(ipps);
    my_printf_flag("redundant_pic_cnt_present_flag", pps->redundant_pic_cnt_present_flag );
    AddTreeItem(ipps);
    if( pps->_more_rbsp_data_present )
    {
        my_printf("more_rbsp_data()" );
        HTREEITEM imrdp = AddTreeItem(ipps);
        my_printf_flag("transform_8x8_mode_flag", pps->transform_8x8_mode_flag ); AddTreeItem(imrdp);
        my_printf_flag("pic_scaling_matrix_present_flag", pps->pic_scaling_matrix_present_flag );
        HTREEITEM psmpf = AddTreeItem(imrdp);
        if( pps->pic_scaling_matrix_present_flag )
        {
            for( int i = 0; i < 6 + 2* pps->transform_8x8_mode_flag; i++ )
            {
                my_printf_flag2("pic_scaling_list_present_flag", i, pps->pic_scaling_list_present_flag[i] );
                 AddTreeItem(psmpf);
                if( pps->pic_scaling_list_present_flag[ i ] )
                {
                    if( i < 6 )
                    {
                        my_printf("ScalingList4x4[%d]: %d  (v bits)", i, *(pps->ScalingList4x4[i]) );
                        AddTreeItem(psmpf);
                    }
                    else
                    {
                        my_printf("ScalingList4xScalingList8x84[%d]: %d  (v bits)", i, *(pps->ScalingList8x8[i]) );
                        AddTreeItem(psmpf);
                    }
                }
            }
        }
        my_printf("second_chroma_qp_index_offset: %d  (v bits)", pps->second_chroma_qp_index_offset ); AddTreeItem(imrdp);
    }
    my_printf("rbsp_trailing_bits()"); AddTreeItem(ipps);
}

void CNalParser::h264_debug_slice_header(h264_stream_t* h, HTREEITEM root)
{
    sps_t* sps = h->sps;
    pps_t* pps = h->pps;
    slice_header_t* sh = h->sh;
    nal_t* nal = h->nal;

    my_printf("slice_layer_without_partitioning_rbsp()");
    HTREEITEM slice = AddTreeItem(root);
    my_printf("slice_header()");
    HTREEITEM iheader = AddTreeItem(slice);
    my_printf("first_mb_in_slice: %d  (v bits)", sh->first_mb_in_slice );
    const char* slice_type_name = NULL; AddTreeItem(iheader);
    switch(sh->slice_type)
    {
    case SH_SLICE_TYPE_P:       slice_type_name = "P slice"; break;
    case SH_SLICE_TYPE_B:       slice_type_name = "B slice"; break;
    case SH_SLICE_TYPE_I:       slice_type_name = "I slice"; break;
    case SH_SLICE_TYPE_SP:      slice_type_name = "SP slice"; break;
    case SH_SLICE_TYPE_SI:      slice_type_name = "SI slice"; break;
    case SH_SLICE_TYPE_P_ONLY:  slice_type_name = "P slice only"; break;
    case SH_SLICE_TYPE_B_ONLY:  slice_type_name = "B slice only"; break;
    case SH_SLICE_TYPE_I_ONLY:  slice_type_name = "I slice only"; break;
    case SH_SLICE_TYPE_SP_ONLY: slice_type_name = "SP slice only"; break;
    case SH_SLICE_TYPE_SI_ONLY: slice_type_name = "SI slice only"; break;
    default:                    slice_type_name = "Unknown"; break;
    }
    my_printf("slice_type: %d (%s)  (v bits)", sh->slice_type, slice_type_name ); AddTreeItem(iheader);

    my_printf("pic_parameter_set_id: %d  (v bits)", sh->pic_parameter_set_id );
    HTREEITEM ppsid = AddTreeItem(iheader);
    if (sps->separate_colour_plane_flag == 1)
    {
        my_printf("colour_plane_id: %d  (2 bits)", sh->colour_plane_id ); AddTreeItem(ppsid);
    }
    my_printf("frame_num: %d  (%d bits)", sh->frame_num, sh->frame_num_bytes ); AddTreeItem(iheader);
    if( !sps->frame_mbs_only_flag )
    {
        my_printf_flag("field_pic_flag", sh->field_pic_flag );
        HTREEITEM fpf = AddTreeItem(iheader);
        if( sh->field_pic_flag )
        {
            my_printf_flag("bottom_field_flag", sh->bottom_field_flag );
            AddTreeItem(fpf);
        }
    }
    if( nal->nal_unit_type == 5 )
    {
        my_printf("idr_pic_id: %d  (v bits)", sh->idr_pic_id ); AddTreeItem(iheader);
    }
    if( sps->pic_order_cnt_type == 0 )
    {
        my_printf("pic_order_cnt_lsb: %d  (%d bits)", sh->pic_order_cnt_lsb, sh->pic_order_cnt_lsb_bytes ); AddTreeItem(iheader);
        if( pps->bottom_field_pic_order_in_frame_present_flag && !sh->field_pic_flag )
        {
            my_printf("delta_pic_order_cnt_bottom: %d  (v bits)", sh->delta_pic_order_cnt_bottom ); AddTreeItem(iheader);
        }
    }

    if( sps->pic_order_cnt_type == 1 && !sps->delta_pic_order_always_zero_flag )
    {
        my_printf("delta_pic_order_cnt[0]: %d  (v bits)", sh->delta_pic_order_cnt[0] ); AddTreeItem(iheader);
        if( pps->bottom_field_pic_order_in_frame_present_flag && !sh->field_pic_flag )
        {
            my_printf("delta_pic_order_cnt[1]: %d  (v bits)", sh->delta_pic_order_cnt[1] );
            AddTreeItem(iheader);
        }
    }
    if( pps->redundant_pic_cnt_present_flag )
    {
        my_printf("redundant_pic_cnt: %d  (v bits)", sh->redundant_pic_cnt ); AddTreeItem(iheader);
    }
    if( is_slice_type( sh->slice_type, SH_SLICE_TYPE_B ) )
    {
        my_printf_flag("direct_spatial_mv_pred_flag", sh->direct_spatial_mv_pred_flag );
        AddTreeItem(iheader);
    }
    if( is_slice_type( sh->slice_type, SH_SLICE_TYPE_P ) || is_slice_type( sh->slice_type, SH_SLICE_TYPE_SP ) || is_slice_type( sh->slice_type, SH_SLICE_TYPE_B ) )
    {
        my_printf_flag("num_ref_idx_active_override_flag", sh->num_ref_idx_active_override_flag );
        AddTreeItem(iheader);
        if( sh->num_ref_idx_active_override_flag )
        {
            my_printf("num_ref_idx_l0_active_minus1: %d  (v bits)", sh->num_ref_idx_l0_active_minus1 );
            AddTreeItem(iheader);
            if( is_slice_type( sh->slice_type, SH_SLICE_TYPE_B ) )
            {
                my_printf("num_ref_idx_l1_active_minus1: %d  (v bits)", sh->num_ref_idx_l1_active_minus1 );
                AddTreeItem(iheader);
            }
        }
    }
    // ref_pic_list_modification
    if (nal->nal_unit_type == 20 || nal->nal_unit_type == 21)
    {
        // todo.....
        my_printf("ref_pic_list_mvc_modification()");
        HTREEITEM rplmm = AddTreeItem(iheader);
    }
    else
    {
        my_printf("ref_pic_list_modification()");
        HTREEITEM rplm = AddTreeItem(iheader);
        if( ! is_slice_type( sh->slice_type, SH_SLICE_TYPE_I ) && ! is_slice_type( sh->slice_type, SH_SLICE_TYPE_SI ) )
        {
            my_printf_flag("ref_pic_list_modification_flag_l0", sh->rplm.ref_pic_list_modification_flag_l0 );
            AddTreeItem(rplm);
            if( sh->rplm.ref_pic_list_modification_flag_l0 )
            {
                for (unsigned int i = 0; i < sh->rplm.rplm.size(); i++)
                {
                    my_printf("modification_of_pic_nums_idc: %d  (v bits)", sh->rplm.rplm[i].modification_of_pic_nums_idc);
                    AddTreeItem(rplm);
                    if( sh->rplm.rplm[i].modification_of_pic_nums_idc == 0 ||
                        sh->rplm.rplm[i].modification_of_pic_nums_idc == 1 )
                    {
                        my_printf("abs_diff_pic_num_minus1: %d  (v bits)", sh->rplm.rplm[i].abs_diff_pic_num_minus1 );
                        AddTreeItem(rplm);
                    }
                    else if( sh->rplm.rplm[i].modification_of_pic_nums_idc == 2 )
                    {
                        my_printf("long_term_pic_num: %d  (v bits)", sh->rplm.rplm[i].long_term_pic_num );
                        AddTreeItem(rplm);
                    }
                }
            }
        }
        if( is_slice_type( sh->slice_type, SH_SLICE_TYPE_B ) )
        {
            my_printf_flag("ref_pic_list_modification_flag_l1", sh->rplm.ref_pic_list_modification_flag_l1 );
            AddTreeItem(rplm);
            if( sh->rplm.ref_pic_list_modification_flag_l1 )
            {
                for (unsigned int i = 0; i < sh->rplm.rplm.size(); i++)
                {
                    my_printf("modification_of_pic_nums_idc: %d  (v bits)", sh->rplm.rplm[i].modification_of_pic_nums_idc );
                    AddTreeItem(rplm);
                    if( sh->rplm.rplm[i].modification_of_pic_nums_idc == 0 ||
                        sh->rplm.rplm[i].modification_of_pic_nums_idc == 1 )
                    {
                        my_printf("abs_diff_pic_num_minus1: %d  (v bits)", sh->rplm.rplm[i].abs_diff_pic_num_minus1 );
                        AddTreeItem(rplm);
                    }
                    else if( sh->rplm.rplm[i].modification_of_pic_nums_idc == 2 )
                    {
                        my_printf("long_term_pic_num: %d  (v bits)", sh->rplm.rplm[i].long_term_pic_num );
                        AddTreeItem(rplm);
                    }
                }
            }
        }
    }

    // pred_weight_table()
    if( ( pps->weighted_pred_flag && ( is_slice_type( sh->slice_type, SH_SLICE_TYPE_P ) || is_slice_type( sh->slice_type, SH_SLICE_TYPE_SP ) ) ) ||
        ( pps->weighted_bipred_idc == 1 && is_slice_type( sh->slice_type, SH_SLICE_TYPE_B ) ) )
    {
        my_printf("pred_weight_table()");
        HTREEITEM ipwt = AddTreeItem(iheader);
        my_printf("luma_log2_weight_denom: %d  (v bits)", sh->pwt.luma_log2_weight_denom ); AddTreeItem(ipwt);
        if( sps->ChromaArrayType != 0 )
        {
            my_printf("chroma_log2_weight_denom: %d  (v bits)", sh->pwt.chroma_log2_weight_denom );
            AddTreeItem(ipwt);
        }
        HTREEITEM ilwl0 = NULL;
        // 将luma和chroma分开显示
        if (sh->num_ref_idx_l0_active_minus1 >= 0)
        {
            my_printf("luma_weight_l0()");
            ilwl0 = AddTreeItem(ipwt);
        }
        for( int i = 0; i <= sh->num_ref_idx_l0_active_minus1; i++ )
        {
            my_printf_flag2("luma_weight_l0_flag", i, sh->pwt.luma_weight_l0_flag[i] );
            HTREEITEM ilwl0f = AddTreeItem(ilwl0);
            if( sh->pwt.luma_weight_l0_flag[i] )
            {
                my_printf("luma_weight_l0[%d]: %d  (v bits)", i, sh->pwt.luma_weight_l0[i] );
                AddTreeItem(ilwl0f);
                my_printf("luma_offset_l0[%d]: %d  (v bits)", i, sh->pwt.luma_offset_l0[i] );
                AddTreeItem(ilwl0f);
            }
        }
        HTREEITEM icwl0 = NULL;
        if (sh->num_ref_idx_l0_active_minus1 >= 0)
        {
            my_printf("chroma_weight_l0()");
            icwl0 = AddTreeItem(ipwt);
        }
        for( int i = 0; i <= sh->num_ref_idx_l0_active_minus1; i++ )
        {
            if ( sps->ChromaArrayType != 0 )
            {
                my_printf_flag2("chroma_weight_l0_flag", i, sh->pwt.chroma_weight_l0_flag[i] );
                HTREEITEM icwl0f = AddTreeItem(icwl0);
                if( sh->pwt.chroma_weight_l0_flag[i] )
                {
                    for( int j =0; j < 2; j++ )
                    {
                        my_printf("chroma_weight_l0[%d][%d]: %d  (v bits)", i, j, sh->pwt.chroma_weight_l0[i][j] );
                        AddTreeItem(icwl0f);
                        my_printf("chroma_weight_l0[%d][%d]: %d  (v bits)", i, j, sh->pwt.chroma_offset_l0[i][j] );
                        AddTreeItem(icwl0f);
                    }
                }
            }
        }
        if( is_slice_type( sh->slice_type, SH_SLICE_TYPE_B ) )
        {
            HTREEITEM ilwl1 = NULL;
            if (sh->num_ref_idx_l1_active_minus1 >= 0)
            {
                my_printf("luma_weight_l1()");
                ilwl1 = AddTreeItem(ipwt);
            }
            for( int i = 0; i <= sh->num_ref_idx_l1_active_minus1; i++ )
            {
                my_printf_flag2("luma_weight_l1_flag", i, sh->pwt.luma_weight_l1_flag[i] );
                HTREEITEM ilwl1f = AddTreeItem(ilwl1);
                if( sh->pwt.luma_weight_l1_flag[i] )
                {
                    my_printf("luma_weight_l1[%d]: %d  (v bits)", i, sh->pwt.luma_weight_l1[i] );
                    AddTreeItem(ilwl1f);
                    my_printf("luma_offset_l1[%d]: %d  (v bits)", i, sh->pwt.luma_offset_l1[i] );
                    AddTreeItem(ilwl1f);
                }
            }
            HTREEITEM icwl1 = NULL;
            if (sh->num_ref_idx_l1_active_minus1 >= 0)
            {
                my_printf("chroma_weight_l1()");
                icwl1 = AddTreeItem(ipwt);
            }
            for( int i = 0; i <= sh->num_ref_idx_l1_active_minus1; i++ )
            {
                if ( sps->ChromaArrayType != 0 )
                {
                    my_printf_flag2("chroma_weight_l1_flag", i, sh->pwt.chroma_weight_l1_flag[i] );
                    HTREEITEM icwl1f = AddTreeItem(icwl1);
                    if( sh->pwt.chroma_weight_l1_flag[i] )
                    {
                        for( int j =0; j < 2; j++ )
                        {
                            my_printf("chroma_weight_l1[%d][%d]: %d  (v bits)", i, j, sh->pwt.chroma_weight_l1[i][j] );
                            AddTreeItem(icwl1f);
                            my_printf("chroma_offset_l1[%d][%d]: %d  (v bits)", i, j, sh->pwt.chroma_offset_l1[i][j] );
                            AddTreeItem(icwl1f);
                        }
                    }
                }
            }
        }
    }
    // dec_ref_pic_marking()
    if( nal->nal_ref_idc != 0 )
    {
        my_printf("dec_ref_pic_marking()");
        HTREEITEM idrpm = AddTreeItem(iheader);
        if( h->nal->nal_unit_type == 5 )
        {
            my_printf_flag("no_output_of_prior_pics_flag", sh->drpm.no_output_of_prior_pics_flag );
            AddTreeItem(idrpm);
            my_printf_flag("long_term_reference_flag", sh->drpm.long_term_reference_flag );
            AddTreeItem(idrpm);
        }
        else
        {
            my_printf_flag("adaptive_ref_pic_marking_mode_flag", sh->drpm.adaptive_ref_pic_marking_mode_flag );
            HTREEITEM arpmmf = AddTreeItem(idrpm);
            if( sh->drpm.adaptive_ref_pic_marking_mode_flag )
            {
                for (unsigned int i = 0; i < sh->drpm.drpm.size(); i++)
                {
                    my_printf("memory_management_control_operation[%u]: %d  (v bits)", i, sh->drpm.drpm[i].memory_management_control_operation );
                    AddTreeItem(arpmmf);
                    if( sh->drpm.drpm[i].memory_management_control_operation == 1 ||
                        sh->drpm.drpm[i].memory_management_control_operation == 3 )
                    {
                        my_printf("difference_of_pic_nums_minus1[%u]: %d  (v bits)", i, sh->drpm.drpm[i].difference_of_pic_nums_minus1 );
                        AddTreeItem(arpmmf);
                    }
                    if(sh->drpm.drpm[i].memory_management_control_operation == 2 )
                    {
                        my_printf("long_term_pic_num[%u]: %d  (v bits)", i, sh->drpm.drpm[i].long_term_pic_num );
                        AddTreeItem(arpmmf);
                    }
                    if( sh->drpm.drpm[i].memory_management_control_operation == 3 ||
                        sh->drpm.drpm[i].memory_management_control_operation == 6 )
                    {
                        my_printf("long_term_frame_idx[%u]: %d  (v bits)", i, sh->drpm.drpm[i].long_term_frame_idx );
                        AddTreeItem(arpmmf);
                    }
                    if( sh->drpm.drpm[i].memory_management_control_operation == 4 )
                    {
                        my_printf("max_long_term_frame_idx_plus1[%u]: %d  (v bits)", i, sh->drpm.drpm[i].max_long_term_frame_idx_plus1 );
                        AddTreeItem(arpmmf);
                    }
                }
            }
        }
    }
    if( pps->entropy_coding_mode_flag && ! is_slice_type( sh->slice_type, SH_SLICE_TYPE_I ) && ! is_slice_type( sh->slice_type, SH_SLICE_TYPE_SI ) )
    {
        my_printf("cabac_init_idc: %d", sh->cabac_init_idc );
        AddTreeItem(iheader);
    }
    my_printf("slice_qp_delta: %d", sh->slice_qp_delta );
    AddTreeItem(iheader);
    if( is_slice_type( sh->slice_type, SH_SLICE_TYPE_SP ) || is_slice_type( sh->slice_type, SH_SLICE_TYPE_SI ) )
    {
        if( is_slice_type( sh->slice_type, SH_SLICE_TYPE_SP ) )
        {
            my_printf_flag("sp_for_switch_flag", sh->sp_for_switch_flag );
            AddTreeItem(iheader);
        }
        my_printf("slice_qs_delta: %d  (v bits)", sh->slice_qs_delta );
        AddTreeItem(iheader);
    }
    if( pps->deblocking_filter_control_present_flag )
    {
        my_printf("disable_deblocking_filter_idc: %d  (v bits)", sh->disable_deblocking_filter_idc );
        AddTreeItem(iheader);
        if( sh->disable_deblocking_filter_idc != 1 )
        {
            my_printf("slice_alpha_c0_offset_div2: %d  (v bits)", sh->slice_alpha_c0_offset_div2 );
            AddTreeItem(iheader);
            my_printf("slice_beta_offset_div2: %d  (v bits)", sh->slice_beta_offset_div2 );
            AddTreeItem(iheader);
        }
    }

    if( pps->num_slice_groups_minus1 > 0 &&
        pps->slice_group_map_type >= 3 && pps->slice_group_map_type <= 5)
    {
        my_printf("slice_group_change_cycle: %d  (%d bits)", sh->slice_group_change_cycle, sh->slice_group_change_cycle_bytes );
        AddTreeItem(iheader);
    }

    my_printf("slice_data()"); AddTreeItem(slice);
    my_printf("rbsp_slice_trailing_bits()"); AddTreeItem(slice);
}

void CNalParser::h264_debug_aud(aud_t* aud, HTREEITEM root)
{
    my_printf("access_unit_delimiter_rbsp()");
    HTREEITEM iaud = AddTreeItem(root);
    const char* primary_pic_type_name = NULL;
    switch (aud->primary_pic_type)
    {
    case AUD_PRIMARY_PIC_TYPE_I:       primary_pic_type_name = "I"; break;
    case AUD_PRIMARY_PIC_TYPE_IP:      primary_pic_type_name = "I, P"; break;
    case AUD_PRIMARY_PIC_TYPE_IPB:     primary_pic_type_name = "I, P, B"; break;
    case AUD_PRIMARY_PIC_TYPE_SI:      primary_pic_type_name = "SI"; break;
    case AUD_PRIMARY_PIC_TYPE_SISP:    primary_pic_type_name = "SI, SP"; break;
    case AUD_PRIMARY_PIC_TYPE_ISI:     primary_pic_type_name = "I, SI"; break;
    case AUD_PRIMARY_PIC_TYPE_ISIPSP:  primary_pic_type_name = "I, SI, P, SP"; break;
    case AUD_PRIMARY_PIC_TYPE_ISIPSPB: primary_pic_type_name = "I, SI, P, SP, B"; break;
    default: primary_pic_type_name = "Unknown"; break;
    }
    my_printf("primary_pic_type: %d (%s)  (3 bits)", aud->primary_pic_type, primary_pic_type_name );
    AddTreeItem(iaud);
}

void CNalParser::h264_debug_seis( h264_stream_t* h, HTREEITEM root)
{
    sei_t** seis = h->seis;
    int num_seis = h->num_seis;
    int i = 0;

    my_printf("sei_rbsp()");
    HTREEITEM isei = AddTreeItem(root);
    my_printf("sei_message()");
    HTREEITEM iisei = AddTreeItem(isei);

    for (i = 0; i < num_seis; i++)
    {
        sei_t* s = seis[i];
        my_printf("payloadType: %d  (v bits)", s->payloadType); AddTreeItem(iisei);
        my_printf("payloadSize: %d  (v bits)", s->payloadSize); AddTreeItem(iisei);
        my_printf("sei_payload()");
        HTREEITEM sp = AddTreeItem(iisei);
        switch(s->payloadType)
        {
        case SEI_TYPE_BUFFERING_PERIOD:
            my_printf("buffering_period()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_PIC_TIMING:
            my_printf("pic_timing()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_PAN_SCAN_RECT:
            my_printf("pan_scan_rect()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_FILLER_PAYLOAD:
            my_printf("filler_payload()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_USER_DATA_REGISTERED_ITU_T_T35:
            my_printf("user_data_registered_itu_t_t35()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_USER_DATA_UNREGISTERED :
            {
                char uuid[64] = {0};
                char tmp[8] = {0};
                for (int j = 0; j < 16; j++)
                {
                    sprintf(tmp, "%X", s->payload[j]);
                    strcat(uuid, tmp);
                }
                my_printf("uuid_iso_iec_11578: %s", uuid);
                HTREEITEM udpb = AddTreeItem(sp);
                for (int j = 16; j < s->payloadSize && s->payload[j] != 0; j++)
                {
                    my_printf("user_data_payload_byte: %d('%c')", s->payload[j], s->payload[j]);
                    AddTreeItem(sp);
                }
            }
            break;
        case SEI_TYPE_RECOVERY_POINT:
            my_printf("recovery_point()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_DEC_REF_PIC_MARKING_REPETITION:
            my_printf("Dec ref pic marking repetition()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_SPARE_PIC:
            my_printf("Spare pic()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_SCENE_INFO:
            my_printf("scene_info()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_SUB_SEQ_INFO:
            my_printf("Sub seq info()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_SUB_SEQ_LAYER_CHARACTERISTICS:
            my_printf("Sub seq layer characteristics()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_SUB_SEQ_CHARACTERISTICS:
            my_printf("Sub seq characteristics()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_FULL_FRAME_FREEZE:
            my_printf("Full frame freeze()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_FULL_FRAME_FREEZE_RELEASE:
            my_printf("Full frame freeze release()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_FULL_FRAME_SNAPSHOT:
            my_printf("picture_snapshot()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_PROGRESSIVE_REFINEMENT_SEGMENT_START:
            my_printf("progressive_refinement_segment_start()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_PROGRESSIVE_REFINEMENT_SEGMENT_END:
            my_printf("progressive_refinement_segment_end()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_MOTION_CONSTRAINED_SLICE_GROUP_SET:
            my_printf("Motion constrained slice group set()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_FILM_GRAIN_CHARACTERISTICS:
            my_printf("film_grain_characteristics()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_DEBLOCKING_FILTER_DISPLAY_PREFERENCE:
            my_printf("Deblocking filter display preference()"); AddTreeItem(sp);
            break;
        case SEI_TYPE_STEREO_VIDEO_INFO:
            my_printf("Stereo video info()"); AddTreeItem(sp);
            break;
        default:
            my_printf("Unknown()"); AddTreeItem(sp);
            break;
        }
    }

    my_printf("rbsp_trailing_bits()"); AddTreeItem(isei);
}

/**
 Print the contents of a NAL unit to standard output.
 The NAL which is printed out has a type determined by nal and data which comes from other fields within h depending on its type.
 @param[in]      h          the stream object
 @param[in]      nal        the nal unit
 */
void CNalParser::h264_debug_nal_t(h264_stream_t* h, nal_t* nal)
{
    m_pTree->DeleteAllItems();
    my_printf("NAL");
    HTREEITEM root = AddTreeItem(TVI_ROOT);

    my_printf("forbidden_zero_bit: %d (1 bit)", nal->forbidden_zero_bit ); AddTreeItem(root);
    my_printf("nal_ref_idc: %d (2 bits)", nal->nal_ref_idc ); AddTreeItem(root);
    // TODO make into subroutine
    const char* nal_unit_type_name;
    switch (nal->nal_unit_type)
    {
    case  NAL_UNIT_TYPE_UNSPECIFIED:                   nal_unit_type_name = "Unspecified"; break;
    case  NAL_UNIT_TYPE_CODED_SLICE_NON_IDR:           nal_unit_type_name = "Coded slice of a non-IDR picture"; break;
    case  NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_A:  nal_unit_type_name = "Coded slice data partition A"; break;
    case  NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_B:  nal_unit_type_name = "Coded slice data partition B"; break;
    case  NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_C:  nal_unit_type_name = "Coded slice data partition C"; break;
    case  NAL_UNIT_TYPE_CODED_SLICE_IDR:               nal_unit_type_name = "Coded slice of an IDR picture"; break;
    case  NAL_UNIT_TYPE_SEI:                           nal_unit_type_name = "Supplemental enhancement information (SEI)"; break;
    case  NAL_UNIT_TYPE_SPS:                           nal_unit_type_name = "Sequence parameter set"; break;
    case  NAL_UNIT_TYPE_PPS:                           nal_unit_type_name = "Picture parameter set"; break;
    case  NAL_UNIT_TYPE_AUD:                           nal_unit_type_name = "Access unit delimiter"; break;
    case  NAL_UNIT_TYPE_END_OF_SEQUENCE:               nal_unit_type_name = "End of sequence"; break;
    case  NAL_UNIT_TYPE_END_OF_STREAM:                 nal_unit_type_name = "End of stream"; break;
    case  NAL_UNIT_TYPE_FILLER:                        nal_unit_type_name = "Filler data"; break;
    case  NAL_UNIT_TYPE_SPS_EXT:                       nal_unit_type_name = "Sequence parameter set extension"; break;
        // 14..18    // Reserved
    case  NAL_UNIT_TYPE_CODED_SLICE_AUX:               nal_unit_type_name = "Coded slice of an auxiliary coded picture without partitioning"; break;
        // 20..23    // Reserved
        // 24..31    // Unspecified
    default:                                           nal_unit_type_name = "Unknown"; break;
    }
    my_printf("nal_unit_type: %d (%s) (5 bits)", nal->nal_unit_type, nal_unit_type_name );
    AddTreeItem(root);

    if( nal->nal_unit_type == NAL_UNIT_TYPE_CODED_SLICE_NON_IDR) { h264_debug_slice_header(h, root); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_CODED_SLICE_IDR) { h264_debug_slice_header(h, root); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_SPS) { h264_debug_sps(h->sps, root); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_PPS) { h264_debug_pps(h->pps, root); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_AUD) { h264_debug_aud(h->aud, root); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_SEI) { h264_debug_seis(h, root); }
}

////////////////////////////////////////////////////////
void CNalParser::h265_debug_ptl(profile_tier_level_t* ptl, int profilePresentFlag, int max_sub_layers_minus1, HTREEITEM root)
{
    if (profilePresentFlag)
    {
        my_printf("general_profile_space: %d  (2 bits)", ptl->general_profile_space); AddTreeItem(root);
        my_printf_flag("general_tier_flag", ptl->general_tier_flag); AddTreeItem(root);
        my_printf("general_profile_idc: %d  (5 bits)", ptl->general_profile_idc); AddTreeItem(root);
        for (int i = 0; i < 32; i++)
        {
            my_printf_flag2("general_profile_compatibility_flag", i, ptl->general_profile_compatibility_flag[i]); AddTreeItem(root);
        }
        my_printf_flag("general_progressive_source_flag", ptl->general_progressive_source_flag); AddTreeItem(root);
        my_printf_flag("general_interlaced_source_flag", ptl->general_interlaced_source_flag); AddTreeItem(root);
        my_printf_flag("general_non_packed_constraint_flag", ptl->general_non_packed_constraint_flag); AddTreeItem(root);
        my_printf_flag("general_frame_only_constraint_flag", ptl->general_frame_only_constraint_flag); AddTreeItem(root);
        if (ptl->general_profile_idc==4 || ptl->general_profile_compatibility_flag[4] ||
            ptl->general_profile_idc==5 || ptl->general_profile_compatibility_flag[5] ||
            ptl->general_profile_idc==6 || ptl->general_profile_compatibility_flag[6] ||
            ptl->general_profile_idc==7 || ptl->general_profile_compatibility_flag[7])
        {
            my_printf_flag("general_max_12bit_constraint_flag", ptl->general_max_12bit_constraint_flag); AddTreeItem(root);
            my_printf_flag("general_max_10bit_constraint_flag", ptl->general_max_10bit_constraint_flag); AddTreeItem(root);
            my_printf_flag("general_max_8bit_constraint_flag", ptl->general_max_8bit_constraint_flag); AddTreeItem(root);
            my_printf_flag("general_max_422chroma_constraint_flag", ptl->general_max_422chroma_constraint_flag); AddTreeItem(root);
            my_printf_flag("general_max_420chroma_constraint_flag", ptl->general_max_420chroma_constraint_flag); AddTreeItem(root);
            my_printf_flag("general_max_monochrome_constraint_flag", ptl->general_max_monochrome_constraint_flag); AddTreeItem(root);
            my_printf_flag("general_intra_constraint_flag", ptl->general_intra_constraint_flag); AddTreeItem(root);
            my_printf_flag("general_one_picture_only_constraint_flag", ptl->general_one_picture_only_constraint_flag); AddTreeItem(root);
            my_printf_flag("general_lower_bit_rate_constraint_flag", ptl->general_lower_bit_rate_constraint_flag); AddTreeItem(root);
            my_printf("general_reserved_zero_34bits: %u  (34 bits)", ptl->general_reserved_zero_34bits); AddTreeItem(root); // tocheck
        }
        else
        {
            my_printf("general_reserved_zero_43bits: %u  (43 bits)", ptl->general_reserved_zero_43bits); AddTreeItem(root);// tocheck
        }
        if ((ptl->general_profile_idc>=1 && ptl->general_profile_idc<=5) ||
            ptl->general_profile_compatibility_flag[1] || ptl->general_profile_compatibility_flag[2] ||
            ptl->general_profile_compatibility_flag[3] || ptl->general_profile_compatibility_flag[4] ||
            ptl->general_profile_compatibility_flag[5])
        {
            my_printf_flag("general_inbld_flag", ptl->general_inbld_flag); AddTreeItem(root);
        }
        else
        {
            my_printf("general_reserved_zero_bit: %d  (1 bit)", ptl->general_reserved_zero_bit); AddTreeItem(root);
        }
    }
        
    my_printf("general_level_idc: %d  (8 bits)", ptl->general_level_idc); AddTreeItem(root);
    for (int i = 0; i < max_sub_layers_minus1; i++)
    {
        my_printf_flag2("sub_layer_profile_present_flag", i, ptl->sub_layer_profile_present_flag[i]); AddTreeItem(root);
        my_printf_flag2("sub_layer_level_present_flag", i, ptl->sub_layer_level_present_flag[i]); AddTreeItem(root);
    }
    if (max_sub_layers_minus1 > 0)
    {
        for (int i = max_sub_layers_minus1; i < 8; i++)
        {
            my_printf("reserved_zero_2bits[%d]: %d  (2 bits)", i, ptl->reserved_zero_2bits[i]); AddTreeItem(root);
        }
    }
    for (int i = 0; i < max_sub_layers_minus1; i++)
    {
        if (ptl->sub_layer_profile_present_flag[i])
        {
            my_printf("sub_layer_profile_space[%d]: %d  (2 bits)", i, ptl->sub_layer_profile_space[i]); AddTreeItem(root);
            my_printf_flag2("sub_layer_tier_flag", i, ptl->sub_layer_tier_flag[i]); AddTreeItem(root);
            my_printf("sub_layer_profile_idc[%d]: %d  (5 bits)", i, ptl->sub_layer_profile_idc[i]); AddTreeItem(root);
            for (int j = 0; j < 32; j++)
            {
                my_printf_flag3("sub_layer_profile_compatibility_flag", i, j, ptl->sub_layer_profile_compatibility_flag[i][j]);
                AddTreeItem(root);
            }
            my_printf_flag2("sub_layer_progressive_source_flag", i, ptl->sub_layer_progressive_source_flag[i]);AddTreeItem(root);
            my_printf_flag2("sub_layer_interlaced_source_flag", i, ptl->sub_layer_interlaced_source_flag[i]);AddTreeItem(root);
            my_printf_flag2("sub_layer_non_packed_constraint_flag", i, ptl->sub_layer_non_packed_constraint_flag[i]);AddTreeItem(root);
            my_printf_flag2("sub_layer_frame_only_constraint_flag", i, ptl->sub_layer_frame_only_constraint_flag[i]);AddTreeItem(root);
            if (ptl->sub_layer_profile_idc[i]==4 || ptl->sub_layer_profile_compatibility_flag[i][4] ||
                ptl->sub_layer_profile_idc[i]==5 || ptl->sub_layer_profile_compatibility_flag[i][5] ||
                ptl->sub_layer_profile_idc[i]==6 || ptl->sub_layer_profile_compatibility_flag[i][6] ||
                ptl->sub_layer_profile_idc[i]==7 || ptl->sub_layer_profile_compatibility_flag[i][7])
            {
                my_printf_flag2("sub_layer_max_12bit_constraint_flag", i, ptl->sub_layer_max_12bit_constraint_flag[i]);AddTreeItem(root);
                my_printf_flag2("sub_layer_max_10bit_constraint_flag", i, ptl->sub_layer_max_10bit_constraint_flag[i]);AddTreeItem(root);
                my_printf_flag2("sub_layer_max_8bit_constraint_flag", i, ptl->sub_layer_max_8bit_constraint_flag[i]);AddTreeItem(root);
                my_printf_flag2("sub_layer_max_422chroma_constraint_flag", i, ptl->sub_layer_max_422chroma_constraint_flag[i]);AddTreeItem(root);
                my_printf_flag2("sub_layer_max_420chroma_constraint_flag", i, ptl->sub_layer_max_420chroma_constraint_flag[i]);AddTreeItem(root);
                my_printf_flag2("sub_layer_max_monochrome_constraint_flag", i, ptl->sub_layer_max_monochrome_constraint_flag[i]);AddTreeItem(root);
                my_printf_flag2("sub_layer_intra_constraint_flag", i, ptl->sub_layer_intra_constraint_flag[i]);AddTreeItem(root);
                my_printf_flag2("sub_layer_one_picture_only_constraint_flag", i, ptl->sub_layer_one_picture_only_constraint_flag[i]);AddTreeItem(root);
                my_printf_flag2("sub_layer_lower_bit_rate_constraint_flag", i, ptl->sub_layer_lower_bit_rate_constraint_flag[i]);AddTreeItem(root);
                my_printf("sub_layer_reserved_zero_34bits[%d]: %ul  (34 bits)", i, ptl->sub_layer_reserved_zero_34bits[i]);AddTreeItem(root);
            }
            else
            {
                my_printf("sub_layer_reserved_zero_43bits: %ul  (43 bits)", ptl->sub_layer_reserved_zero_43bits[i]);AddTreeItem(root);
            }
            // to check
            if ((ptl->sub_layer_profile_idc[i]>=1 && ptl->sub_layer_profile_idc[i]<=5) ||
                ptl->sub_layer_profile_compatibility_flag[i][1] ||
                ptl->sub_layer_profile_compatibility_flag[i][2] ||
                ptl->sub_layer_profile_compatibility_flag[i][3] ||
                ptl->sub_layer_profile_compatibility_flag[i][4] ||
                ptl->sub_layer_profile_compatibility_flag[i][5])
            {
                my_printf_flag2("sub_layer_inbld_flag", i, ptl->sub_layer_inbld_flag[i]);AddTreeItem(root);
            }
            else
            {
                my_printf("sub_layer_reserved_zero_bit[%d]: %d  (1 bit)", i, ptl->sub_layer_reserved_zero_bit[i]);AddTreeItem(root);
            }
        }
        if (ptl->sub_layer_level_present_flag[i])
        {
            my_printf("sub_layer_level_idc[%d]: %d  (8 bits)", i, ptl->sub_layer_level_idc[i]);AddTreeItem(root);
        }
    }
}

void CNalParser::h265_debug_sub_layer_hrd_parameters(sub_layer_hrd_parameters_t* subhrd, int sub_pic_hrd_params_present_flag, int CpbCnt, int subLayerId, const char* p, HTREEITEM root)
{
    my_printf("[%s] sub_layer_hrd_parameters(%d)", p, subLayerId);
    HTREEITEM slhp = AddTreeItem(root);
    for (int i = 0; i <= CpbCnt; i++)
    {
        my_printf("bit_rate_value_minus1[%d]: %d  (v bits)", i, subhrd->bit_rate_value_minus1[i]);AddTreeItem(slhp);
        my_printf("cpb_size_value_minus1[%d]: %d  (v bits)", i, subhrd->cpb_size_value_minus1[i]);AddTreeItem(slhp);
        if (sub_pic_hrd_params_present_flag)
        {
            my_printf("cpb_size_du_value_minus1[%d]: %d  (v bits)", i, subhrd->cpb_size_du_value_minus1[i]);AddTreeItem(slhp);
            my_printf("bit_rate_du_value_minus1[%d]: %d  (v bits)", i, subhrd->bit_rate_du_value_minus1[i]);AddTreeItem(slhp);
        }
        my_printf_flag2("cbr_flag", i, subhrd->cbr_flag[i]);AddTreeItem(slhp);
    }
}
void CNalParser::h265_debug_hrd_parameters(hrd_parameters_t* hrd, int commonInfPresentFlag, int maxNumSubLayersMinus1, HTREEITEM root)
{
    my_printf("hrd_parameters()");
    HTREEITEM ihrd = AddTreeItem(root);
    if(commonInfPresentFlag)
    {
        my_printf_flag("nal_hrd_parameters_present_flag", hrd->nal_hrd_parameters_present_flag); AddTreeItem(ihrd);
        my_printf_flag("vcl_hrd_parameters_present_flag", hrd->vcl_hrd_parameters_present_flag); AddTreeItem(ihrd);
        if (hrd->nal_hrd_parameters_present_flag ||
            hrd->vcl_hrd_parameters_present_flag)
        {
            my_printf_flag("sub_pic_hrd_params_present_flag", hrd->sub_pic_hrd_params_present_flag); AddTreeItem(ihrd);
            if (hrd->sub_pic_hrd_params_present_flag)
            {
                my_printf("tick_divisor_minus2: %d  (8 bits)", hrd->tick_divisor_minus2); AddTreeItem(ihrd);
                my_printf("du_cpb_removal_delay_increment_length_minus1: %d  (8 bits)", hrd->du_cpb_removal_delay_increment_length_minus1); AddTreeItem(ihrd);
                my_printf_flag("sub_pic_cpb_params_in_pic_timing_sei_flag", hrd->sub_pic_cpb_params_in_pic_timing_sei_flag); AddTreeItem(ihrd);
                my_printf("dpb_output_delay_du_length_minus1: %d  (5 bits)", hrd->dpb_output_delay_du_length_minus1); AddTreeItem(ihrd);
            }
            my_printf("bit_rate_scale: %d  (4 bits)", hrd->bit_rate_scale); AddTreeItem(ihrd);
            my_printf("cpb_size_scale: %d  (4 bits)", hrd->cpb_size_scale); AddTreeItem(ihrd);
            if (hrd->sub_pic_hrd_params_present_flag)
                my_printf("cpb_size_du_scale: %d  (4 bits)", hrd->cpb_size_du_scale); AddTreeItem(ihrd);
            my_printf("initial_cpb_removal_delay_length_minus1: %d  (5 bits)", hrd->initial_cpb_removal_delay_length_minus1); AddTreeItem(ihrd);
            my_printf("au_cpb_removal_delay_length_minus1: %d  (5 bits)", hrd->au_cpb_removal_delay_length_minus1); AddTreeItem(ihrd);
            my_printf("dpb_output_delay_length_minus1: %d  (5 bits)", hrd->dpb_output_delay_length_minus1); AddTreeItem(ihrd);
        }
    }
    for (int i = 0; i <= maxNumSubLayersMinus1; i++)
    {
        my_printf_flag2("fixed_pic_rate_general_flag", i, hrd->fixed_pic_rate_general_flag[i]); AddTreeItem(ihrd);
        HTREEITEM fprwcf = ihrd;
        if (!hrd->fixed_pic_rate_general_flag[i])
        {
            my_printf_flag2("fixed_pic_rate_within_cvs_flag", i, hrd->fixed_pic_rate_general_flag[i]);
            fprwcf = AddTreeItem(ihrd);
        }
        if (hrd->fixed_pic_rate_within_cvs_flag[i])
        {
            my_printf("elemental_duration_in_tc_minus1[%d]: %d  (v bits)", i, hrd->elemental_duration_in_tc_minus1[i]);
            AddTreeItem(fprwcf);
        }
        else
        {
            my_printf_flag2("low_delay_hrd_flag", i, hrd->low_delay_hrd_flag[i]);
            AddTreeItem(fprwcf);
        }
        if (!hrd->low_delay_hrd_flag[i])
        {
            my_printf("cpb_cnt_minus1[%d]: %d  (v bits)", i, hrd->cpb_cnt_minus1[i]); AddTreeItem(ihrd);
        }
            
        if(hrd->nal_hrd_parameters_present_flag)
        {
            h265_debug_sub_layer_hrd_parameters(&(hrd->sub_layer_hrd_parameters), hrd->sub_pic_hrd_params_present_flag, hrd->cpb_cnt_minus1[i], i, "nal", ihrd);
        }
        if(hrd->vcl_hrd_parameters_present_flag)
        {
            h265_debug_sub_layer_hrd_parameters(&(hrd->sub_layer_hrd_parameters), hrd->sub_pic_hrd_params_present_flag, hrd->cpb_cnt_minus1[i], i, "vcl", ihrd);
        }
    }
}

// vps
void CNalParser::h265_debug_vps(h265_vps_t* vps, HTREEITEM root)
{
    int i, j;
    my_printf("video_parameter_set_rbsp()");
    HTREEITEM ivps = AddTreeItem(root);
    my_printf("vps_video_parameter_set_id: %d  (4 bits)", vps->vps_video_parameter_set_id); AddTreeItem(ivps);
    my_printf_flag("vps_base_layer_internal_flag", vps->vps_base_layer_internal_flag); AddTreeItem(ivps);
    my_printf_flag("vps_base_layer_available_flag", vps->vps_base_layer_available_flag); AddTreeItem(ivps);
    my_printf("vps_max_layers_minus1: %d  (6 bits)", vps->vps_max_layers_minus1); AddTreeItem(ivps);
    my_printf("vps_max_sub_layers_minus1: %d  (3 bits)", vps->vps_max_sub_layers_minus1); AddTreeItem(ivps);
    my_printf_flag("vps_temporal_id_nesting_flag", vps->vps_temporal_id_nesting_flag); AddTreeItem(ivps);
    my_printf("vps_reserved_0xffff_16bits: %d  (16 bits)", vps->vps_reserved_0xffff_16bits); AddTreeItem(ivps);
    // ptl
    my_printf("profile_tier_level()");
    HTREEITEM iptl = AddTreeItem(ivps);
    h265_debug_ptl(&vps->ptl, 1, vps->vps_max_layers_minus1, iptl);
    
    my_printf_flag("vps_sub_layer_ordering_info_present_flag", vps->vps_sub_layer_ordering_info_present_flag);
    AddTreeItem(ivps);
    
    if (vps->vps_sub_layer_ordering_info_present_flag)
    {
        my_printf("SubLayers");
        HTREEITEM isub = AddTreeItem(ivps);
        for (i = (vps->vps_sub_layer_ordering_info_present_flag ? 0 : vps->vps_max_sub_layers_minus1);
            i <= vps->vps_max_sub_layers_minus1; i++ )
        {
            my_printf("vps_max_dec_pic_buffering_minus1[%d]: %d  (v bits)", i, vps->vps_max_dec_pic_buffering_minus1[i]); AddTreeItem(isub);
            my_printf("vps_max_num_reorder_pics[%d]: %d  (v bits)", i, vps->vps_max_num_reorder_pics[i]); AddTreeItem(isub);
            my_printf("vps_max_latency_increase_plus1[%d]: %d  (v bits)", i, vps->vps_max_latency_increase_plus1[i]); AddTreeItem(isub);
        }
    }

    my_printf("vps_max_layer_id: %d  (6 bits)", vps->vps_max_layer_id); AddTreeItem(ivps);
    my_printf("vps_num_layer_sets_minus1: %d  (v bits)", vps->vps_num_layer_sets_minus1); AddTreeItem(ivps);
    for (i = 1; i <= vps->vps_num_layer_sets_minus1; i++)
    {
        for (j = 0; j <= vps->vps_max_layer_id; j++)
        {
            my_printf_flag3("layer_id_included_flag", i, j, vps->layer_id_included_flag[i][j]);AddTreeItem(ivps);
        }
    }
    my_printf_flag("vps_timing_info_present_flag", vps->vps_timing_info_present_flag);
    HTREEITEM tipf = AddTreeItem(ivps);
    if (vps->vps_timing_info_present_flag)
    {
        my_printf("vps_num_units_in_tick: %d  (32 bits)", vps->vps_num_units_in_tick); AddTreeItem(tipf);
        my_printf("vps_time_scale: %d  (32 bits)", vps->vps_time_scale); AddTreeItem(tipf);
        my_printf_flag("vps_poc_proportional_to_timing_flag", vps->vps_poc_proportional_to_timing_flag);
        HTREEITEM ppttf = AddTreeItem(tipf);
        if (vps->vps_poc_proportional_to_timing_flag)
        {
            my_printf("vps_num_ticks_poc_diff_one_minus1: %d  (v bits)", vps->vps_num_ticks_poc_diff_one_minus1); AddTreeItem(ppttf);
        }
        my_printf("vps_num_hrd_parameters: %d  (v bits)", vps->vps_num_hrd_parameters);AddTreeItem(tipf);
        for (i = 0; i < vps->vps_num_hrd_parameters; i++)
        {
            my_printf("hrd_layer_set_idx[%d]: %d  (v bits)", i, vps->hrd_layer_set_idx[i]); AddTreeItem(tipf);
            if (i > 0)
            {
                my_printf_flag2("cprms_present_flag", i, vps->cprms_present_flag[i]); AddTreeItem(tipf);
            }
            //  hrd_parameters()
            h265_debug_hrd_parameters(&(vps->hrd_parameters), vps->cprms_present_flag[i], vps->vps_max_sub_layers_minus1, tipf);
        }
    }
    my_printf_flag("vps_extension_flag", vps->vps_extension_flag); AddTreeItem(ivps);
    if (vps->vps_extension_flag)
    {
        // do nothing...
    }

    my_printf("rbsp_trailing_bits()"); AddTreeItem(ivps);
}

void CNalParser::h265_debug_scaling_list(scaling_list_data_t* sld, HTREEITEM root)
{
    my_printf("scaling_list_data()");
    HTREEITEM isl = AddTreeItem(root);
    for(int sizeId = 0; sizeId < 4; sizeId++)
    {
        for(int matrixId = 0; matrixId < 6; matrixId += ( sizeId == 3 ) ? 3 : 1)
        {
            my_printf_flag3("scaling_list_pred_mode_flag", sizeId, matrixId, sld->scaling_list_pred_mode_flag[sizeId][matrixId]);
            AddTreeItem(isl);
            if (!sld->scaling_list_pred_mode_flag[sizeId][matrixId])
            {
                my_printf_flag3("scaling_list_pred_matrix_id_delta[%d][%d]: %d  (v bits)", sizeId, matrixId, sld->scaling_list_pred_matrix_id_delta[sizeId][matrixId]);
                AddTreeItem(isl);
            }
            else
            {
                if (sizeId > 1)
                {
                    my_printf("scaling_list_dc_coef_minus8[%d][%d]: %d  (v bits)", sizeId, matrixId, sld->scaling_list_dc_coef_minus8[sizeId - 2][matrixId]);
                    AddTreeItem(isl);
                }
                for (int i = 0; i < sld->coefNum; i++)
                {
                    my_printf("ScalingList[%d][%d][%d]: %d  (v bits)", sizeId, matrixId, i, sld->ScalingList[sizeId][matrixId][i]);
                    AddTreeItem(isl);
                }
            }
        }
    }
}
void CNalParser::h265_debug_short_term_ref_pic_set(h265_sps_t* sps, st_ref_pic_set_t*st, referencePictureSets_t* rps, int stRpsIdx, HTREEITEM root)
{
    my_printf("short_term_ref_pic_set( %d )", stRpsIdx);
    HTREEITEM srps = AddTreeItem(root);
    HTREEITEM srpsii = srps;
    if (stRpsIdx != 0)
    {
        my_printf_flag("inter_ref_pic_set_prediction_flag", st->inter_ref_pic_set_prediction_flag);
        srpsii = AddTreeItem(srps);
    }
    if (st->inter_ref_pic_set_prediction_flag)
    {
        my_printf("delta_idx_minus1: %d (v bits)", st->delta_idx_minus1); AddTreeItem(srpsii);
        my_printf("delta_rps_sign: %d (1 bit)", st->delta_rps_sign); AddTreeItem(srpsii);
        my_printf("abs_delta_rps_minus1: %d (v bits)", st->abs_delta_rps_minus1); AddTreeItem(srpsii);
        int rIdx = stRpsIdx - 1 - st->delta_idx_minus1;
        referencePictureSets_t* rpsRef = &sps->m_RPSList[rIdx];
        for (int j = 0; j <= rpsRef->m_numberOfPictures; j++)
        {
            my_printf_flag2("used_by_curr_pic_flag", j, st->used_by_curr_pic_flag[j]); AddTreeItem(srpsii);
            if (!st->used_by_curr_pic_flag[j])
            {
                my_printf_flag2("use_delta_flag", j, st->use_delta_flag[j]); AddTreeItem(srpsii);
            }
        }
    }
    else
    {
        my_printf("num_negative_pics: %d (v bits)", st->num_negative_pics); AddTreeItem(srpsii);
        my_printf("num_positive_pics: %d (v bits)", st->num_positive_pics); AddTreeItem(srpsii);
        for (int i = 0; i < st->num_negative_pics; i++)
        {
            my_printf("delta_poc_s0_minus1[%d]: %d  (v bits)", i, st->delta_poc_s0_minus1[i]); AddTreeItem(srpsii);
            my_printf_flag2("used_by_curr_pic_s0_flag", i, st->used_by_curr_pic_s0_flag[i]); AddTreeItem(srpsii);
        }
        for (int i = 0; i < st->num_positive_pics; i++)
        {
            my_printf("delta_poc_s1_minus1[%d]: %d (v bits)", i, st->delta_poc_s1_minus1[i]); AddTreeItem(srpsii);
            my_printf_flag2("used_by_curr_pic_s1_flag", i, st->used_by_curr_pic_s1_flag[i]); AddTreeItem(srpsii);
        }
    }
}
void CNalParser::h265_debug_vui_parameters(vui_parameters_t* vui, int maxNumSubLayersMinus1, HTREEITEM root)
{
    my_printf("vui_parameters()");
    HTREEITEM ivp = AddTreeItem(root);
    my_printf_flag("aspect_ratio_info_present_flag", vui->aspect_ratio_info_present_flag);
    HTREEITEM aripf = AddTreeItem(ivp);
    if (vui->aspect_ratio_info_present_flag)
    {
        my_printf("aspect_ratio_idc: %d  (8 bits)", vui->aspect_ratio_idc);AddTreeItem(aripf);
        if (vui->aspect_ratio_idc == H265_SAR_Extended)
        {
            my_printf("sar_width: %d   (16 bits)", vui->sar_width);AddTreeItem(aripf);
            my_printf("sar_height: %d  (16 bits)", vui->sar_height);AddTreeItem(aripf);
        }
    }
    my_printf_flag("overscan_info_present_flag", vui->overscan_info_present_flag);
    HTREEITEM oipf = AddTreeItem(ivp);
    if (vui->overscan_info_present_flag)
    {
        my_printf_flag("overscan_appropriate_flag", vui->overscan_appropriate_flag);AddTreeItem(oipf);
    }
    my_printf_flag("video_signal_type_present_flag", vui->video_signal_type_present_flag);
    HTREEITEM vstpf = AddTreeItem(ivp);
    if (vui->video_signal_type_present_flag)
    {
        my_printf("video_format: %d  (3 bits)", vui->video_format); AddTreeItem(vstpf);
        my_printf_flag("video_full_range_flag", vui->video_full_range_flag); AddTreeItem(vstpf);
        my_printf_flag("colour_description_present_flag", vui->colour_description_present_flag);
        HTREEITEM cdpf = AddTreeItem(vstpf);
        if (vui->colour_description_present_flag)
        {
            my_printf("colour_primaries: %d  (8 bits)", vui->colour_primaries); AddTreeItem(cdpf);
            my_printf("transfer_characteristics: %d  (8 bits)", vui->transfer_characteristics); AddTreeItem(cdpf);
            my_printf("matrix_coeffs: %d  (8 bits)", vui->matrix_coeffs); AddTreeItem(cdpf);
        }
    }
    my_printf_flag("chroma_loc_info_present_flag", vui->chroma_loc_info_present_flag);
    HTREEITEM clipf = AddTreeItem(ivp);
    if (vui->chroma_loc_info_present_flag)
    {
        my_printf("chroma_sample_loc_type_top_field: %d  (v bits)", vui->chroma_sample_loc_type_top_field);
        AddTreeItem(clipf);
        my_printf("chroma_sample_loc_type_bottom_field: %d  (v bits)", vui->chroma_sample_loc_type_bottom_field);
        AddTreeItem(clipf);
    }
    my_printf_flag("neutral_chroma_indication_flag", vui->neutral_chroma_indication_flag);AddTreeItem(ivp);
    my_printf_flag("field_seq_flag", vui->field_seq_flag);AddTreeItem(ivp);
    my_printf_flag("frame_field_info_present_flag", vui->frame_field_info_present_flag);AddTreeItem(ivp);
    my_printf_flag("default_display_window_flag", vui->default_display_window_flag);
    HTREEITEM ddwf = AddTreeItem(ivp);
    if (vui->default_display_window_flag)
    {
        my_printf("def_disp_win_left_offset: %d  (v bits)", vui->def_disp_win_left_offset); AddTreeItem(ddwf);
        my_printf("def_disp_win_right_offset: %d  (v bits)", vui->def_disp_win_right_offset); AddTreeItem(ddwf);
        my_printf("def_disp_win_top_offset: %d  (v bits)", vui->def_disp_win_top_offset); AddTreeItem(ddwf);
        my_printf("def_disp_win_bottom_offset: %d  (v bits)", vui->def_disp_win_bottom_offset); AddTreeItem(ddwf);
    }
    my_printf_flag("vui_timing_info_present_flag", vui->vui_timing_info_present_flag);
    HTREEITEM vtipf = AddTreeItem(ivp);
    if (vui->vui_timing_info_present_flag)
    {
        my_printf("vui_num_units_in_tick: %u  (32 bits)", vui->vui_num_units_in_tick); AddTreeItem(vtipf);
        my_printf("vui_time_scale: %u  (32 bits)", vui->vui_time_scale); AddTreeItem(vtipf);
        my_printf_flag("vui_poc_proportional_to_timing_flag", vui->vui_poc_proportional_to_timing_flag);
        HTREEITEM vppttf = AddTreeItem(vtipf);
        if (vui->vui_poc_proportional_to_timing_flag)
        {
            my_printf("vui_num_ticks_poc_diff_one_minus1: %d  (v bits)", vui->vui_num_ticks_poc_diff_one_minus1);
            AddTreeItem(vppttf);
        }
        my_printf_flag("vui_hrd_parameters_present_flag", vui->vui_hrd_parameters_present_flag);
        HTREEITEM vhppf = AddTreeItem(vtipf);
        if (vui->vui_hrd_parameters_present_flag)
        {
            h265_debug_hrd_parameters(&vui->hrd_parameters, 1, maxNumSubLayersMinus1, vhppf);
        }
    }
    my_printf_flag("bitstream_restriction_flag", vui->bitstream_restriction_flag);
    HTREEITEM brf = AddTreeItem(vtipf);
    if (vui->bitstream_restriction_flag)
    {
        my_printf_flag("tiles_fixed_structure_flag", vui->tiles_fixed_structure_flag); AddTreeItem(brf);
        my_printf_flag("motion_vectors_over_pic_boundaries_flag", vui->motion_vectors_over_pic_boundaries_flag);AddTreeItem(brf);
        my_printf_flag("restricted_ref_pic_lists_flag", vui->restricted_ref_pic_lists_flag);AddTreeItem(brf);
        my_printf("min_spatial_segmentation_idc: %d  (v bits)", vui->min_spatial_segmentation_idc);AddTreeItem(brf);
        my_printf("max_bytes_per_pic_denom: %d  (v bits)", vui->max_bytes_per_pic_denom);AddTreeItem(brf);
        my_printf("max_bits_per_min_cu_denom: %d  (v bits)", vui->max_bits_per_min_cu_denom);AddTreeItem(brf);
        my_printf("log2_max_mv_length_horizontal: %d  (v bits)", vui->log2_max_mv_length_horizontal);AddTreeItem(brf);
        my_printf("log2_max_mv_length_vertical: %d  (v bits)", vui->bitstream_restriction_flag);AddTreeItem(brf);
    }
}
// sps
void CNalParser::h265_debug_sps(h265_sps_t* sps, HTREEITEM root)
{
    my_printf("seq_parameter_set_rbsp()");
    HTREEITEM isps = AddTreeItem(root);
    my_printf("sps_video_parameter_set_id: %d  (4 bits)", sps->sps_video_parameter_set_id); AddTreeItem(isps);
    my_printf("sps_max_sub_layers_minus1: %d  (4 bits)", sps->sps_max_sub_layers_minus1); AddTreeItem(isps);
    my_printf_flag("sps_temporal_id_nesting_flag", sps->sps_temporal_id_nesting_flag); AddTreeItem(isps);
    // ptl
    my_printf("profile_tier_level()");
    HTREEITEM iptl = AddTreeItem(isps);
    h265_debug_ptl(&sps->ptl, 1, sps->sps_max_sub_layers_minus1, iptl);

    my_printf("sps_seq_parameter_set_id: %d  (v bits)", sps->sps_seq_parameter_set_id); AddTreeItem(isps);
    my_printf("chroma_format_idc: %d  (v bits)", sps->chroma_format_idc); AddTreeItem(isps);
    if (sps->chroma_format_idc == 3)
    {
        my_printf_flag("separate_colour_plane_flag", sps->separate_colour_plane_flag); AddTreeItem(isps);
    }
    my_printf("pic_width_in_luma_samples: %d  (v bits)", sps->pic_width_in_luma_samples); AddTreeItem(isps);
    my_printf("pic_height_in_luma_samples: %d  (v bits)", sps->pic_height_in_luma_samples); AddTreeItem(isps);
    my_printf_flag("conformance_window_flag", sps->conformance_window_flag);
    HTREEITEM cwf = AddTreeItem(isps);
    if (sps->conformance_window_flag)
    {
        my_printf("conf_win_left_offset: %d  (v bits)", sps->conf_win_left_offset); AddTreeItem(cwf);
        my_printf("conf_win_right_offset: %d  (v bits)", sps->conf_win_right_offset); AddTreeItem(cwf);
        my_printf("conf_win_top_offset: %d  (v bits)", sps->conf_win_top_offset); AddTreeItem(cwf);
        my_printf("conf_win_bottom_offset: %d  (v bits)", sps->conf_win_bottom_offset); AddTreeItem(cwf);
    }
    my_printf("bit_depth_luma_minus8: %d  (v bits)", sps->bit_depth_luma_minus8); AddTreeItem(isps);
    my_printf("bit_depth_chroma_minus8: %d  (v bits)", sps->bit_depth_chroma_minus8); AddTreeItem(isps);
    my_printf("log2_max_pic_order_cnt_lsb_minus4: %d  (v bits)", sps->log2_max_pic_order_cnt_lsb_minus4); AddTreeItem(isps);
    my_printf_flag("sps_sub_layer_ordering_info_present_flag", sps->sps_sub_layer_ordering_info_present_flag);AddTreeItem(isps);
    my_printf("SubLayers");
    HTREEITEM isloripf = AddTreeItem(isps);
    for (int i = (sps->sps_sub_layer_ordering_info_present_flag ? 0 : sps->sps_max_sub_layers_minus1);
        i <= sps->sps_max_sub_layers_minus1; i++ )
    {
        my_printf("sps_max_dec_pic_buffering_minus1[%d]: %d  (v bits)", i, sps->sps_max_dec_pic_buffering_minus1[i]); AddTreeItem(isloripf);
        my_printf("sps_max_num_reorder_pics[%d]: %d  (v bits)", i, sps->sps_max_num_reorder_pics[i]); AddTreeItem(isloripf);
        my_printf("sps_max_latency_increase_plus1[%d]: %d  (v bits)", i, sps->sps_max_latency_increase_plus1[i]); AddTreeItem(isloripf);
    }
    my_printf("log2_min_luma_coding_block_size_minus3: %d  (v bits)", sps->log2_min_luma_coding_block_size_minus3); AddTreeItem(isps);
    my_printf("log2_diff_max_min_luma_coding_block_size: %d  (v bits)", sps->log2_diff_max_min_luma_coding_block_size); AddTreeItem(isps);
    my_printf("log2_min_luma_transform_block_size_minus2: %d  (v bits)", sps->log2_min_luma_transform_block_size_minus2); AddTreeItem(isps);
    my_printf("log2_diff_max_min_luma_transform_block_size: %d  (v bits)", sps->log2_diff_max_min_luma_transform_block_size); AddTreeItem(isps);
    my_printf("max_transform_hierarchy_depth_inter: %d  (v bits)", sps->max_transform_hierarchy_depth_inter); AddTreeItem(isps);
    my_printf("max_transform_hierarchy_depth_intra: %d  (v bits)", sps->max_transform_hierarchy_depth_intra); AddTreeItem(isps);
    my_printf_flag("scaling_list_enabled_flag", sps->scaling_list_enabled_flag);
    HTREEITEM slef = AddTreeItem(isps);
    if (sps->scaling_list_enabled_flag)
    {
        my_printf_flag("sps_scaling_list_data_present_flag", sps->sps_scaling_list_data_present_flag);
        HTREEITEM ssldpf = AddTreeItem(slef);
        if (sps->sps_scaling_list_data_present_flag)
        {
            h265_debug_scaling_list(&sps->scaling_list_data, ssldpf);
        }
    }

    my_printf_flag("amp_enabled_flag", sps->amp_enabled_flag); AddTreeItem(isps);
    my_printf_flag("sample_adaptive_offset_enabled_flag", sps->sample_adaptive_offset_enabled_flag); AddTreeItem(isps);
    my_printf_flag("pcm_enabled_flag", sps->pcm_enabled_flag);
    HTREEITEM pef = AddTreeItem(isps);
    if (sps->pcm_enabled_flag)
    {
        my_printf("pcm_sample_bit_depth_luma_minus1: %d  (4 bits)", sps->pcm_sample_bit_depth_luma_minus1); AddTreeItem(pef);
        my_printf("pcm_sample_bit_depth_chroma_minus1: %d  (4 bits)", sps->pcm_sample_bit_depth_chroma_minus1); AddTreeItem(pef);
        my_printf("log2_min_pcm_luma_coding_block_size_minus3: %d  (v bits)", sps->log2_min_pcm_luma_coding_block_size_minus3); AddTreeItem(pef);
        my_printf("log2_diff_max_min_pcm_luma_coding_block_size: %d  (v bits)", sps->log2_diff_max_min_pcm_luma_coding_block_size); AddTreeItem(pef);
        my_printf_flag("pcm_loop_filter_disabled_flag", sps->pcm_loop_filter_disabled_flag); AddTreeItem(pef);
    }
    my_printf("num_short_term_ref_pic_sets: %d  (v bits)", sps->num_short_term_ref_pic_sets); 
    referencePictureSets_t* rps = NULL;
    st_ref_pic_set_t* st = NULL;
    HTREEITEM nstrps = isps;
    if (sps->num_short_term_ref_pic_sets > 0)
    {
        nstrps = AddTreeItem(isps);
    }
    for (int i = 0; i < sps->num_short_term_ref_pic_sets; i++)
    {
        st = &sps->st_ref_pic_set[i];
        rps = &sps->m_RPSList[i];
        h265_debug_short_term_ref_pic_set(sps, st, rps, i, nstrps);
    }
    my_printf_flag("long_term_ref_pics_present_flag", sps->long_term_ref_pics_present_flag);
    HTREEITEM ltrppf = AddTreeItem(isps);
    if (sps->long_term_ref_pics_present_flag)
    {
        my_printf("num_long_term_ref_pics_sps: %d  (v bits)", sps->num_long_term_ref_pics_sps);
        AddTreeItem(ltrppf);
        for (int i = 0; i < sps->num_long_term_ref_pics_sps; i++)
        {
            my_printf("lt_ref_pic_poc_lsb_sps[%d]: %d  (u %d bits)", i, sps->lt_ref_pic_poc_lsb_sps[i], sps->lt_ref_pic_poc_lsb_sps_bytes);
            AddTreeItem(ltrppf);
            my_printf_flag2("used_by_curr_pic_lt_sps_flag", i, sps->used_by_curr_pic_lt_sps_flag[i]);
            AddTreeItem(ltrppf);
        }
    }
    my_printf_flag("sps_temporal_mvp_enabled_flag", sps->sps_temporal_mvp_enabled_flag); AddTreeItem(isps);
    my_printf_flag("strong_intra_smoothing_enabled_flag", sps->strong_intra_smoothing_enabled_flag); AddTreeItem(isps);
    my_printf_flag("vui_parameters_present_flag", sps->vui_parameters_present_flag);
    HTREEITEM vppf = AddTreeItem(isps);
    if (sps->vui_parameters_present_flag)
    {
        // vui
        h265_debug_vui_parameters(&sps->vui, sps->sps_max_sub_layers_minus1, vppf);
    }
    my_printf_flag("sps_extension_present_flag", sps->sps_extension_present_flag);
    HTREEITEM sepf = AddTreeItem(isps);
    if (sps->sps_extension_present_flag)
    {
        my_printf_flag("sps_range_extension_flag", sps->sps_range_extension_flag);
        HTREEITEM sref = AddTreeItem(sepf);
        if (sps->sps_range_extension_flag)
        {
            my_printf_flag("transform_skip_rotation_enabled_flag", sps->sps_range_extension.transform_skip_rotation_enabled_flag); AddTreeItem(sref);
            my_printf_flag("transform_skip_context_enabled_flag", sps->sps_range_extension.transform_skip_context_enabled_flag); AddTreeItem(sref);
            my_printf_flag("implicit_rdpcm_enabled_flag", sps->sps_range_extension.implicit_rdpcm_enabled_flag); AddTreeItem(sref);
            my_printf_flag("explicit_rdpcm_enabled_flag", sps->sps_range_extension.explicit_rdpcm_enabled_flag); AddTreeItem(sref);
            my_printf_flag("extended_precision_processing_flag", sps->sps_range_extension.extended_precision_processing_flag); AddTreeItem(sref);
            my_printf_flag("intra_smoothing_disabled_flag", sps->sps_range_extension.intra_smoothing_disabled_flag); AddTreeItem(sref);
            my_printf_flag("high_precision_offsets_enabled_flag", sps->sps_range_extension.high_precision_offsets_enabled_flag); AddTreeItem(sref);
            my_printf_flag("persistent_rice_adaptation_enabled_flag", sps->sps_range_extension.persistent_rice_adaptation_enabled_flag); AddTreeItem(sref);
            my_printf_flag("cabac_bypass_alignment_enabled_flag", sps->sps_range_extension.cabac_bypass_alignment_enabled_flag); AddTreeItem(sref);
        }
        my_printf_flag("sps_multilayer_extension_flag", sps->sps_multilayer_extension_flag);
        HTREEITEM smef = AddTreeItem(sepf);
        if (sps->sps_multilayer_extension_flag)
        {
            my_printf_flag("inter_view_mv_vert_constraint_flag", sps->inter_view_mv_vert_constraint_flag);
            AddTreeItem(smef);
        }
        my_printf_flag("sps_3d_extension_flag", sps->sps_3d_extension_flag); AddTreeItem(sepf);
        my_printf("sps_extension_5bits: %d  (5 bits)", sps->sps_extension_5bits); AddTreeItem(sepf);
    }


    // todo sps_3d_extension_flag

    my_printf("rbsp_trailing_bits()"); AddTreeItem(isps);
}

// pps
void CNalParser::h265_debug_pps(h265_pps_t* pps, HTREEITEM root)
{
    my_printf("pic_parameter_set_rbsp()");
    HTREEITEM ipps = AddTreeItem(root);
    my_printf("pps_pic_parameter_set_id: %d  (v bits)", pps->pps_pic_parameter_set_id);AddTreeItem(ipps);
    my_printf("pps_seq_parameter_set_id: %d  (v bits)", pps->pps_seq_parameter_set_id);AddTreeItem(ipps);
    my_printf_flag("dependent_slice_segments_enabled_flag", pps->dependent_slice_segments_enabled_flag);AddTreeItem(ipps);
    my_printf_flag("output_flag_present_flag", pps->output_flag_present_flag);AddTreeItem(ipps);
    my_printf("num_extra_slice_header_bits: %d  (3 bits)", pps->num_extra_slice_header_bits);AddTreeItem(ipps);
    my_printf_flag("sign_data_hiding_enabled_flag", pps->sign_data_hiding_enabled_flag);AddTreeItem(ipps);
    my_printf_flag("cabac_init_present_flag", pps->cabac_init_present_flag);AddTreeItem(ipps);
    my_printf("num_ref_idx_l0_default_active_minus1: %d  (v bits)", pps->num_ref_idx_l0_default_active_minus1);AddTreeItem(ipps);
    my_printf("num_ref_idx_l1_default_active_minus1: %d  (v bits)", pps->num_ref_idx_l1_default_active_minus1);AddTreeItem(ipps);
    my_printf("init_qp_minus26: %d  (v bits)", pps->init_qp_minus26);AddTreeItem(ipps);
    my_printf_flag("constrained_intra_pred_flag", pps->constrained_intra_pred_flag);AddTreeItem(ipps);
    my_printf_flag("transform_skip_enabled_flag", pps->transform_skip_enabled_flag);AddTreeItem(ipps);
    my_printf_flag("cu_qp_delta_enabled_flag", pps->cu_qp_delta_enabled_flag);
    HTREEITEM cqdef = AddTreeItem(ipps);
    if (pps->cu_qp_delta_enabled_flag)
    {
        my_printf("diff_cu_qp_delta_depth: %d  (v bits)", pps->diff_cu_qp_delta_depth);
        AddTreeItem(cqdef);
    }
    my_printf("pps_cb_qp_offset: %d  (v bits)", pps->pps_cb_qp_offset);AddTreeItem(ipps);
    my_printf("pps_cr_qp_offset: %d  (v bits)", pps->pps_cr_qp_offset);AddTreeItem(ipps);
    my_printf_flag("pps_slice_chroma_qp_offsets_present_flag", pps->pps_slice_chroma_qp_offsets_present_flag);AddTreeItem(ipps);
    my_printf_flag("weighted_pred_flag", pps->weighted_pred_flag);AddTreeItem(ipps);
    my_printf_flag("weighted_bipred_flag", pps->weighted_bipred_flag);AddTreeItem(ipps);
    my_printf_flag("transquant_bypass_enabled_flag", pps->transquant_bypass_enabled_flag);AddTreeItem(ipps);
    my_printf_flag("tiles_enabled_flag", pps->tiles_enabled_flag);
    HTREEITEM tef = AddTreeItem(ipps);
    if (pps->tiles_enabled_flag)
    {
        my_printf("num_tile_columns_minus1: %d  (v bits)", pps->num_tile_columns_minus1);AddTreeItem(tef);
        my_printf("num_tile_rows_minus1: %d  (v bits)", pps->num_tile_rows_minus1);AddTreeItem(tef);
        my_printf_flag("uniform_spacing_flag", pps->uniform_spacing_flag);
        HTREEITEM usf = AddTreeItem(tef);
        if (!pps->uniform_spacing_flag)
        {
            for (int i = 0; i < pps->num_tile_columns_minus1; i++)
            {
                my_printf("column_width_minus1[%d]: %d  (v bits)", i, pps->column_width_minus1[i]);AddTreeItem(usf);
            }
            for (int i = 0; i < pps->num_tile_rows_minus1; i++)
            {
                my_printf("row_height_minus1[%d]: %d  (v bits)", i, pps->row_height_minus1[i]);AddTreeItem(usf);
            }
        }
        my_printf_flag("loop_filter_across_tiles_enabled_flag", pps->loop_filter_across_tiles_enabled_flag); // to check
        AddTreeItem(tef);
    }
    my_printf_flag("entropy_coding_sync_enabled_flag", pps->entropy_coding_sync_enabled_flag);AddTreeItem(ipps);

    my_printf_flag("pps_loop_filter_across_slices_enabled_flag", pps->pps_loop_filter_across_slices_enabled_flag); // to check
    AddTreeItem(ipps);
    my_printf_flag("deblocking_filter_control_present_flag", pps->deblocking_filter_control_present_flag);
    HTREEITEM dfcpf = AddTreeItem(ipps);
    if (pps->deblocking_filter_control_present_flag)
    {
        my_printf_flag("deblocking_filter_override_enabled_flag", pps->deblocking_filter_override_enabled_flag);
        AddTreeItem(dfcpf);
        my_printf_flag("pps_deblocking_filter_disabled_flag", pps->pps_deblocking_filter_disabled_flag);
        HTREEITEM dfdf = AddTreeItem(dfcpf);
        if (pps->pps_deblocking_filter_disabled_flag)
        {
            my_printf("pps_beta_offset_div2: %d  (v bits)", pps->pps_beta_offset_div2);AddTreeItem(dfdf);
            my_printf("pps_tc_offset_div2: %d  (v bits)", pps->pps_tc_offset_div2);AddTreeItem(dfdf);
        }
    }
    my_printf_flag("pps_scaling_list_data_present_flag", pps->pps_scaling_list_data_present_flag);
    HTREEITEM sldf = AddTreeItem(ipps);
    if (pps->pps_scaling_list_data_present_flag)
    {
        // scaling_list_data()
        h265_debug_scaling_list(&pps->scaling_list_data, sldf);
    }
    my_printf_flag("lists_modification_present_flag", pps->lists_modification_present_flag);AddTreeItem(ipps);
    my_printf("log2_parallel_merge_level_minus2: %d  (v bits)", pps->log2_parallel_merge_level_minus2);AddTreeItem(ipps);
    my_printf_flag("slice_segment_header_extension_present_flag", pps->slice_segment_header_extension_present_flag);AddTreeItem(ipps);
    my_printf_flag("pps_extension_present_flag", pps->pps_extension_present_flag);
    HTREEITEM epf = AddTreeItem(ipps);
    if (pps->pps_extension_present_flag)
    {
        my_printf_flag("pps_range_extension_flag", pps->pps_range_extension_flag);AddTreeItem(epf);
        HTREEITEM iref = AddTreeItem(epf);
        if (pps->pps_range_extension_flag)
        {
            if (pps->transform_skip_enabled_flag)
            {
                my_printf("log2_max_transform_skip_block_size_minus2: %d  (v bits)", pps->pps_range_extension.log2_max_transform_skip_block_size_minus2);
                AddTreeItem(iref);
            }
            my_printf_flag("cross_component_prediction_enabled_flag", pps->pps_range_extension.cross_component_prediction_enabled_flag);AddTreeItem(iref);
            my_printf_flag("chroma_qp_offset_list_enabled_flag", pps->pps_range_extension.chroma_qp_offset_list_enabled_flag);AddTreeItem(iref);
            if (pps->pps_range_extension.chroma_qp_offset_list_enabled_flag)
            {
                my_printf("diff_cu_chroma_qp_offset_depth: %d  (v bits)", pps->pps_range_extension.diff_cu_chroma_qp_offset_depth);AddTreeItem(iref);
                my_printf("chroma_qp_offset_list_len_minus1: %d  (v bits)", pps->pps_range_extension.chroma_qp_offset_list_len_minus1);AddTreeItem(iref);
                for (int i = 0; i < pps->pps_range_extension.chroma_qp_offset_list_len_minus1; i++)
                {
                    my_printf("cb_qp_offset_list[%d]: %d  (v bits)", i, pps->pps_range_extension.cb_qp_offset_list[i]);AddTreeItem(iref);
                    my_printf("cr_qp_offset_list[%d]: %d  (v bits)", i, pps->pps_range_extension.cb_qp_offset_list[i]);AddTreeItem(iref);
                }
            }
            my_printf("log2_sao_offset_scale_luma: %d  (v bits)", pps->pps_range_extension.log2_sao_offset_scale_luma);AddTreeItem(iref);
            my_printf("log2_sao_offset_scale_chroma: %d  (v bits)", pps->pps_range_extension.log2_sao_offset_scale_chroma);AddTreeItem(iref);
        }
        my_printf_flag("pps_multilayer_extension_flag", pps->pps_multilayer_extension_flag);AddTreeItem(epf);
        my_printf_flag("pps_3d_extension_flag", pps->pps_3d_extension_flag);AddTreeItem(epf);
        my_printf("pps_extension_5bits: %d  (5 bits)", pps->pps_extension_5bits);AddTreeItem(epf);
    }

    if (pps->pps_multilayer_extension_flag)
    {
        // todo...
    }
    if (pps->pps_3d_extension_flag)
    {
        // todo...
    }

    my_printf("rbsp_trailing_bits()"); AddTreeItem(ipps);
}

// aud
void CNalParser::h265_debug_aud(h265_aud_t* aud, HTREEITEM root)
{
    const char* pic_type = NULL;

    my_printf("access_unit_delimiter_rbsp()");
    HTREEITEM iaud = AddTreeItem(root);

    switch (aud->pic_type)
    {
        case H265_AUD_PRIMARY_PIC_TYPE_I:    pic_type = "I"; break;
        case H265_AUD_PRIMARY_PIC_TYPE_IP:   pic_type = "P, I"; break;
        case H265_AUD_PRIMARY_PIC_TYPE_IPB:  pic_type = "B, P, I"; break;
        default: pic_type = "Unknown"; break;
    }
    my_printf("pic_type: %d ( %s )", aud->pic_type, pic_type);AddTreeItem(iaud);
    my_printf("rbsp_trailing_bits()");AddTreeItem(root);
}

// sei
void CNalParser::h265_debug_seis(h265_stream_t* h, HTREEITEM root)
{
    h265_sei_t** seis = h->seis;
    int num_seis = h->num_seis;
    const char* sei_type_name = NULL;
    int i;

    my_printf("sei_rbsp()");
    HTREEITEM isei = AddTreeItem(root);
    my_printf("sei_message()");
    HTREEITEM iisei = AddTreeItem(isei);
    for (i = 0; i < num_seis; i++)
    {
        h265_sei_t* s = seis[i];
        my_printf("payloadType: %d  (v bits)", s->payloadType); AddTreeItem(iisei);
        my_printf("payloadSize: %d  (v bits)", s->payloadSize); AddTreeItem(iisei);
        my_printf("sei_payload()");
        HTREEITEM sp = AddTreeItem(iisei);
        if (h->nal->nal_unit_type == NAL_UNIT_PREFIX_SEI)
        {
            switch(s->payloadType)
            {
            case 0:
                my_printf("buffering_period()"); AddTreeItem(sp);
                break;
            case 1:
                my_printf("pic_timing()"); AddTreeItem(sp);
                break;
            case 2:
                my_printf("pan_scan_rect()"); AddTreeItem(sp);
                break;
            case 3:
                my_printf("filler_payload()"); AddTreeItem(sp);
                break;
            case 4:
                my_printf("user_data_registered_itu_t_t35()"); AddTreeItem(sp);
                break;
            case 5:
                {
                    my_printf("user_data_unregistered()");AddTreeItem(sp);
                    char uuid[64] = {0};
                    char tmp[8] = {0};
                    for (int j = 0; j < 16; j++)
                    {
                        sprintf(tmp, "%X", s->payload[j]);
                        strcat(uuid, tmp);
                    }
                    my_printf("uuid_iso_iec_11578: %s", uuid);
                    HTREEITEM udpb = AddTreeItem(sp);
                    for (int j = 16; j < s->payloadSize; j++)
                    {
                        my_printf("user_data_payload_byte: %d('%c')", s->payload[j], s->payload[j]);
                        AddTreeItem(sp);
                    }
                }
                break;
            case 6:
                my_printf("recovery_point()"); AddTreeItem(sp);
                break;
            case 9:
                my_printf("scene_info()"); AddTreeItem(sp);
                break;
            case 15:
                my_printf("picture_snapshot()"); AddTreeItem(sp);
                break;
            case 16:
                my_printf("progressive_refinement_segment_start()"); AddTreeItem(sp);
                break;
            case 17:
                my_printf("progressive_refinement_segment_end()"); AddTreeItem(sp);
                break;
            default:
                my_printf("reserved_sei_message()"); AddTreeItem(sp);
                break;
            }
        }
        else if (h->nal->nal_unit_type == NAL_UNIT_SUFFIX_SEI)
        {
            switch(s->payloadType)
            {
            case 3:
                my_printf("filler_payload()"); AddTreeItem(sp);
                break;
            case 4:
                my_printf("user_data_registered_itu_t_t35()"); AddTreeItem(sp);
                break;
            case 5:
                my_printf("user_data_unregistered()"); AddTreeItem(sp);
                break;
            case 17:
                my_printf("progressive_refinement_segment_end()"); AddTreeItem(sp);
                break;
            case 22:
                my_printf("post_filter_hint()"); AddTreeItem(sp);
                break;
            case 132:
                my_printf("decoded_picture_hash()"); AddTreeItem(sp);
                break;
            case 16:
                my_printf("progressive_refinement_segment_start()"); AddTreeItem(sp);
                break;
            default:
                my_printf("reserved_sei_message()"); AddTreeItem(sp);
                break;
            }
        }
    }

    my_printf("rbsp_trailing_bits()"); AddTreeItem(isei);
}

void CNalParser::h265_debug_ref_pic_lists_modification(h265_slice_header_t* hrd, HTREEITEM root)
{
    my_printf("ref_pic_list_modification_flag_l0: %d", hrd->ref_pic_lists_modification.ref_pic_list_modification_flag_l0);
    if (hrd->ref_pic_lists_modification.ref_pic_list_modification_flag_l0)
    {
        for (int i = 0; i <= hrd->num_ref_idx_l0_active_minus1; i++)
            my_printf("list_entry_l0[%d]: %u", i, hrd->ref_pic_lists_modification.list_entry_l0[i]);
    }
    if (hrd->slice_type == H265_SH_SLICE_TYPE_B)
    {
        my_printf("ref_pic_list_modification_flag_l1: %d", hrd->ref_pic_lists_modification.ref_pic_list_modification_flag_l1);
        for (int i = 0; i <= hrd->num_ref_idx_l1_active_minus1; i++)
            my_printf("list_entry_l1[%d]: %u", i, hrd->ref_pic_lists_modification.list_entry_l1[i]);
    }
}
void CNalParser::h265_debug_pred_weight_table(h265_stream_t* h, HTREEITEM root)
{
    pred_weight_table_t* pwt = &h->sh->pred_weight_table;
    h265_sps_t* sps = h->sps;

    my_printf("pred_weight_table()");
    HTREEITEM ipwt = AddTreeItem(root);

    my_printf("luma_log2_weight_denom: %d  (v bits)", pwt->luma_log2_weight_denom); AddTreeItem(ipwt);
    if (h->sps->chroma_format_idc != 0)
    {
        my_printf("delta_chroma_log2_weight_denom: %d  (v bits)", pwt->delta_chroma_log2_weight_denom);
        AddTreeItem(ipwt);
    }

    HTREEITEM nria;
    if (h->sh->num_ref_idx_l0_active_minus1 >= 0)
    {
        my_printf("NumRefIdxL0Active");
        nria = AddTreeItem(ipwt);
    }

    for (int i = 0; i <= h->sh->num_ref_idx_l0_active_minus1; i++)
    {
        my_printf_flag2("luma_weight_l0_flag", i, pwt->luma_weight_l0_flag[i]);
        AddTreeItem(nria);
    }
    if (h->sps->chroma_format_idc != 0)
    {
        for (int i = 0; i <= h->sh->num_ref_idx_l0_active_minus1; i++)
        {
            my_printf_flag2("chroma_weight_l0_flag", i, pwt->chroma_weight_l0_flag[i]);
            AddTreeItem(nria);
        }
    }


    for (int i = 0; i <= h->sh->num_ref_idx_l0_active_minus1; i++)
    {
        if (pwt->luma_weight_l0_flag[i])
        {
            my_printf("delta_luma_weight_l0[%d]: %d  (v bits)", i, pwt->delta_luma_weight_l0[i]);AddTreeItem(nria);
            my_printf("luma_offset_l0[%d]: %d  (v bits)", i, pwt->luma_offset_l0[i]);AddTreeItem(nria);
        }
        if (pwt->chroma_weight_l0_flag[i])
        {
            for (int j = 0; j < 2; j++)
            {
                my_printf("delta_chroma_weight_l0[%d][%d]: %d  (v bits)", i, j, pwt->delta_chroma_weight_l0[i][j]);AddTreeItem(nria);
                my_printf("delta_chroma_offset_l0[%d][%d]: %d  (v bits)", i, j, pwt->delta_chroma_offset_l0[i][j]);AddTreeItem(nria);
            }
        }
    }

    HTREEITEM nria_l;
    if ((h->sh->slice_type == H265_SH_SLICE_TYPE_B) && (h->sh->num_ref_idx_l1_active_minus1 >= 0))
    {
        my_printf("NumRefIdxL1Active");
        nria_l = AddTreeItem(ipwt);
    }

    if (h->sh->slice_type == H265_SH_SLICE_TYPE_B)
    {
        for (int i = 0; i <= h->sh->num_ref_idx_l1_active_minus1; i++)
        {
            my_printf_flag2("luma_weight_l1_flag", i, pwt->luma_weight_l1_flag[i]);
            AddTreeItem(nria_l);
        }
        if (h->sps->chroma_format_idc != 0)
        {
            for (int i = 0; i <= h->sh->num_ref_idx_l1_active_minus1; i++)
            {
                my_printf_flag2("chroma_weight_l1_flag", i, pwt->chroma_weight_l1_flag[i]);
                AddTreeItem(nria_l);
            }
        }
        for (int i = 0; i <= h->sh->num_ref_idx_l1_active_minus1; i++)
        {
            if (pwt->luma_weight_l1_flag[i])
            {
                my_printf("delta_luma_weight_l1[%d]: %d  (v bits)", i, pwt->delta_luma_weight_l1[i]);AddTreeItem(nria_l);
                my_printf("luma_offset_l1[%d]: %d  (v bits)", i, pwt->luma_offset_l1[i]);AddTreeItem(nria_l);
                
            }
            if (pwt->chroma_weight_l1_flag[i])
            {
                for (int j = 0; j < 2; j++)
                {
                    my_printf("delta_chroma_weight_l1[%d][%d]: %d  (v bits)", i, j, pwt->delta_chroma_weight_l1[i][j]);
                    AddTreeItem(nria_l);
                    my_printf("delta_chroma_offset_l1[%d][%d]: %d  (v bits)", i, j, pwt->delta_chroma_offset_l1[i][j]);
                    AddTreeItem(nria_l);
                }
            }
        }
    }
}

void CNalParser::h265_debug_slice_header(h265_stream_t* h, HTREEITEM root)
{
    h265_slice_header_t* hrd = h->sh;
    h265_sps_t* sps = NULL;
    h265_pps_t* pps = NULL;
    int nal_unit_type = h->nal->nal_unit_type;
    h->pps = h->pps_table[hrd->slice_pic_parameter_set_id];
    pps = h->pps;
    h->sps = h->sps_table[pps->pps_seq_parameter_set_id];
    sps = h->sps;
    if (pps == NULL || sps == NULL) return;

    my_printf("slice_segment_layer_rbsp()");
    HTREEITEM islce = AddTreeItem(root);

    my_printf("slice_segment_header()");
    HTREEITEM ihrd = AddTreeItem(islce);

    my_printf_flag("first_slice_segment_in_pic_flag", hrd->first_slice_segment_in_pic_flag); AddTreeItem(ihrd);
    if (nal_unit_type >= NAL_UNIT_CODED_SLICE_BLA_W_LP && nal_unit_type <= NAL_UNIT_RESERVED_IRAP_VCL23)
    {
        my_printf_flag("no_output_of_prior_pics_flag", hrd->no_output_of_prior_pics_flag); AddTreeItem(ihrd);
    }
    my_printf("slice_pic_parameter_set_id: %d (v bits)", hrd->slice_pic_parameter_set_id); AddTreeItem(ihrd);
    if (!hrd->first_slice_segment_in_pic_flag)
    {
        if (pps->dependent_slice_segments_enabled_flag)
        {
            my_printf_flag("dependent_slice_segment_flag", hrd->dependent_slice_segment_flag); AddTreeItem(ihrd);
        }
        my_printf("slice_segment_address: %d (v %d bits)", hrd->slice_segment_address, hrd->slice_segment_address_bytes);
        AddTreeItem(ihrd);
    }

    if (!hrd->dependent_slice_segment_flag)
    {
        my_printf("dependent_slice_segment_flag");
        HTREEITEM dssf = AddTreeItem(ihrd);
        for (int i = 0; i < pps->num_extra_slice_header_bits; i++)
        {
            my_printf("slice_reserved_flag[%d]: %d", i, hrd->slice_reserved_flag[i]); // todo
            AddTreeItem(dssf);
        }
        const char* slice_type_name;
        switch(hrd->slice_type)
        {
            case H265_SH_SLICE_TYPE_P:  slice_type_name = "P slice"; break;
            case H265_SH_SLICE_TYPE_B:  slice_type_name = "B slice"; break;
            case H265_SH_SLICE_TYPE_I:  slice_type_name = "I slice"; break;
            default:                    slice_type_name = "Unknown"; break;
        }
        my_printf("slice_type: %d (%s) (v bits)", hrd->slice_type, slice_type_name); AddTreeItem(dssf);
        if (pps->output_flag_present_flag)
        {
            my_printf_flag("pic_output_flag", hrd->pic_output_flag); AddTreeItem(dssf);
        }
        if (sps->separate_colour_plane_flag == 1)
        {
            my_printf("colour_plane_id: %d  (2 bits)", hrd->colour_plane_id); AddTreeItem(dssf);
        }
        if (nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL || nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP)
        {
            // do nothing...
        }
        else
        {
            my_printf("slice_pic_order_cnt_lsb: %d  (%d bits)", hrd->slice_pic_order_cnt_lsb, hrd->slice_pic_order_cnt_lsb_bytes);AddTreeItem(dssf);
            my_printf_flag("short_term_ref_pic_set_sps_flag", hrd->short_term_ref_pic_set_sps_flag); AddTreeItem(dssf);
            if (!hrd->short_term_ref_pic_set_sps_flag)
            {
                referencePictureSets_t* rps = &hrd->m_localRPS;
                h265_debug_short_term_ref_pic_set(sps, &hrd->st_ref_pic_set, rps, sps->num_short_term_ref_pic_sets, dssf);
            }
            else if (sps->num_short_term_ref_pic_sets > 1)
            {
                my_printf("short_term_ref_pic_set_idx: %d  (v %d bits)", hrd->short_term_ref_pic_set_idx, hrd->short_term_ref_pic_set_idx_bytes);
                AddTreeItem(dssf);
            }
            if (sps->long_term_ref_pics_present_flag)
            {
                if (sps->num_long_term_ref_pics_sps > 0)
                {
                    my_printf("num_long_term_sps: %d  (v bits)", hrd->num_long_term_sps); AddTreeItem(dssf);
                }
                my_printf("num_long_term_pics: %d  (v bits)", hrd->num_long_term_pics); AddTreeItem(dssf);
                for (int i = 0; i < (int)hrd->lt_idx_sps.size(); i++)
                {
                    if (i < hrd->num_long_term_sps)
                    {
                        if (sps->num_long_term_ref_pics_sps > 1)
                        {
                            my_printf("lt_idx_sps[%d]: %d  (v bits)", i, hrd->lt_idx_sps[i]); AddTreeItem(dssf);
                        }
                    }
                    else
                    {
                        my_printf("poc_lsb_lt[%d]: %d  (v bits)", i, hrd->poc_lsb_lt[i]); AddTreeItem(dssf);
                        my_printf_flag2("used_by_curr_pic_lt_flag", i, hrd->used_by_curr_pic_lt_flag[i]); AddTreeItem(dssf);
                    }
                    my_printf_flag2("delta_poc_msb_present_flag", i, hrd->delta_poc_msb_present_flag[i]); AddTreeItem(dssf);
                    if (hrd->delta_poc_msb_present_flag[i])
                    {
                        my_printf("delta_poc_msb_cycle_lt[%d]: %d  (v bits)", i, hrd->delta_poc_msb_cycle_lt[i]);
                        AddTreeItem(dssf);
                    }
                }
            }
            if(sps->sps_temporal_mvp_enabled_flag)
            {
                my_printf_flag("slice_temporal_mvp_enabled_flag", hrd->slice_temporal_mvp_enabled_flag); AddTreeItem(dssf);
            }
        }

        if(sps->sample_adaptive_offset_enabled_flag)
        {
            my_printf_flag("slice_sao_luma_flag", hrd->slice_sao_luma_flag); AddTreeItem(dssf);
            my_printf_flag("slice_sao_chroma_flag", hrd->slice_sao_chroma_flag); AddTreeItem(dssf);
        }
        if (hrd->slice_type == H265_SH_SLICE_TYPE_P || hrd->slice_type == H265_SH_SLICE_TYPE_B)
        {
            my_printf_flag("num_ref_idx_active_override_flag", hrd->num_ref_idx_active_override_flag);
            HTREEITEM nriaof = AddTreeItem(dssf); // 根据H265VideoESViewer，没有节点
            if (hrd->num_ref_idx_active_override_flag)
            {
                my_printf("num_ref_idx_l0_active_minus1: %d  (v bits)", hrd->num_ref_idx_l0_active_minus1); AddTreeItem(dssf);
                if (hrd->slice_type == H265_SH_SLICE_TYPE_B)
                {
                    my_printf("num_ref_idx_l1_active_minus1: %d  (v bits)", hrd->num_ref_idx_l1_active_minus1); AddTreeItem(dssf);
                }
            }
            if(pps->lists_modification_present_flag)
            {
                h265_debug_ref_pic_lists_modification(hrd);
            }
            if (hrd->slice_type == H265_SH_SLICE_TYPE_B)
            {
                my_printf_flag("mvd_l1_zero_flag", hrd->mvd_l1_zero_flag); AddTreeItem(dssf);
            }
            if (pps->cabac_init_present_flag)
            {
                my_printf_flag("cabac_init_flag", hrd->cabac_init_flag); AddTreeItem(dssf);
            }
            if (hrd->slice_temporal_mvp_enabled_flag)
            {
                if (hrd->slice_type == H265_SH_SLICE_TYPE_B)
                {
                    my_printf_flag("collocated_from_l0_flag", hrd->collocated_from_l0_flag); AddTreeItem(dssf);
                }
                if ((hrd->collocated_from_l0_flag && hrd->num_ref_idx_l0_active_minus1 > 0) ||
                    (!hrd->collocated_from_l0_flag && hrd->num_ref_idx_l1_active_minus1 > 0))
                {
                    my_printf("collocated_ref_idx: %d  (v bits)", hrd->collocated_ref_idx); AddTreeItem(dssf);
                }
            }
            if ((pps->weighted_pred_flag && hrd->slice_type == H265_SH_SLICE_TYPE_P) ||
                (pps->weighted_bipred_flag && hrd->slice_type == H265_SH_SLICE_TYPE_B))
                h265_debug_pred_weight_table(h, dssf);
            my_printf("five_minus_max_num_merge_cand: %d  (v bits)", hrd->five_minus_max_num_merge_cand); AddTreeItem(dssf);
        }
        my_printf("slice_qp_delta: %d  (v bits)", hrd->slice_qp_delta); AddTreeItem(dssf);
        if (pps->pps_slice_chroma_qp_offsets_present_flag)
        {
            my_printf("slice_cb_qp_offset: %d  (v bits)", hrd->slice_cb_qp_offset); AddTreeItem(dssf);
            my_printf("slice_cr_qp_offset: %d  (v bits)", hrd->slice_cr_qp_offset); AddTreeItem(dssf);
        }
        if (pps->pps_range_extension.chroma_qp_offset_list_enabled_flag)
        {
            my_printf_flag("cu_chroma_qp_offset_enabled_flag", hrd->cu_chroma_qp_offset_enabled_flag); AddTreeItem(dssf);
        }
        if (pps->deblocking_filter_override_enabled_flag)
        {
            my_printf_flag("deblocking_filter_override_flag", hrd->deblocking_filter_override_flag); AddTreeItem(dssf);
        }
        if (hrd->deblocking_filter_override_flag)
        {
            my_printf_flag("slice_deblocking_filter_disabled_flag", hrd->slice_deblocking_filter_disabled_flag);
            HTREEITEM dfof = AddTreeItem(dssf);
            if (!hrd->slice_deblocking_filter_disabled_flag)
            {
                my_printf("slice_beta_offset_div2: %d  (v bits)", hrd->slice_beta_offset_div2); AddTreeItem(dfof);
                my_printf("slice_tc_offset_div2: %d  (v bits)", hrd->slice_tc_offset_div2); AddTreeItem(dfof);
            }
        }
        if (pps-> pps_loop_filter_across_slices_enabled_flag &&
            (hrd->slice_sao_luma_flag || hrd->slice_sao_chroma_flag ||
            !hrd->slice_deblocking_filter_disabled_flag))
        {
            my_printf_flag("slice_loop_filter_across_slices_enabled_flag", hrd->slice_loop_filter_across_slices_enabled_flag);
            AddTreeItem(dssf);
        }
    }
    if (pps->tiles_enabled_flag || pps->entropy_coding_sync_enabled_flag)
    {
        my_printf("num_entry_point_offsets: %d (v bits)", hrd->num_entry_point_offsets);
        HTREEITEM inepo = AddTreeItem(ihrd);
        if (hrd->num_entry_point_offsets > 0)
        {
            my_printf("offset_len_minus1: %d (v bits)", hrd->offset_len_minus1); AddTreeItem(inepo);
            my_printf("NumEntryPointOffsets");
            HTREEITEM iinepo = AddTreeItem(ihrd);
            for (int i = 0; i < hrd->num_entry_point_offsets; i++)
            {
                my_printf("entry_point_offset_minus1[%d]: %d (%d bits)", i, hrd->entry_point_offset_minus1[i], hrd->entry_point_offset_minus1_bytes); // to add len
                AddTreeItem(iinepo);
            }
        }
    }
    if (pps->slice_segment_header_extension_present_flag)
    {
        my_printf("slice_segment_header_extension_length: %d (v bits)", hrd->slice_segment_header_extension_length);
        AddTreeItem(ihrd);
        for (int i = 0; i < hrd->slice_segment_header_extension_length; i++)
        {
            my_printf("slice_segment_header_extension_data_byte[%d]: %d  (8 bits)", i, hrd->slice_segment_header_extension_data_byte[i]);
            AddTreeItem(ihrd);
        }
    }
    // no need to debug...
    my_printf("slice_segment_data()");
    AddTreeItem(islce);
    my_printf("rbsp_slice_segment_trailing_bits()");
    AddTreeItem(islce);
}

void CNalParser::h265_debug_nal_t(h265_stream_t* h, h265_nal_t* nal)
{
    int my_nal_type = -1;

    const char* nal_unit_type_name = NULL;
    
    m_pTree->DeleteAllItems();

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
    // 根节点
    my_printf("NAL");
    HTREEITEM root = AddTreeItem(TVI_ROOT);

    // 根据手册，nal头是一个节点
    my_printf("nal_unit_header");
    HTREEITEM subroot = AddTreeItem(root);

    my_printf("forbidden_zero_bit: %d (1 bit)", nal->forbidden_zero_bit); AddTreeItem(subroot);
    my_printf("nal_unit_type: %d (%s) (6 bits)", nal->nal_unit_type, nal_unit_type_name); AddTreeItem(subroot);
    my_printf("nuh_layer_id: %d (6 bits)", nal->nuh_layer_id); AddTreeItem(subroot);
    my_printf("nuh_temporal_id_plus1: %d (3 bits)", nal->nuh_temporal_id_plus1); AddTreeItem(subroot);
    
    // nal具体的内容也是一个节点
    if(my_nal_type == 0)
        h265_debug_vps(h->vps, root);
    else if(my_nal_type == 1)
        h265_debug_sps(h->sps, root);
    else if(my_nal_type == 2)
        h265_debug_pps(h->pps, root);
    else if(my_nal_type == 3)
        h265_debug_aud(h->aud, root);
    else if(my_nal_type == 4)
        h265_debug_seis(h, root);
    else if(my_nal_type == 5)
        h265_debug_slice_header(h, root);
}
