/* 
H.265 ver:
3.0  ITU-T H.265 (V3)  2015-04-29 
ref: HM16.6 source code
*/

#ifndef _H265_STREAM_H
#define _H265_STREAM_H        1

#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include "bs.h"

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#include <vector>
using std::vector;

#ifdef __cplusplus
extern "C" {
#endif

/**
   Network Abstraction Layer (NAL) unit
   @see 7.3.1 NAL unit syntax
*/
typedef struct
{
    int forbidden_zero_bit;
    int nal_unit_type;
    int nuh_layer_id;
    int nuh_temporal_id_plus1;

    void* parsed; // FIXME
    int sizeof_parsed;

    //uint8_t* rbsp_buf;
    //int rbsp_size;
} h265_nal_t;

typedef struct
{
    int payloadType;
    int payloadSize;
    uint8_t* payload;
} h265_sei_t;

/**
   Profile, tier and level 
   @see 7.3.3 Profile, tier and level syntax 
*/
/*
vector<int> sub_layer_profile_present_flag;
vector<int> sub_layer_level_present_flag;
vector<int> reserved_zero_2bits;
vector<int> sub_layer_profile_space;
vector<int> sub_layer_tier_flag;
vector<int> sub_layer_profile_idc;
//int sub_layer_profile_compatibility_flag[256][32];
vector<vector <int> > sub_layer_profile_compatibility_flag;
vector<int> sub_layer_progressive_source_flag;
vector<int> sub_layer_interlaced_source_flag;
vector<int> sub_layer_non_packed_constraint_flag;
vector<int> sub_layer_frame_only_constraint_flag;
vector<int> sub_layer_max_12bit_constraint_flag;
vector<int> sub_layer_max_10bit_constraint_flag;
vector<int> sub_layer_max_8bit_constraint_flag;
// todo...
vector<int> sub_layer_max_422chroma_constraint_flag;
vector<int> sub_layer_max_420chroma_constraint_flag;
vector<int> sub_layer_max_monochrome_constraint_flag;
vector<int> sub_layer_intra_constraint_flag;
vector<int> sub_layer_one_picture_only_constraint_flag;
vector<int> sub_layer_lower_bit_rate_constraint_flag;
vector<uint64_t> sub_layer_reserved_zero_34bits;
vector<uint64_t> sub_layer_reserved_zero_43bits;
*/
typedef struct
{
    int general_profile_space;
    int general_tier_flag;
    int general_profile_idc;
    int general_profile_compatibility_flag[32];
    int general_progressive_source_flag;
    int general_interlaced_source_flag;
    int general_non_packed_constraint_flag;
    int general_frame_only_constraint_flag;
    int general_max_12bit_constraint_flag;
    int general_max_10bit_constraint_flag;
    int general_max_8bit_constraint_flag;
    int general_max_422chroma_constraint_flag;
    int general_max_420chroma_constraint_flag;
    int general_max_monochrome_constraint_flag;
    int general_intra_constraint_flag;
    int general_one_picture_only_constraint_flag;
    int general_lower_bit_rate_constraint_flag;
    uint64_t general_reserved_zero_34bits; // todo
    uint64_t general_reserved_zero_43bits; // todo
    int general_inbld_flag;
    int general_reserved_zero_bit;
    int general_level_idc;
    int sub_layer_profile_present_flag[256];
    int sub_layer_level_present_flag[256];
    int reserved_zero_2bits[256];
    int sub_layer_profile_space[256];
    int sub_layer_tier_flag[256];
    int sub_layer_profile_idc[256];
    int sub_layer_profile_compatibility_flag[256][32];
    int sub_layer_progressive_source_flag[256];
    int sub_layer_interlaced_source_flag[256];
    int sub_layer_non_packed_constraint_flag[256];
    int sub_layer_frame_only_constraint_flag[256];
    int sub_layer_max_12bit_constraint_flag[32];
    int sub_layer_max_10bit_constraint_flag[32];
    int sub_layer_max_8bit_constraint_flag[32];
    int sub_layer_max_422chroma_constraint_flag[32];
    int sub_layer_max_420chroma_constraint_flag[32];
    int sub_layer_max_monochrome_constraint_flag[32];
    int sub_layer_intra_constraint_flag[32];
    int sub_layer_one_picture_only_constraint_flag[32];
    int sub_layer_lower_bit_rate_constraint_flag[32];
    uint64_t sub_layer_reserved_zero_34bits[32];
    uint64_t sub_layer_reserved_zero_43bits[32];
    int sub_layer_inbld_flag[32];
    int sub_layer_reserved_zero_bit[32];
    int sub_layer_level_idc[32];

} profile_tier_level_t;

typedef struct 
{
    int bit_rate_value_minus1[256];
    int cpb_size_value_minus1[256];
    int cpb_size_du_value_minus1[256];
    int bit_rate_du_value_minus1[256];
    int cbr_flag[256];
} sub_layer_hrd_parameters_t;

/**
E.2.2  HRD parameters syntax 
*/
typedef struct 
{
    int nal_hrd_parameters_present_flag;
    int vcl_hrd_parameters_present_flag;
      int sub_pic_hrd_params_present_flag;
        int tick_divisor_minus2;
        int du_cpb_removal_delay_increment_length_minus1;
        int sub_pic_cpb_params_in_pic_timing_sei_flag;
        int dpb_output_delay_du_length_minus1;
      int bit_rate_scale;
      int cpb_size_scale;
      int cpb_size_du_scale;
      int initial_cpb_removal_delay_length_minus1;
      int au_cpb_removal_delay_length_minus1;
      int dpb_output_delay_length_minus1;
    int fixed_pic_rate_general_flag[256];
    int fixed_pic_rate_within_cvs_flag[256];
    int elemental_duration_in_tc_minus1[256];
    int low_delay_hrd_flag[256];
    int cpb_cnt_minus1[256];
    sub_layer_hrd_parameters_t sub_layer_hrd_parameters;
    sub_layer_hrd_parameters_t sub_layer_hrd_parameters_v;
} hrd_parameters_t;

/**
sps_range_extension
@see 7.3.2.3.1  General picture parameter set RBSP syntax 
*/
typedef struct 
{
    int transform_skip_rotation_enabled_flag;
    int transform_skip_context_enabled_flag;
    int implicit_rdpcm_enabled_flag;
    int explicit_rdpcm_enabled_flag;
    int extended_precision_processing_flag;
    int intra_smoothing_disabled_flag;
    int high_precision_offsets_enabled_flag;
    int persistent_rice_adaptation_enabled_flag;
    int cabac_bypass_alignment_enabled_flag;
} sps_range_extension_t;

/**
   Access unit delimiter
   @see 7.3.2.5  Access unit delimiter RBSP syntax
*/
typedef struct
{
    int pic_type;
} h265_aud_t;

/**
 @see 7.3.2.3.2  Picture parameter set range extension syntax
*/
typedef struct
{
    int log2_max_transform_skip_block_size_minus2;
    int cross_component_prediction_enabled_flag;
    int chroma_qp_offset_list_enabled_flag;
      int diff_cu_chroma_qp_offset_depth;
      int chroma_qp_offset_list_len_minus1;
      int cb_qp_offset_list[256];
      int cr_qp_offset_list[256];
    int log2_sao_offset_scale_luma;
    int log2_sao_offset_scale_chroma;
} pps_range_extension_t;

/**
7.3.4  Scaling list data syntax 
*/
typedef struct
{
    int scaling_list_pred_mode_flag[4][6];
    int scaling_list_pred_matrix_id_delta[4][6];
    int scaling_list_dc_coef_minus8[4][6];
    int ScalingList[4][6][64];
} scaling_list_data_t;

/**
E.2.1  VUI parameters syntax 
*/
typedef struct
{
    int aspect_ratio_info_present_flag;
      int aspect_ratio_idc;
      int sar_width;
      int sar_height;
    int overscan_info_present_flag;
      int overscan_appropriate_flag;
    int video_signal_type_present_flag;
      int video_format;
      int video_full_range_flag;
      int colour_description_present_flag;
        int colour_primaries;
        int transfer_characteristics;
        int matrix_coeffs;
    int chroma_loc_info_present_flag;
      int chroma_sample_loc_type_top_field;
      int chroma_sample_loc_type_bottom_field;
    int neutral_chroma_indication_flag;
    int field_seq_flag;
    int frame_field_info_present_flag;
    int default_display_window_flag;
      int def_disp_win_left_offset;
      int def_disp_win_right_offset;
      int def_disp_win_top_offset;
      int def_disp_win_bottom_offset;
    int vui_timing_info_present_flag;
      int vui_num_units_in_tick;
      int vui_time_scale;
      int vui_poc_proportional_to_timing_flag;
        int vui_num_ticks_poc_diff_one_minus1;
      int vui_hrd_parameters_present_flag;
      hrd_parameters_t hrd_parameters;
    int bitstream_restriction_flag;
      int tiles_fixed_structure_flag;
      int motion_vectors_over_pic_boundaries_flag;
      int restricted_ref_pic_lists_flag;
      int min_spatial_segmentation_idc;
      int max_bytes_per_pic_denom;
      int max_bits_per_min_cu_denom;
      int log2_max_mv_length_horizontal;
      int log2_max_mv_length_vertical;
} vui_parameters_t;

/**
   Weighted prediction table
   @see 7.3.6.3  Weighted prediction parameters syntax
*/
typedef struct
{
    int luma_log2_weight_denom;
    int delta_chroma_log2_weight_denom;
    int luma_weight_l0_flag[256];
    int chroma_weight_l0_flag[256];
    int delta_luma_weight_l0[256];
    int luma_offset_l0[256];
    int delta_chroma_weight_l0[256][2];
    int delta_chroma_offset_l0[256][2];
    int luma_weight_l1_flag[256];
    int chroma_weight_l1_flag[256];
    int delta_luma_weight_l1[256];
    int luma_offset_l1[256];
    int delta_chroma_weight_l1[256][2];
    int delta_chroma_offset_l1[256][2];
} pred_weight_table_t;

/**
   Video Parameter Set
   @see 7.3.2.1 Video parameter set RBSP syntax
*/
typedef struct
{
    int vps_video_parameter_set_id; // u(4)
    // note: vps_base_layer_available_flag + vps_base_layer_internal_flag = vps_reserved_three_2bits 
    int vps_base_layer_internal_flag; // u(1)
    int vps_base_layer_available_flag; // u(1) 
    int vps_max_layers_minus1; // u(6) 
    int vps_max_sub_layers_minus1; // u(3) 
    int vps_temporal_id_nesting_flag; // u(1) 
    int vps_reserved_0xffff_16bits; // u(16) 
    profile_tier_level_t profile_tier_level;
    int vps_sub_layer_ordering_info_present_flag;
    // Sublayers
    int vps_max_dec_pic_buffering_minus1[8]; // max u(3)
    int vps_max_num_reorder_pics[8];
    int vps_max_latency_increase_plus1[8];
    int vps_max_layer_id;
    int vps_num_layer_sets_minus1;
    int layer_id_included_flag[64][256];
    int vps_timing_info_present_flag;
      int vps_num_units_in_tick;
      int vps_time_scale;
      int vps_poc_proportional_to_timing_flag;
        int vps_num_ticks_poc_diff_one_minus1;
      int vps_num_hrd_parameters;
      int hrd_layer_set_idx[256];
      int cprms_present_flag[256];
      hrd_parameters_t hrd_parameters;
    int vps_extension_flag;
      int vps_extension_data_flag;
} h265_vps_t;

/**
   Sequence Parameter Set
   @see 7.3.2.2 Sequence parameter set RBSP syntax
*/
typedef struct
{
    int sps_video_parameter_set_id;
    int sps_max_sub_layers_minus1;
    int sps_temporal_id_nesting_flag;
    profile_tier_level_t profile_tier_level;
    int sps_seq_parameter_set_id;
    int chroma_format_idc;
    int separate_colour_plane_flag;
    int pic_width_in_luma_samples;
    int pic_height_in_luma_samples;
    int conformance_window_flag;
      int conf_win_left_offset;
      int conf_win_right_offset;
      int conf_win_top_offset;
      int conf_win_bottom_offset;
    int bit_depth_luma_minus8;
    int bit_depth_chroma_minus8;
    int log2_max_pic_order_cnt_lsb_minus4;
    int sps_sub_layer_ordering_info_present_flag;
    int sps_max_dec_pic_buffering_minus1[8]; // max u(3)
    int sps_max_num_reorder_pics[8];
    int sps_max_latency_increase_plus1[8];
    int log2_min_luma_coding_block_size_minus3;
    int log2_diff_max_min_luma_coding_block_size;
    int log2_min_luma_transform_block_size_minus2;
    int log2_diff_max_min_luma_transform_block_size;
    int max_transform_hierarchy_depth_inter;
    int max_transform_hierarchy_depth_intra;
    int scaling_list_enabled_flag;
      int sps_infer_scaling_list_flag;
      int sps_scaling_list_ref_layer_id;
      int sps_scaling_list_data_present_flag;
        scaling_list_data_t scaling_list_data;
    int amp_enabled_flag;
    int sample_adaptive_offset_enabled_flag;
    int pcm_enabled_flag;
      int pcm_sample_bit_depth_luma_minus1;
      int pcm_sample_bit_depth_chroma_minus1;
      int log2_min_pcm_luma_coding_block_size_minus3;
      int log2_diff_max_min_pcm_luma_coding_block_size;
      int pcm_loop_filter_disabled_flag;
    int num_short_term_ref_pic_sets;
    //st_ref_pic_set_t st_ref_pic_set;
    int long_term_ref_pics_present_flag;
      int num_long_term_ref_pics_sps;
      int lt_ref_pic_poc_lsb_sps[256]; // todo
      int used_by_curr_pic_lt_sps_flag[256]; // todo
    int sps_temporal_mvp_enabled_flag;
    int strong_intra_smoothing_enabled_flag;
    int vui_parameters_present_flag;
      vui_parameters_t vui_parameters;
    int sps_extension_present_flag;
      int sps_range_extension_flag;
      int sps_multilayer_extension_flag;
      int sps_3d_extension_flag;
      int sps_extension_5bits;
    sps_range_extension_t sps_range_extension;
    //sps_multilayer_extension_t sps_multilayer_extension;
    //sps_3d_extension_t sps_3d_extension;
    //int sps_extension_data_flag; // no need
    // rbsp_trailing_bits()...
    
} h265_sps_t;

/**
   Picture Parameter Set
   @see 7.3.2.3.1 General picture parameter set RBSP syntax
*/
typedef struct 
{
    int pps_pic_parameter_set_id;
    int pps_seq_parameter_set_id;
    int dependent_slice_segments_enabled_flag;
    int output_flag_present_flag;
    int num_extra_slice_header_bits;
    int sign_data_hiding_enabled_flag;
    int cabac_init_present_flag;
    int num_ref_idx_l0_default_active_minus1;
    int num_ref_idx_l1_default_active_minus1;
    int init_qp_minus26;
    int constrained_intra_pred_flag;
    int transform_skip_enabled_flag;
    int cu_qp_delta_enabled_flag;
    int diff_cu_qp_delta_depth;
    int pps_cb_qp_offset;
    int pps_cr_qp_offset;
    int pps_slice_chroma_qp_offsets_present_flag;
    int weighted_pred_flag;
    int weighted_bipred_flag;
    int transquant_bypass_enabled_flag;
    int tiles_enabled_flag;
    int entropy_coding_sync_enabled_flag;
      int num_tile_columns_minus1;
      int num_tile_rows_minus1;
      int uniform_spacing_flag;
        int column_width_minus1[256];
        int row_height_minus1[256];
      int loop_filter_across_tiles_enabled_flag;
    int pps_loop_filter_across_slices_enabled_flag;
    int deblocking_filter_control_present_flag;
      int deblocking_filter_override_enabled_flag;
      int pps_deblocking_filter_disabled_flag;
        int pps_beta_offset_div2;
        int pps_tc_offset_div2;
    int pps_scaling_list_data_present_flag;
      scaling_list_data_t scaling_list_data;
    int lists_modification_present_flag;
    int log2_parallel_merge_level_minus2;
    int slice_segment_header_extension_present_flag;
    int pps_extension_present_flag;
      int pps_range_extension_flag;
      int pps_multilayer_extension_flag;
      int pps_3d_extension_flag;
      int pps_extension_5bits;
    pps_range_extension_t pps_range_extension;
    //pps_multilayer_extension_t pps_multilayer_extension;
    //pps_3d_extension_t pps_3d_extension;
    int pps_extension_data_flag;
    // rbsp_trailing_bits( ) ...
} h265_pps_t;

/**
  Slice Header
  @see 7.3.6.1  General slice segment header syntax 
*/
typedef struct
{
    int first_slice_segment_in_pic_flag;
    int no_output_of_prior_pics_flag;
    int slice_pic_parameter_set_id;
      int dependent_slice_segment_flag;
      int slice_segment_address;
        int slice_reserved_flag[256];
        int slice_type;
        int pic_output_flag;
        int colour_plane_id;
        int slice_pic_order_cnt_lsb;
        int short_term_ref_pic_set_sps_flag;
        // st_ref_pic_set_t st_ref_pic_set
        int short_term_ref_pic_set_idx;
        int num_long_term_sps;
        int num_long_term_pics;
        int lt_idx_sps[256];
        int poc_lsb_lt[256];
        int used_by_curr_pic_lt_flag[256];
        int delta_poc_msb_present_flag[256];
        int delta_poc_msb_cycle_lt[256];
    int slice_temporal_mvp_enabled_flag;
    int slice_sao_luma_flag;
    int slice_sao_chroma_flag;
    int num_ref_idx_active_override_flag;
    int num_ref_idx_l0_active_minus1;
    int num_ref_idx_l1_active_minus1;
    int mvd_l1_zero_flag;
    int cabac_init_flag;
    int collocated_from_l0_flag;
    int collocated_ref_idx;
    pred_weight_table_t pred_weight_table;
    int five_minus_max_num_merge_cand;
    int slice_qp_delta;
    int slice_cb_qp_offset;
    int slice_cr_qp_offset;
    int cu_chroma_qp_offset_enabled_flag;
    int deblocking_filter_override_flag;
    int slice_deblocking_filter_disabled_flag;
    int slice_beta_offset_div2;
    int slice_tc_offset_div2;
    int slice_loop_filter_across_slices_enabled_flag;
    int num_entry_point_offsets;
    int offset_len_minus1;
    int entry_point_offset_minus1[256];
    int slice_segment_header_extension_length;
    int slice_segment_header_extension_data_byte[256];
    // byte_alignment( )...
} h265_slice_header_t;

typedef struct
{
  int rbsp_size;
  uint8_t* rbsp_buf;
} h265_slice_data_rbsp_t;

/**
   H265 stream
   Contains data structures for all NAL types that can be handled by this library.  
   When reading, data is read into those, and when writing it is written from those.  
   The reason why they are all contained in one place is that some of them depend on others, we need to 
   have all of them available to read or write correctly.
 */
typedef struct
{
    h265_nal_t* nal;
    h265_vps_t* vps;
    h265_sps_t* sps;
    h265_pps_t* pps;
    h265_aud_t* aud;
    h265_sei_t* sei; //This is a TEMP pointer at whats in h->seis...    
    int num_seis;
    h265_slice_header_t* sh;
    h265_slice_data_rbsp_t* slice_data;

    h265_vps_t* vps_table[32];   // todo 码流文件中不知有多少个
    h265_sps_t* sps_table[32];
    h265_pps_t* pps_table[256];
    h265_sei_t** seis;

} h265_stream_t;

h265_stream_t* h265_new();
void h265_free(h265_stream_t* h);

int h265_read_nal_unit(h265_stream_t* h, uint8_t* buf, int size);

void h265_read_vps_rbsp(h265_stream_t* h, bs_t* b); 
void h265_read_sps_rbsp(h265_stream_t* h, bs_t* b); 
void h265_read_pps_rbsp(h265_stream_t* h, bs_t* b);
void h265_read_sei_rbsp(h265_stream_t* h, bs_t* b);
void h265_read_aud_rbsp(h265_stream_t* h, bs_t* b);
void h265_read_slice_layer_rbsp(h265_stream_t* h, bs_t* b);
void h265_read_end_of_seq_rbsp(h265_stream_t* h, bs_t* b);
void h265_read_end_of_stream_rbsp(h265_stream_t* h, bs_t* b);
void h265_read_rbsp_trailing_bits(bs_t* b);
int h265_more_rbsp_trailing_data(bs_t* b);

//Table 7-1 NAL unit type codes and NAL unit type classes 
enum NalUnitType
{
    NAL_UNIT_CODED_SLICE_TRAIL_N = 0, // 0
    NAL_UNIT_CODED_SLICE_TRAIL_R,     // 1

    NAL_UNIT_CODED_SLICE_TSA_N,       // 2
    NAL_UNIT_CODED_SLICE_TSA_R,       // 3

    NAL_UNIT_CODED_SLICE_STSA_N,      // 4
    NAL_UNIT_CODED_SLICE_STSA_R,      // 5

    NAL_UNIT_CODED_SLICE_RADL_N,      // 6
    NAL_UNIT_CODED_SLICE_RADL_R,      // 7

    NAL_UNIT_CODED_SLICE_RASL_N,      // 8
    NAL_UNIT_CODED_SLICE_RASL_R,      // 9

    NAL_UNIT_RESERVED_VCL_N10,
    NAL_UNIT_RESERVED_VCL_R11,
    NAL_UNIT_RESERVED_VCL_N12,
    NAL_UNIT_RESERVED_VCL_R13,
    NAL_UNIT_RESERVED_VCL_N14,
    NAL_UNIT_RESERVED_VCL_R15,

    NAL_UNIT_CODED_SLICE_BLA_W_LP,    // 16
    NAL_UNIT_CODED_SLICE_BLA_W_RADL,  // 17
    NAL_UNIT_CODED_SLICE_BLA_N_LP,    // 18
    NAL_UNIT_CODED_SLICE_IDR_W_RADL,  // 19
    NAL_UNIT_CODED_SLICE_IDR_N_LP,    // 20
    NAL_UNIT_CODED_SLICE_CRA,         // 21
    NAL_UNIT_RESERVED_IRAP_VCL22,
    NAL_UNIT_RESERVED_IRAP_VCL23,

    NAL_UNIT_RESERVED_VCL24,
    NAL_UNIT_RESERVED_VCL25,
    NAL_UNIT_RESERVED_VCL26,
    NAL_UNIT_RESERVED_VCL27,
    NAL_UNIT_RESERVED_VCL28,
    NAL_UNIT_RESERVED_VCL29,
    NAL_UNIT_RESERVED_VCL30,
    NAL_UNIT_RESERVED_VCL31,

    // non-VCL
    NAL_UNIT_VPS,                     // 32
    NAL_UNIT_SPS,                     // 33
    NAL_UNIT_PPS,                     // 34
    NAL_UNIT_AUD,                     // 35
    NAL_UNIT_EOS,                     // 36
    NAL_UNIT_EOB,                     // 37
    NAL_UNIT_FILLER_DATA,             // 38
    NAL_UNIT_PREFIX_SEI,              // 39
    NAL_UNIT_SUFFIX_SEI,              // 40

    NAL_UNIT_RESERVED_NVCL41,
    NAL_UNIT_RESERVED_NVCL42,
    NAL_UNIT_RESERVED_NVCL43,
    NAL_UNIT_RESERVED_NVCL44,
    NAL_UNIT_RESERVED_NVCL45,
    NAL_UNIT_RESERVED_NVCL46,
    NAL_UNIT_RESERVED_NVCL47,
    NAL_UNIT_UNSPECIFIED_48,
    NAL_UNIT_UNSPECIFIED_49,
    NAL_UNIT_UNSPECIFIED_50,
    NAL_UNIT_UNSPECIFIED_51,
    NAL_UNIT_UNSPECIFIED_52,
    NAL_UNIT_UNSPECIFIED_53,
    NAL_UNIT_UNSPECIFIED_54,
    NAL_UNIT_UNSPECIFIED_55,
    NAL_UNIT_UNSPECIFIED_56,
    NAL_UNIT_UNSPECIFIED_57,
    NAL_UNIT_UNSPECIFIED_58,
    NAL_UNIT_UNSPECIFIED_59,
    NAL_UNIT_UNSPECIFIED_60,
    NAL_UNIT_UNSPECIFIED_61,
    NAL_UNIT_UNSPECIFIED_62,
    NAL_UNIT_UNSPECIFIED_63,
    NAL_UNIT_INVALID,
};

// Table 7-7 – Name association to slice_type --check
#define H265_SH_SLICE_TYPE_B        0        // P (P slice)
#define H265_SH_SLICE_TYPE_P        1        // B (B slice)
#define H265_SH_SLICE_TYPE_I        2        // I (I slice)

//7.4.3.5 Table 7-2 – Interpretation of pic_type --check
#define H265_AUD_PRIMARY_PIC_TYPE_I       0                // I
#define H265_AUD_PRIMARY_PIC_TYPE_IP      1                // P, I
#define H265_AUD_PRIMARY_PIC_TYPE_IPB     2                // B, P, I

//Appendix E. Table E-1  Meaning of sample aspect ratio indicator --check
#define H265_SAR_Unspecified  0           // Unspecified
#define H265_SAR_1_1        1             //  1:1
#define H265_SAR_12_11      2             // 12:11
#define H265_SAR_10_11      3             // 10:11
#define H265_SAR_16_11      4             // 16:11
#define H265_SAR_40_33      5             // 40:33
#define H265_SAR_24_11      6             // 24:11
#define H265_SAR_20_11      7             // 20:11
#define H265_SAR_32_11      8             // 32:11
#define H265_SAR_80_33      9             // 80:33
#define H265_SAR_18_11     10             // 18:11
#define H265_SAR_15_11     11             // 15:11
#define H265_SAR_64_33     12             // 64:33
#define H265_SAR_160_99    13             // 160:99
#define H265_SAR_4_3       14             // 160:99
#define H265_SAR_3_2       15             // 160:99
#define H265_SAR_2_1       16             // 160:99
                                          // 17..254           Reserved
#define H265_SAR_Extended  255        // Extended_SAR

/// chroma formats (according to semantics of chroma_format_idc)
enum ChromaFormat
{
  CHROMA_400        = 0,
  CHROMA_420        = 1,
  CHROMA_422        = 2,
  CHROMA_444        = 3,
  NUM_CHROMA_FORMAT = 4
};

// file handle for debug output
extern FILE* h265_dbgfile;

#ifdef __cplusplus
}
#endif

#endif
