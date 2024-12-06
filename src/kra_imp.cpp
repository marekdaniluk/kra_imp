/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#include <kra_imp/kra_imp.hpp>
#include "lzf/lzf.h"
#include <zip.h>
#include <pugixml.hpp>
#include <array>
#include <charconv>
#include <cstring>
#include <functional>
#include <string_view>

static constexpr const char* KRA_IMP_MAIN_DOC_FILE_NAME{ "maindoc.xml" };
static constexpr const char* KRA_IMP_LAYERS_DIRECTORY_NAME{ "layers" };
static constexpr const pugi::char_t* KRA_IMP_DOC_IMAGE_NODE{ "DOC/IMAGE" };
static constexpr const pugi::char_t* KRA_IMP_NAME_ATTRIBUTE{ "name" };
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
static constexpr const char KRA_IMP_COMPRESSION_FLAG{ 1 };
static constexpr const char KRA_IMP_SEPARATOR{ ',' };
static constexpr const char* KRA_IMP_COMPRESSION_TYPE{ "LZF" };
static constexpr const unsigned char KRA_IMP_MAX_NAME_LENGTH{ 255 };

struct kra_imp_archive_t
{
	zip_t* _archive{ nullptr };

};

kra_imp_layer_type_e kra_imp_to_layer_type(const std::string_view string)
{
	static constexpr const std::array<std::string_view, 7> NODE_STRINGS
	{
		"grouplayer",
		"paintlayer",
		"cloneLayer",
		"colorizemask",
		"filelayer",
		"transformmask",
		"transparencymask"
	};

	for (unsigned char i = 0; i < NODE_STRINGS.size(); ++i)
	{
		if (NODE_STRINGS[i].compare(string) == 0)
		{
			return static_cast<kra_imp_layer_type_e>(i + 1);
		}
	}

	return KRA_IMP_UNKNOWN_LAYER_TYPE;
}

KRA_IMP_API unsigned int kra_imp_get_version()
{
	return KRA_IMP_VERSION;
}

KRA_IMP_API kra_imp_archive_t* kra_imp_open_archive(const char* archive_buffer, const unsigned long long archive_buffer_size)
{
	if (archive_buffer == nullptr || archive_buffer_size == 0)
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
		return KRA_IMP_FAIL;
	}

	const unsigned long long entry_size = zip_entry_size(archive->_archive);
	if (zip_entry_close(archive->_archive) != 0)
	{
		return KRA_IMP_FAIL;
	}

	return entry_size;
}

KRA_IMP_API unsigned long long kra_imp_load_file(kra_imp_archive_t* archive, const char* file_path, char* file_buffer, const unsigned long long file_buffer_size)
{
	if (archive == nullptr || zip_entry_open(archive->_archive, file_path) != 0)
	{
		return KRA_IMP_FAIL;
	}

	const unsigned long long read_bytes = zip_entry_noallocread(archive->_archive, file_buffer, file_buffer_size);
	if (zip_entry_close(archive->_archive) != 0)
	{
		return KRA_IMP_FAIL;
	}

	return read_bytes;
}

KRA_IMP_API const char* kra_imp_get_main_doc_name()
{
	return KRA_IMP_MAIN_DOC_FILE_NAME;
}

KRA_IMP_API const char* kra_imp_get_layer_directory_name()
{
	return KRA_IMP_LAYERS_DIRECTORY_NAME;
}

KRA_IMP_API unsigned char kra_imp_read_main_doc(const char* xml_buffer, const unsigned long long xml_buffer_size, kra_imp_main_doc_t* main_doc)
{
	if (xml_buffer == nullptr || main_doc == nullptr)
	{
		return KRA_IMP_FAIL;
	}

	pugi::xml_document main_doc_xml_document;
	const pugi::xml_parse_result parse_result = main_doc_xml_document.load_buffer(xml_buffer, xml_buffer_size);
	if (!parse_result)
	{
		return KRA_IMP_FAIL;
	}

	const pugi::xpath_node node = main_doc_xml_document.select_node(KRA_IMP_DOC_IMAGE_NODE);
	std::memset(main_doc->_image_name, KRA_IMP_EMPTY_CHAR, KRA_IMP_MAX_NAME_LENGTH);
	std::strcpy(main_doc->_image_name, node.node().attribute(KRA_IMP_NAME_ATTRIBUTE).value());
	std::memset(main_doc->_color_space, KRA_IMP_EMPTY_CHAR, KRA_IMP_MAX_NAME_LENGTH);
	std::strcpy(main_doc->_color_space, node.node().attribute(KRA_IMP_COLOR_SPACE_NAME_ATTRIBUTE).value());
	main_doc->_width = node.node().attribute(KRA_IMP_WIDTH_ATTRIBUTE).as_ullong();
	main_doc->_height = node.node().attribute(KRA_IMP_HEIGHT_ATTRIBUTE).as_ullong();

	const pugi::xpath_node_set layer_nodes = main_doc_xml_document.select_nodes(KRA_IMP_LAYER_NODES);
	main_doc->_layers_count = 0;
	std::function<void(const pugi::xml_node&, unsigned long long&)> layers_counter_recursive;
	layers_counter_recursive = [&layers_counter_recursive](const pugi::xml_node& xml_node, unsigned long long& layers_count)
	{
		const pugi::char_t* node_type_attribute = xml_node.attribute(KRA_IMP_NODE_TYPE_ATTRIBUTE).value();
		const char current_layer_type = kra_imp_to_layer_type(std::string_view(node_type_attribute));
		if (current_layer_type == KRA_IMP_GROUP_LAYER_TYPE)
		{
			const pugi::xpath_node_set layer_nodes = xml_node.select_nodes(KRA_IMP_INNER_LAYER_NODES);
			for (pugi::xpath_node_set::const_iterator it = layer_nodes.begin(); it != layer_nodes.end(); ++it)
			{
				layers_counter_recursive(it->node(), layers_count);
			}
		}
		++layers_count;
	};
	for (pugi::xpath_node_set::const_iterator it = layer_nodes.begin(); it != layer_nodes.end(); ++it)
	{
		layers_counter_recursive(it->node(), main_doc->_layers_count);
	}

	return KRA_IMP_SUCCESS;
}

KRA_IMP_API unsigned char kra_imp_read_image_layer(const char* xml_buffer, const unsigned long long  xml_buffer_size, const unsigned long long layer_index, kra_imp_image_layer_t* image_layer)
{
	if (xml_buffer == nullptr || image_layer == nullptr)
	{
		return KRA_IMP_FAIL;
	}

	pugi::xml_document main_doc_xml_document;
	const pugi::xml_parse_result parse_result = main_doc_xml_document.load_buffer(xml_buffer, xml_buffer_size);
	if (!parse_result)
	{
		return KRA_IMP_FAIL;
	}

	std::memset(image_layer->_file_name, KRA_IMP_EMPTY_CHAR, KRA_IMP_MAX_NAME_LENGTH);
	std::memset(image_layer->_frame_name, KRA_IMP_EMPTY_CHAR, KRA_IMP_MAX_NAME_LENGTH);
	std::memset(image_layer->_name, KRA_IMP_EMPTY_CHAR, KRA_IMP_MAX_NAME_LENGTH);

	const pugi::xpath_node node = main_doc_xml_document.select_node(KRA_IMP_DOC_IMAGE_NODE);
	const pugi::xpath_node_set layer_nodes = main_doc_xml_document.select_nodes(KRA_IMP_LAYER_NODES);
	std::function<void(const pugi::xml_node&, unsigned long long&, const unsigned long long)> layers_counter_recursive;
	layers_counter_recursive = [&layers_counter_recursive, layer_index, image_layer](const pugi::xml_node& xml_node, unsigned long long& current_layer_index, const unsigned long long parent_index)
	{
		const pugi::char_t* node_type_attribute = xml_node.attribute(KRA_IMP_NODE_TYPE_ATTRIBUTE).value();
		kra_imp_layer_type_e current_layer_type = kra_imp_to_layer_type(std::string_view(node_type_attribute));
		if (current_layer_index == layer_index)
		{
			image_layer->_index = layer_index;
			image_layer->_type = current_layer_type;
			image_layer->_parent_index = parent_index;
			std::strcpy(image_layer->_file_name, xml_node.attribute(KRA_IMP_FILE_NAME_ATTRIBUTE).value());
			std::strcpy(image_layer->_name, xml_node.attribute(KRA_IMP_NAME_ATTRIBUTE).value());
			std::strcpy(image_layer->_frame_name, xml_node.attribute(KRA_IMP_KEY_FRAMES_ATTRIBUTE).value());
			return true;
		}

		const unsigned long long current_parent_index = current_layer_index;
		++current_layer_index;
		if (current_layer_type == KRA_IMP_GROUP_LAYER_TYPE)
		{
			const pugi::xpath_node_set layer_nodes = xml_node.select_nodes(KRA_IMP_INNER_LAYER_NODES);
			for (pugi::xpath_node_set::const_iterator it = layer_nodes.begin(); it != layer_nodes.end(); ++it)
			{
				layers_counter_recursive(it->node(), current_layer_index, current_parent_index);
			}
		}

		return false;
	};

	unsigned long long current_layer_index = 0;
	for (pugi::xpath_node_set::const_iterator it = layer_nodes.begin(); it != layer_nodes.end(); ++it)
	{
		layers_counter_recursive(it->node(), current_layer_index, ULONG_MAX);
	}

	return KRA_IMP_SUCCESS;
}

KRA_IMP_API unsigned long long kra_imp_get_image_key_frames_count(const char* xml_buffer, const unsigned long long xml_buffer_size)
{
	if (xml_buffer == nullptr)
	{
		return KRA_IMP_FAIL;
	}

	pugi::xml_document key_frames_xml_document;
	const pugi::xml_parse_result parse_result = key_frames_xml_document.load_buffer(xml_buffer, xml_buffer_size);
	if (!parse_result)
	{
		return KRA_IMP_FAIL;
	}

	const pugi::xpath_node_set key_frame_nodes = key_frames_xml_document.select_nodes(KRA_IMP_KEY_FRAME_NODES);
	return key_frame_nodes.size();
}

KRA_IMP_API unsigned char kra_imp_read_image_key_frame(const char* xml_buffer, const unsigned long long xml_buffer_size, const unsigned long long key_frame_index, kra_imp_image_key_frame_t* image_key_frame)
{
	if (xml_buffer == nullptr || image_key_frame == nullptr)
	{
		return KRA_IMP_FAIL;
	}

	pugi::xml_document key_frames_xml_document;
	const pugi::xml_parse_result parse_result = key_frames_xml_document.load_buffer(xml_buffer, xml_buffer_size);
	if (!parse_result)
	{
		return KRA_IMP_FAIL;
	}

	const pugi::xpath_node_set key_frame_nodes = key_frames_xml_document.select_nodes(KRA_IMP_KEY_FRAME_NODES);
	if (key_frame_nodes.size() <= key_frame_index)
	{
		return KRA_IMP_FAIL;
	}

	pugi::xpath_node_set::const_iterator it = key_frame_nodes.begin() + key_frame_index;
	const pugi::xml_node& xml_node = it->node();
	image_key_frame->_index = key_frame_index;
	image_key_frame->_time = xml_node.attribute(KRA_IMP_TIME_ATTRIBUTE).as_uint();
	const pugi::xml_node& offset_node = xml_node.child(KRA_IMP_OFFSET_NODE);
	image_key_frame->_x = offset_node.attribute(KRA_IMP_X_ATTRIBUTE).as_int();
	image_key_frame->_y = offset_node.attribute(KRA_IMP_Y_ATTRIBUTE).as_int();
	std::memset(image_key_frame->_frame, KRA_IMP_EMPTY_CHAR, KRA_IMP_MAX_NAME_LENGTH);
	std::strcpy(image_key_frame->_frame, xml_node.attribute(KRA_IMP_FRAME_ATTRIBUTE).value());
	return KRA_IMP_SUCCESS;
}

KRA_IMP_API unsigned char kra_imp_read_layer_data_header(const char* buffer, const unsigned long long buffer_size, kra_imp_layer_data_header_t* layer_data_header)
{
	static constexpr const std::array<std::string_view, 5> KRA_IMP_HEADER
	{
		"VERSION ",
		"TILEWIDTH ",
		"TILEHEIGHT ",
		"PIXELSIZE ",
		"DATA "
	};

	for (unsigned int i = 0; i < KRA_IMP_HEADER.size(); ++i)
	{
		const std::string_view current_header(buffer + layer_data_header->_header_size, KRA_IMP_HEADER[i].size());
		if (KRA_IMP_HEADER[i].compare(current_header) != 0)
		{
			return KRA_IMP_FAIL;
		}

		layer_data_header->_header_size += KRA_IMP_HEADER[i].size();
		int end_position = layer_data_header->_header_size + 1;
		while (buffer_size > end_position && buffer[end_position] != KRA_IMP_END)
		{
			++end_position;
		}
		const std::string_view current_hader_value(buffer + layer_data_header->_header_size, end_position - layer_data_header->_header_size);
		switch (i)
		{
		case 0:
		{
			if (std::from_chars(current_hader_value.data(), current_hader_value.data() + current_hader_value.size(), layer_data_header->_version).ec != std::errc())
			{
				return KRA_IMP_FAIL;
			}
			break;
		}
		case 1:
		{
			if (std::from_chars(current_hader_value.data(), current_hader_value.data() + current_hader_value.size(), layer_data_header->_layer_data_width).ec != std::errc())
			{
				return KRA_IMP_FAIL;
			}
			break;
		}
		case 2:
		{
			if (std::from_chars(current_hader_value.data(), current_hader_value.data() + current_hader_value.size(), layer_data_header->_layer_data_height).ec != std::errc())
			{
				return KRA_IMP_FAIL;
			}
			break;
		}
		case 3:
		{
			if (std::from_chars(current_hader_value.data(), current_hader_value.data() + current_hader_value.size(), layer_data_header->_layer_data_pixel_size).ec != std::errc())
			{
				return KRA_IMP_FAIL;
			}
			break;
		}
		case 4:
		{
			if (std::from_chars(current_hader_value.data(), current_hader_value.data() + current_hader_value.size(), layer_data_header->_layer_datas_count).ec != std::errc())
			{
				return KRA_IMP_FAIL;
			}
			break;
		}
		default:
		{
			return KRA_IMP_FAIL;
		}
		}
		layer_data_header->_header_size = end_position + 1;
	}
	return KRA_IMP_SUCCESS;
}

KRA_IMP_API unsigned char kra_imp_read_layer_data_tile(const char* input, const unsigned long long input_size, const unsigned long long layer_data_tile_index, char* output, const unsigned long long output_size, int* x_offset, int* y_offset)
{
	unsigned long long start_position = 0;//layer_data_header->_header_size;
	unsigned long long end_position = start_position + 1;

	auto separator_splitter = [input, input_size](unsigned long long* end_position)
	{
		while(input_size > *end_position)
		{
			if (input[*end_position] == KRA_IMP_SEPARATOR || input[*end_position] == KRA_IMP_END)
			{
				return true;
			}
			++(*end_position);
		}

		return false;
	};
	for (unsigned long long i = 0; i < 0/*layer_data_header->_layer_datas_count*/; ++i)
	{
		if (!separator_splitter(&end_position))
		{
			return KRA_IMP_FAIL;
		}
		
		const std::string_view tile_x_offset(&input[start_position], end_position - start_position);
		if (std::from_chars(tile_x_offset.data(), tile_x_offset.data() + tile_x_offset.size(), *x_offset).ec != std::errc())
		{
			return KRA_IMP_FAIL;
		}

		start_position = end_position + 1;
		end_position = start_position + 1;
		if (!separator_splitter(&end_position))
		{
			return KRA_IMP_FAIL;
		}

		const std::string_view tile_y_offset(&input[start_position], end_position - start_position);
		if (std::from_chars(tile_y_offset.data(), tile_y_offset.data() + tile_y_offset.size(), *y_offset).ec != std::errc())
		{
			return KRA_IMP_FAIL;
		}

		start_position = end_position + 1;
		end_position = start_position + 1;
		if (!separator_splitter(&end_position))
		{
			return KRA_IMP_FAIL;
		}

		const std::string_view tile_compression_type(&input[start_position], end_position - start_position);
		if (tile_compression_type.compare(KRA_IMP_COMPRESSION_TYPE) != 0)
		{
			return KRA_IMP_FAIL;
		}

		start_position = end_position + 1;
		end_position = start_position + 1;
		if (!separator_splitter(&end_position))
		{
			return KRA_IMP_FAIL;
		}

		const std::string_view tile_compressed_size(&input[start_position], end_position - start_position);
		unsigned int compressed_size = 0;
		if (std::from_chars(tile_compressed_size.data(), tile_compressed_size.data() + tile_compressed_size.size(), compressed_size).ec != std::errc())
		{
			return KRA_IMP_FAIL;
		}

		start_position = end_position + 1;
		end_position = start_position + 1;
		if (i == layer_data_tile_index)
		{
			if (KRA_IMP_COMPRESSION_FLAG == input[start_position])
			{
				if (lzf_decompress(&input[start_position + 1], compressed_size - 1, output, output_size) != output_size)
				{
					return KRA_IMP_FAIL;
				}
			}
			else
			{
				std::memcpy(output, &input[start_position + 1], output_size);
			}

			return KRA_IMP_SUCCESS;
		}
		start_position += compressed_size;
		end_position = start_position + 1;
	}
	return KRA_IMP_FAIL;
}

KRA_IMP_API void kra_imp_delinearize_to_bgra(const char* input, const unsigned long long buffer_size, char* output, const unsigned int x_offset, const unsigned int y_offset, const unsigned int input_width, const unsigned int output_width, const char pixel_size)
{
	const unsigned int input_rows = buffer_size / (pixel_size * input_width);
	const unsigned int pixels = buffer_size / pixel_size;
	unsigned int output_idx = y_offset * output_width * pixel_size + x_offset * pixel_size;
	for (unsigned int y = 0; y < input_rows; ++y)
	{
		for (unsigned int x = 0; x < input_width; ++x)
		{
			unsigned int input_idx = y * input_width + x;
			for (unsigned int channel_index = 0; channel_index < pixel_size; ++channel_index)
			{
				unsigned int output_channel_idx = output_idx + (x * pixel_size) + channel_index;
				unsigned int input_channel_idx = channel_index * pixels + input_idx;
				output[output_channel_idx] = input[input_channel_idx];
			}
		}
		output_idx += output_width * pixel_size;
	}
}
