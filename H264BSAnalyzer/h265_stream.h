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
#include "h264_stream.h" // for nal_to_rbsp, etc...

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#include <vector>
using std::vector;

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
typedef struct
{
    uint8_t general_profile_space;
    uint8_t general_tier_flag;
    uint8_t general_profile_idc;
    uint8_t general_profile_compatibility_flag[32];
    uint8_t general_progressive_source_flag;
    uint8_t general_interlaced_source_flag;
    uint8_t general_non_packed_constraint_flag;
    uint8_t general_frame_only_constraint_flag;
    uint8_t general_max_12bit_constraint_flag;
    uint8_t general_max_10bit_constraint_flag;
    uint8_t general_max_8bit_constraint_flag;
    uint8_t general_max_422chroma_constraint_flag;
    uint8_t general_max_420chroma_constraint_flag;
    uint8_t general_max_monochrome_constraint_flag;
    uint8_t general_intra_constraint_flag;
    uint8_t general_one_picture_only_constraint_flag;
    uint8_t general_lower_bit_rate_constraint_flag;
    uint64_t general_reserved_zero_34bits; // todo
    uint64_t general_reserved_zero_43bits; // todo
    uint8_t general_inbld_flag;
    uint8_t general_reserved_zero_bit;
    uint8_t general_level_idc;
    vector<uint8_t> sub_layer_profile_present_flag;
    vector<uint8_t> sub_layer_level_present_flag;
    uint8_t reserved_zero_2bits[8];
    vector<uint8_t> sub_layer_profile_space;
    vector<uint8_t> sub_layer_tier_flag;
    vector<uint8_t> sub_layer_profile_idc;
    vector<vector<uint8_t> > sub_layer_profile_compatibility_flag;
    vector<uint8_t> sub_layer_progressive_source_flag;
    vector<uint8_t> sub_layer_interlaced_source_flag;
    vector<uint8_t> sub_layer_non_packed_constraint_flag;
    vector<uint8_t> sub_layer_frame_only_constraint_flag;
    vector<uint8_t> sub_layer_max_12bit_constraint_flag;
    vector<uint8_t> sub_layer_max_10bit_constraint_flag;
    vector<uint8_t> sub_layer_max_8bit_constraint_flag;
    vector<uint8_t> sub_layer_max_422chroma_constraint_flag;
    vector<uint8_t> sub_layer_max_420chroma_constraint_flag;
    vector<uint8_t> sub_layer_max_monochrome_constraint_flag;
    vector<uint8_t> sub_layer_intra_constraint_flag;
    vector<uint8_t> sub_layer_one_picture_only_constraint_flag;
    vector<uint8_t> sub_layer_lower_bit_rate_constraint_flag;
    vector<uint64_t> sub_layer_reserved_zero_34bits;
    vector<uint64_t> sub_layer_reserved_zero_43bits;
    vector<uint8_t> sub_layer_inbld_flag;
    vector<uint8_t> sub_layer_reserved_zero_bit;
    vector<uint8_t> sub_layer_level_idc;

} profile_tier_level_t;

typedef struct
{
    vector<int> bit_rate_value_minus1;
    vector<int> cpb_size_value_minus1;
    vector<int> cpb_size_du_value_minus1;
    vector<int> bit_rate_du_value_minus1;
    vector<uint8_t> cbr_flag;
} sub_layer_hrd_parameters_t;

/**
E.2.2  HRD parameters syntax
*/
typedef struct
{
    uint8_t nal_hrd_parameters_present_flag;
    uint8_t vcl_hrd_parameters_present_flag;
      uint8_t sub_pic_hrd_params_present_flag;
        uint8_t tick_divisor_minus2;
        uint8_t du_cpb_removal_delay_increment_length_minus1;
        uint8_t sub_pic_cpb_params_in_pic_timing_sei_flag;
        uint8_t dpb_output_delay_du_length_minus1;
      uint8_t bit_rate_scale;
      uint8_t cpb_size_scale;
      uint8_t cpb_size_du_scale;
      uint8_t initial_cpb_removal_delay_length_minus1;
      uint8_t au_cpb_removal_delay_length_minus1;
      uint8_t dpb_output_delay_length_minus1;
    vector<uint8_t> fixed_pic_rate_general_flag;
    vector<uint8_t> fixed_pic_rate_within_cvs_flag;
    vector<int> elemental_duration_in_tc_minus1;
    vector<uint8_t> low_delay_hrd_flag;
    vector<int> cpb_cnt_minus1;
    sub_layer_hrd_parameters_t sub_layer_hrd_parameters; // nal
    sub_layer_hrd_parameters_t sub_layer_hrd_parameters_v; // vlc
} hrd_parameters_t;

/**
sps_range_extension
@see 7.3.2.3.1  General picture parameter set RBSP syntax
*/
typedef struct
{
    uint8_t transform_skip_rotation_enabled_flag;
    uint8_t transform_skip_context_enabled_flag;
    uint8_t implicit_rdpcm_enabled_flag;
    uint8_t explicit_rdpcm_enabled_flag;
    uint8_t extended_precision_processing_flag;
    uint8_t intra_smoothing_disabled_flag;
    uint8_t high_precision_offsets_enabled_flag;
    uint8_t persistent_rice_adaptation_enabled_flag;
    uint8_t cabac_bypass_alignment_enabled_flag;
} sps_range_extension_t;

/**
   Access unit delimiter
   @see 7.3.2.5  Access unit delimiter RBSP syntax
*/
typedef struct
{
    uint8_t pic_type;
} h265_aud_t;

/**
 @see 7.3.2.3.2  Picture parameter set range extension syntax
*/
typedef struct
{
    int log2_max_transform_skip_block_size_minus2;
    uint8_t cross_component_prediction_enabled_flag;
    uint8_t chroma_qp_offset_list_enabled_flag;
      int diff_cu_chroma_qp_offset_depth;
      int chroma_qp_offset_list_len_minus1;
      vector<int> cb_qp_offset_list;
      vector<int> cr_qp_offset_list;
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
    int coefNum;
} scaling_list_data_t;

/**
E.2.1  VUI parameters syntax
*/
typedef struct
{
    uint8_t aspect_ratio_info_present_flag;
      uint8_t aspect_ratio_idc;
      int sar_width;
      int sar_height;
    uint8_t overscan_info_present_flag;
      uint8_t overscan_appropriate_flag;
    uint8_t video_signal_type_present_flag;
      uint8_t video_format;
      uint8_t video_full_range_flag;
      uint8_t colour_description_present_flag;
        uint8_t colour_primaries;
        uint8_t transfer_characteristics;
        uint8_t matrix_coeffs;
    uint8_t chroma_loc_info_present_flag;
      int chroma_sample_loc_type_top_field;
      int chroma_sample_loc_type_bottom_field;
    uint8_t neutral_chroma_indication_flag;
    uint8_t field_seq_flag;
    uint8_t frame_field_info_present_flag;
    uint8_t default_display_window_flag;
      int def_disp_win_left_offset;
      int def_disp_win_right_offset;
      int def_disp_win_top_offset;
      int def_disp_win_bottom_offset;
    uint8_t vui_timing_info_present_flag;
      uint32_t vui_num_units_in_tick;
      uint32_t vui_time_scale;
      uint8_t vui_poc_proportional_to_timing_flag;
        int vui_num_ticks_poc_diff_one_minus1;
      uint8_t vui_hrd_parameters_present_flag;
      hrd_parameters_t hrd_parameters;
    uint8_t bitstream_restriction_flag;
      uint8_t tiles_fixed_structure_flag;
      uint8_t motion_vectors_over_pic_boundaries_flag;
      uint8_t restricted_ref_pic_lists_flag;
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
    vector<uint8_t> luma_weight_l0_flag;
    vector<uint8_t> chroma_weight_l0_flag;
    vector<int> delta_luma_weight_l0;
    vector<int> luma_offset_l0;
    vector<vector <int> > delta_chroma_weight_l0;
    vector<vector <int> > delta_chroma_offset_l0;
    vector<uint8_t> luma_weight_l1_flag;
    vector<uint8_t> chroma_weight_l1_flag;
    vector<int> delta_luma_weight_l1;
    vector<int> luma_offset_l1;
    vector<vector<int> > delta_chroma_weight_l1;
    vector<vector<int> > delta_chroma_offset_l1;
} pred_weight_table_t;

typedef struct
{
    uint8_t inter_ref_pic_set_prediction_flag;
    int delta_idx_minus1;
    uint8_t delta_rps_sign;
    int abs_delta_rps_minus1;
    vector<uint8_t> used_by_curr_pic_flag;
    vector<uint8_t> use_delta_flag;
    int num_negative_pics;
    int num_positive_pics;
    //vector<int> delta_poc_s0_minus1;
    //vector<uint8_t> used_by_curr_pic_s0_flag;
    //vector<int> delta_poc_s1_minus1;
    //vector<uint8_t> used_by_curr_pic_s1_flag;
    int delta_poc_s0_minus1[256];
    uint8_t used_by_curr_pic_s0_flag[256];
    int delta_poc_s1_minus1[256];
    uint8_t used_by_curr_pic_s1_flag[256];
} st_ref_pic_set_t;

#define MAX_NUM_REF_PICS 16 ///< max. number of pictures used for reference

typedef struct
{
    int m_numberOfPictures;
    int m_numberOfNegativePictures;
    int m_numberOfPositivePictures;
    int m_numberOfLongtermPictures;
    int m_deltaPOC[MAX_NUM_REF_PICS];
    int m_POC[MAX_NUM_REF_PICS];
    int m_used[MAX_NUM_REF_PICS];
    int m_interRPSPrediction;
    int m_deltaRIdxMinus1;
    int m_deltaRPS;
    int m_numRefIdc;
    int m_refIdc[MAX_NUM_REF_PICS+1];
    int m_bCheckLTMSB[MAX_NUM_REF_PICS];
    int m_pocLSBLT[MAX_NUM_REF_PICS];
    int m_deltaPOCMSBCycleLT[MAX_NUM_REF_PICS];
    int m_deltaPocMSBPresentFlag[MAX_NUM_REF_PICS];
} referencePictureSets_t;

typedef struct
{
    uint8_t ref_pic_list_modification_flag_l0;
    uint32_t list_entry_l0[32]; // according to HM16.6 source code
    uint8_t ref_pic_list_modification_flag_l1;
    uint32_t list_entry_l1[32];
} ref_pic_lists_modification_t;

/**
   Video Parameter Set
   @see 7.3.2.1 Video parameter set RBSP syntax
*/
typedef struct
{
    uint8_t vps_video_parameter_set_id; // u(4)
    uint8_t vps_base_layer_internal_flag; // u(1)
    uint8_t vps_base_layer_available_flag; // u(1)
    uint8_t vps_max_layers_minus1; // u(6)
    uint8_t vps_max_sub_layers_minus1; // u(3)
    uint8_t vps_temporal_id_nesting_flag; // u(1)
    int vps_reserved_0xffff_16bits; // u(16)
    profile_tier_level_t ptl;
    uint8_t vps_sub_layer_ordering_info_present_flag;
    // Sublayers
    int vps_max_dec_pic_buffering_minus1[8]; // max u(3)
    int vps_max_num_reorder_pics[8];
    int vps_max_latency_increase_plus1[8];
    uint8_t vps_max_layer_id;
    int vps_num_layer_sets_minus1;
    vector<vector<uint8_t> > layer_id_included_flag;
    uint8_t vps_timing_info_present_flag;
      int vps_num_units_in_tick;
      int vps_time_scale;
      uint8_t vps_poc_proportional_to_timing_flag;
        int vps_num_ticks_poc_diff_one_minus1;
      int vps_num_hrd_parameters;
      vector<int> hrd_layer_set_idx;
      vector<uint8_t> cprms_present_flag;
      hrd_parameters_t hrd_parameters;
    uint8_t vps_extension_flag;
      uint8_t vps_extension_data_flag;
} h265_vps_t;

/**
   Sequence Parameter Set
   @see 7.3.2.2 Sequence parameter set RBSP syntax
*/
typedef struct
{
    uint8_t sps_video_parameter_set_id;
    uint8_t sps_max_sub_layers_minus1;
    uint8_t sps_temporal_id_nesting_flag;
    profile_tier_level_t ptl;
    int sps_seq_parameter_set_id;
    int chroma_format_idc;
    uint8_t separate_colour_plane_flag;
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
    uint8_t sps_sub_layer_ordering_info_present_flag;
    int sps_max_dec_pic_buffering_minus1[8]; // max u(3)
    int sps_max_num_reorder_pics[8];
    int sps_max_latency_increase_plus1[8];
    int log2_min_luma_coding_block_size_minus3;
    int log2_diff_max_min_luma_coding_block_size;
    int log2_min_luma_transform_block_size_minus2;
    int log2_diff_max_min_luma_transform_block_size;
    int max_transform_hierarchy_depth_inter;
    int max_transform_hierarchy_depth_intra;
    uint8_t scaling_list_enabled_flag;
      uint8_t sps_infer_scaling_list_flag;
      int sps_scaling_list_ref_layer_id;
      int sps_scaling_list_data_present_flag;
        scaling_list_data_t scaling_list_data;
    uint8_t amp_enabled_flag;
    uint8_t sample_adaptive_offset_enabled_flag;
    uint8_t pcm_enabled_flag;
      uint8_t pcm_sample_bit_depth_luma_minus1;
      uint8_t pcm_sample_bit_depth_chroma_minus1;
      int log2_min_pcm_luma_coding_block_size_minus3;
      int log2_diff_max_min_pcm_luma_coding_block_size;
      uint8_t pcm_loop_filter_disabled_flag;
    int num_short_term_ref_pic_sets;
    vector<st_ref_pic_set_t> st_ref_pic_set;
    vector<referencePictureSets_t> m_RPSList; // store
    uint8_t long_term_ref_pics_present_flag;
      int num_long_term_ref_pics_sps;
      vector<int> lt_ref_pic_poc_lsb_sps;
      vector<uint8_t> used_by_curr_pic_lt_sps_flag;
    uint8_t sps_temporal_mvp_enabled_flag;
    uint8_t strong_intra_smoothing_enabled_flag;
    uint8_t vui_parameters_present_flag;
      vui_parameters_t vui;
    uint8_t sps_extension_present_flag;
      uint8_t sps_range_extension_flag;
      uint8_t sps_multilayer_extension_flag;
      uint8_t sps_3d_extension_flag;
      uint8_t sps_extension_5bits;
    sps_range_extension_t sps_range_extension;
    uint8_t inter_view_mv_vert_constraint_flag ; //sps_multilayer_extension_t sps_multilayer_extension;
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
    uint8_t pps_pic_parameter_set_id;
    uint8_t pps_seq_parameter_set_id;
    uint8_t dependent_slice_segments_enabled_flag;
    uint8_t output_flag_present_flag;
    uint8_t num_extra_slice_header_bits;
    uint8_t sign_data_hiding_enabled_flag;
    uint8_t cabac_init_present_flag;
    int num_ref_idx_l0_default_active_minus1;
    int num_ref_idx_l1_default_active_minus1;
    int init_qp_minus26;
    uint8_t constrained_intra_pred_flag;
    uint8_t transform_skip_enabled_flag;
    uint8_t cu_qp_delta_enabled_flag;
    int diff_cu_qp_delta_depth;
    int pps_cb_qp_offset;
    int pps_cr_qp_offset;
    uint8_t pps_slice_chroma_qp_offsets_present_flag;
    uint8_t weighted_pred_flag;
    int weighted_bipred_flag;
    uint8_t transquant_bypass_enabled_flag;
    uint8_t tiles_enabled_flag;
    uint8_t entropy_coding_sync_enabled_flag;
      int num_tile_columns_minus1;
      int num_tile_rows_minus1;
      int uniform_spacing_flag;
        vector<int> column_width_minus1;
        vector<int> row_height_minus1;
      uint8_t loop_filter_across_tiles_enabled_flag;
    uint8_t pps_loop_filter_across_slices_enabled_flag;
    uint8_t deblocking_filter_control_present_flag;
      uint8_t deblocking_filter_override_enabled_flag;
      uint8_t pps_deblocking_filter_disabled_flag;
        int pps_beta_offset_div2;
        int pps_tc_offset_div2;
    uint8_t pps_scaling_list_data_present_flag;
      scaling_list_data_t scaling_list_data;
    uint8_t lists_modification_present_flag;
    int log2_parallel_merge_level_minus2;
    uint8_t slice_segment_header_extension_present_flag;
    uint8_t pps_extension_present_flag;
      uint8_t pps_range_extension_flag;
      uint8_t pps_multilayer_extension_flag;
      uint8_t pps_3d_extension_flag;
      uint8_t pps_extension_5bits;
    pps_range_extension_t pps_range_extension;
    //pps_multilayer_extension_t pps_multilayer_extension;
    //pps_3d_extension_t pps_3d_extension;
    uint8_t pps_extension_data_flag;
    // rbsp_trailing_bits( ) ...
} h265_pps_t;

/**
  Slice Header
  @see 7.3.6.1  General slice segment header syntax
*/
typedef struct
{
    int read_slice_type; // see if we only read slice type and return

    int first_slice_segment_in_pic_flag;
    uint8_t no_output_of_prior_pics_flag;
    int slice_pic_parameter_set_id;
      uint8_t dependent_slice_segment_flag;
      int slice_segment_address;
        vector<uint8_t> slice_reserved_flag;
        int slice_type;
        uint8_t pic_output_flag;
        int colour_plane_id;
        int slice_pic_order_cnt_lsb;
        uint8_t short_term_ref_pic_set_sps_flag;
        st_ref_pic_set_t st_ref_pic_set;
        referencePictureSets_t* m_pRPS;
        referencePictureSets_t m_localRPS;
        int short_term_ref_pic_set_idx;
        int num_long_term_sps;
        int num_long_term_pics;
        vector<int> lt_idx_sps;
        vector<int> poc_lsb_lt;
        vector<uint8_t> used_by_curr_pic_lt_flag;
        vector<uint8_t> delta_poc_msb_present_flag;
        vector<int> delta_poc_msb_cycle_lt;
    uint8_t slice_temporal_mvp_enabled_flag;
    uint8_t slice_sao_luma_flag;
    uint8_t slice_sao_chroma_flag;
    uint8_t num_ref_idx_active_override_flag;
    int num_ref_idx_l0_active_minus1;
    int num_ref_idx_l1_active_minus1;
    ref_pic_lists_modification_t ref_pic_lists_modification;
    uint8_t mvd_l1_zero_flag;
    uint8_t cabac_init_flag;
    uint8_t collocated_from_l0_flag;
    int collocated_ref_idx;
    pred_weight_table_t pred_weight_table;
    int five_minus_max_num_merge_cand;
    int slice_qp_delta;
    int slice_cb_qp_offset;
    int slice_cr_qp_offset;
    uint8_t cu_chroma_qp_offset_enabled_flag;
    uint8_t deblocking_filter_override_flag;
    uint8_t slice_deblocking_filter_disabled_flag;
    int slice_beta_offset_div2;
    int slice_tc_offset_div2;
    uint8_t slice_loop_filter_across_slices_enabled_flag;
    int num_entry_point_offsets;
    int offset_len_minus1;
    vector<int> entry_point_offset_minus1;
    int slice_segment_header_extension_length;
    vector<int> slice_segment_header_extension_data_byte;
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

    h265_vps_t* vps_table[16];   // --checked
    h265_sps_t* sps_table[32];
    h265_pps_t* pps_table[256];
    h265_sei_t** seis;
    videoinfo_t* info;
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

// Table 7-7 ¨C Name association to slice_type --checked
enum SliceType
{
    H265_SH_SLICE_TYPE_B = 0,        // P (P slice)
    H265_SH_SLICE_TYPE_P = 1,        // B (B slice)
    H265_SH_SLICE_TYPE_I = 2,        // I (I slice)
};

//7.4.3.5 Table 7-2 ¨C Interpretation of pic_type --checked
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

enum ProfileName
{
    PROFILE_NONE = 0,
    PROFILE_MAIN = 1,
    PROFILE_MAIN10 = 2,
    PROFILE_MAINSTILLPICTURE = 3,
    PROFILE_MAINREXT = 4,
    PROFILE_HIGHTHROUGHPUTREXT = 5
};

enum Tier
{
    TIER_MAIN = 0,
    TIER_HIGH = 1,
};

enum Level
{
    // code = (level * 30)
    LEVEL_NONE     = 0,
    LEVEL1   = 30,
    LEVEL2   = 60,
    LEVEL2_1 = 63,
    LEVEL3   = 90,
    LEVEL3_1 = 93,
    LEVEL4   = 120,
    LEVEL4_1 = 123,
    LEVEL5   = 150,
    LEVEL5_1 = 153,
    LEVEL5_2 = 156,
    LEVEL6   = 180,
    LEVEL6_1 = 183,
    LEVEL6_2 = 186,
    LEVEL8_5 = 255,
};

// file handle for debug output
extern FILE* h265_dbgfile;

#endif
