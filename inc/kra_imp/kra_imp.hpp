/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#pragma once
#include <kra_imp/config.hpp>
#include <kra_imp/types.hpp>

#ifdef __cplusplus
extern "C" {
#endif
	KRA_IMP_API kkra_archive_t* kkra_open_archive(const char* archive_data, const unsigned long long archive_data_size);
	KRA_IMP_API void kkra_close_archive(kkra_archive_t* archive);
	KRA_IMP_API unsigned long long kkra_file_size(kkra_archive_t* archive, const char* file_name);
	KRA_IMP_API unsigned long long kkra_file_load(kkra_archive_t* archive, const char* file_name, char* buffer, const unsigned long long buffer_size);
	KRA_IMP_API const char* kkra_main_doc_name();
	KRA_IMP_API unsigned char kkra_main_doc_read(kkra_archive_t* archive, const char* buffer, const unsigned long long buffer_size);
	KRA_IMP_API const char* kkra_image_name(kkra_archive_t* archive);
	KRA_IMP_API const char* kkra_color_space(kkra_archive_t* archive);
	KRA_IMP_API unsigned long long kkra_image_height(kkra_archive_t* archive);
	KRA_IMP_API unsigned long long kkra_image_width(kkra_archive_t* archive);
	KRA_IMP_API unsigned long long kkra_layers_count(kkra_archive_t* archive);
	KRA_IMP_API unsigned char kkra_layer_read(kkra_archive_t* archive, const unsigned long long layer_index);
	KRA_IMP_API unsigned char kkra_current_layer_type(kkra_archive_t* archive);
	KRA_IMP_API unsigned long long kkra_current_layer_parent_index(kkra_archive_t* archive);
	KRA_IMP_API const char* kkra_current_layer_name(kkra_archive_t* archive);
	KRA_IMP_API const char* kkra_current_layer_file_name(kkra_archive_t* archive);
	KRA_IMP_API const char* kkra_current_layer_key_frame_file_name(kkra_archive_t* archive);
	KRA_IMP_API unsigned char kkra_layer_file_path(char* layer_path_buffer, const unsigned char layer_path_buffer_size, const char* image_name, const char* file_name);
	KRA_IMP_API unsigned char kkra_key_frames_read(kkra_archive_t* archive, const char* buffer, const unsigned long long buffer_size);
	KRA_IMP_API unsigned long long kkra_key_frames_count(kkra_archive_t* archive);
	KRA_IMP_API unsigned char kkra_current_key_frame_read(kkra_archive_t* archive, const unsigned long long key_frame_index);
	KRA_IMP_API const char* kkra_current_key_frame_name(kkra_archive_t* archive);
	KRA_IMP_API unsigned char kkra_layer_data_header_read(const char* buffer, const unsigned long long buffer_size, kkra_layer_data_header_t* layer_data_header);
	KRA_IMP_API unsigned char kkra_layer_data_tile_read(kkra_layer_data_header_t* layer_data_header, const unsigned long long layer_data_tile_index, char* buffer, const unsigned long long buffer_size, int* x_offset, int* y_offset);
	KRA_IMP_API void kkra_delinearize_colors(const char* input, const unsigned long long buffer_size, char* output, const unsigned int x_offset, const unsigned int y_offset, const unsigned int input_width, const unsigned int output_width, const char pixel_size);
#ifdef __cplusplus
}
#endif
