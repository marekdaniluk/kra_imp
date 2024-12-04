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
	/**
	 * @brief Gets version of the kra_imp library.
	 *
	 * @details
	 * Versioning uses semantic versioning in the format MAJOR.MINOR.PATCH. Each label uses 1 byte:
	 * - Major version: `(kra_imp_get_version() >> 16) & 0xff`
	 * - Minor version: `(kra_imp_get_version() >> 8) & 0xff`
	 * - Patch version: `kra_imp_get_version() & 0xff`
	 *
	 * @return Combined version as a single shifted integer.
	 */
	KRA_IMP_API unsigned int kra_imp_get_version();
	/**
	 * @brief Opens a KRA archive from a memory buffer.
	 *
	 * @details
	 * The KRA format is a compressed archive containing various files. To read or import data, the archive
	 * must be opened first. This function allocates and initializes the archive.
	 *
	 * @param[in] archive_buffer Pointer to the memory buffer containing the KRA archive data.
	 * @param[in] archive_buffer_size Size of the memory buffer in bytes.
	 *
	 * @return Pointer to the opened KRA archive on success, or nullptr on failure.
	 */
	KRA_IMP_API kra_imp_archive_t* kra_imp_open_archive(const char* archive_buffer, const unsigned long long archive_buffer_size);
	/**
	 * @brief Closes an opened KRA archive and frees allocated memory.
	 *
	 * @param[in] archive Pointer to the archive to close.
	 */
	KRA_IMP_API void kra_imp_close_archive(kra_imp_archive_t* archive);
	/**
	 * @brief Gets the uncompressed size of a file in the archive.
	 *
	 * @details
	 * Retrieves the size of a specified file in the archive.
	 *
	 * @param[in] archive Pointer to the opened archive.
	 * @param[in] file_path Path to the file within the archive's structure.
	 *
	 * @return Size of the file in bytes on success, or 0 on failure.
	 */
	KRA_IMP_API unsigned long long kra_imp_get_file_size(kra_imp_archive_t* archive, const char* file_path);
	/**
	 * @brief Loads a file from the archive into a preallocated buffer.
	 *
	 * @details
	 * Reads the specified file from the archive into a user-provided buffer.
	 *
	 * @param[in] archive Pointer to the opened archive.
	 * @param[in] file_path Path to the file within the archive's structure.
	 * @param[out] file_buffer Buffer to store the file data.
	 * @param[in] file_buffer_size Size of the buffer in bytes.
	 *
	 * @return Number of bytes read on success (should match file size). Returns 0 on failure.
	 */
	KRA_IMP_API unsigned long long kra_imp_load_file(kra_imp_archive_t* archive, const char* file_path, char* file_buffer, const unsigned long long file_buffer_size);
	/**
	 * @brief Gets the name of the main document in the archive.
	 *
	 * @details
	 * For the current KRA format, the main document is expected to have the name "maindoc.xml".
	 *
	 * @return Name of the main document file as a string.
	 */
	KRA_IMP_API const char* kra_imp_get_main_doc_name();
	/**
	 * @brief Reads the main document XML from a buffer.
	 *
	 * @details
	 * Parses the main document XML and caches relevant information in the archive.
	 *
	 * @param[in] archive Pointer to the opened archive.
	 * @param[in] file_buffer Buffer containing the main document XML data.
	 * @param[in] file_buffer_size Size of the buffer in bytes.
	 *
	 * @return KRA_IMP_SUCCESS on success, or KRA_IMP_FAIL on failure.
	 */
	KRA_IMP_API unsigned char kra_imp_read_main_doc(kra_imp_archive_t* archive, const char* file_buffer, const unsigned long long file_buffer_size);
	/**
	 * @brief Gets the internal image name from the main document.
	 *
	 * @details
	 * Requires `kra_imp_read_main_doc` to be called beforehand.
	 *
	 * @param[in] archive Pointer to the opened archive.
	 *
	 * @return Image name as a string on success, or null/empty string on failure.
	 */
	KRA_IMP_API const char* kra_imp_get_image_name(kra_imp_archive_t* archive);
	/**
	 * @brief Gets the color space name of the image.
	 *
	 * @details
	 * Requires `kra_imp_read_main_doc` to be called beforehand.
	 *
	 * @param[in] archive Pointer to the opened archive.
	 *
	 * @return Color space name as a string on success, or null/empty string on failure.
	 */
	KRA_IMP_API const char* kra_imp_get_image_color_space_name(kra_imp_archive_t* archive);
	/**
	 * @brief Gets the height of the image.
	 *
	 * @details
	 * Requires `kra_imp_read_main_doc` to be called beforehand.
	 *
	 * @param[in] archive Pointer to the opened archive.
	 *
	 * @return Image height in pixels on success, or 0 on failure.
	 */
	KRA_IMP_API unsigned long long kra_imp_get_image_height(kra_imp_archive_t* archive);
	/**
	 * @brief Gets the width of the image.
	 *
	 * @details
	 * Requires `kra_imp_read_main_doc` to be called beforehand.
	 *
	 * @param[in] archive Pointer to the opened archive.
	 *
	 * @return Image width in pixels on success, or 0 on failure.
	 */
	KRA_IMP_API unsigned long long kra_imp_get_image_width(kra_imp_archive_t* archive);
	/**
	 * @brief Gets the number of layers in the image.
	 *
	 * @details
	 * Requires `kra_imp_read_main_doc` to be called beforehand.
	 *
	 * @param[in] archive Pointer to the opened archive.
	 *
	 * @return Number of layers in the image on success, or 0 on failure.
	 */
	KRA_IMP_API unsigned long long kra_imp_get_image_layers_count(kra_imp_archive_t* archive);
	/**
	* @brief Reads image layer under layer_index.
	 *
	 * @details
	 * Reads image layer and caches it in the archive under the index. The layers in a KRA file are organized
	 * as a tree structure, where each layer may have a parent layer. This tree structure is flattened,
	 * and the hierarchical relationships can be reconstructed by using the parent indices of the layers.
	 * Requires `kra_imp_read_image_layer` to be called beforehand.
	 *
	 * @param[in] archive Pointer to the opened archive.
	 * @param[in] layer_index Index of the layer to read.
	 *
	 * @return KRA_IMP_SUCCESS if the layer was read and cached successfully, or KRA_IMP_FAIL on failure.
	 */
	KRA_IMP_API unsigned char kra_imp_read_image_layer(kra_imp_archive_t* archive, const unsigned long long layer_index);
	/**
	 * @brief Gets the type of a cached image layer.
	 *
	 * @details
	 * Requires `kra_imp_read_image_layer` to be called beforehand.
	 *
	 * @param[in] archive Pointer to the opened archive.
	 *
	 * @return Layer type as an enumeration value.
	 */
	KRA_IMP_API unsigned char kra_imp_get_image_layer_type(kra_imp_archive_t* archive);
	/**
	 * @brief Gets the parent index of a cached image layer.
	 *
	 * @details
	 * Layer indices are arranged top to bottom in a flattened structure. This function retrieves the parent index
	 * of the layer. Requires `kra_imp_read_image_layer` to be called beforehand.
	 *
	 * @param[in] archive Pointer to the opened archive.
	 *
	 * @return Parent index on success, or ULONG_MAX if the layer has no parent.
	 */
	KRA_IMP_API unsigned long long kra_imp_get_image_layer_parent_index(kra_imp_archive_t* archive);
	/**
	 * @brief Gets the name of a cached image layer.
	 *
	 * @details
	 * Requires `kra_imp_read_image_layer` to be called beforehand.
	 *
	 * @param[in] archive Pointer to the opened archive.
	 *
	 * @return Layer name as a string on success, or null/empty string on failure.
	 */
	KRA_IMP_API const char* kra_imp_get_image_layer_name(kra_imp_archive_t* archive);
	/**
	 * @brief Gets the file name of a cached image layer.
	 *
	 * @details
	 * Requires `kra_imp_read_image_layer` to be called beforehand.
	 *
	 * @param[in] archive Pointer to the opened archive.
	 *
	 * @return Layer file name as a string on success, or null/empty string on failure.
	 */
	KRA_IMP_API const char* kra_imp_get_image_layer_file_name(kra_imp_archive_t* archive);
	/**
	 * @brief Gets the key frame file name of a cached image layer.
	 *
	 * @details
	 * Requires `kra_imp_read_image_layer` to be called beforehand. The presence of a key frame file
	 * name indicates that the layer uses animation.
	 *
	 * @param[in] archive Pointer to the opened archive.
	 *
	 * @return Key frame file name as a string on success, or null/empty string on failure.
	 */
	KRA_IMP_API const char* kra_imp_get_image_layer_key_frame_file_name(kra_imp_archive_t* archive);
	/**
	 * @brief Constructs the file path for a layer file.
	 *
	 * @details
	 * Constructs the full file path for a layer file within an archive using the provided image and file names.
	 * The constructed path may exceed the size of `layer_path_buffer`. In such cases, the function will fail.
	 * Ensure that `layer_path_buffer` is large enough to accommodate the constructed path to avoid errors.
	 *
	 * @param[out] layer_path_buffer Buffer to store the constructed file path.
	 * @param[in] layer_path_buffer_size Size of the buffer in bytes.
	 * @param[in] image_name Name of the image containing the layer.
	 * @param[in] file_name Name of the layer file.
	 *
	 * @return KRA_IMP_SUCCESS on success, or KRA_IMP_FAIL on failure.
	 */
	KRA_IMP_API unsigned char kra_imp_get_layer_file_path(char* layer_path_buffer, const unsigned char layer_path_buffer_size, const char* image_name, const char* file_name);
	/**
	 * @brief Reads all key frames from the provided buffer and caches them in the archive.
	 *
	 * @details
	 * Parses the key frames XML and caches relevant information in the archive.
	 * This function is necessary for accessing key frame-specific data or performing subsequent operations on key frames.
	 *
	 * @param[in] archive Pointer to the opened archive.
	 * @param[in] buffer Memory buffer containing the key frame data.
	 * @param[in] buffer_size Size of the buffer in bytes.
	 *
	 * @return KRA_IMP_SUCCESS if the key frames were successfully read and cached, or KRA_IMP_FAIL on failure.
	 */
	KRA_IMP_API unsigned char kra_imp_read_image_key_frames(kra_imp_archive_t* archive, const char* buffer, const unsigned long long buffer_size);
	/**
	 * @brief Retrieves the total number of key frames for the cached key frames file.
	 *
	 * @details
	 * Returns the number of key frames available, as read and cached in the archive.
	 * Requires `kra_imp_read_image_key_frames` to be called beforehand.
	 *
	 * @param[in] archive Pointer to the opened archive.
	 *
	 * @return The number of key frames on success, or 0 if no key frames are available or on failure.
	 */
	KRA_IMP_API unsigned long long kra_imp_get_image_key_frames_count(kra_imp_archive_t* archive);
	/**
	 * @brief Reads a specific key frame by index and caches its data in the archive.
	 *
	 * @details
	 * Reads a key frame by its index from the archive and caches its data.
	 * Requires `kra_imp_read_image_key_frames` to be called beforehand.
	 *
	 * @param[in] archive Pointer to the opened archive.
	 * @param[in] key_frame_index Index of the key frame to read.
	 *
	 * @return KRA_IMP_SUCCESS if the key frame was successfully read and cached, or KRA_IMP_FAIL on failure.
	 */
	KRA_IMP_API unsigned char kra_imp_read_image_key_frame(kra_imp_archive_t* archive, const unsigned long long key_frame_index);
	/**
	 * @brief Retrieves the name of the currently cached key frame.
	 *
	 * @details
	 * Provides the name of the key frame cached in the archive. Requires `kra_imp_read_image_key_frames` to be called beforehand.
	 *
	 * @param[in] archive Archive containing the cached key frame metadata.
	 *
	 * @return Name of the key frame on success, or null/empty string on failure.
	 */
	KRA_IMP_API const char* kra_imp_get_image_key_frame_name(kra_imp_archive_t* archive);
	/**
	 * @brief Reads and parses the header data of an layer data.
	 *
	 * @details
	 * Parses the header of a layer data from the provided buffer and populates the `layer_data_header` structure.
	 * The header contains data necessary for further processing.
	 *
	 * @param[in] buffer Memory buffer containing the layer data.
	 * @param[in] buffer_size Size of the buffer in bytes.
	 * @param[out] layer_data_header Structure to store the parsed header data.
	 *
	 * @return KRA_IMP_SUCCESS if the header was successfully read and parsed, or KRA_IMP_FAIL on failure.
	 */
	KRA_IMP_API unsigned char kra_imp_read_layer_data_header(const char* buffer, const unsigned long long buffer_size, kra_imp_layer_data_header_t* layer_data_header);
	/**
	 * @brief Reads a tile of layer data.
	 *
	 * @details
	 * Reads and parses a specific tile of layer data, identified by its tile index.
	 * The tile data is stored in the provided buffer, and its offset is also populated.
	 *
	 * @param[in] layer_data_header Header containing data about the layer data.
	 * @param[in] layer_data_tile_index Index of the tile to read.
	 * @param[out] buffer Memory buffer to store the tile data.
	 * @param[in] buffer_size Size of the buffer in bytes.
	 * @param[out] x_offset Horizontal offset of the tile in the overall layer dimensions.
	 * @param[out] y_offset Vertical offset of the tile in the overall layer dimensions.
	 *
	 * @return KRA_IMP_SUCCESS if the tile was successfully read, or KRA_IMP_FAIL on failure.
	 */
	KRA_IMP_API unsigned char kra_imp_read_layer_data_tile(kra_imp_layer_data_header_t* layer_data_header, const unsigned long long layer_data_tile_index, char* buffer, const unsigned long long buffer_size, int* x_offset, int* y_offset);
	/**
	 * @brief Converts a linear color buffer to BGRA format.
	 *
	 * @details
	 * Transforms a linear color representation in the input buffer to BGRA format in the output buffer.
	 * This function supports offsets and different input/output widths for flexible processing.
	 *
	 * @param[in] input Linear color buffer to convert.
	 * @param[in] buffer_size Size of the input buffer in bytes.
	 * @param[out] output Buffer to store the converted BGRA data.
	 * @param[in] x_offset Horizontal offset for the output data.
	 * @param[in] y_offset Vertical offset for the output data.
	 * @param[in] input_width Width of the input data.
	 * @param[in] output_width Width of the output data.
	 * @param[in] pixel_size Size of a single pixel in bytes.
	 */
	KRA_IMP_API void kra_imp_delinearize_to_bgra(const char* input, const unsigned long long buffer_size, char* output, const unsigned int x_offset, const unsigned int y_offset, const unsigned int input_width, const unsigned int output_width, const char pixel_size);
#ifdef __cplusplus
}
#endif
