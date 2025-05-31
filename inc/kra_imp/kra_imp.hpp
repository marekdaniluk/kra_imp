/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#pragma once
#include <kra_imp/config.hpp>
#include <kra_imp/types.hpp>
/**
 * @mainpage
 *
 * Documentation for @ref kra_imp.
 *
 * @defgroup kra_imp kra_imp API
 */
#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * @ingroup kra_imp
     *
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
     * @ingroup kra_imp
     *
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
     * @ingroup kra_imp
     *
     * @brief Closes an opened KRA archive and frees allocated memory.
     *
     * @param[in] archive Pointer to the archive to close.
     */
    KRA_IMP_API void kra_imp_close_archive(kra_imp_archive_t* archive);
    /**
     * @ingroup kra_imp
     *
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
     * @ingroup kra_imp
     *
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
     * @ingroup kra_imp
     *
     * @brief Gets the name of the main document file in the archive.
     *
     * @details
     * For the current KRA format, the main document is expected to have the name `maindoc.xml`.
     *
     * @return Name of the main document file as a string.
     */
    KRA_IMP_API const char* kra_imp_get_main_doc_file_name();
    /**
     * @ingroup kra_imp
     *
     * @brief Retrieves the name of the directory where layer files are stored.
     *
     * @details
     * Returns the name of the directory containing the layer files in the current KRA format.
     * By default, this directory is named "layers." To construct the full file path for a layer,
     * the format follows: `IMAGE_NAME/LAYERS_DIRECTORY_NAME/FILENAME`. This function provides the
     * directory name portion of that path.
     *
     * @return Name of the directory for layer files as a string, typically "layers".
     *
     * @note This function does not validate or construct the full file path. Instead, it only
     * provides the directory name, which must be combined with the image name and filename
     * to create the complete path.
     */
    KRA_IMP_API const char* kra_imp_get_layer_directory_name();
    /**
     * @ingroup kra_imp
     *
     * @brief Reads and parses the main document from the provided XML buffer.
     *
     * @details
     * Extracts and processes the main document (`maindoc.xml`) from the given XML buffer, storing the
     * parsed information in the provided `kra_imp_main_doc_t` structure. The main document contains
     * essential metadata and structure about the image, including its layers, dimensions, and other properties.
     *
     * @param[in] xml_buffer Pointer to the memory buffer containing the XML data.
     * @param[in] xml_buffer_size Size of the XML buffer in bytes.
     * @param[out] main_doc Pointer to the structure where the parsed main document data will be stored.
     *
     * @return KRA_IMP_SUCCESS if the main document was successfully read and parsed, or other `kra_imp_error_code_e` on failure.
     */
    KRA_IMP_API kra_imp_error_code_e kra_imp_read_main_doc(const char* xml_buffer, const unsigned long long xml_buffer_size, kra_imp_main_doc_t* main_doc);
    /**
     * @ingroup kra_imp
     *
     * @brief Reads and parses a specific layer from the provided XML buffer.
     *
     * @details
     * Extracts information about a specific layer in the image by parsing the XML buffer.
     * The layer is identified by its index, and the resulting data is stored in the provided
     * `kra_imp_image_layer_t` structure. This function is used to reconstruct individual layers
     * from the hierarchical layer structure stored in KRA archives.
     *
     * @param[in] xml_buffer Pointer to the memory buffer containing the XML data.
     * @param[in] xml_buffer_size Size of the XML buffer in bytes.
     * @param[in] layer_index Index of the layer to read.
     * @param[out] image_layer Pointer to the structure where the parsed layer data will be stored.
     *
     * @return KRA_IMP_SUCCESS if the layer was successfully read and parsed, or other `kra_imp_error_code_e` on failure.
     */
    KRA_IMP_API kra_imp_error_code_e kra_imp_read_image_layer(const char* xml_buffer, const unsigned long long xml_buffer_size, const unsigned int layer_index,
                                                              kra_imp_image_layer_t* image_layer);
    /**
     * @ingroup kra_imp
     *
     * @brief Retrieves the total number of key frames from the provided XML buffer.
     *
     * @details
     * Parses the XML buffer to determine the total number of key frames in the image's animation timeline.
     * This function is useful for iterating through key frames when working with animation data in KRA archives.
     *
     * @param[in] xml_buffer Pointer to the memory buffer containing the XML data.
     * @param[in] xml_buffer_size Size of the XML buffer in bytes.
     *
     * @return The number of key frames on success, or 0 if no key frames are available or on failure.
     */
    KRA_IMP_API unsigned int kra_imp_get_image_key_frames_count(const char* xml_buffer, const unsigned long long xml_buffer_size);
    /**
     * @ingroup kra_imp
     *
     * @brief Reads a specific key frame from the provided XML buffer.
     *
     * @details
     * Parses and extracts information about a specific key frame from the XML buffer of an image.
     * The key frame is identified by its index, and the resulting data is stored in the provided
     * `kra_imp_image_key_frame_t` structure.
     *
     * @param[in] xml_buffer Pointer to the memory buffer containing the XML data.
     * @param[in] xml_buffer_size Size of the XML buffer in bytes.
     * @param[in] key_frame_index Index of the key frame to read.
     * @param[out] image_key_frame Pointer to the structure where the parsed key frame data will be stored.
     *
     * @return KRA_IMP_SUCCESS if the key frame was successfully read, or other `kra_imp_error_code_e` on failure.
     */
    KRA_IMP_API kra_imp_error_code_e kra_imp_read_image_key_frame(const char* xml_buffer, const unsigned long long xml_buffer_size, const unsigned int key_frame_index,
                                                                  kra_imp_image_key_frame_t* image_key_frame);
    /**
     * @ingroup kra_imp
     *
     * @brief Reads and parses the header data of an layer data.
     *
     * @details
     * Parses the header of a layer data from the provided buffer and populates the `kra_imp_layer_data_header_t` structure.
     * The header contains data necessary for further processing.
     *
     * @param[in] buffer Memory buffer containing the layer data.
     * @param[in] buffer_size Size of the buffer in bytes.
     * @param[out] layer_data_header Structure to store the parsed header data.
     *
     * @return KRA_IMP_SUCCESS if the header was successfully read and parsed, or other `kra_imp_error_code_e` on failure.
     */
    KRA_IMP_API kra_imp_error_code_e kra_imp_read_layer_data_header(const char* buffer, const unsigned long long buffer_size, kra_imp_layer_data_header_t* layer_data_header);
    /**
     * @ingroup kra_imp
     *
     * @brief Reads a tile of layer data.
     *
     * @details
     * Reads and parses a specific tile of layer data, identified by its tile index.
     * The tile data is stored in the provided buffer, and its offset is also populated.
     *
     * @deprecated This function is deprecated. Use `kra_imp_read_layer_data` instead.
     *
     * @param[in] input Pointer to the input buffer containing layer data.
     * @param[in] input_size Size of the input buffer in bytes.
     * @param[in] layer_data_tile_index Index of the tile to read.
     * @param[out] output Memory buffer to store the tile data.
     * @param[in] output_size Size of the output buffer in bytes.
     * @param[out] x_offset Horizontal offset of the tile in the overall layer dimensions.
     * @param[out] y_offset Vertical offset of the tile in the overall layer dimensions.
     *
     * @return KRA_IMP_SUCCESS if the tile was successfully read, or other `kra_imp_error_code_e` on failure.
     */
    KRA_IMP_API kra_imp_error_code_e kra_imp_read_layer_data_tile(const char* input, const unsigned long long input_size, const unsigned int layer_data_tile_index, char* output,
                                                                  const unsigned long long output_size, int* x_offset, int* y_offset);
    /**
     * @ingroup kra_imp
     *
     * @brief Reads and parses layer data.
     *
     * @details
     * This function reads and parses layer data from the provided buffer. It supports reading
     * data for a specific index and populates the provided `kra_imp_layer_output_data_t` structure
     * with the parsed data, including the buffer, offsets, and size.
     *
     * @param[in] buffer Memory buffer containing the layer data.
     * @param[in] buffer_size Size of the buffer in bytes.
     * @param[in] data_index Index of the layer data to read.
     * @param[out] output Pointer to the `kra_imp_layer_output_data_t` structure where the parsed data will be stored.
     *
     * @return KRA_IMP_SUCCESS if the layer data was successfully read, or other `kra_imp_error_code_e` on failure.
     *
     * @note This function is the recommended replacement for `kra_imp_read_layer_data_tile`.
     */
    KRA_IMP_API kra_imp_error_code_e kra_imp_read_layer_data(const char* buffer, const unsigned long long buffer_size, const unsigned int data_index,
                                                             kra_imp_layer_output_data_t* output);
    /**
     * @ingroup kra_imp
     *
     * @brief Converts a linear color buffer with an alpha channel to BGRA format.
     *
     * @details
     * This function transforms a linear color representation in the input buffer to BGRA format in the output buffer.
     * The input and output buffers are expected to have the same dimensions.
     *
     * @param[in] input Linear color buffer to convert.
     * @param[out] output Buffer to store the converted BGRA data.
     * @param[in] buffer_size Size of the input and output buffers in bytes.
     * @param[in] input_width Width of the input and output buffers in pixels.
     *
     * @return KRA_IMP_SUCCESS if the conversion was successful, or other `kra_imp_error_code_e` on failure.
     */
    KRA_IMP_API kra_imp_error_code_e kra_imp_delinearize_to_bgra(const char* input, char* output, const unsigned long long buffer_size, const unsigned int width);
    /**
     * @ingroup kra_imp
     *
     * @brief Converts a linear color buffer with an alpha channel to BGRA format with an offset.
     *
     * @details
     * This function transforms a linear color representation in the input buffer to BGRA format in the output buffer.
     * It supports offsets and different input/output widths for flexible processing while maintaining the original image dimensions.
     *
     * @deprecated This function is deprecated. Use `kra_imp_delinearize_with_offset` instead.
     *
     * @param[in] input Linear color buffer to convert.
     * @param[in] input_size Size of the input buffer in bytes.
     * @param[in] input_width Width of the input data in pixels.
     * @param[out] output Buffer to store the converted BGRA data.
     * @param[in] output_size Size of the output buffer in bytes.
     * @param[in] output_width Width of the output data in pixels.
     * @param[in] output_offset Offset for the output buffer data where to start conversion in bytes.
     *
     * @return KRA_IMP_SUCCESS if the conversion was successful, or other `kra_imp_error_code_e` on failure.
     *
     * @note Ensure that the output buffer is large enough to hold the converted data to avoid buffer overflow.
     * The size of the output buffer should be at least (output_width * height * 4) bytes, where height is derived
     * from input_size / (input_width * 4). 4 stands for pixel size in bytes (BGRA).
     */
    KRA_IMP_API kra_imp_error_code_e kra_imp_delinearize_to_bgra_with_offset(const char* input, const unsigned long long input_size, const unsigned int input_width, char* output,
                                                                             const unsigned long long output_size, const unsigned int output_width,
                                                                             const unsigned long long output_offset);
    /**
     * @ingroup kra_imp
     *
     * @brief Converts a linear color buffer to BGRA output with an offset.
     *
     * @details
     * This function processes a linear color buffer and converts it to a specified output format.
     * It supports offsets and different input/output widths for flexible processing while maintaining
     * the original image dimensions. The output is stored in a `kra_imp_delinerize_output_t` structure.
     *
     * @param[in] input Linear color buffer to convert.
     * @param[in] input_size Size of the input buffer in bytes.
     * @param[in] input_width Width of the input data in pixels.
     * @param[out] output Pointer to the `kra_imp_delinerize_output_t` structure where the converted data will be stored.
     *
     * @return KRA_IMP_SUCCESS if the conversion was successful, or other `kra_imp_error_code_e` on failure.
     *
     * @note Ensure that the `output` structure is properly initialized before calling this function.
     * The size of the output buffer should be at least (output_width * height * 4) bytes, where height is derived
     * from input_size / (input_width * 4). 4 stands for pixel size in bytes (BGRA).
     */
    KRA_IMP_API kra_imp_error_code_e kra_imp_delinearize_with_offset(const char* input, const unsigned long long input_size, const unsigned int input_width,
                                                                     kra_imp_delinerize_output_t* output);
#ifdef __cplusplus
}
#endif
