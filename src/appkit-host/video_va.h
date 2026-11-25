// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The part of VA-API that the decoders use. The libraries are opened when a program plays a video, so that nothing else
// needs them.

#pragma once

#include <cstddef>
#include <cstdint>

namespace cutefish::reef::video {

using VADisplay = void*;
using VAStatus = int;
using VAConfigID = unsigned int;
using VAContextID = unsigned int;
using VASurfaceID = unsigned int;
using VABufferID = unsigned int;
using VAImageID = unsigned int;

constexpr VAStatus VA_STATUS_SUCCESS = 0;
constexpr unsigned int VA_INVALID_ID = 0xffffffff;
constexpr VASurfaceID VA_INVALID_SURFACE = VA_INVALID_ID;

enum VAProfile {
    VAProfileNone = -1,
    VAProfileH264Main = 6,
    VAProfileH264High = 7,
    VAProfileH264ConstrainedBaseline = 13,
    VAProfileHEVCMain = 17,
    VAProfileHEVCMain10 = 18,
    VAProfileVP9Profile0 = 19,
    VAProfileVP9Profile2 = 21,
};

enum VAEntrypoint {
    VAEntrypointVLD = 1,
};

enum VAConfigAttribType {
    VAConfigAttribRTFormat = 0,
};

struct VAConfigAttrib
{
    VAConfigAttribType type;
    uint32_t value;
};

constexpr uint32_t VA_RT_FORMAT_YUV420 = 0x1;
constexpr uint32_t VA_RT_FORMAT_YUV420_10 = 0x100;
constexpr uint32_t VA_FOURCC_NV12 = 0x3231564e;
constexpr uint32_t VA_FOURCC_P010 = 0x30313050;
constexpr int VA_PROGRESSIVE = 0x1;

enum VAGenericValueType {
    VAGenericValueTypeInteger = 1,
};

struct VAGenericValue
{
    VAGenericValueType type;
    union {
        int32_t i;
        float f;
        void* p;
        void (*fn)();
    } value;
};

enum VASurfaceAttribType {
    VASurfaceAttribPixelFormat = 1,
};

constexpr uint32_t VA_SURFACE_ATTRIB_SETTABLE = 0x2;

struct VASurfaceAttrib
{
    VASurfaceAttribType type;
    uint32_t flags;
    VAGenericValue value;
};

struct VAImageFormat
{
    uint32_t fourcc;
    uint32_t byte_order;
    uint32_t bits_per_pixel;
    uint32_t depth;
    uint32_t red_mask;
    uint32_t green_mask;
    uint32_t blue_mask;
    uint32_t alpha_mask;
    uint32_t va_reserved[4];
};

struct VAImage
{
    VAImageID image_id;
    VAImageFormat format;
    VABufferID buf;
    uint16_t width;
    uint16_t height;
    uint32_t data_size;
    uint32_t num_planes;
    uint32_t pitches[3];
    uint32_t offsets[3];
    int32_t num_palette_entries;
    int32_t entry_bytes;
    int8_t component_order[4];
    uint32_t va_reserved[4];
};

constexpr uint32_t VA_SURFACE_ATTRIB_MEM_TYPE_DRM_PRIME_2 = 0x40000000;
constexpr uint32_t VA_EXPORT_SURFACE_READ_ONLY = 0x1;
constexpr uint32_t VA_EXPORT_SURFACE_SEPARATE_LAYERS = 0x4;

struct VADRMPRIMESurfaceDescriptor
{
    uint32_t fourcc;
    uint32_t width;
    uint32_t height;
    uint32_t num_objects;
    struct {
        int fd;
        uint32_t size;
        uint64_t drm_format_modifier;
    } objects[4];
    uint32_t num_layers;
    struct {
        uint32_t drm_format;
        uint32_t num_planes;
        uint32_t object_index[4];
        uint32_t offset[4];
        uint32_t pitch[4];
    } layers[4];
};

enum VABufferType {
    VAPictureParameterBufferType = 0,
    VAIQMatrixBufferType = 1,
    VASliceParameterBufferType = 4,
    VASliceDataBufferType = 5,
};

// H.264

constexpr uint32_t VA_PICTURE_H264_INVALID = 0x1;
constexpr uint32_t VA_PICTURE_H264_SHORT_TERM_REFERENCE = 0x8;
constexpr uint32_t VA_PICTURE_H264_LONG_TERM_REFERENCE = 0x10;

struct VAPictureH264
{
    VASurfaceID picture_id;
    uint32_t frame_idx;
    uint32_t flags;
    int32_t TopFieldOrderCnt;
    int32_t BottomFieldOrderCnt;
    uint32_t va_reserved[4];
};

struct VAPictureParameterBufferH264
{
    VAPictureH264 CurrPic;
    VAPictureH264 ReferenceFrames[16];
    uint16_t picture_width_in_mbs_minus1;
    uint16_t picture_height_in_mbs_minus1;
    uint8_t bit_depth_luma_minus8;
    uint8_t bit_depth_chroma_minus8;
    uint8_t num_ref_frames;
    union {
        struct {
            uint32_t chroma_format_idc : 2;
            uint32_t residual_colour_transform_flag : 1;
            uint32_t gaps_in_frame_num_value_allowed_flag : 1;
            uint32_t frame_mbs_only_flag : 1;
            uint32_t mb_adaptive_frame_field_flag : 1;
            uint32_t direct_8x8_inference_flag : 1;
            uint32_t MinLumaBiPredSize8x8 : 1;
            uint32_t log2_max_frame_num_minus4 : 4;
            uint32_t pic_order_cnt_type : 2;
            uint32_t log2_max_pic_order_cnt_lsb_minus4 : 4;
            uint32_t delta_pic_order_always_zero_flag : 1;
        } bits;
        uint32_t value;
    } seq_fields;
    uint8_t num_slice_groups_minus1;
    uint8_t slice_group_map_type;
    uint16_t slice_group_change_rate_minus1;
    int8_t pic_init_qp_minus26;
    int8_t pic_init_qs_minus26;
    int8_t chroma_qp_index_offset;
    int8_t second_chroma_qp_index_offset;
    union {
        struct {
            uint32_t entropy_coding_mode_flag : 1;
            uint32_t weighted_pred_flag : 1;
            uint32_t weighted_bipred_idc : 2;
            uint32_t transform_8x8_mode_flag : 1;
            uint32_t field_pic_flag : 1;
            uint32_t constrained_intra_pred_flag : 1;
            uint32_t pic_order_present_flag : 1;
            uint32_t deblocking_filter_control_present_flag : 1;
            uint32_t redundant_pic_cnt_present_flag : 1;
            uint32_t reference_pic_flag : 1;
        } bits;
        uint32_t value;
    } pic_fields;
    uint16_t frame_num;
    uint32_t va_reserved[8];
};

struct VAIQMatrixBufferH264
{
    uint8_t ScalingList4x4[6][16];
    uint8_t ScalingList8x8[2][64];
    uint32_t va_reserved[4];
};

struct VASliceParameterBufferH264
{
    uint32_t slice_data_size;
    uint32_t slice_data_offset;
    uint32_t slice_data_flag;
    uint16_t slice_data_bit_offset;
    uint16_t first_mb_in_slice;
    uint8_t slice_type;
    uint8_t direct_spatial_mv_pred_flag;
    uint8_t num_ref_idx_l0_active_minus1;
    uint8_t num_ref_idx_l1_active_minus1;
    uint8_t cabac_init_idc;
    int8_t slice_qp_delta;
    uint8_t disable_deblocking_filter_idc;
    int8_t slice_alpha_c0_offset_div2;
    int8_t slice_beta_offset_div2;
    VAPictureH264 RefPicList0[32];
    VAPictureH264 RefPicList1[32];
    uint8_t luma_log2_weight_denom;
    uint8_t chroma_log2_weight_denom;
    uint8_t luma_weight_l0_flag;
    int16_t luma_weight_l0[32];
    int16_t luma_offset_l0[32];
    uint8_t chroma_weight_l0_flag;
    int16_t chroma_weight_l0[32][2];
    int16_t chroma_offset_l0[32][2];
    uint8_t luma_weight_l1_flag;
    int16_t luma_weight_l1[32];
    int16_t luma_offset_l1[32];
    uint8_t chroma_weight_l1_flag;
    int16_t chroma_weight_l1[32][2];
    int16_t chroma_offset_l1[32][2];
    uint32_t va_reserved[4];
};

// HEVC

constexpr uint32_t VA_PICTURE_HEVC_INVALID = 0x1;
constexpr uint32_t VA_PICTURE_HEVC_LONG_TERM_REFERENCE = 0x8;
constexpr uint32_t VA_PICTURE_HEVC_RPS_ST_CURR_BEFORE = 0x10;
constexpr uint32_t VA_PICTURE_HEVC_RPS_ST_CURR_AFTER = 0x20;
constexpr uint32_t VA_PICTURE_HEVC_RPS_LT_CURR = 0x40;

struct VAPictureHEVC
{
    VASurfaceID picture_id;
    int32_t pic_order_cnt;
    uint32_t flags;
    uint32_t va_reserved[4];
};

struct VAPictureParameterBufferHEVC
{
    VAPictureHEVC CurrPic;
    VAPictureHEVC ReferenceFrames[15];
    uint16_t pic_width_in_luma_samples;
    uint16_t pic_height_in_luma_samples;
    union {
        struct {
            uint32_t chroma_format_idc : 2;
            uint32_t separate_colour_plane_flag : 1;
            uint32_t pcm_enabled_flag : 1;
            uint32_t scaling_list_enabled_flag : 1;
            uint32_t transform_skip_enabled_flag : 1;
            uint32_t amp_enabled_flag : 1;
            uint32_t strong_intra_smoothing_enabled_flag : 1;
            uint32_t sign_data_hiding_enabled_flag : 1;
            uint32_t constrained_intra_pred_flag : 1;
            uint32_t cu_qp_delta_enabled_flag : 1;
            uint32_t weighted_pred_flag : 1;
            uint32_t weighted_bipred_flag : 1;
            uint32_t transquant_bypass_enabled_flag : 1;
            uint32_t tiles_enabled_flag : 1;
            uint32_t entropy_coding_sync_enabled_flag : 1;
            uint32_t pps_loop_filter_across_slices_enabled_flag : 1;
            uint32_t loop_filter_across_tiles_enabled_flag : 1;
            uint32_t pcm_loop_filter_disabled_flag : 1;
            uint32_t NoPicReorderingFlag : 1;
            uint32_t NoBiPredFlag : 1;
            uint32_t ReservedBits : 11;
        } bits;
        uint32_t value;
    } pic_fields;
    uint8_t sps_max_dec_pic_buffering_minus1;
    uint8_t bit_depth_luma_minus8;
    uint8_t bit_depth_chroma_minus8;
    uint8_t pcm_sample_bit_depth_luma_minus1;
    uint8_t pcm_sample_bit_depth_chroma_minus1;
    uint8_t log2_min_luma_coding_block_size_minus3;
    uint8_t log2_diff_max_min_luma_coding_block_size;
    uint8_t log2_min_transform_block_size_minus2;
    uint8_t log2_diff_max_min_transform_block_size;
    uint8_t log2_min_pcm_luma_coding_block_size_minus3;
    uint8_t log2_diff_max_min_pcm_luma_coding_block_size;
    uint8_t max_transform_hierarchy_depth_intra;
    uint8_t max_transform_hierarchy_depth_inter;
    int8_t init_qp_minus26;
    uint8_t diff_cu_qp_delta_depth;
    int8_t pps_cb_qp_offset;
    int8_t pps_cr_qp_offset;
    uint8_t log2_parallel_merge_level_minus2;
    uint8_t num_tile_columns_minus1;
    uint8_t num_tile_rows_minus1;
    uint16_t column_width_minus1[19];
    uint16_t row_height_minus1[21];
    union {
        struct {
            uint32_t lists_modification_present_flag : 1;
            uint32_t long_term_ref_pics_present_flag : 1;
            uint32_t sps_temporal_mvp_enabled_flag : 1;
            uint32_t cabac_init_present_flag : 1;
            uint32_t output_flag_present_flag : 1;
            uint32_t dependent_slice_segments_enabled_flag : 1;
            uint32_t pps_slice_chroma_qp_offsets_present_flag : 1;
            uint32_t sample_adaptive_offset_enabled_flag : 1;
            uint32_t deblocking_filter_override_enabled_flag : 1;
            uint32_t pps_disable_deblocking_filter_flag : 1;
            uint32_t slice_segment_header_extension_present_flag : 1;
            uint32_t RapPicFlag : 1;
            uint32_t IdrPicFlag : 1;
            uint32_t IntraPicFlag : 1;
            uint32_t ReservedBits : 18;
        } bits;
        uint32_t value;
    } slice_parsing_fields;
    uint8_t log2_max_pic_order_cnt_lsb_minus4;
    uint8_t num_short_term_ref_pic_sets;
    uint8_t num_long_term_ref_pic_sps;
    uint8_t num_ref_idx_l0_default_active_minus1;
    uint8_t num_ref_idx_l1_default_active_minus1;
    int8_t pps_beta_offset_div2;
    int8_t pps_tc_offset_div2;
    uint8_t num_extra_slice_header_bits;
    uint32_t st_rps_bits;
    uint32_t va_reserved[8];
};

struct VAIQMatrixBufferHEVC
{
    uint8_t ScalingList4x4[6][16];
    uint8_t ScalingList8x8[6][64];
    uint8_t ScalingList16x16[6][64];
    uint8_t ScalingList32x32[2][64];
    uint8_t ScalingListDC16x16[6];
    uint8_t ScalingListDC32x32[2];
    uint32_t va_reserved[4];
};

struct VASliceParameterBufferHEVC
{
    uint32_t slice_data_size;
    uint32_t slice_data_offset;
    uint32_t slice_data_flag;
    uint32_t slice_data_byte_offset;
    uint32_t slice_segment_address;
    uint8_t RefPicList[2][15];
    union {
        struct {
            uint32_t LastSliceOfPic : 1;
            uint32_t dependent_slice_segment_flag : 1;
            uint32_t slice_type : 2;
            uint32_t color_plane_id : 2;
            uint32_t slice_sao_luma_flag : 1;
            uint32_t slice_sao_chroma_flag : 1;
            uint32_t mvd_l1_zero_flag : 1;
            uint32_t cabac_init_flag : 1;
            uint32_t slice_temporal_mvp_enabled_flag : 1;
            uint32_t slice_deblocking_filter_disabled_flag : 1;
            uint32_t collocated_from_l0_flag : 1;
            uint32_t slice_loop_filter_across_slices_enabled_flag : 1;
            uint32_t reserved : 18;
        } fields;
        uint32_t value;
    } LongSliceFlags;
    uint8_t collocated_ref_idx;
    uint8_t num_ref_idx_l0_active_minus1;
    uint8_t num_ref_idx_l1_active_minus1;
    int8_t slice_qp_delta;
    int8_t slice_cb_qp_offset;
    int8_t slice_cr_qp_offset;
    int8_t slice_beta_offset_div2;
    int8_t slice_tc_offset_div2;
    uint8_t luma_log2_weight_denom;
    int8_t delta_chroma_log2_weight_denom;
    int8_t delta_luma_weight_l0[15];
    int8_t luma_offset_l0[15];
    int8_t delta_chroma_weight_l0[15][2];
    int8_t ChromaOffsetL0[15][2];
    int8_t delta_luma_weight_l1[15];
    int8_t luma_offset_l1[15];
    int8_t delta_chroma_weight_l1[15][2];
    int8_t ChromaOffsetL1[15][2];
    uint8_t five_minus_max_num_merge_cand;
    uint16_t num_entry_point_offsets;
    uint16_t entry_offset_to_subset_array;
    uint16_t slice_data_num_emu_prevn_bytes;
    uint32_t va_reserved[2];
};

// VP9

struct VADecPictureParameterBufferVP9
{
    uint16_t frame_width;
    uint16_t frame_height;
    VASurfaceID reference_frames[8];
    union {
        struct {
            uint32_t subsampling_x : 1;
            uint32_t subsampling_y : 1;
            uint32_t frame_type : 1;
            uint32_t show_frame : 1;
            uint32_t error_resilient_mode : 1;
            uint32_t intra_only : 1;
            uint32_t allow_high_precision_mv : 1;
            uint32_t mcomp_filter_type : 3;
            uint32_t frame_parallel_decoding_mode : 1;
            uint32_t reset_frame_context : 2;
            uint32_t refresh_frame_context : 1;
            uint32_t frame_context_idx : 2;
            uint32_t segmentation_enabled : 1;
            uint32_t segmentation_temporal_update : 1;
            uint32_t segmentation_update_map : 1;
            uint32_t last_ref_frame : 3;
            uint32_t last_ref_frame_sign_bias : 1;
            uint32_t golden_ref_frame : 3;
            uint32_t golden_ref_frame_sign_bias : 1;
            uint32_t alt_ref_frame : 3;
            uint32_t alt_ref_frame_sign_bias : 1;
            uint32_t lossless_flag : 1;
        } bits;
        uint32_t value;
    } pic_fields;
    uint8_t filter_level;
    uint8_t sharpness_level;
    uint8_t log2_tile_rows;
    uint8_t log2_tile_columns;
    uint8_t frame_header_length_in_bytes;
    uint16_t first_partition_size;
    uint8_t mb_segment_tree_probs[7];
    uint8_t segment_pred_probs[3];
    uint8_t profile;
    uint8_t bit_depth;
    uint32_t va_reserved[8];
};

struct VASegmentParameterVP9
{
    union {
        struct {
            uint16_t segment_reference_enabled : 1;
            uint16_t segment_reference : 2;
            uint16_t segment_reference_skipped : 1;
        } fields;
        uint16_t value;
    } segment_flags;
    uint8_t filter_level[4][2];
    int16_t luma_ac_quant_scale;
    int16_t luma_dc_quant_scale;
    int16_t chroma_ac_quant_scale;
    int16_t chroma_dc_quant_scale;
    uint32_t va_reserved[4];
};

struct VASliceParameterBufferVP9
{
    uint32_t slice_data_size;
    uint32_t slice_data_offset;
    uint32_t slice_data_flag;
    VASegmentParameterVP9 seg_param[8];
    uint32_t va_reserved[4];
};

using VAMessageCallback = void (*)(void* context, const char* message);

struct VaApi
{
    VAStatus (*initialize)(VADisplay display, int* major, int* minor);
    VAStatus (*terminate)(VADisplay display);
    const char* (*errorStr)(VAStatus status);
    VAMessageCallback (*setErrorCallback)(VADisplay display, VAMessageCallback callback, void* context);
    VAMessageCallback (*setInfoCallback)(VADisplay display, VAMessageCallback callback, void* context);
    int (*maxNumProfiles)(VADisplay display);
    VAStatus (*queryConfigProfiles)(VADisplay display, VAProfile* profiles, int* count);
    int (*maxNumEntrypoints)(VADisplay display);
    VAStatus (*queryConfigEntrypoints)(VADisplay display, VAProfile profile, VAEntrypoint* entrypoints, int* count);
    VAStatus (*getConfigAttributes)(VADisplay display, VAProfile profile, VAEntrypoint entrypoint, VAConfigAttrib* attributes,
                                    int count);
    VAStatus (*createConfig)(VADisplay display, VAProfile profile, VAEntrypoint entrypoint, VAConfigAttrib* attributes,
                             int count, VAConfigID* config);
    VAStatus (*destroyConfig)(VADisplay display, VAConfigID config);
    VAStatus (*createSurfaces)(VADisplay display, unsigned int format, unsigned int width, unsigned int height,
                               VASurfaceID* surfaces, unsigned int count, VASurfaceAttrib* attributes,
                               unsigned int attributeCount);
    VAStatus (*destroySurfaces)(VADisplay display, VASurfaceID* surfaces, int count);
    VAStatus (*createContext)(VADisplay display, VAConfigID config, int width, int height, int flag, VASurfaceID* targets,
                              int count, VAContextID* context);
    VAStatus (*destroyContext)(VADisplay display, VAContextID context);
    VAStatus (*createBuffer)(VADisplay display, VAContextID context, VABufferType type, unsigned int size, unsigned int count,
                             void* data, VABufferID* buffer);
    VAStatus (*destroyBuffer)(VADisplay display, VABufferID buffer);
    VAStatus (*beginPicture)(VADisplay display, VAContextID context, VASurfaceID target);
    VAStatus (*renderPicture)(VADisplay display, VAContextID context, VABufferID* buffers, int count);
    VAStatus (*endPicture)(VADisplay display, VAContextID context);
    VAStatus (*syncSurface)(VADisplay display, VASurfaceID surface);
    int (*maxNumImageFormats)(VADisplay display);
    VAStatus (*queryImageFormats)(VADisplay display, VAImageFormat* formats, int* count);
    VAStatus (*createImage)(VADisplay display, VAImageFormat* format, int width, int height, VAImage* image);
    VAStatus (*deriveImage)(VADisplay display, VASurfaceID surface, VAImage* image);
    VAStatus (*getImage)(VADisplay display, VASurfaceID surface, int x, int y, unsigned int width, unsigned int height,
                         VAImageID image);
    VAStatus (*destroyImage)(VADisplay display, VAImageID image);
    VAStatus (*mapBuffer)(VADisplay display, VABufferID buffer, void** data);
    VAStatus (*unmapBuffer)(VADisplay display, VABufferID buffer);
    VAStatus (*exportSurfaceHandle)(VADisplay display, VASurfaceID surface, uint32_t type, uint32_t flags, void* descriptor);
    VADisplay (*getDisplayDRM)(int fd);
    bool loaded;
};

// Opens the libraries the first time.
const VaApi& vaApi();

} // namespace cutefish::reef::video
