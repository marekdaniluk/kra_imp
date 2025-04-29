/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#include "kra_imp/kra_imp.hpp"
#include "lzf/lzf.h"
#include <array>
#include <charconv>
#include <cstring>
#include <functional>
#include <pugixml.hpp>
#include <zip.h>

static constexpr const char* KRA_IMP_MAIN_DOC_FILE_NAME{ "maindoc.xml" };
static constexpr const char* KRA_IMP_LAYERS_DIRECTORY_NAME{ "layers" };
static constexpr const pugi::char_t* KRA_IMP_DOC_IMAGE_NODE{ "DOC/IMAGE" };
static constexpr const pugi::char_t* KRA_IMP_DOC_ANIMATION_NODE{ "DOC/IMAGE/animation" };
static constexpr const pugi::char_t* KRA_IMP_FRAME_RATE_NODE{ "framerate" };
static constexpr const pugi::char_t* KRA_IMP_RANGE_NODE{ "range" };
static constexpr const pugi::char_t* KRA_IMP_VALUE_ATTRIBUTE{ "value" };
static constexpr const pugi::char_t* KRA_IMP_TO_ATTRIBUTE{ "to" };
static constexpr const pugi::char_t* KRA_IMP_FROM_ATTRIBUTE{ "from" };
static constexpr const pugi::char_t* KRA_IMP_NAME_ATTRIBUTE{ "name" };
static constexpr const pugi::char_t* KRA_IMP_OPACITY_ATTRIBUTE{ "opacity" };
static constexpr const pugi::char_t* KRA_IMP_VISIBLE_ATTRIBUTE{ "visible" };
static constexpr const pugi::char_t* KRA_IMP_COLOR_SPACE_NAME_ATTRIBUTE{ "colorspacename" };
static constexpr const pugi::char_t* KRA_IMP_HEIGHT_ATTRIBUTE{ "height" };
static constexpr const pugi::char_t* KRA_IMP_WIDTH_ATTRIBUTE{ "width" };
static constexpr const pugi::char_t* KRA_IMP_FILE_NAME_ATTRIBUTE{ "filename" };
static constexpr const pugi::char_t* KRA_IMP_KEY_FRAMES_ATTRIBUTE{ "keyframes" };
static constexpr const pugi::char_t* KRA_IMP_LAYER_NODES{ "DOC/IMAGE/layers/layer" };
static constexpr const pugi::char_t* KRA_IMP_NODE_TYPE_ATTRIBUTE{ "nodetype" };
static constexpr const pugi::char_t* KRA_IMP_INNER_LAYER_NODES{ "layers/layer" };
static constexpr const pugi::char_t* KRA_IMP_KEY_FRAME_NODES{ "keyframes/channel/keyframe" };
static constexpr const pugi::char_t* KRA_IMP_X_ATTRIBUTE{ "x" };
static constexpr const pugi::char_t* KRA_IMP_Y_ATTRIBUTE{ "y" };
static constexpr const pugi::char_t* KRA_IMP_FRAME_ATTRIBUTE{ "frame" };
static constexpr const pugi::char_t* KRA_IMP_TIME_ATTRIBUTE{ "time" };
static constexpr const pugi::char_t* KRA_IMP_OFFSET_NODE{ "offset" };
static constexpr const char KRA_IMP_EMPTY_CHAR{ '\0' };
static constexpr const char KRA_IMP_END{ '\n' };
static constexpr const char KRA_IMP_UNCOMPRESSED_FLAG{ 0 };
static constexpr const char KRA_IMP_COMPRESSED_FLAG{ 1 };
static constexpr const char KRA_IMP_SEPARATOR{ ',' };
static constexpr const char* KRA_IMP_COMPRESSION_TYPE{ "LZF" };
static constexpr const unsigned char KRA_IMP_MAX_NAME_LENGTH{ 255 };

struct kra_imp_archive_t
{
    zip_t* _archive{ nullptr };
};

kra_imp_layer_type_e to_layer_type(const std::string_view string)
{
    if (string == "grouplayer")
        return KRA_IMP_GROUP_LAYER_TYPE;
    if (string == "paintlayer")
        return KRA_IMP_PAINT_LAYER_TYPE;
    if (string == "cloneLayer")
        return KRA_IMP_CLONE_LAYER_TYPE;
    if (string == "colorizemask")
        return KRA_IMP_COLORIZEMASK_LAYER_TYPE;
    if (string == "filelayer")
        return KRA_IMP_FILE_LAYER_TYPE;
    if (string == "transformmask")
        return KRA_IMP_TRANSFORMMASK_LAYER_TYPE;
    if (string == "transparencymask")
        return KRA_IMP_TRANSPARENCYMASK_LAYER_TYPE;

    return KRA_IMP_UNKNOWN_LAYER_TYPE;
}

kra_imp_color_space_model_e to_color_space_model(const std::string_view string)
{
    if (string.find("LABA") != std::string::npos)
        return KRA_IMP_CIELAB_COLOR_SPACE_MODEL;
    if (string.find("CMYK") != std::string::npos)
        return KRA_IMP_CMYK_COLOR_SPACE_MODEL;
    if (string.find("GRAYA") != std::string::npos)
        return KRA_IMP_GRAYA_COLOR_SPACE_MODEL;
    if (string.find("RGBA") != std::string::npos)
        return KRA_IMP_RGBA_COLOR_SPACE_MODEL;
    if (string.find("XYZA") != std::string::npos)
        return KRA_IMP_XYZA_COLOR_SPACE_MODEL;
    if (string.find("YCBCRA") != std::string::npos)
        return KRA_IMP_YCBCR_COLOR_SPACE_MODEL;

    return KRA_IMP_UNKNOWN_COLOR_SPACE_MODEL;
}

KRA_IMP_API unsigned int kra_imp_get_version()
{
    return KRA_IMP_VERSION;
}

KRA_IMP_API kra_imp_archive_t* kra_imp_open_archive(const char* archive_buffer, const unsigned long long archive_buffer_size)
{
    if (archive_buffer == nullptr || archive_buffer_size == 0ULL)
    {
        return nullptr;
    }

    static constexpr const char READ_MODE{ 'r' };
    zip_t* zip_archive = zip_stream_open(archive_buffer, archive_buffer_size, 0, READ_MODE);
    if (zip_archive == nullptr)
    {
        return nullptr;
    }

    kra_imp_archive_t* archive = new kra_imp_archive_t;
    archive->_archive = zip_archive;
    return archive;
}

KRA_IMP_API void kra_imp_close_archive(kra_imp_archive_t* archive)
{
    if (archive == nullptr)
    {
        return;
    }

    zip_stream_close(archive->_archive);
    delete archive;
}

KRA_IMP_API unsigned long long kra_imp_get_file_size(kra_imp_archive_t* archive, const char* file_path)
{
    if (archive == nullptr || zip_entry_open(archive->_archive, file_path) != 0)
    {
        return 0ULL;
    }

    const unsigned long long entry_size = zip_entry_size(archive->_archive);
    if (zip_entry_close(archive->_archive) != 0)
    {
        return 0ULL;
    }

    return entry_size;
}

KRA_IMP_API unsigned long long kra_imp_load_file(kra_imp_archive_t* archive, const char* file_path, char* file_buffer, const unsigned long long file_buffer_size)
{
    if (archive == nullptr || file_buffer_size == 0ULL || zip_entry_open(archive->_archive, file_path) != 0)
    {
        return 0ULL;
    }

    const unsigned long long read_bytes = zip_entry_noallocread(archive->_archive, file_buffer, file_buffer_size);
    if (zip_entry_close(archive->_archive) != 0)
    {
        return 0ULL;
    }

    return read_bytes;
}

KRA_IMP_API const char* kra_imp_get_main_doc_file_name()
{
    return KRA_IMP_MAIN_DOC_FILE_NAME;
}

KRA_IMP_API const char* kra_imp_get_layer_directory_name()
{
    return KRA_IMP_LAYERS_DIRECTORY_NAME;
}

void iterate_layers_recursive(const pugi::xml_node& xml_node, unsigned int& layers_count)
{
    const pugi::char_t* node_type_attribute = xml_node.attribute(KRA_IMP_NODE_TYPE_ATTRIBUTE).value();
    const char current_layer_type = to_layer_type(std::string_view(node_type_attribute));
    if (current_layer_type == KRA_IMP_GROUP_LAYER_TYPE)
    {
        const pugi::xpath_node_set layer_nodes = xml_node.select_nodes(KRA_IMP_INNER_LAYER_NODES);
        for (pugi::xpath_node_set::const_iterator it = layer_nodes.begin(); it != layer_nodes.end(); ++it)
        {
            iterate_layers_recursive(it->node(), layers_count);
        }
    }
    ++layers_count;
}

void parse_animation(const pugi::xml_node& xml_node, kra_imp_animation_t& animation)
{
    const pugi::xpath_node frame_rate_xnode = xml_node.select_node(KRA_IMP_FRAME_RATE_NODE);
    const pugi::xml_node frame_rate_node = frame_rate_xnode.node();
    animation._frame_rate = frame_rate_node.empty() ? 0U : frame_rate_node.attribute(KRA_IMP_VALUE_ATTRIBUTE).as_uint();
    const pugi::xpath_node range_xnode = xml_node.select_node(KRA_IMP_RANGE_NODE);
    const pugi::xml_node range_node = range_xnode.node();
    animation._from = range_node.empty() ? 0U : range_node.attribute(KRA_IMP_FROM_ATTRIBUTE).as_uint();
    animation._to = range_node.empty() ? 0U : range_node.attribute(KRA_IMP_TO_ATTRIBUTE).as_uint();
}

KRA_IMP_API kra_imp_error_code_e kra_imp_read_main_doc(const char* xml_buffer, const unsigned long long xml_buffer_size, kra_imp_main_doc_t* main_doc)
{
    if (xml_buffer == nullptr || xml_buffer_size == 0ULL || main_doc == nullptr)
    {
        return KRA_IMP_PARAMS_ERROR;
    }

    pugi::xml_document main_doc_xml_document;
    const pugi::xml_parse_result parse_result = main_doc_xml_document.load_buffer(xml_buffer, xml_buffer_size);
    if (!parse_result)
    {
        return KRA_IMP_PARSE_ERROR;
    }

    const pugi::xpath_node image_xnode = main_doc_xml_document.select_node(KRA_IMP_DOC_IMAGE_NODE);
    const pugi::xml_node image_node = image_xnode.node();
    if (image_node.empty())
    {
        return KRA_IMP_FAIL;
    }

    const pugi::xpath_node animation_xnode = main_doc_xml_document.select_node(KRA_IMP_DOC_ANIMATION_NODE);
    parse_animation(animation_xnode.node(), main_doc->_animation);
    std::memset(main_doc->_image_name, KRA_IMP_EMPTY_CHAR, KRA_IMP_MAX_NAME_LENGTH);
    std::strcpy(main_doc->_image_name, image_node.attribute(KRA_IMP_NAME_ATTRIBUTE).value());
    const pugi::char_t* color_space_attribute = image_node.attribute(KRA_IMP_COLOR_SPACE_NAME_ATTRIBUTE).value();
    main_doc->_color_space_model = to_color_space_model(std::string_view(color_space_attribute));
    main_doc->_width = image_node.attribute(KRA_IMP_WIDTH_ATTRIBUTE).as_ullong();
    main_doc->_height = image_node.attribute(KRA_IMP_HEIGHT_ATTRIBUTE).as_ullong();
    const pugi::xpath_node_set layer_nodes = main_doc_xml_document.select_nodes(KRA_IMP_LAYER_NODES);
    main_doc->_layers_count = 0;
    for (pugi::xpath_node_set::const_iterator it = layer_nodes.begin(); it != layer_nodes.end(); ++it)
    {
        iterate_layers_recursive(it->node(), main_doc->_layers_count);
    }
    return KRA_IMP_SUCCESS;
}

bool read_image_layer_recursive(const unsigned int layer_index, kra_imp_image_layer_t* image_layer, const pugi::xml_node& node, long& current_layer_index, const long parent_index)
{
    const pugi::char_t* node_type_attribute = node.attribute(KRA_IMP_NODE_TYPE_ATTRIBUTE).value();
    const kra_imp_layer_type_e current_layer_type = to_layer_type(std::string_view(node_type_attribute));
    if (current_layer_index == layer_index)
    {
        image_layer->_type = current_layer_type;
        image_layer->_opacity = static_cast<unsigned char>(node.attribute(KRA_IMP_OPACITY_ATTRIBUTE).as_uint());
        image_layer->_visibility = static_cast<kra_imp_layer_visibility_e>(node.attribute(KRA_IMP_VISIBLE_ATTRIBUTE).as_int());
        image_layer->_parent_index = parent_index;
        std::strcpy(image_layer->_file_name, node.attribute(KRA_IMP_FILE_NAME_ATTRIBUTE).value());
        std::strcpy(image_layer->_name, node.attribute(KRA_IMP_NAME_ATTRIBUTE).value());
        std::strcpy(image_layer->_frame_file_name, node.attribute(KRA_IMP_KEY_FRAMES_ATTRIBUTE).value());
        return true;
    }

    const unsigned long long current_parent_index = current_layer_index;
    ++current_layer_index;
    if (current_layer_type == KRA_IMP_GROUP_LAYER_TYPE)
    {
        const pugi::xpath_node_set layer_nodes = node.select_nodes(KRA_IMP_INNER_LAYER_NODES);
        for (pugi::xpath_node_set::const_iterator it = layer_nodes.begin(); it != layer_nodes.end(); ++it)
        {
            if (read_image_layer_recursive(layer_index, image_layer, it->node(), current_layer_index, current_parent_index))
            {
                return true;
            }
        }
    }

    return false;
}

KRA_IMP_API kra_imp_error_code_e kra_imp_read_image_layer(const char* xml_buffer, const unsigned long long xml_buffer_size, const unsigned int layer_index,
                                                          kra_imp_image_layer_t* image_layer)
{
    if (xml_buffer == nullptr || xml_buffer_size == 0ULL || image_layer == nullptr)
    {
        return KRA_IMP_PARAMS_ERROR;
    }

    pugi::xml_document main_doc_xml_document;
    const pugi::xml_parse_result parse_result = main_doc_xml_document.load_buffer(xml_buffer, xml_buffer_size);
    if (!parse_result)
    {
        return KRA_IMP_PARSE_ERROR;
    }

    std::memset(image_layer->_file_name, KRA_IMP_EMPTY_CHAR, KRA_IMP_MAX_NAME_LENGTH);
    std::memset(image_layer->_frame_file_name, KRA_IMP_EMPTY_CHAR, KRA_IMP_MAX_NAME_LENGTH);
    std::memset(image_layer->_name, KRA_IMP_EMPTY_CHAR, KRA_IMP_MAX_NAME_LENGTH);

    const pugi::xpath_node node = main_doc_xml_document.select_node(KRA_IMP_DOC_IMAGE_NODE);
    const pugi::xpath_node_set layer_nodes = main_doc_xml_document.select_nodes(KRA_IMP_LAYER_NODES);
    long current_layer_index = 0;
    for (pugi::xpath_node_set::const_iterator it = layer_nodes.begin(); it != layer_nodes.end(); ++it)
    {
        if (read_image_layer_recursive(layer_index, image_layer, it->node(), current_layer_index, -1L))
        {
            return KRA_IMP_SUCCESS;
        }
    }

    return KRA_IMP_FAIL;
}

KRA_IMP_API unsigned int kra_imp_get_image_key_frames_count(const char* xml_buffer, const unsigned long long xml_buffer_size)
{
    if (xml_buffer == nullptr || xml_buffer_size == 0ULL)
    {
        return 0U;
    }

    pugi::xml_document key_frames_xml_document;
    const pugi::xml_parse_result parse_result = key_frames_xml_document.load_buffer(xml_buffer, xml_buffer_size);
    if (!parse_result)
    {
        return 0U;
    }

    const pugi::xpath_node_set key_frame_nodes = key_frames_xml_document.select_nodes(KRA_IMP_KEY_FRAME_NODES);
    return key_frame_nodes.size();
}

KRA_IMP_API kra_imp_error_code_e kra_imp_read_image_key_frame(const char* xml_buffer, const unsigned long long xml_buffer_size, const unsigned int key_frame_index,
                                                              kra_imp_image_key_frame_t* image_key_frame)
{
    if (xml_buffer == nullptr || image_key_frame == nullptr)
    {
        return KRA_IMP_PARAMS_ERROR;
    }

    pugi::xml_document key_frames_xml_document;
    const pugi::xml_parse_result parse_result = key_frames_xml_document.load_buffer(xml_buffer, xml_buffer_size);
    if (!parse_result)
    {
        return KRA_IMP_PARSE_ERROR;
    }

    const pugi::xpath_node_set key_frame_nodes = key_frames_xml_document.select_nodes(KRA_IMP_KEY_FRAME_NODES);
    if (key_frame_nodes.size() <= key_frame_index)
    {
        return KRA_IMP_FAIL;
    }

    pugi::xpath_node_set::const_iterator it = key_frame_nodes.begin() + key_frame_index;
    const pugi::xml_node& xml_node = it->node();
    image_key_frame->_time = xml_node.attribute(KRA_IMP_TIME_ATTRIBUTE).as_uint();
    const pugi::xml_node& offset_node = xml_node.child(KRA_IMP_OFFSET_NODE);
    image_key_frame->_x = offset_node.attribute(KRA_IMP_X_ATTRIBUTE).as_int();
    image_key_frame->_y = offset_node.attribute(KRA_IMP_Y_ATTRIBUTE).as_int();
    std::memset(image_key_frame->_frame, KRA_IMP_EMPTY_CHAR, KRA_IMP_MAX_NAME_LENGTH);
    std::strcpy(image_key_frame->_frame, xml_node.attribute(KRA_IMP_FRAME_ATTRIBUTE).value());
    return KRA_IMP_SUCCESS;
}

std::string_view parse_header_element(const char* buffer, const unsigned long long buffer_size, const std::string_view header_element, unsigned int& buffer_offset)
{
    const std::string_view current_header(buffer + buffer_offset, header_element.size());
    if (header_element.compare(current_header) != 0)
    {
        return {};
    }
    buffer_offset += header_element.size();
    unsigned int end_position = buffer_offset + 1U;
    while (buffer_size > end_position && buffer[end_position] != KRA_IMP_END)
    {
        ++end_position;
    }
    const std::string_view current_hader_value(buffer + buffer_offset, end_position - buffer_offset);
    buffer_offset = end_position + 1U;
    return current_hader_value;
}

bool get_header_element(const char* buffer, const unsigned long long buffer_size, const std::string_view header, unsigned int& buffer_offset, unsigned int& value)
{
    const std::string_view current_hader_value = parse_header_element(buffer, buffer_size, header, buffer_offset);
    if (current_hader_value.empty())
    {
        return false;
    }

    std::size_t position = 0;
    value = std::stoul(current_hader_value.data(), &position);
    return position == current_hader_value.size();
}

KRA_IMP_API kra_imp_error_code_e kra_imp_read_layer_data_header(const char* buffer, const unsigned long long buffer_size, kra_imp_layer_data_header_t* layer_data_header)
{
    if (buffer == nullptr || buffer_size == 0ULL || layer_data_header == nullptr)
    {
        return KRA_IMP_PARAMS_ERROR;
    }

    static constexpr const std::array<std::string_view, 5> KRA_IMP_HEADERS{ "VERSION ", "TILEWIDTH ", "TILEHEIGHT ", "PIXELSIZE ", "DATA " };

    layer_data_header->_header_size = 0U;
    if (!get_header_element(buffer, buffer_size, KRA_IMP_HEADERS[0], layer_data_header->_header_size, layer_data_header->_version) ||
        !get_header_element(buffer, buffer_size, KRA_IMP_HEADERS[1], layer_data_header->_header_size, layer_data_header->_layer_data_width) ||
        !get_header_element(buffer, buffer_size, KRA_IMP_HEADERS[2], layer_data_header->_header_size, layer_data_header->_layer_data_height) ||
        !get_header_element(buffer, buffer_size, KRA_IMP_HEADERS[3], layer_data_header->_header_size, layer_data_header->_layer_data_pixel_size) ||
        !get_header_element(buffer, buffer_size, KRA_IMP_HEADERS[4], layer_data_header->_header_size, layer_data_header->_layer_datas_count) ||
        layer_data_header->_header_size > buffer_size)
    {
        return KRA_IMP_PARSE_ERROR;
    }

    return KRA_IMP_SUCCESS;
}

KRA_IMP_API kra_imp_error_code_e kra_imp_read_layer_data_tile(const char* input, const unsigned long long input_size, unsigned int layer_data_tile_index, char* output,
                                                              const unsigned long long output_size, int* x_offset, int* y_offset)
{
    if (input == nullptr || input_size == 0ULL || output == nullptr || output_size == 0ULL || x_offset == nullptr || y_offset == nullptr)
    {
        return KRA_IMP_PARAMS_ERROR;
    }

    unsigned long long start_position = 0UL;
    unsigned long long end_position = 1UL;

    auto separator_splitter = [input, input_size](unsigned long long& end_position)
    {
        while (input_size > end_position)
        {
            if (input[end_position] == KRA_IMP_SEPARATOR || input[end_position] == KRA_IMP_END)
            {
                return true;
            }
            ++end_position;
        }

        return false;
    };
    unsigned int current_index = 0U;
    while (end_position < input_size)
    {
        if (!separator_splitter(end_position))
        {
            return KRA_IMP_PARSE_ERROR;
        }

        if (std::from_chars(&input[start_position], &input[end_position], *x_offset).ec != std::errc())
        {
            return KRA_IMP_PARSE_ERROR;
        }

        start_position = end_position + 1UL;
        end_position = start_position + 1UL;
        if (!separator_splitter(end_position))
        {
            return KRA_IMP_PARSE_ERROR;
        }

        if (std::from_chars(&input[start_position], &input[end_position], *y_offset).ec != std::errc())
        {
            return KRA_IMP_PARSE_ERROR;
        }

        start_position = end_position + 1UL;
        end_position = start_position + 1UL;
        if (!separator_splitter(end_position))
        {
            return KRA_IMP_PARSE_ERROR;
        }

        const std::string_view tile_compression_type(&input[start_position], end_position - start_position);
        if (tile_compression_type.compare(KRA_IMP_COMPRESSION_TYPE) != 0)
        {
            return KRA_IMP_PARSE_ERROR;
        }

        start_position = end_position + 1UL;
        end_position = start_position + 1UL;
        if (!separator_splitter(end_position))
        {
            return KRA_IMP_PARSE_ERROR;
        }

        unsigned int compressed_size = 0U;
        if (std::from_chars(&input[start_position], &input[end_position], compressed_size).ec != std::errc())
        {
            return KRA_IMP_PARSE_ERROR;
        }

        start_position = end_position + 1UL;
        end_position = start_position + 1UL;
        if (current_index == layer_data_tile_index)
        {
            if (KRA_IMP_UNCOMPRESSED_FLAG == input[start_position])
            {
                std::memcpy(output, &input[start_position + 1UL], output_size);
            }
            else if (KRA_IMP_COMPRESSED_FLAG == input[start_position])
            {
                if (lzf_decompress(&input[start_position + 1UL], compressed_size - 1U, output, output_size) != output_size)
                {
                    return KRA_IMP_DECOMPRESS_ERROR;
                }
            }
            else
            {
                return KRA_IMP_DECOMPRESS_ERROR;
            }

            return KRA_IMP_SUCCESS;
        }
        start_position += compressed_size;
        end_position = start_position + 1UL;
        ++current_index;
    }
    return KRA_IMP_FAIL;
}

KRA_IMP_API kra_imp_error_code_e kra_imp_delinearize_to_bgra(const char* input, char* output, const unsigned long long buffer_size, const unsigned int width)
{
    return kra_imp_delinearize_to_bgra_with_offset(input, buffer_size, width, output, buffer_size, width, 0ULL);
}

KRA_IMP_API kra_imp_error_code_e kra_imp_delinearize_to_bgra_with_offset(const char* input, const unsigned long long input_size, const unsigned int input_width, char* output,
                                                                         const unsigned long long output_size, const unsigned int output_width,
                                                                         const unsigned long long output_offset)
{
    if (input == nullptr || input_size == 0ULL || output == nullptr || output_size == 0ULL || output_width < input_width || (output_size - output_offset) < input_size)
    {
        return KRA_IMP_PARAMS_ERROR;
    }

    static constexpr unsigned char pixel_size = 4;
    const unsigned long long input_rows = input_size / (pixel_size * input_width);
    const unsigned long long pixels_to_delineearize = input_size / pixel_size;
    unsigned long long output_idx = output_offset;
    for (unsigned long long y = 0UL; y < input_rows; ++y)
    {
        for (unsigned long long x = 0UL; x < input_width; ++x)
        {
            unsigned long long input_idx = y * input_width + x;
            for (unsigned char channel_index = 0; channel_index < pixel_size; ++channel_index)
            {
                unsigned long long output_channel_idx = output_idx + (x * pixel_size) + channel_index;
                unsigned long long input_channel_idx = channel_index * pixels_to_delineearize + input_idx;
                output[output_channel_idx] = input[input_channel_idx];
            }
        }
        output_idx += output_width * pixel_size;
    }

    return KRA_IMP_SUCCESS;
}
