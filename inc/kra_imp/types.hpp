/**
 * kraimp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#pragma once
#include <kra_imp/config.hpp>

#ifdef __cplusplus
extern "C" {
#endif
	struct KRA_IMP_API kkra_archive_t;

	struct KRA_IMP_API kkra_layer_data_header_t
	{
		const char* _buffer{ nullptr };
		unsigned long long _buffer_size{ 0 };
		unsigned long long _header_size{ 0 };
		unsigned int _layer_datas_count{ 0 };
		char _layer_data_pixel_size{ 0 };
		char _layer_data_width{ 0 };
		char _layer_data_height{ 0 };
		char _version{ 0 };

	};

	//struct KRA_IMP_API kkra_layer_data_t
	//{
	//	char* _buffer{ nullptr };
	//	unsigned long long _buffer_size{ 0 };
	//	int _x_offset{ 0 };
	//	int _y_offset{ 0 };
	//
	//};

	enum kkra_error_code_e
	{
		KRA_FAIL = 0,
		KRA_SUCCESS = 1

	};

	enum kkra_layer_type_e
	{
		KRA_UNKNOWN_LAYER_TYPE = 0,
		KRA_GROUP_LAYER_TYPE,
		KRA_PAINT_LAYER_TYPE,
		KRA_CLONE_LAYER_TYPE,
		KRA_FILE_LAYER_TYPE,
		KRA_COLORIZEMASK_LAYER_TYPE,
		KRA_TRANSFORMMASK_LAYER_TYPE,
		KRA_TRANSPARENCYMASK_LAYER_TYPE

	};
#ifdef __cplusplus
}
#endif
