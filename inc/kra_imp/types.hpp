/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#pragma once
#include <kra_imp/config.hpp>

#ifdef __cplusplus
extern "C" {
#endif
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
		const char* _buffer{ nullptr };			/**< A pointer to the raw memory buffer containing the layer's data. */
		unsigned long long _buffer_size{ 0 };	/**< The total size of the memory buffer in bytes. */
		unsigned long long _header_size{ 0 };	/**< The size of the header within the buffer. */
		unsigned int _layer_datas_count{ 0 };	/**< The number of tiles in the layer. */
		char _layer_data_pixel_size{ 0 };		/**< The size of pixels in the data tiles. */
		char _layer_data_width{ 0 };			/**< Width dimension of each data tile in pixels. */
		char _layer_data_height{ 0 };			/**< Height dimension of each data tile in pixels. */
		char _version{ 0 };						/**< Version of the layer data format. */

	};

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
	enum kra_imp_error_code_e
	{
		KRA_IMP_FAIL = 0,						/**< Indicates that the operation failed. */
		KRA_IMP_SUCCESS = 1						/**< Indicates that the operation completed successfully. */

	};

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
	enum kra_imp_layer_type_e
	{
		KRA_IMP_UNKNOWN_LAYER_TYPE = 0,			/**< The layer type is unknown or not recognized. */
		KRA_IMP_GROUP_LAYER_TYPE,				/**< A group layer that can contain other layers, forming a hierarchical structure. */
		KRA_IMP_PAINT_LAYER_TYPE,				/**< A standard paint layer used for raster graphics. */
		KRA_IMP_CLONE_LAYER_TYPE,				/**< A clone layer that mirrors content from another layer. Unsupported. */
		KRA_IMP_FILE_LAYER_TYPE,				/**< A file layer that links to an external file. Unsupported. */
		KRA_IMP_COLORIZEMASK_LAYER_TYPE,		/**< A colorize mask layer used for coloring line art. Unsupported. */
		KRA_IMP_TRANSFORMMASK_LAYER_TYPE,		/**< A transform mask layer used for geometric transformations such as scaling or rotation. Unsupported. */
		KRA_IMP_TRANSPARENCYMASK_LAYER_TYPE		/**< A transparency mask layer that modifies the opacity of its parent layer. Unsupported. */

	};
#ifdef __cplusplus
}
#endif
