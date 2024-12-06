/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#pragma once
#include <kra_imp/config.hpp>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif'
	/**
	 * @brief Defines the maximum length for strings in the kra_imp.
	 *
	 * @details
	 * This macro specifies the maximum number of characters (including the null terminator)
	 * that can be stored in string fields used throughout the kra_imp.
	 */
	#define KRA_IMP_MAX_STRING_LENGTH 255
	/**
	 * @ingroup kra_imp
	 *
	 * @brief Defines error codes used throughout the kra_imp API.
	 *
	 * @details
	 * This enumeration provides standard return codes for functions in the kra_imp API,
	 * indicating the success or failure of an operation.
	 *
	 * @note These error codes are commonly used as return values in API functions
	 * for straightforward status checking.
	 */
	typedef enum kra_imp_error_code_e
	{
		KRA_IMP_FAIL = 0,									/**< Indicates that the operation failed. */
		KRA_IMP_SUCCESS = 1									/**< Indicates that the operation completed successfully. */

	} kra_imp_error_code_e;
	/**
	 * @ingroup kra_imp
	 *
	 * @brief Enumerates the types of layers available in a KRA archive.
	 *
	 * @details
	 * This enumeration defines the various types of layers that can exist in a KRA archive.
	 * Each layer type represents a specific function or behavior, and these types are
	 * used to identify and handle layers appropriately during the import process.
	 *
	 * @note The layer type is determined when reading layer data, and it helps in
	 * reconstructing the layer's behavior and structure during processing.
	 */
	typedef enum kra_imp_layer_type_e
	{
		KRA_IMP_UNKNOWN_LAYER_TYPE = 0,						/**< The layer type is unknown or not recognized. */
		KRA_IMP_GROUP_LAYER_TYPE,							/**< A group layer that can contain other layers, forming a hierarchical structure. */
		KRA_IMP_PAINT_LAYER_TYPE,							/**< A standard paint layer used for raster graphics. */
		KRA_IMP_CLONE_LAYER_TYPE,							/**< A clone layer that mirrors content from another layer. Unsupported. */
		KRA_IMP_FILE_LAYER_TYPE,							/**< A file layer that links to an external file. Unsupported. */
		KRA_IMP_COLORIZEMASK_LAYER_TYPE,					/**< A colorize mask layer used for coloring line art. Unsupported. */
		KRA_IMP_TRANSFORMMASK_LAYER_TYPE,					/**< A transform mask layer used for geometric transformations such as scaling or rotation. Unsupported. */
		KRA_IMP_TRANSPARENCYMASK_LAYER_TYPE					/**< A transparency mask layer that modifies the opacity of its parent layer. Unsupported. */

	} kra_imp_layer_type_e;
	/**
	 * @struct kra_imp_archive_t
	 * 
	 * @brief Represents an opened KRA archive for reading and processing.
	 * 
	 * @details
	 * This structure encapsulates the data of an opened KRA archive,
	 * providing the context required for various operations, such as accessing
	 * files, layers, key frames, and image properties.
	 * 
	 * The `kra_imp_archive_t` is managed internally by the API and must be created
	 * using `kra_imp_open_archive`. Once created, it serves as the primary handle
	 * for interacting with the archive's content. It should be properly closed using
	 * `kra_imp_close_archive` to release allocated resources.
	 * 
	 * @note The structure's internal implementation is opaque to the user and is
	 * fully managed by the API.
	 */
	struct KRA_IMP_API kra_imp_archive_t;
	typedef struct kra_imp_archive_t kra_imp_archive_t;
	/**
	 * @struct kra_imp_main_doc_t
	 * 
	 * @brief Represents the main document metadata of a KRA image.
	 *
	 * @details
	 * This structure contains the primary metadata extracted from the main document of a KRA image.
	 */
	struct KRA_IMP_API kra_imp_main_doc_t
	{
		char _image_name[KRA_IMP_MAX_STRING_LENGTH];		/**< The name of the image as specified in the main document. */
		char _color_space[KRA_IMP_MAX_STRING_LENGTH];		/**< The color space used by the image (e.g., RGB, CMYK). */
		unsigned long long _layers_count;					/**< The total number of layers in the image. */
		unsigned long long _height;							/**< The height of the image in pixels. */
		unsigned long long _width;							/**< The width of the image in pixels. */

	};
	typedef struct kra_imp_main_doc_t kra_imp_main_doc_t;
	/**
	 * @struct kra_imp_image_layer_t
	 * 
	 * @brief Represents a single layer in a KRA image.
	 *
	 * @details
	 * This structure stores information about an individual layer in a KRA image. Layers are part of a
	 * tree structure that can be reconstructed using the parent index information.
	 */
	struct KRA_IMP_API kra_imp_image_layer_t
	{
		char _name[KRA_IMP_MAX_STRING_LENGTH];				/**< The name of the layer. */
		char _file_name[KRA_IMP_MAX_STRING_LENGTH];			/**< The file name associated with the layer's content. */
		char _frame_name[KRA_IMP_MAX_STRING_LENGTH];		/**< The file name of the keyframe associated with the layer, if applicable. */
		kra_imp_layer_type_e _type;							/**< The type of the layer, defined by `kra_imp_layer_type_e`. */
		unsigned long long _index;							/**< The index of the layer in the flattened layer structure. Defaults to `ULLONG_MAX` if undefined. */
		unsigned long long _parent_index;					/**< The index of the parent layer, or `ULLONG_MAX` if the layer has no parent. */

	};
	typedef struct kra_imp_image_layer_t kra_imp_image_layer_t;
	/**
	 * @struct kra_imp_image_key_frame_t
	 * 
	 * @brief Represents an individual keyframe in a KRA image animation.
	 *
	 * @details
	 * This structure stores information about a single keyframe in an animated image within a KRA archive.
	 */
	struct KRA_IMP_API kra_imp_image_key_frame_t
	{
		int _x;												/**< Horizontal position of the keyframe in the image's coordinate space. */
		int _y;												/**< Vertical position of the keyframe in the image's coordinate space. */
		unsigned int _time;									/**< Time of the keyframe in the animation timeline, in milliseconds. */
		char _frame[KRA_IMP_MAX_STRING_LENGTH];				/**< Name of the keyframe. */
		unsigned long long _index;							/**< Index of the keyframe in the animation sequence. Defaults to `ULLONG_MAX` if undefined. */
		unsigned long long _key_frames_count;				/**< Total number of keyframes in the animation. */

	};
	typedef struct kra_imp_image_key_frame_t kra_imp_image_key_frame_t;
	/**
	 * @struct kra_imp_layer_data_header_t
	 * 
	 * @brief Represents data about a layer's tiles in a KRA archive.
	 * 
	 * @details
	 * This structure holds essential information about a layer's data tiles,
	 * including its size, dimensions, and versioning. It is used to manage and
	 * interpret tile-based data efficiently when reading layer data from a KRA archive.
	 * 
	 * @note The internal fields should not be modified by the user directly.
	 * They are managed by the API to ensure correct data interpretation.
	 */
	struct KRA_IMP_API kra_imp_layer_data_header_t
	{
		unsigned long long _header_size;					/**< The size of the header within the buffer. */
		unsigned int _layer_datas_count;					/**< The number of tiles in the layer. */
		char _layer_data_pixel_size;						/**< The size of pixels in the data tiles. */
		char _layer_data_width;								/**< Width dimension of each data tile in pixels. */
		char _layer_data_height;							/**< Height dimension of each data tile in pixels. */
		char _version;										/**< Version of the layer data format. */

	};
	typedef struct kra_imp_layer_data_header_t kra_imp_layer_data_header_t;
#ifdef __cplusplus
}
#endif
