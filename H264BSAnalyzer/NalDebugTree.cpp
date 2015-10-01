#include "stdafx.h"

#include "NaLParse.h"


#define my_printf(...) do { \
    sprintf(m_tmpStore, __VA_ARGS__);\
    } while(0)

#define AddTreeItem(_item) m_pTree->InsertItem(m_tmpStore, _item)

// 以下代码来自h264_stream.c，单独出来
/***************************** debug ******************************/

void CNalParser::h264_debug_sps_t(sps_t* sps, HTREEITEM root)
{
    my_printf("seq_parameter_set_data()");
    HTREEITEM isps = AddTreeItem(root);
    my_printf("profile_idc: %d", sps->profile_idc ); AddTreeItem(isps);
    my_printf("constraint_set0_flag: %d", sps->constraint_set0_flag ); AddTreeItem(isps);
    my_printf("constraint_set1_flag: %d", sps->constraint_set1_flag ); AddTreeItem(isps);
    my_printf("constraint_set2_flag: %d", sps->constraint_set2_flag ); AddTreeItem(isps);
    my_printf("constraint_set3_flag: %d", sps->constraint_set3_flag ); AddTreeItem(isps);
    my_printf("constraint_set4_flag: %d", sps->constraint_set4_flag ); AddTreeItem(isps);
    my_printf("constraint_set5_flag: %d", sps->constraint_set5_flag ); AddTreeItem(isps);
    my_printf("reserved_zero_2bits: %d", sps->reserved_zero_2bits ); AddTreeItem(isps);
    my_printf("level_idc: %d", sps->level_idc ); AddTreeItem(isps);
    my_printf("seq_parameter_set_id: %d", sps->seq_parameter_set_id ); AddTreeItem(isps);
    my_printf("chroma_format_idc: %d", sps->chroma_format_idc ); AddTreeItem(isps);
    if (sps->chroma_format_idc == 3)
        my_printf("separate_colour_plane_flag: %d", sps->separate_colour_plane_flag ); AddTreeItem(isps);
    my_printf("bit_depth_luma_minus8: %d", sps->bit_depth_luma_minus8 ); AddTreeItem(isps);
    my_printf("bit_depth_chroma_minus8: %d", sps->bit_depth_chroma_minus8 ); AddTreeItem(isps);
    my_printf("qpprime_y_zero_transform_bypass_flag: %d", sps->qpprime_y_zero_transform_bypass_flag ); AddTreeItem(isps);
    my_printf("seq_scaling_matrix_present_flag: %d", sps->seq_scaling_matrix_present_flag ); AddTreeItem(isps);
    if (sps->seq_scaling_matrix_present_flag)
    {
        for (int i = 0; i < ((sps->chroma_format_idc!=3) ? 8 : 12); i++)
        {
            my_printf("seq_scaling_list_present_flag[%d]: %d", i, sps->seq_scaling_list_present_flag[i]);
            if( sps->seq_scaling_list_present_flag[ i ] )
            {
                if( i < 6 )
                    my_printf("ScalingList4x4[%d]: %d", i, sps->ScalingList4x4[i] );
                else
                    my_printf("ScalingList4xScalingList8x84[%d]: %d", i, sps->ScalingList8x8[i] );
            }
        }
    }

    my_printf("log2_max_frame_num_minus4: %d", sps->log2_max_frame_num_minus4 );
    my_printf("pic_order_cnt_type: %d", sps->pic_order_cnt_type );
    if( sps->pic_order_cnt_type == 0 )
        my_printf("log2_max_pic_order_cnt_lsb_minus4: %d", sps->log2_max_pic_order_cnt_lsb_minus4 );
    else if( sps->pic_order_cnt_type == 1 )
    {
        my_printf("delta_pic_order_always_zero_flag: %d", sps->delta_pic_order_always_zero_flag );
        my_printf("offset_for_non_ref_pic: %d", sps->offset_for_non_ref_pic );
        my_printf("offset_for_top_to_bottom_field: %d", sps->offset_for_top_to_bottom_field );
        my_printf("num_ref_frames_in_pic_order_cnt_cycle: %d", sps->num_ref_frames_in_pic_order_cnt_cycle );
        for( int i = 0; i < sps->num_ref_frames_in_pic_order_cnt_cycle; i++ )
        {
            my_printf("offset_for_ref_frame[%d]: %d", i, sps->offset_for_ref_frame[i] );
        }
    }
    my_printf("num_ref_frames: %d", sps->num_ref_frames );
    my_printf("gaps_in_frame_num_value_allowed_flag: %d", sps->gaps_in_frame_num_value_allowed_flag );
    my_printf("pic_width_in_mbs_minus1: %d", sps->pic_width_in_mbs_minus1 );
    my_printf("pic_height_in_map_units_minus1: %d", sps->pic_height_in_map_units_minus1 );
    my_printf("frame_mbs_only_flag: %d", sps->frame_mbs_only_flag );
    if( !sps->frame_mbs_only_flag )
        my_printf("mb_adaptive_frame_field_flag: %d", sps->mb_adaptive_frame_field_flag );
    my_printf("direct_8x8_inference_flag: %d", sps->direct_8x8_inference_flag );
    my_printf("frame_cropping_flag: %d", sps->frame_cropping_flag );
    if (sps->frame_cropping_flag)
    {
        my_printf("frame_crop_left_offset: %d", sps->frame_crop_left_offset );
        my_printf("frame_crop_right_offset: %d", sps->frame_crop_right_offset );
        my_printf("frame_crop_top_offset: %d", sps->frame_crop_top_offset );
        my_printf("frame_crop_bottom_offset: %d", sps->frame_crop_bottom_offset );
    }
    my_printf("vui_parameters_present_flag: %d", sps->vui_parameters_present_flag );
    if (sps->vui_parameters_present_flag)
    {
        my_printf("vui_parameters()");
        my_printf("aspect_ratio_info_present_flag: %d", sps->vui.aspect_ratio_info_present_flag );
        if( sps->vui.aspect_ratio_info_present_flag )
        {
            my_printf("aspect_ratio_idc: %d", sps->vui.aspect_ratio_idc );
            if( sps->vui.aspect_ratio_idc == SAR_Extended )
            {
                my_printf("sar_width: %d", sps->vui.sar_width );
                my_printf("sar_height: %d", sps->vui.sar_height );
            }
        }

        my_printf("overscan_info_present_flag: %d", sps->vui.overscan_info_present_flag );
        if( sps->vui.overscan_info_present_flag )
            my_printf("overscan_appropriate_flag: %d", sps->vui.overscan_appropriate_flag );
        my_printf("video_signal_type_present_flag: %d", sps->vui.video_signal_type_present_flag );
        if( sps->vui.video_signal_type_present_flag )
        {
            my_printf("video_format: %d", sps->vui.video_format );
            my_printf("video_full_range_flag: %d", sps->vui.video_full_range_flag );
            my_printf("colour_description_present_flag: %d", sps->vui.colour_description_present_flag );
            if( sps->vui.colour_description_present_flag )
            {
                my_printf("colour_primaries: %d", sps->vui.colour_primaries );
                my_printf("transfer_characteristics: %d", sps->vui.transfer_characteristics );
                my_printf("matrix_coefficients: %d", sps->vui.matrix_coefficients );
            }
        }
        my_printf("chroma_loc_info_present_flag: %d", sps->vui.chroma_loc_info_present_flag );
        if( sps->vui.chroma_loc_info_present_flag )
        {
            my_printf("chroma_sample_loc_type_top_field: %d", sps->vui.chroma_sample_loc_type_top_field );
            my_printf("chroma_sample_loc_type_bottom_field: %d", sps->vui.chroma_sample_loc_type_bottom_field );
        }
        if( sps->vui.timing_info_present_flag )
        {
            my_printf("timing_info_present_flag: %d", sps->vui.timing_info_present_flag );
            my_printf("num_units_in_tick: %d", sps->vui.num_units_in_tick );
            my_printf("time_scale: %d", sps->vui.time_scale );
            my_printf("fixed_frame_rate_flag: %d", sps->vui.fixed_frame_rate_flag );
        }
        my_printf("nal_hrd_parameters_present_flag: %d", sps->vui.nal_hrd_parameters_present_flag );
        if( sps->vui.nal_hrd_parameters_present_flag )
        {
            my_printf("hrd_parameters()");
            my_printf("cpb_cnt_minus1: %d", sps->hrd.cpb_cnt_minus1 );
            my_printf("bit_rate_scale: %d", sps->hrd.bit_rate_scale );
            my_printf("cpb_size_scale: %d", sps->hrd.cpb_size_scale );
            int SchedSelIdx;
            for( SchedSelIdx = 0; SchedSelIdx <= sps->hrd.cpb_cnt_minus1; SchedSelIdx++ )
            {
                my_printf("bit_rate_value_minus1[%d]: %d", SchedSelIdx, sps->hrd.bit_rate_value_minus1[SchedSelIdx] ); // up to cpb_cnt_minus1, which is <= 31
                my_printf("cpb_size_value_minus1[%d]: %d", SchedSelIdx, sps->hrd.cpb_size_value_minus1[SchedSelIdx] );
                my_printf("cbr_flag[%d]: %d", SchedSelIdx, sps->hrd.cbr_flag[SchedSelIdx] );
            }
            my_printf("initial_cpb_removal_delay_length_minus1: %d", sps->hrd.initial_cpb_removal_delay_length_minus1 );
            my_printf("cpb_removal_delay_length_minus1: %d", sps->hrd.cpb_removal_delay_length_minus1 );
            my_printf("dpb_output_delay_length_minus1: %d", sps->hrd.dpb_output_delay_length_minus1 );
            my_printf("time_offset_length: %d", sps->hrd.time_offset_length );
        }
        my_printf("vcl_hrd_parameters_present_flag: %d", sps->vui.vcl_hrd_parameters_present_flag );
        if( sps->vui.vcl_hrd_parameters_present_flag )
        {
            my_printf("hrd_parameters()");
            my_printf("cpb_cnt_minus1: %d", sps->hrd.cpb_cnt_minus1 );
            my_printf("bit_rate_scale: %d", sps->hrd.bit_rate_scale );
            my_printf("cpb_size_scale: %d", sps->hrd.cpb_size_scale );
            int SchedSelIdx;
            for( SchedSelIdx = 0; SchedSelIdx <= sps->hrd.cpb_cnt_minus1; SchedSelIdx++ )
            {
                my_printf("bit_rate_value_minus1[%d]: %d", SchedSelIdx, sps->hrd.bit_rate_value_minus1[SchedSelIdx] ); // up to cpb_cnt_minus1, which is <= 31
                my_printf("cpb_size_value_minus1[%d]: %d", SchedSelIdx, sps->hrd.cpb_size_value_minus1[SchedSelIdx] );
                my_printf("cbr_flag[%d]: %d", SchedSelIdx, sps->hrd.cbr_flag[SchedSelIdx] );
            }
            my_printf("initial_cpb_removal_delay_length_minus1: %d", sps->hrd.initial_cpb_removal_delay_length_minus1 );
            my_printf("cpb_removal_delay_length_minus1: %d", sps->hrd.cpb_removal_delay_length_minus1 );
            my_printf("dpb_output_delay_length_minus1: %d", sps->hrd.dpb_output_delay_length_minus1 );
            my_printf("time_offset_length: %d", sps->hrd.time_offset_length );
        }
        if( sps->vui.nal_hrd_parameters_present_flag || sps->vui.vcl_hrd_parameters_present_flag )
            my_printf("low_delay_hrd_flag: %d", sps->vui.low_delay_hrd_flag );
        my_printf("pic_struct_present_flag: %d", sps->vui.pic_struct_present_flag );
        my_printf("bitstream_restriction_flag: %d", sps->vui.bitstream_restriction_flag );
        if( sps->vui.bitstream_restriction_flag )
        {
            my_printf("motion_vectors_over_pic_boundaries_flag: %d", sps->vui.motion_vectors_over_pic_boundaries_flag );
            my_printf("max_bytes_per_pic_denom: %d", sps->vui.max_bytes_per_pic_denom );
            my_printf("max_bits_per_mb_denom: %d", sps->vui.max_bits_per_mb_denom );
            my_printf("log2_max_mv_length_horizontal: %d", sps->vui.log2_max_mv_length_horizontal );
            my_printf("log2_max_mv_length_vertical: %d", sps->vui.log2_max_mv_length_vertical );
            my_printf("num_reorder_frames: %d", sps->vui.num_reorder_frames );
            my_printf("max_dec_frame_buffering: %d", sps->vui.max_dec_frame_buffering );
        }
    }
}

#if 0
void CNalParser::h264_debug_pps(pps_t* pps)
{
    my_printf("pic_parameter_set_rbsp()");
    my_printf("pic_parameter_set_id: %d", pps->pic_parameter_set_id );
    my_printf("seq_parameter_set_id: %d", pps->seq_parameter_set_id );
    my_printf("entropy_coding_mode_flag: %d", pps->entropy_coding_mode_flag );
    my_printf("pic_order_present_flag: %d", pps->pic_order_present_flag );
    my_printf("num_slice_groups_minus1: %d", pps->num_slice_groups_minus1 );
    if( pps->num_slice_groups_minus1 > 0 )
    {
        my_printf("slice_group_map_type: %d", pps->slice_group_map_type );
        if( pps->slice_group_map_type == 0 )
        {
            for( int i_group = 0; i_group <= pps->num_slice_groups_minus1; i_group++ )
                my_printf("run_length_minus1[%d]: %d", i_group, pps->run_length_minus1[i_group] );
        }
        else if( pps->slice_group_map_type == 2 )
        {
            for( int i_group = 0; i_group <= pps->num_slice_groups_minus1; i_group++ )
            {
                my_printf("top_left[%d]: %d", i_group, pps->top_left[i_group] );
                my_printf("bottom_right[%d]: %d", i_group, pps->bottom_right[i_group] );
            }
        }
        else if( pps->slice_group_map_type == 3 ||
            pps->slice_group_map_type == 4 ||
            pps->slice_group_map_type == 5 )
        {
            my_printf("slice_group_change_direction_flag: %d", pps->slice_group_change_direction_flag );
            my_printf("slice_group_change_rate_minus1: %d", pps->slice_group_change_rate_minus1 );
        }
        else if( pps->slice_group_map_type == 6 )
        {            
            my_printf("pic_size_in_map_units_minus1: %d", pps->pic_size_in_map_units_minus1 );
            for( int i = 0; i <= pps->pic_size_in_map_units_minus1; i++ )
                my_printf("slice_group_id[%d]: %d", i, pps->slice_group_id[i] );
        }
    }
    my_printf("num_ref_idx_l0_active_minus1: %d", pps->num_ref_idx_l0_active_minus1 );
    my_printf("num_ref_idx_l1_active_minus1: %d", pps->num_ref_idx_l1_active_minus1 );
    my_printf("weighted_pred_flag: %d", pps->weighted_pred_flag );
    my_printf("weighted_bipred_idc: %d", pps->weighted_bipred_idc );
    my_printf("pic_init_qp_minus26: %d", pps->pic_init_qp_minus26 );
    my_printf("pic_init_qs_minus26: %d", pps->pic_init_qs_minus26 );
    my_printf("chroma_qp_index_offset: %d", pps->chroma_qp_index_offset );
    my_printf("deblocking_filter_control_present_flag: %d", pps->deblocking_filter_control_present_flag );
    my_printf("constrained_intra_pred_flag: %d", pps->constrained_intra_pred_flag );
    my_printf("redundant_pic_cnt_present_flag: %d", pps->redundant_pic_cnt_present_flag );
    if( pps->_more_rbsp_data_present )
    {
        my_printf("more_rbsp_data()" );
        my_printf("transform_8x8_mode_flag: %d", pps->transform_8x8_mode_flag );
        my_printf("pic_scaling_matrix_present_flag: %d", pps->pic_scaling_matrix_present_flag );
        if( pps->pic_scaling_matrix_present_flag )
        {
            for( int i = 0; i < 6 + 2* pps->transform_8x8_mode_flag; i++ )
            {
                my_printf("pic_scaling_list_present_flag[%d]: %d", i, pps->pic_scaling_list_present_flag[i] );
                if( pps->pic_scaling_list_present_flag[ i ] )
                {
                    if( i < 6 )
                        my_printf("ScalingList4x4[%d]: %d", i, pps->ScalingList4x4[i] );
                    else
                        my_printf("ScalingList4xScalingList8x84[%d]: %d", i, pps->ScalingList8x8[i] );
                }
            }
        }
        my_printf("second_chroma_qp_index_offset: %d", pps->second_chroma_qp_index_offset );
    }
    my_printf("rbsp_trailing_bits()");
}

void CNalParser::h264_debug_slice_header(h264_stream_t* h)
{
    sps_t* sps = h->sps;
    pps_t* pps = h->pps;
    slice_header_t* sh = h->sh;
    nal_t* nal = h->nal;

    my_printf("slice_layer_without_partitioning_rbsp()");
    my_printf("slice_header()");
    my_printf("first_mb_in_slice: %d", sh->first_mb_in_slice );
    const char* slice_type_name;
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
    my_printf("slice_type: %d (%s)", sh->slice_type, slice_type_name );

    my_printf("pic_parameter_set_id: %d", sh->pic_parameter_set_id );
    if (sps->separate_colour_plane_flag == 1)
    {
        my_printf("colour_plane_id: %d", sh->colour_plane_id );
    }
    my_printf("frame_num: %d", sh->frame_num );
    if( !sps->frame_mbs_only_flag )
    {
        my_printf("field_pic_flag: %d", sh->field_pic_flag );
        if( sh->field_pic_flag )
            my_printf("bottom_field_flag: %d", sh->bottom_field_flag );
    }
    if( nal->nal_unit_type == 5 )
        my_printf("idr_pic_id: %d", sh->idr_pic_id );
    if( sps->pic_order_cnt_type == 0 )
    {
        my_printf("pic_order_cnt_lsb: %d", sh->pic_order_cnt_lsb );
        if( pps->pic_order_present_flag && !sh->field_pic_flag )
            my_printf("delta_pic_order_cnt_bottom: %d", sh->delta_pic_order_cnt_bottom );
    }

    if( sps->pic_order_cnt_type == 1 && !sps->delta_pic_order_always_zero_flag )
    {
        my_printf("delta_pic_order_cnt[0]: %d", sh->delta_pic_order_cnt[0] );
        if( pps->pic_order_present_flag && !sh->field_pic_flag )
            my_printf("delta_pic_order_cnt[1]: %d", sh->delta_pic_order_cnt[1] );
    }
    if( pps->redundant_pic_cnt_present_flag )
        my_printf("redundant_pic_cnt: %d", sh->redundant_pic_cnt );
    if( is_slice_type( sh->slice_type, SH_SLICE_TYPE_B ) )
        my_printf("direct_spatial_mv_pred_flag: %d", sh->direct_spatial_mv_pred_flag );
    if( is_slice_type( sh->slice_type, SH_SLICE_TYPE_P ) || is_slice_type( sh->slice_type, SH_SLICE_TYPE_SP ) || is_slice_type( sh->slice_type, SH_SLICE_TYPE_B ) )
    {
        my_printf("num_ref_idx_active_override_flag: %d", sh->num_ref_idx_active_override_flag );
        if( sh->num_ref_idx_active_override_flag )
        {
            my_printf("num_ref_idx_l0_active_minus1: %d", sh->num_ref_idx_l0_active_minus1 );
            if( is_slice_type( sh->slice_type, SH_SLICE_TYPE_B ) )
                my_printf("num_ref_idx_l1_active_minus1: %d", sh->num_ref_idx_l1_active_minus1 );
        }
    }
    // ref_pic_list_modification
    if (nal->nal_unit_type == 20 || nal->nal_unit_type == 21)
    {
        // todo.....
        my_printf("ref_pic_list_mvc_modification()");
    }
    else
    {
        my_printf("ref_pic_list_modification()");
        if( ! is_slice_type( sh->slice_type, SH_SLICE_TYPE_I ) && ! is_slice_type( sh->slice_type, SH_SLICE_TYPE_SI ) )
        {
            my_printf("ref_pic_list_modification_flag_l0: %d", sh->rplm.ref_pic_list_modification_flag_l0 );
            if( sh->rplm.ref_pic_list_modification_flag_l0 )
            {
                for (unsigned int i = 0; i < sh->rplm.rplm.size(); i++)
                {
                    my_printf("modification_of_pic_nums_idc: %d", sh->rplm.rplm[i].modification_of_pic_nums_idc);
                    if( sh->rplm.rplm[i].modification_of_pic_nums_idc == 0 ||
                        sh->rplm.rplm[i].modification_of_pic_nums_idc == 1 )
                        my_printf("abs_diff_pic_num_minus1: %d", sh->rplm.rplm[i].abs_diff_pic_num_minus1 );
                    else if( sh->rplm.rplm[i].modification_of_pic_nums_idc == 2 )
                        my_printf("long_term_pic_num: %d", sh->rplm.rplm[i].long_term_pic_num );
                }

            }
        }
        if( is_slice_type( sh->slice_type, SH_SLICE_TYPE_B ) )
        {
            my_printf("ref_pic_list_modification_flag_l1: %d", sh->rplm.ref_pic_list_modification_flag_l1 );
            if( sh->rplm.ref_pic_list_modification_flag_l1 )
            {
                for (unsigned int i = 0; i < sh->rplm.rplm.size(); i++)
                {
                    my_printf("modification_of_pic_nums_idc: %d", sh->rplm.rplm[i].modification_of_pic_nums_idc );
                    if( sh->rplm.rplm[i].modification_of_pic_nums_idc == 0 ||
                        sh->rplm.rplm[i].modification_of_pic_nums_idc == 1 )
                        my_printf("abs_diff_pic_num_minus1: %d", sh->rplm.rplm[i].abs_diff_pic_num_minus1 );
                    else if( sh->rplm.rplm[i].modification_of_pic_nums_idc == 2 )
                        my_printf("long_term_pic_num: %d", sh->rplm.rplm[i].long_term_pic_num );
                }
            }
        }
        
    }

    // pred_weight_table()
    if( ( pps->weighted_pred_flag && ( is_slice_type( sh->slice_type, SH_SLICE_TYPE_P ) || is_slice_type( sh->slice_type, SH_SLICE_TYPE_SP ) ) ) ||
        ( pps->weighted_bipred_idc == 1 && is_slice_type( sh->slice_type, SH_SLICE_TYPE_B ) ) )
    {
        my_printf("pred_weight_table()");
        my_printf("luma_log2_weight_denom: %d", sh->pwt.luma_log2_weight_denom );
        if( sps->ChromaArrayType != 0 )
            my_printf("chroma_log2_weight_denom: %d", sh->pwt.chroma_log2_weight_denom );
        // 将luma和chroma分开显示
        if (sh->num_ref_idx_l0_active_minus1 > 0)
            my_printf("luma_weight_l0()");
        for( int i = 0; i <= sh->num_ref_idx_l0_active_minus1; i++ )
        {
            my_printf("luma_weight_l0_flag[%d]: %d", i, sh->pwt.luma_weight_l0_flag[i] );
            if( sh->pwt.luma_weight_l0_flag[i] )
            {
                my_printf("luma_weight_l0[%d]: %d", i, sh->pwt.luma_weight_l0[i] );
                my_printf("luma_offset_l0[%d]: %d", i, sh->pwt.luma_offset_l0[i] );
            }
        }
        if (sh->num_ref_idx_l0_active_minus1 > 0)
            my_printf("chroma_weight_l0()");
        for( int i = 0; i <= sh->num_ref_idx_l0_active_minus1; i++ )
        {
            if ( sps->ChromaArrayType != 0 )
            {
                my_printf("chroma_weight_l0_flag[%d]: %d", i, sh->pwt.chroma_weight_l0_flag[i] );
                if( sh->pwt.chroma_weight_l0_flag[i] )
                {
                    for( int j =0; j < 2; j++ )
                    {
                        my_printf("chroma_weight_l0[%d][%d]: %d", i, j, sh->pwt.chroma_weight_l0[i][j] );
                        my_printf("chroma_weight_l0[%d][%d]: %d", i, j, sh->pwt.chroma_offset_l0[i][j] );
                    }
                }
            }
        }
        if( is_slice_type( sh->slice_type, SH_SLICE_TYPE_B ) )
        {
            if (sh->num_ref_idx_l1_active_minus1 > 0)
                my_printf("luma_weight_l1()");
            for( int i = 0; i <= sh->num_ref_idx_l1_active_minus1; i++ )
            {
                my_printf("luma_weight_l1_flag[%d]: %d", i, sh->pwt.luma_weight_l1_flag[i] );
                if( sh->pwt.luma_weight_l1_flag[i] )
                {
                    my_printf("luma_weight_l1[%d]: %d", i, sh->pwt.luma_weight_l1[i] );
                    my_printf("luma_offset_l1[%d]: %d", i, sh->pwt.luma_offset_l1[i] );
                }
            }
            if (sh->num_ref_idx_l1_active_minus1 > 0)
                my_printf("chroma_weight_l1()");
            for( int i = 0; i <= sh->num_ref_idx_l1_active_minus1; i++ )
            {
                if ( sps->ChromaArrayType != 0 )
                {
                    my_printf("chroma_weight_l1_flag[%d]: %d", i, sh->pwt.chroma_weight_l1_flag[i] );
                    if( sh->pwt.chroma_weight_l1_flag[i] )
                    {
                        for( int j =0; j < 2; j++ )
                        {
                            my_printf("chroma_weight_l1[%d][%d]: %d", i, j, sh->pwt.chroma_weight_l1[i][j] );
                            my_printf("chroma_offset_l1[%d][%d]: %d", i, j, sh->pwt.chroma_offset_l1[i][j] );
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
        if( h->nal->nal_unit_type == 5 )
        {
            my_printf("no_output_of_prior_pics_flag: %d", sh->drpm.no_output_of_prior_pics_flag );
            my_printf("long_term_reference_flag: %d", sh->drpm.long_term_reference_flag );
        }
        else
        {
            my_printf("adaptive_ref_pic_marking_mode_flag: %d", sh->drpm.adaptive_ref_pic_marking_mode_flag );
            if( sh->drpm.adaptive_ref_pic_marking_mode_flag )
            {
                for (unsigned int i = 0; i < sh->drpm.drpm.size(); i++)
                {
                    my_printf("memory_management_control_operation: %d", sh->drpm.drpm[i].memory_management_control_operation );
                    if( sh->drpm.drpm[i].memory_management_control_operation == 1 ||
                        sh->drpm.drpm[i].memory_management_control_operation == 3 )
                        my_printf("difference_of_pic_nums_minus1: %d", sh->drpm.drpm[i].difference_of_pic_nums_minus1 );
                    if(sh->drpm.drpm[i].memory_management_control_operation == 2 )
                        my_printf("long_term_pic_num: %d", sh->drpm.drpm[i].long_term_pic_num );
                    if( sh->drpm.drpm[i].memory_management_control_operation == 3 ||
                        sh->drpm.drpm[i].memory_management_control_operation == 6 )
                        my_printf("long_term_frame_idx: %d", sh->drpm.drpm[i].long_term_frame_idx );
                    if( sh->drpm.drpm[i].memory_management_control_operation == 4 )
                        my_printf("max_long_term_frame_idx_plus1: %d", sh->drpm.drpm[i].max_long_term_frame_idx_plus1 );
                }
            }
        }
    }
    if( pps->entropy_coding_mode_flag && ! is_slice_type( sh->slice_type, SH_SLICE_TYPE_I ) && ! is_slice_type( sh->slice_type, SH_SLICE_TYPE_SI ) )
        my_printf("cabac_init_idc: %d", sh->cabac_init_idc );
    my_printf("slice_qp_delta: %d", sh->slice_qp_delta );
    if( is_slice_type( sh->slice_type, SH_SLICE_TYPE_SP ) || is_slice_type( sh->slice_type, SH_SLICE_TYPE_SI ) )
    {
        if( is_slice_type( sh->slice_type, SH_SLICE_TYPE_SP ) )
            my_printf("sp_for_switch_flag: %d", sh->sp_for_switch_flag );
        my_printf("slice_qs_delta: %d", sh->slice_qs_delta );
    }
    if( pps->deblocking_filter_control_present_flag )
    {
        my_printf("disable_deblocking_filter_idc: %d", sh->disable_deblocking_filter_idc );
        if( sh->disable_deblocking_filter_idc != 1 )
        {
            my_printf("slice_alpha_c0_offset_div2: %d", sh->slice_alpha_c0_offset_div2 );
            my_printf("slice_beta_offset_div2: %d", sh->slice_beta_offset_div2 );
        }
    }

    if( pps->num_slice_groups_minus1 > 0 &&
        pps->slice_group_map_type >= 3 && pps->slice_group_map_type <= 5)
        my_printf("slice_group_change_cycle: %d", sh->slice_group_change_cycle );

    my_printf("slice_data()");
    my_printf("rbsp_slice_trailing_bits()");
}

void CNalParser::h264_debug_aud(aud_t* aud)
{
    my_printf("======= Access Unit Delimiter =======");
    const char* primary_pic_type_name;
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
    my_printf("primary_pic_type: %d (%s)", aud->primary_pic_type, primary_pic_type_name );
}

void CNalParser::h264_debug_seis( h264_stream_t* h)
{
    sei_t** seis = h->seis;
    int num_seis = h->num_seis;

    my_printf("======= SEI =======");
    const char* sei_type_name;
    int i;
    for (i = 0; i < num_seis; i++)
    {
        sei_t* s = seis[i];
        my_printf("payloadType: %d", s->payloadType );
        my_printf("payloadSize: %d", s->payloadSize );
        switch(s->payloadType)
        {
        case SEI_TYPE_BUFFERING_PERIOD:          sei_type_name = "Buffering period"; break;
        case SEI_TYPE_PIC_TIMING:                sei_type_name = "Pic timing"; break;
        case SEI_TYPE_PAN_SCAN_RECT:             sei_type_name = "Pan scan rect"; break;
        case SEI_TYPE_FILLER_PAYLOAD:            sei_type_name = "Filler payload"; break;
        case SEI_TYPE_USER_DATA_REGISTERED_ITU_T_T35: sei_type_name = "User data registered ITU-T T35"; break;
        case SEI_TYPE_USER_DATA_UNREGISTERED :
            {
                
                my_printf("sei_payload()");
                my_printf("user_data_unregistered()");
                my_printf("uuid_iso_iec_11578: ");
                for (int j = 0; j < 16; j++)
                    my_printf("%X", s->payload[j]);
                my_printf ("\r\n   ");
                for (int j = 16; j < s->payloadSize; j++)
                {
                    my_printf("%c", s->payload[j]);
                    if ((j+1) % 128 == 0) my_printf ("");
                }
                break;
            }
            
        case SEI_TYPE_RECOVERY_POINT:            sei_type_name = "Recovery point"; break;
        case SEI_TYPE_DEC_REF_PIC_MARKING_REPETITION: sei_type_name = "Dec ref pic marking repetition"; break;
        case SEI_TYPE_SPARE_PIC:                 sei_type_name = "Spare pic"; break;
        case SEI_TYPE_SCENE_INFO:                sei_type_name = "Scene info"; break;
        case SEI_TYPE_SUB_SEQ_INFO:              sei_type_name = "Sub seq info"; break;
        case SEI_TYPE_SUB_SEQ_LAYER_CHARACTERISTICS: sei_type_name = "Sub seq layer characteristics"; break;
        case SEI_TYPE_SUB_SEQ_CHARACTERISTICS:   sei_type_name = "Sub seq characteristics"; break;
        case SEI_TYPE_FULL_FRAME_FREEZE:         sei_type_name = "Full frame freeze"; break;
        case SEI_TYPE_FULL_FRAME_FREEZE_RELEASE: sei_type_name = "Full frame freeze release"; break;
        case SEI_TYPE_FULL_FRAME_SNAPSHOT:       sei_type_name = "Full frame snapshot"; break;
        case SEI_TYPE_PROGRESSIVE_REFINEMENT_SEGMENT_START: sei_type_name = "Progressive refinement segment start"; break;
        case SEI_TYPE_PROGRESSIVE_REFINEMENT_SEGMENT_END: sei_type_name = "Progressive refinement segment end"; break;
        case SEI_TYPE_MOTION_CONSTRAINED_SLICE_GROUP_SET: sei_type_name = "Motion constrained slice group set"; break;
        case SEI_TYPE_FILM_GRAIN_CHARACTERISTICS: sei_type_name = "Film grain characteristics"; break;
        case SEI_TYPE_DEBLOCKING_FILTER_DISPLAY_PREFERENCE: sei_type_name = "Deblocking filter display preference"; break;
        case SEI_TYPE_STEREO_VIDEO_INFO:         sei_type_name = "Stereo video info"; break;
        default: sei_type_name = "Unknown"; break;
        }
    }
}
#endif
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

    if( nal->nal_unit_type == NAL_UNIT_TYPE_CODED_SLICE_NON_IDR) { h264_debug_slice_header(h); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_CODED_SLICE_IDR) { h264_debug_slice_header(h); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_SPS) { h264_debug_sps_t(h->sps, root); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_PPS) { h264_debug_pps(h->pps); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_AUD) { h264_debug_aud(h->aud); }
    else if( nal->nal_unit_type == NAL_UNIT_TYPE_SEI) { h264_debug_seis(h); }
}

////////////////////////////////////////////////////////
void CNalParser::h265_debug_ptl_t(profile_tier_level_t* ptl, int profilePresentFlag, int max_sub_layers_minus1, HTREEITEM root)
{
    if (profilePresentFlag)
    {
        my_printf("general_profile_space: %d", ptl->general_profile_space); AddTreeItem(root);
        my_printf("general_tier_flag: %d", ptl->general_tier_flag); AddTreeItem(root);
        my_printf("general_profile_idc: %d", ptl->general_profile_idc); AddTreeItem(root);
        for (int i = 0; i < 32; i++)
        {
            my_printf("general_profile_compatibility_flag[%d]: %d", i, ptl->general_profile_compatibility_flag[i]); AddTreeItem(root);
        }
        my_printf("general_progressive_source_flag: %d", ptl->general_progressive_source_flag); AddTreeItem(root);
        my_printf("general_interlaced_source_flag: %d", ptl->general_interlaced_source_flag); AddTreeItem(root);
        my_printf("general_non_packed_constraint_flag: %d", ptl->general_non_packed_constraint_flag); AddTreeItem(root);
        my_printf("general_frame_only_constraint_flag: %d", ptl->general_frame_only_constraint_flag); AddTreeItem(root);
        if (ptl->general_profile_idc==4 || ptl->general_profile_compatibility_flag[4] ||
            ptl->general_profile_idc==5 || ptl->general_profile_compatibility_flag[5] ||
            ptl->general_profile_idc==6 || ptl->general_profile_compatibility_flag[6] ||
            ptl->general_profile_idc==7 || ptl->general_profile_compatibility_flag[7])
        {
            my_printf("general_max_12bit_constraint_flag: %d", ptl->general_max_12bit_constraint_flag); AddTreeItem(root);
            my_printf("general_max_10bit_constraint_flag: %d", ptl->general_max_10bit_constraint_flag); AddTreeItem(root);
            my_printf("general_max_8bit_constraint_flag: %d", ptl->general_max_8bit_constraint_flag); AddTreeItem(root);
            my_printf("general_max_422chroma_constraint_flag: %d", ptl->general_max_422chroma_constraint_flag); AddTreeItem(root);
            my_printf("general_max_420chroma_constraint_flag: %d", ptl->general_max_420chroma_constraint_flag); AddTreeItem(root);
            my_printf("general_max_monochrome_constraint_flag: %d", ptl->general_max_monochrome_constraint_flag); AddTreeItem(root);
            my_printf("general_intra_constraint_flag: %d", ptl->general_intra_constraint_flag); AddTreeItem(root);
            my_printf("general_one_picture_only_constraint_flag: %d", ptl->general_one_picture_only_constraint_flag); AddTreeItem(root);
            my_printf("general_lower_bit_rate_constraint_flag: %d", ptl->general_lower_bit_rate_constraint_flag); AddTreeItem(root);
            my_printf("general_reserved_zero_34bits: %u", ptl->general_reserved_zero_34bits); AddTreeItem(root); // tocheck
        }
        else
        {
            my_printf("general_reserved_zero_43bits: %u", ptl->general_reserved_zero_43bits); AddTreeItem(root);// tocheck
        }
        if ((ptl->general_profile_idc>=1 && ptl->general_profile_idc<=5) ||
            ptl->general_profile_compatibility_flag[1] || ptl->general_profile_compatibility_flag[2] ||
            ptl->general_profile_compatibility_flag[3] || ptl->general_profile_compatibility_flag[4] ||
            ptl->general_profile_compatibility_flag[5])
        {
            my_printf("general_inbld_flag: %d", ptl->general_inbld_flag); AddTreeItem(root);
        }
        else
        {
            my_printf("general_reserved_zero_bit: %d", ptl->general_reserved_zero_bit); AddTreeItem(root);
        }
    }
        
    my_printf("general_level_idc: %d", ptl->general_level_idc); AddTreeItem(root);
    for (int i = 0; i < max_sub_layers_minus1; i++)
    {
        my_printf("sub_layer_profile_present_flag[%d]: %d", i, ptl->sub_layer_profile_present_flag[i]); AddTreeItem(root);
        my_printf("sub_layer_level_present_flag[%d]: %d", i, ptl->sub_layer_level_present_flag[i]); AddTreeItem(root);
    }
    if (max_sub_layers_minus1 > 0)
    {
        for (int i = max_sub_layers_minus1; i < 8; i++)
        {
            my_printf("reserved_zero_2bits[%d]: %d", i, ptl->reserved_zero_2bits[i]); AddTreeItem(root);
        }
    }
    for (int i = 0; i < max_sub_layers_minus1; i++)
    {
        if (ptl->sub_layer_profile_present_flag[i])
        {
            my_printf("sub_layer_profile_space[%d]: %d", i, ptl->sub_layer_profile_space[i]); AddTreeItem(root);
            my_printf("sub_layer_tier_flag[%d]: %d", i, ptl->sub_layer_tier_flag[i]); AddTreeItem(root);
            my_printf("sub_layer_profile_idc[%d]: %d", i, ptl->sub_layer_profile_idc[i]); AddTreeItem(root);
            for (int j = 0; j < 32; j++)
            {
                my_printf("sub_layer_profile_compatibility_flag[%d][%d]: %d", i, j, ptl->sub_layer_profile_compatibility_flag[i][j]);
            }
            my_printf("sub_layer_progressive_source_flag[%d]: %d", i, ptl->sub_layer_progressive_source_flag[i]);
            my_printf("sub_layer_interlaced_source_flag[%d]: %d", i, ptl->sub_layer_interlaced_source_flag[i]);
            my_printf("sub_layer_non_packed_constraint_flag[%d]: %d", i, ptl->sub_layer_non_packed_constraint_flag[i]);
            my_printf("sub_layer_frame_only_constraint_flag[%d]: %d", i, ptl->sub_layer_frame_only_constraint_flag[i]);
            if (ptl->sub_layer_profile_idc[i]==4 || ptl->sub_layer_profile_compatibility_flag[i][4] ||
                ptl->sub_layer_profile_idc[i]==5 || ptl->sub_layer_profile_compatibility_flag[i][5] ||
                ptl->sub_layer_profile_idc[i]==6 || ptl->sub_layer_profile_compatibility_flag[i][6] ||
                ptl->sub_layer_profile_idc[i]==7 || ptl->sub_layer_profile_compatibility_flag[i][7])
            {
                my_printf("sub_layer_max_12bit_constraint_flag[%d]: %d", i, ptl->sub_layer_max_12bit_constraint_flag[i]);
                my_printf("sub_layer_max_10bit_constraint_flag[%d]: %d", i, ptl->sub_layer_max_10bit_constraint_flag[i]);
                my_printf("sub_layer_max_8bit_constraint_flag[%d]: %d", i, ptl->sub_layer_max_8bit_constraint_flag[i]);
                my_printf("sub_layer_max_422chroma_constraint_flag[%d]: %d", i, ptl->sub_layer_max_422chroma_constraint_flag[i]);
                my_printf("sub_layer_max_420chroma_constraint_flag[%d]: %d", i, ptl->sub_layer_max_420chroma_constraint_flag[i]);
                my_printf("sub_layer_max_monochrome_constraint_flag[%d]: %d", i, ptl->sub_layer_max_monochrome_constraint_flag[i]);
                my_printf("sub_layer_intra_constraint_flag[%d]: %d", i, ptl->sub_layer_intra_constraint_flag[i]);
                my_printf("sub_layer_one_picture_only_constraint_flag[%d]: %d", i, ptl->sub_layer_one_picture_only_constraint_flag[i]);
                my_printf("sub_layer_lower_bit_rate_constraint_flag[%d]: %d", i, ptl->sub_layer_lower_bit_rate_constraint_flag[i]);
                my_printf("sub_layer_reserved_zero_34bits[%d]: %ul", i, ptl->sub_layer_reserved_zero_34bits[i]);
            }
            else
            {
                my_printf("sub_layer_reserved_zero_43bits: %ul", ptl->sub_layer_reserved_zero_43bits);
            }
            // to check
            if ((ptl->sub_layer_profile_idc[i]>=1 && ptl->sub_layer_profile_idc[i]<=5) ||
                ptl->sub_layer_profile_compatibility_flag[i][1] ||
                ptl->sub_layer_profile_compatibility_flag[i][2] ||
                ptl->sub_layer_profile_compatibility_flag[i][3] ||
                ptl->sub_layer_profile_compatibility_flag[i][4] ||
                ptl->sub_layer_profile_compatibility_flag[i][5])
            {
                my_printf("sub_layer_inbld_flag[%d]: %d", i, ptl->sub_layer_inbld_flag[i]);
            }
            else
            {
                my_printf("sub_layer_reserved_zero_bit[%d]: %d", i, ptl->sub_layer_reserved_zero_bit[i]);
            }
        }
        if (ptl->sub_layer_level_present_flag[i])
        {
            my_printf("sub_layer_level_idc[%d]: %d", i, ptl->sub_layer_level_idc[i]);
        }
    }
    
}

void CNalParser::h265_debug_sub_layer_hrd_parameters_t(sub_layer_hrd_parameters_t* subhrd, int sub_pic_hrd_params_present_flag, int CpbCnt, const char* p, HTREEITEM root)
{
    my_printf("[%s] sub_layer_hrd_parameters(%d)", p);
    for (int i = 0; i <= CpbCnt; i++)
    {
        my_printf("bit_rate_value_minus1[%d]: %d", i, subhrd->bit_rate_value_minus1[i]);
        my_printf("cpb_size_value_minus1[%d]: %d", i, subhrd->cpb_size_value_minus1[i]);
        if (sub_pic_hrd_params_present_flag)
        {
            my_printf("cpb_size_du_value_minus1[%d]: %d", i, subhrd->cpb_size_du_value_minus1[i]);
            my_printf("bit_rate_du_value_minus1[%d]: %d", i, subhrd->bit_rate_du_value_minus1[i]);
        }
        my_printf("cbr_flag[%d]: %d", i, subhrd->cbr_flag[i]);
    }
}
void CNalParser::h265_debug_hrd_parameters_t(hrd_parameters_t* hrd, int commonInfPresentFlag, int maxNumSubLayersMinus1, HTREEITEM root)
{
    my_printf("hrd_parameters()");
    HTREEITEM ihrd = AddTreeItem(root);
    if(commonInfPresentFlag)
    {
        my_printf("nal_hrd_parameters_present_flag: %d", hrd->nal_hrd_parameters_present_flag); AddTreeItem(ihrd);
        my_printf("vcl_hrd_parameters_present_flag: %d", hrd->vcl_hrd_parameters_present_flag); AddTreeItem(ihrd);
        if (hrd->nal_hrd_parameters_present_flag ||
            hrd->vcl_hrd_parameters_present_flag)
        {
            
            my_printf("sub_pic_hrd_params_present_flag: %d", hrd->sub_pic_hrd_params_present_flag); AddTreeItem(ihrd);
            if (hrd->sub_pic_hrd_params_present_flag)
            {
                my_printf("tick_divisor_minus2: %d", hrd->tick_divisor_minus2); AddTreeItem(ihrd);
                my_printf("du_cpb_removal_delay_increment_length_minus1: %d", hrd->du_cpb_removal_delay_increment_length_minus1); AddTreeItem(ihrd);
                my_printf("sub_pic_cpb_params_in_pic_timing_sei_flag: %d", hrd->sub_pic_cpb_params_in_pic_timing_sei_flag); AddTreeItem(ihrd);
                my_printf("dpb_output_delay_du_length_minus1: %d", hrd->dpb_output_delay_du_length_minus1); AddTreeItem(ihrd);
            }
            my_printf("bit_rate_scale: %d", hrd->bit_rate_scale); AddTreeItem(ihrd);
            my_printf("cpb_size_scale: %d", hrd->cpb_size_scale); AddTreeItem(ihrd);
            if (hrd->sub_pic_hrd_params_present_flag)
                my_printf("cpb_size_du_scale: %d", hrd->cpb_size_du_scale); AddTreeItem(ihrd);
            my_printf("initial_cpb_removal_delay_length_minus1: %d", hrd->initial_cpb_removal_delay_length_minus1); AddTreeItem(ihrd);
            my_printf("au_cpb_removal_delay_length_minus1: %d", hrd->au_cpb_removal_delay_length_minus1); AddTreeItem(ihrd);
            my_printf("dpb_output_delay_length_minus1: %d", hrd->dpb_output_delay_length_minus1); AddTreeItem(ihrd);
        }
    }
    for (int i = 0; i <= maxNumSubLayersMinus1; i++)
    {
        my_printf("fixed_pic_rate_general_flag[%d]: %d", i, hrd->fixed_pic_rate_general_flag[i]); AddTreeItem(ihrd);
        if (!hrd->fixed_pic_rate_general_flag[i])
            my_printf("fixed_pic_rate_general_flag[%d]: %d", i, hrd->fixed_pic_rate_general_flag[i]); AddTreeItem(ihrd);
        if (hrd->fixed_pic_rate_within_cvs_flag[i])
        {
            my_printf("elemental_duration_in_tc_minus1[%d]: %d", i, hrd->elemental_duration_in_tc_minus1[i]); AddTreeItem(ihrd);
        }
        else
        {
            my_printf("low_delay_hrd_flag[%d]: %d", i, hrd->low_delay_hrd_flag[i]); AddTreeItem(ihrd);
        }
        if (!hrd->low_delay_hrd_flag[i])
            my_printf("cpb_cnt_minus1[%d]: %d", i, hrd->cpb_cnt_minus1[i]); AddTreeItem(ihrd);
            
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
void CNalParser::h265_debug_vps_t(h265_vps_t* vps, HTREEITEM root)
{
    int i, j;
    my_printf("video_parameter_set_rbsp()");
    HTREEITEM ivps = AddTreeItem(root);
    my_printf("vps_video_parameter_set_id: %d", vps->vps_video_parameter_set_id); AddTreeItem(ivps);
    my_printf("vps_base_layer_internal_flag: %d", vps->vps_base_layer_internal_flag); AddTreeItem(ivps);
    my_printf("vps_base_layer_available_flag: %d", vps->vps_base_layer_available_flag); AddTreeItem(ivps);
    my_printf("vps_max_layers_minus1: %d", vps->vps_max_layers_minus1); AddTreeItem(ivps);
    my_printf("vps_max_sub_layers_minus1: %d", vps->vps_max_sub_layers_minus1); AddTreeItem(ivps);
    my_printf("vps_temporal_id_nesting_flag: %d", vps->vps_temporal_id_nesting_flag); AddTreeItem(ivps);
    my_printf("vps_reserved_0xffff_16bits: %d", vps->vps_reserved_0xffff_16bits); AddTreeItem(ivps);
    // ptl
    my_printf("profile_tier_level()");
    HTREEITEM iptl = AddTreeItem(ivps);
    h265_debug_ptl_t(&vps->ptl, 1, vps->vps_max_layers_minus1, iptl);
    
    my_printf("vps_sub_layer_ordering_info_present_flag: %d", vps->vps_sub_layer_ordering_info_present_flag);
    AddTreeItem(ivps);
    
    if (vps->vps_sub_layer_ordering_info_present_flag)
    {
        my_printf("SubLayers");
        HTREEITEM isub = AddTreeItem(ivps);
        for (i = (vps->vps_sub_layer_ordering_info_present_flag ? 0 : vps->vps_max_sub_layers_minus1);
            i <= vps->vps_max_sub_layers_minus1; i++ )
        {
            my_printf("vps_max_dec_pic_buffering_minus1[%d]: %d", i, vps->vps_max_dec_pic_buffering_minus1[i]); AddTreeItem(isub);
            my_printf("vps_max_num_reorder_pics[%d]: %d", i, vps->vps_max_num_reorder_pics[i]); AddTreeItem(isub);
            my_printf("vps_max_latency_increase_plus1[%d]: %d", i, vps->vps_max_latency_increase_plus1[i]); AddTreeItem(isub);
        }
    }

    my_printf("vps_max_layer_id: %d", vps->vps_max_layer_id); AddTreeItem(ivps);
    my_printf("vps_num_layer_sets_minus1: %d", vps->vps_num_layer_sets_minus1); AddTreeItem(ivps);
    for (i = 1; i <= vps->vps_num_layer_sets_minus1; i++)
    {
        for (j = 0; j <= vps->vps_max_layer_id; j++)
        {
            my_printf("layer_id_included_flag[%d][%d]: %d", i, j, vps->layer_id_included_flag[i][j]);AddTreeItem(ivps);
        }
    }
    my_printf("vps_timing_info_present_flag: %d", vps->vps_timing_info_present_flag); AddTreeItem(ivps);
    if (vps->vps_timing_info_present_flag)
    {
        my_printf("timing_info");
        HTREEITEM itime = AddTreeItem(ivps);
        my_printf("vps_num_units_in_tick: %d", vps->vps_num_units_in_tick); AddTreeItem(itime);
        my_printf("vps_time_scale: %d", vps->vps_time_scale); AddTreeItem(itime);
        my_printf("vps_poc_proportional_to_timing_flag: %d", vps->vps_poc_proportional_to_timing_flag); AddTreeItem(itime);
        if (vps->vps_poc_proportional_to_timing_flag)
        {
            my_printf("vps_num_ticks_poc_diff_one_minus1: %d", vps->vps_num_ticks_poc_diff_one_minus1); AddTreeItem(itime);
        }
        for (i = 0; i < vps->vps_num_hrd_parameters; i++)
        {
            my_printf("hrd_layer_set_idx[%d]: %d", i, vps->hrd_layer_set_idx[i]); AddTreeItem(itime);
            if (i > 0)
            {
                my_printf("cprms_present_flag[%d]: %d", i, vps->cprms_present_flag[i]); AddTreeItem(itime);
            }
            //  hrd_parameters()
            h265_debug_hrd_parameters_t(&(vps->hrd_parameters), vps->cprms_present_flag[i], vps->vps_max_sub_layers_minus1, itime);
        }
    }
    my_printf("vps_extension_flag: %d", vps->vps_extension_flag); AddTreeItem(ivps);
    if (vps->vps_extension_flag)
    {
        // do nothing...
    }
}

#if 0
void CNalParser::h265_debug_scaling_list(scaling_list_data_t* sld)
{
    for(int sizeId = 0; sizeId < 4; sizeId++)
    {
        for(int matrixId = 0; matrixId < 6; matrixId += ( sizeId == 3 ) ? 3 : 1)
        {
            my_printf("scaling_list_pred_mode_flag[%d][%d]: %d", sizeId, matrixId, sld->scaling_list_pred_mode_flag[sizeId][matrixId]);
            if (!sld->scaling_list_pred_mode_flag[sizeId][matrixId])
            {
                my_printf("scaling_list_pred_mode_flag[%d][%d]: %d", sizeId, matrixId, sld->scaling_list_pred_matrix_id_delta[sizeId][matrixId]);
            }
            else
            {
                if (sizeId > 1)
                {
                    my_printf("scaling_list_dc_coef_minus8[%d][%d]: %d", sizeId, matrixId, sld->scaling_list_dc_coef_minus8[sizeId - 2][matrixId]);
                }
                for (int i = 0; i < sld->coefNum; i++)
                {

                    my_printf("ScalingList[%d][%d][%d]: %d", sizeId, matrixId, i, sld->ScalingList[sizeId][matrixId][i]);
                }
            }
        }
    }
}
void CNalParser::h265_debug_short_term_ref_pic_set(h265_sps_t* sps, st_ref_pic_set_t*st, referencePictureSets_t* rps, int stRpsIdx)
{
    my_printf("st_ref_pic_set[%d]", stRpsIdx);

    if (stRpsIdx != 0)
        my_printf("inter_ref_pic_set_prediction_flag: %d", st->inter_ref_pic_set_prediction_flag);
    if (st->inter_ref_pic_set_prediction_flag)
    {
        my_printf("delta_idx_minus1: %d", st->delta_idx_minus1);
        my_printf("delta_rps_sign: %d", st->delta_rps_sign);
        my_printf("abs_delta_rps_minus1: %d", st->abs_delta_rps_minus1);
        int rIdx = stRpsIdx - 1 - st->delta_idx_minus1;
        referencePictureSets_t* rpsRef = &sps->m_RPSList[rIdx];
        for (int j = 0; j <= rpsRef->m_numberOfPictures; j++)
        {
            my_printf("used_by_curr_pic_flag[%d]: %d", j, st->used_by_curr_pic_flag[j]);
            if (!st->used_by_curr_pic_flag[j])
            {
                my_printf("use_delta_flag[%d]: %d", j, st->use_delta_flag[j]);
            }
        }
    }
    else
    {
        my_printf("num_negative_pics: %d", st->num_negative_pics);
        my_printf("num_positive_pics: %d", st->num_positive_pics);
        for (int i = 0; i < st->num_negative_pics; i++)
        {
            my_printf("delta_poc_s0_minus1[%d]: %d", i, st->delta_poc_s0_minus1[i]);
            my_printf("used_by_curr_pic_s0_flag[%d]: %d", i, st->used_by_curr_pic_s0_flag[i]);
        }
        for (int i = 0; i < st->num_positive_pics; i++)
        {
            my_printf("delta_poc_s1_minus1[%d]: %d", i, st->delta_poc_s1_minus1[i]);
            my_printf("used_by_curr_pic_s1_flag[%d]: %d", i, st->used_by_curr_pic_s1_flag[i]);
        }
    }
}
void CNalParser::h265_debug_vui_parameters(vui_parameters_t* vui, int maxNumSubLayersMinus1)
{
    my_printf("aspect_ratio_info_present_flag: %d", vui->aspect_ratio_info_present_flag);
    if (vui->aspect_ratio_info_present_flag)
    {
        my_printf("aspect_ratio_idc: %d", vui->aspect_ratio_idc);
        if (vui->aspect_ratio_idc == H265_SAR_Extended)
        {
            my_printf("sar_width: %d", vui->sar_width);
            my_printf("sar_height: %d", vui->sar_height);
        }
    }
    my_printf("overscan_info_present_flag: %d", vui->overscan_info_present_flag);
    if (vui->overscan_info_present_flag)
    {
        my_printf("overscan_appropriate_flag: %d", vui->overscan_appropriate_flag);
    }
    my_printf("video_signal_type_present_flag: %d", vui->video_signal_type_present_flag);
    if (vui->video_signal_type_present_flag)
    {
        my_printf("video_format: %d", vui->video_format);
        my_printf("video_full_range_flag: %d", vui->video_full_range_flag);
        my_printf("colour_description_present_flag: %d", vui->colour_description_present_flag);
        if (vui->colour_description_present_flag)
        {
            my_printf("colour_primaries: %d", vui->colour_primaries);
            my_printf("transfer_characteristics: %d", vui->transfer_characteristics);
            my_printf("matrix_coeffs: %d", vui->matrix_coeffs);
        }
    }
    my_printf("chroma_loc_info_present_flag: %d", vui->chroma_loc_info_present_flag);

    if (vui->chroma_loc_info_present_flag)
    {
        my_printf("chroma_sample_loc_type_top_field: %d", vui->chroma_sample_loc_type_top_field);
        my_printf("chroma_sample_loc_type_bottom_field: %d", vui->chroma_sample_loc_type_bottom_field);
    }
    my_printf("neutral_chroma_indication_flag: %d", vui->neutral_chroma_indication_flag);
    my_printf("field_seq_flag: %d", vui->field_seq_flag);
    my_printf("frame_field_info_present_flag: %d", vui->frame_field_info_present_flag);
    my_printf("default_display_window_flag: %d", vui->default_display_window_flag);
    if (vui->default_display_window_flag)
    {
        my_printf("def_disp_win_left_offset: %d", vui->def_disp_win_left_offset);
        my_printf("def_disp_win_left_offset: %d", vui->def_disp_win_left_offset);
        my_printf("def_disp_win_right_offset: %d", vui->def_disp_win_right_offset);
        my_printf("def_disp_win_bottom_offset: %d", vui->def_disp_win_bottom_offset);
    }
    my_printf("vui_timing_info_present_flag: %d", vui->vui_timing_info_present_flag);
    if (vui->vui_timing_info_present_flag)
    {
        my_printf("vui_num_units_in_tick: %d", vui->vui_num_units_in_tick);
        my_printf("vui_time_scale: %d", vui->vui_time_scale);
        my_printf("vui_poc_proportional_to_timing_flag: %d", vui->vui_poc_proportional_to_timing_flag);
        if (vui->vui_poc_proportional_to_timing_flag)
        {
            my_printf("vui_num_ticks_poc_diff_one_minus1: %d", vui->vui_num_ticks_poc_diff_one_minus1);
        }
        my_printf("vui_hrd_parameters_present_flag: %d", vui->vui_hrd_parameters_present_flag);
        if (vui->vui_hrd_parameters_present_flag)
        {
            h265_debug_hrd_parameters(&vui->hrd_parameters, 1, maxNumSubLayersMinus1);
        }
    }
    my_printf("bitstream_restriction_flag: %d", vui->bitstream_restriction_flag);
    if (vui->bitstream_restriction_flag)
    {
        my_printf("tiles_fixed_structure_flag: %d", vui->tiles_fixed_structure_flag);
        my_printf("motion_vectors_over_pic_boundaries_flag: %d", vui->motion_vectors_over_pic_boundaries_flag);
        my_printf("restricted_ref_pic_lists_flag: %d", vui->restricted_ref_pic_lists_flag);
        my_printf("min_spatial_segmentation_idc: %d", vui->min_spatial_segmentation_idc);
        my_printf("max_bytes_per_pic_denom: %d", vui->max_bytes_per_pic_denom);
        my_printf("max_bits_per_min_cu_denom: %d", vui->max_bits_per_min_cu_denom);
        my_printf("log2_max_mv_length_horizontal: %d", vui->log2_max_mv_length_horizontal);
        my_printf("log2_max_mv_length_vertical: %d", vui->bitstream_restriction_flag);
    }
}
// sps
void CNalParser::h265_debug_sps(h265_sps_t* sps)
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
        my_printf("separate_colour_plane_flag: %d", sps->separate_colour_plane_flag);
    }
    my_printf("pic_width_in_luma_samples: %d", sps->pic_width_in_luma_samples);
    my_printf("pic_height_in_luma_samples: %d", sps->pic_height_in_luma_samples);
    my_printf("conformance_window_flag: %d", sps->conformance_window_flag);
    if (sps->conformance_window_flag)
    {
        my_printf("conf_win_left_offset: %d", sps->conf_win_left_offset);
        my_printf("conf_win_right_offset: %d", sps->conf_win_right_offset);
        my_printf("conf_win_top_offset: %d", sps->conf_win_top_offset);
        my_printf("conf_win_bottom_offset: %d", sps->conf_win_bottom_offset);
    }
    my_printf("bit_depth_luma_minus8: %d", sps->bit_depth_luma_minus8);
    my_printf("bit_depth_chroma_minus8: %d", sps->bit_depth_chroma_minus8);
    my_printf("log2_max_pic_order_cnt_lsb_minus4: %d", sps->log2_max_pic_order_cnt_lsb_minus4);
    my_printf("sps_sub_layer_ordering_info_present_flag: %d", sps->sps_sub_layer_ordering_info_present_flag);
    for (int i = (sps->sps_sub_layer_ordering_info_present_flag ? 0 : sps->sps_max_sub_layers_minus1);
        i <= sps->sps_max_sub_layers_minus1; i++ )
    {
        my_printf("sps_max_dec_pic_buffering_minus1[%d]: %d", i, sps->sps_max_dec_pic_buffering_minus1[i]);
        my_printf("sps_max_num_reorder_pics[%d]: %d", i, sps->sps_max_num_reorder_pics[i]);
        my_printf("sps_max_latency_increase_plus1[%d]: %d", i, sps->sps_max_latency_increase_plus1[i]);
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
        my_printf("sps_scaling_list_data_present_flag: %d", sps->sps_scaling_list_data_present_flag);
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
        my_printf("pcm_sample_bit_depth_luma_minus1: %d", sps->pcm_sample_bit_depth_luma_minus1);
        my_printf("pcm_sample_bit_depth_chroma_minus1: %d", sps->pcm_sample_bit_depth_chroma_minus1);
        my_printf("log2_min_pcm_luma_coding_block_size_minus3: %d", sps->log2_min_pcm_luma_coding_block_size_minus3);
        my_printf("log2_diff_max_min_pcm_luma_coding_block_size: %d", sps->log2_diff_max_min_pcm_luma_coding_block_size);
        my_printf("pcm_loop_filter_disabled_flag: %d", sps->pcm_loop_filter_disabled_flag);
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
        my_printf("num_long_term_ref_pics_sps: %d", sps->num_long_term_ref_pics_sps);
        for (int i = 0; i < sps->num_long_term_ref_pics_sps; i++)
        {
            my_printf("lt_ref_pic_poc_lsb_sps[%d]: %d", i, sps->lt_ref_pic_poc_lsb_sps[i]);
            my_printf("used_by_curr_pic_lt_sps_flag[%d]: %d", i, sps->used_by_curr_pic_lt_sps_flag[i]);
        }
    }
    my_printf("sps_temporal_mvp_enabled_flag: %d", sps->sps_temporal_mvp_enabled_flag);
    my_printf("strong_intra_smoothing_enabled_flag: %d", sps->strong_intra_smoothing_enabled_flag);
    my_printf("vui_parameters_present_flag: %d", sps->vui_parameters_present_flag);
    if (sps->vui_parameters_present_flag)
    {
        // vui
        h265_debug_vui_parameters(&sps->vui, sps->sps_max_sub_layers_minus1);
    }
    my_printf("sps_extension_present_flag: %d", sps->sps_extension_present_flag);
    if (sps->sps_extension_present_flag)
    {
        my_printf("sps_range_extension_flag: %d", sps->sps_range_extension_flag);
        my_printf("sps_multilayer_extension_flag: %d", sps->sps_multilayer_extension_flag);
        my_printf("sps_3d_extension_flag: %d", sps->sps_3d_extension_flag);
        my_printf("sps_extension_5bits: %d", sps->sps_extension_5bits);
    }
    if (sps->sps_range_extension_flag)
    {
        my_printf("transform_skip_rotation_enabled_flag: %d", sps->sps_range_extension.transform_skip_rotation_enabled_flag);
        my_printf("transform_skip_context_enabled_flag: %d", sps->sps_range_extension.transform_skip_context_enabled_flag);
        my_printf("implicit_rdpcm_enabled_flag: %d", sps->sps_range_extension.implicit_rdpcm_enabled_flag);
        my_printf("explicit_rdpcm_enabled_flag: %d", sps->sps_range_extension.explicit_rdpcm_enabled_flag);
        my_printf("extended_precision_processing_flag: %d", sps->sps_range_extension.extended_precision_processing_flag);
        my_printf("intra_smoothing_disabled_flag: %d", sps->sps_range_extension.intra_smoothing_disabled_flag);
        my_printf("high_precision_offsets_enabled_flag: %d", sps->sps_range_extension.high_precision_offsets_enabled_flag);
        my_printf("persistent_rice_adaptation_enabled_flag: %d", sps->sps_range_extension.persistent_rice_adaptation_enabled_flag);
        my_printf("cabac_bypass_alignment_enabled_flag: %d", sps->sps_range_extension.cabac_bypass_alignment_enabled_flag);
    }
    if (sps->sps_multilayer_extension_flag)
    {
        my_printf("inter_view_mv_vert_constraint_flag: %d", sps->inter_view_mv_vert_constraint_flag);
    }
    // todo sps_3d_extension_flag

}

// pps
void CNalParser::h265_debug_pps(h265_pps_t* pps)
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
                my_printf("column_width_minus1[%d]: %d", i, pps->column_width_minus1[i]);
            for (int i = 0; i < pps->num_tile_rows_minus1; i++)
                my_printf("row_height_minus1[%d]: %d", i, pps->row_height_minus1[i]);
        }
        my_printf("loop_filter_across_tiles_enabled_flag: %d", pps->loop_filter_across_tiles_enabled_flag); // to check
    }
    my_printf("pps_loop_filter_across_slices_enabled_flag: %d", pps->pps_loop_filter_across_slices_enabled_flag); // to check
    my_printf("deblocking_filter_control_present_flag: %d", pps->deblocking_filter_control_present_flag);
    if (pps->deblocking_filter_control_present_flag)
    {
        my_printf("deblocking_filter_override_enabled_flag: %d", pps->deblocking_filter_override_enabled_flag);
        my_printf("pps_deblocking_filter_disabled_flag: %d", pps->pps_deblocking_filter_disabled_flag);
        if (pps->pps_deblocking_filter_disabled_flag)
        {
            my_printf("pps_beta_offset_div2: %d", pps->pps_beta_offset_div2);
            my_printf("pps_tc_offset_div2: %d", pps->pps_tc_offset_div2);
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
        my_printf("pps_range_extension_flag: %d", pps->pps_range_extension_flag);
        my_printf("pps_multilayer_extension_flag: %d", pps->pps_multilayer_extension_flag);
        my_printf("pps_3d_extension_flag: %d", pps->pps_3d_extension_flag);
        my_printf("pps_extension_5bits: %d", pps->pps_extension_5bits);
    }
    if (pps->pps_range_extension_flag)
    {
        if (pps->transform_skip_enabled_flag)
            my_printf("pps_extension_5bits: %d", pps->pps_range_extension.log2_max_transform_skip_block_size_minus2);
        my_printf("cross_component_prediction_enabled_flag: %d", pps->pps_range_extension.cross_component_prediction_enabled_flag);
        my_printf("chroma_qp_offset_list_enabled_flag: %d", pps->pps_range_extension.chroma_qp_offset_list_enabled_flag);
        if (pps->pps_range_extension.chroma_qp_offset_list_enabled_flag)
        {
            my_printf("diff_cu_chroma_qp_offset_depth: %d", pps->pps_range_extension.diff_cu_chroma_qp_offset_depth);
            my_printf("chroma_qp_offset_list_len_minus1: %d", pps->pps_range_extension.chroma_qp_offset_list_len_minus1);
            for (int i = 0; i < pps->pps_range_extension.chroma_qp_offset_list_len_minus1; i++)
            {
                my_printf("cb_qp_offset_list[%d]: %d", i, pps->pps_range_extension.cb_qp_offset_list[i]);
                my_printf("cr_qp_offset_list[%d]: %d", i, pps->pps_range_extension.cb_qp_offset_list[i]);
            }
        }
        my_printf("log2_sao_offset_scale_luma: %d", pps->pps_range_extension.log2_sao_offset_scale_luma);
        my_printf("log2_sao_offset_scale_chroma: %d", pps->pps_range_extension.log2_sao_offset_scale_chroma);
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
void CNalParser::h265_debug_aud(h265_aud_t* aud)
{
    my_printf("======= HEVC AUD =======");
    const char* pic_type;
    switch (aud->pic_type)
    {
    case H265_AUD_PRIMARY_PIC_TYPE_I:    pic_type = "I"; break;
    case H265_AUD_PRIMARY_PIC_TYPE_IP:   pic_type = "P, I"; break;
    case H265_AUD_PRIMARY_PIC_TYPE_IPB:  pic_type = "B, P, I"; break;
    default: pic_type = "Unknown"; break;
    }
    my_printf("pic_type: %d ( %s ) ", aud->pic_type, pic_type );
}

// sei
void CNalParser::h265_debug_seis(h265_stream_t* h)
{
    h265_sei_t** seis = h->seis;
    int num_seis = h->num_seis;

    my_printf("======= HEVC SEI =======");
    const char* sei_type_name;
    int i;
    for (i = 0; i < num_seis; i++)
    {
        h265_sei_t* s = seis[i];
        my_printf("payloadType: %d", s->payloadType);
        my_printf("payloadSize: %d", s->payloadSize);
        my_printf("sei_payload()");
        if (h->nal->nal_unit_type == NAL_UNIT_PREFIX_SEI)
        {
            switch(s->payloadType)
            {
            case 0:
                my_printf("buffering_period()");
                break;
            case 1:
                my_printf("pic_timing()");
                break;
            case 2:
                my_printf("pan_scan_rect()");
                break;
            case 3:
                my_printf("pan_scan_rect()");
                break;
            case 4:
                my_printf("pan_scan_rect()");
                break;
            case 5:
                my_printf("user_data_unregistered()");
                my_printf("uuid_iso_iec_11578: ");
                for (int j = 0; j < 16; j++)
                    my_printf("%X", s->payload[j]);
                my_printf ("\r\n   ");
                for (int j = 16; j < s->payloadSize; j++)
                {
                    my_printf("%c", s->payload[j]);
                    if ((j+1) % 128 == 0) my_printf ("");
                }
                break;
            case 6:
                my_printf("recovery_point()");
                break;
            case 9:
                my_printf("scene_info()");
                break;
            case 15:
                my_printf("picture_snapshot()");
                break;
            case 16:
                my_printf("progressive_refinement_segment_start()");
                break;
            case 17:
                my_printf("progressive_refinement_segment_end()");
                break;
            default:
                my_printf("reserved_sei_message()");
                break;
            }
        }
        else if (h->nal->nal_unit_type == NAL_UNIT_SUFFIX_SEI)
        {
            switch(s->payloadType)
            {
            case 3:
                my_printf("filler_payload()");
                break;
            case 4:
                my_printf("user_data_registered_itu_t_t35()");
                break;
            case 5:
                my_printf("user_data_unregistered()");
                break;
            case 17:
                my_printf("progressive_refinement_segment_end()");
                break;
            case 22:
                my_printf("post_filter_hint()");
                break;
            case 132:
                my_printf("decoded_picture_hash()");
                break;
            case 16:
                my_printf("progressive_refinement_segment_start()");
                break;
            default:
                my_printf("reserved_sei_message()");
                break;
            }
        }
    }
}

void CNalParser::h265_debug_ref_pic_lists_modification(h265_slice_header_t* hrd)
{
    my_printf("ref_pic_list_modification_flag_l0: %d", hrd->ref_pic_lists_modification.ref_pic_list_modification_flag_l0);
    if (hrd->ref_pic_lists_modification.ref_pic_list_modification_flag_l0)
    {
        for (int i = 0; i <= hrd->num_ref_idx_l0_active_minus1; i++)
            my_printf("list_entry_l0[%d]: %d", i, hrd->ref_pic_lists_modification.list_entry_l0[i]);
    }
    if (hrd->slice_type == H265_SH_SLICE_TYPE_B)
    {
        my_printf("ref_pic_list_modification_flag_l1: %d", hrd->ref_pic_lists_modification.ref_pic_list_modification_flag_l1);
        for (int i = 0; i <= hrd->num_ref_idx_l1_active_minus1; i++)
            my_printf("list_entry_l1[%d]: %d", i, hrd->ref_pic_lists_modification.list_entry_l1[i]);
    }
}
void CNalParser::h265_debug_pred_weight_table(h265_stream_t* h)
{
    pred_weight_table_t* pwt = &h->sh->pred_weight_table;
    h265_sps_t* sps = h->sps;
    int l0_size = h->sh->num_ref_idx_l0_active_minus1+1;
    int l1_size = h->sh->num_ref_idx_l1_active_minus1+1;

    my_printf("pred_weight_table()");

    my_printf("luma_log2_weight_denom: %d", pwt->luma_log2_weight_denom);
    if (h->sps->chroma_format_idc != 0)
        my_printf("delta_chroma_log2_weight_denom: %d", pwt->delta_chroma_log2_weight_denom);
    for (int i = 0; i <= h->sh->num_ref_idx_l0_active_minus1; i++)
        my_printf("luma_weight_l0_flag[%d]: %d", i, pwt->luma_weight_l0_flag[i]);
    if (h->sps->chroma_format_idc != 0)
    {
        for (int i = 0; i <= h->sh->num_ref_idx_l0_active_minus1; i++)
            my_printf("chroma_weight_l0_flag[%d]: %d", i, pwt->chroma_weight_l0_flag[i]);
    }

    for (int i = 0; i <= h->sh->num_ref_idx_l0_active_minus1; i++)
    {
        if (pwt->luma_weight_l0_flag[i])
        {
            my_printf("delta_luma_weight_l0[%d]: %d", i, pwt->delta_luma_weight_l0[i]);
            my_printf("luma_offset_l0[%d]: %d", i, pwt->luma_offset_l0[i]);
        }
        if (pwt->chroma_weight_l0_flag[i])
        {
            for (int j = 0; j < 2; j++)
            {
                my_printf("delta_chroma_weight_l0[%d][%d]: %d", i, j, pwt->delta_chroma_weight_l0[i][j]);
                my_printf("delta_chroma_offset_l0[%d][%d]: %d", i, j, pwt->delta_chroma_offset_l0[i][j]);
            }
        }
    }

    if (h->sh->slice_type == H265_SH_SLICE_TYPE_B)
    {
        for (int i = 0; i <= h->sh->num_ref_idx_l1_active_minus1; i++)
            my_printf("luma_weight_l1_flag[%d]: %d", i, pwt->luma_weight_l1_flag[i]);
        if (h->sps->chroma_format_idc != 0)
            for (int i = 0; i <= h->sh->num_ref_idx_l1_active_minus1; i++)
                my_printf("chroma_weight_l1_flag[%d]: %d", i, pwt->chroma_weight_l1_flag[i]);
        for (int i = 0; i <= h->sh->num_ref_idx_l1_active_minus1; i++)
        {
            if (pwt->luma_weight_l1_flag[i])
            {
                my_printf("delta_luma_weight_l1[%d]: %d", i, pwt->delta_luma_weight_l1[i]);
                my_printf("luma_offset_l1[%d]: %d", i, pwt->luma_offset_l1[i]);
                
            }
            if (pwt->chroma_weight_l1_flag[i])
            {
                for (int j = 0; j < 2; j++)
                {
                    my_printf("delta_chroma_weight_l1[%d][%d]: %d", i, j, pwt->delta_chroma_weight_l1[i][j]);
                    my_printf("delta_chroma_offset_l1[%d][%d]: %d", i, j, pwt->delta_chroma_offset_l1[i][j]);
                }
            }
        }
    }
}

void CNalParser::h265_debug_slice_header(h265_stream_t* h)
{
    h265_slice_header_t* hrd = h->sh;
    h265_sps_t* sps = NULL;
    h265_pps_t* pps = NULL;
    int nal_unit_type = h->nal->nal_unit_type;
    pps = h->pps = h->pps_table[hrd->slice_pic_parameter_set_id];
    sps = h->sps = h->sps_table[pps->pps_seq_parameter_set_id];

    my_printf("======= HEVC Slice Header =======");
    my_printf("first_slice_segment_in_pic_flag: %d", hrd->first_slice_segment_in_pic_flag);
    if (nal_unit_type >= NAL_UNIT_CODED_SLICE_BLA_W_LP && nal_unit_type <= NAL_UNIT_RESERVED_IRAP_VCL23)
        my_printf("no_output_of_prior_pics_flag: %d", hrd->no_output_of_prior_pics_flag);
    my_printf("slice_pic_parameter_set_id: %d", hrd->slice_pic_parameter_set_id);
    if (!hrd->first_slice_segment_in_pic_flag)
    {
        if (pps->dependent_slice_segments_enabled_flag)
            my_printf("dependent_slice_segment_flag: %d", hrd->dependent_slice_segment_flag);
        my_printf("slice_segment_address: %d", hrd->slice_segment_address);
    }

    if (!hrd->dependent_slice_segment_flag)
    {
        my_printf("dependent_slice_segment_flag");
        for (int i = 0; i < pps->num_extra_slice_header_bits; i++)
            my_printf("slice_reserved_flag[%d]: %d", i, hrd->slice_reserved_flag[i]);
        const char* slice_type_name;
        switch(hrd->slice_type)
        {
            case H265_SH_SLICE_TYPE_P:  slice_type_name = "P slice"; break;
            case H265_SH_SLICE_TYPE_B:  slice_type_name = "B slice"; break;
            case H265_SH_SLICE_TYPE_I:  slice_type_name = "I slice"; break;
            default:                   slice_type_name = "Unknown"; break;
        }
        my_printf("slice_type: %d (%s)", hrd->slice_type, slice_type_name);
        if (pps->output_flag_present_flag)
            my_printf("pic_output_flag: %d", hrd->pic_output_flag);
        if (sps->separate_colour_plane_flag == 1)
            my_printf("colour_plane_id: %d", hrd->colour_plane_id);
        if (nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL || nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP)
        {
            // do nothing...
        }
        else
        {
            my_printf("slice_pic_order_cnt_lsb: %d", hrd->slice_pic_order_cnt_lsb);
            my_printf("short_term_ref_pic_set_sps_flag: %d", hrd->short_term_ref_pic_set_sps_flag);
            if (!hrd->short_term_ref_pic_set_sps_flag)
            {
                referencePictureSets_t* rps = &hrd->m_localRPS;
                h265_debug_short_term_ref_pic_set(sps, &hrd->st_ref_pic_set, rps, sps->num_short_term_ref_pic_sets);
            }
            else if (sps->num_short_term_ref_pic_sets > 1)
            {
                my_printf("short_term_ref_pic_set_idx: %d", hrd->short_term_ref_pic_set_idx);
            }
            if (sps->long_term_ref_pics_present_flag)
            {
                my_printf("num_long_term_sps: %d", hrd->num_long_term_sps);
                my_printf("num_long_term_pics: %d", hrd->num_long_term_pics);
                for (int i = 0; i < (int)hrd->lt_idx_sps.size(); i++)
                {
                    if (i < hrd->num_long_term_sps)
                        my_printf("hrd->lt_idx_sps[%d]: %d", i, hrd->lt_idx_sps[i]);
                    else
                    {
                        my_printf("hrd->poc_lsb_lt[%d]: %d", i, hrd->poc_lsb_lt[i]);
                        my_printf("hrd->used_by_curr_pic_lt_flag[%d]: %d", i, hrd->used_by_curr_pic_lt_flag[i]);
                    }
                    my_printf("hrd->delta_poc_msb_present_flag[%d]: %d", i, hrd->delta_poc_msb_present_flag[i]);
                    if (hrd->delta_poc_msb_present_flag[i])
                        my_printf("hrd->delta_poc_msb_cycle_lt[%d]: %d", i, hrd->delta_poc_msb_cycle_lt[i]);
                }
            }
            if(sps->sps_temporal_mvp_enabled_flag)
            {
                my_printf("slice_temporal_mvp_enabled_flag: %d", hrd->slice_temporal_mvp_enabled_flag);
            }
        }

        if(sps->sample_adaptive_offset_enabled_flag)
        {
            my_printf("slice_sao_luma_flag: %d", hrd->slice_sao_luma_flag);
            my_printf("slice_sao_chroma_flag: %d", hrd->slice_sao_chroma_flag);
        }
        if (hrd->slice_type == H265_SH_SLICE_TYPE_P || hrd->slice_type == H265_SH_SLICE_TYPE_B)
        {
            my_printf("num_ref_idx_active_override_flag: %d", hrd->num_ref_idx_active_override_flag);
            if (hrd->num_ref_idx_active_override_flag)
            {
                my_printf("num_ref_idx_l0_active_minus1: %d", hrd->num_ref_idx_l0_active_minus1);
                if (hrd->slice_type == H265_SH_SLICE_TYPE_B)
                    my_printf("num_ref_idx_l1_active_minus1: %d", hrd->num_ref_idx_l1_active_minus1);
            }
            if(pps->lists_modification_present_flag)
            {
                h265_debug_ref_pic_lists_modification(hrd);
            }
            my_printf("mvd_l1_zero_flag: %d", hrd->mvd_l1_zero_flag);
            if (pps->cabac_init_present_flag)
                my_printf("cabac_init_flag: %d", hrd->cabac_init_flag);
            if (hrd->slice_temporal_mvp_enabled_flag)
            {
                if (hrd->slice_type == H265_SH_SLICE_TYPE_B)
                    my_printf("collocated_from_l0_flag: %d", hrd->collocated_from_l0_flag);
                if ((hrd->collocated_from_l0_flag && hrd->num_ref_idx_l0_active_minus1 > 0) ||
                    (!hrd->collocated_from_l0_flag && hrd->num_ref_idx_l1_active_minus1 > 0))
                    my_printf("collocated_ref_idx: %d", hrd->collocated_ref_idx);
            }
            if ((pps->weighted_pred_flag && hrd->slice_type == H265_SH_SLICE_TYPE_P) ||
                (pps->weighted_bipred_flag && hrd->slice_type == H265_SH_SLICE_TYPE_B))
                h265_debug_pred_weight_table(h);
            my_printf("five_minus_max_num_merge_cand: %d", hrd->five_minus_max_num_merge_cand);
        }
        my_printf("slice_qp_delta: %d", hrd->slice_qp_delta);
        if (pps->pps_slice_chroma_qp_offsets_present_flag)
        {
            my_printf("slice_cb_qp_offset: %d", hrd->slice_cb_qp_offset);
            my_printf("slice_cr_qp_offset: %d", hrd->slice_cr_qp_offset);
        }
        if (pps->pps_range_extension.chroma_qp_offset_list_enabled_flag)
        {
            my_printf("cu_chroma_qp_offset_enabled_flag: %d", hrd->cu_chroma_qp_offset_enabled_flag);
        }
        if (pps->deblocking_filter_override_enabled_flag)
        {
            my_printf("deblocking_filter_override_flag: %d", hrd->deblocking_filter_override_flag);
        }
        if (hrd->deblocking_filter_override_flag)
        {
            my_printf("slice_deblocking_filter_disabled_flag: %d", hrd->slice_deblocking_filter_disabled_flag);
            if (!hrd->slice_deblocking_filter_disabled_flag)
            {
                my_printf("slice_beta_offset_div2: %d", hrd->slice_beta_offset_div2);
                my_printf("slice_tc_offset_div2: %d", hrd->slice_tc_offset_div2);
            }
        }
        my_printf("slice_loop_filter_across_slices_enabled_flag: %d", hrd->slice_loop_filter_across_slices_enabled_flag);
        
    }
    if (pps->tiles_enabled_flag || pps->entropy_coding_sync_enabled_flag)
    {
        my_printf("num_entry_point_offsets: %d", hrd->num_entry_point_offsets);
        if (hrd->num_entry_point_offsets > 0)
        {
            my_printf("offset_len_minus1: %d", hrd->offset_len_minus1);
            my_printf("NumEntryPointOffsets");
            for (int i = 0; i < hrd->num_entry_point_offsets; i++)
                my_printf("entry_point_offset_minus1[%d]: %d", i, hrd->entry_point_offset_minus1[i]);
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
#endif
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
    // nal header
    my_printf("NAL");
    HTREEITEM root = AddTreeItem(TVI_ROOT);

    //my_printf("nal_unit_header");
    //HTREEITEM subroot = AddTreeItem(root);

    my_printf("forbidden_zero_bit: %d (1 bit)", nal->forbidden_zero_bit); AddTreeItem(root);
    my_printf("nal_unit_type: %d (%s) (6 bits)", nal->nal_unit_type, nal_unit_type_name); AddTreeItem(root);
    my_printf("nal_ref_idc: %d (6 bits)", nal->nuh_layer_id); AddTreeItem(root);
    my_printf("nuh_temporal_id_plus1: %d (3 bits)", nal->nuh_temporal_id_plus1); AddTreeItem(root);
    
    // nal unit
    if(my_nal_type == 0)
        h265_debug_vps_t(h->vps, root);
    else if(my_nal_type == 1)
        h265_debug_sps(h->sps);
    else if(my_nal_type == 2)
        h265_debug_pps(h->pps);
    else if(my_nal_type == 3)
        h265_debug_aud(h->aud);
    else if(my_nal_type == 4)
        h265_debug_seis(h);
    else if(my_nal_type == 5)
        h265_debug_slice_header(h);
}
