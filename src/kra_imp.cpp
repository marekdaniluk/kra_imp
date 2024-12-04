/**
 * kral - kra file import library
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

static constexpr const char* KRA_MAIN_DOC_FILE_NAME{ "maindoc.xml" };
static constexpr const pugi::char_t* KRA_DOC_IMAGE_NODE{ "DOC/IMAGE" };
static constexpr const pugi::char_t* KRA_NAME_ATTRIBUTE{ "name" };
static constexpr const pugi::char_t* KRA_COLOR_SPACE_NAME_ATTRIBUTE{ "colorspacename" };
static constexpr const pugi::char_t* KRA_HEIGHT_ATTRIBUTE{ "height" };
static constexpr const pugi::char_t* KRA_WIDTH_ATTRIBUTE{ "width" };
static constexpr const pugi::char_t* KRA_FILENAME_ATTRIBUTE{ "filename" };
static constexpr const pugi::char_t* KRA_KEY_FRAMES_ATTRIBUTE{ "keyframes" };
static constexpr const pugi::char_t* KRA_LAYER_NODES{ "DOC/IMAGE/layers/layer" };
static constexpr const pugi::char_t* KRA_NODE_TYPE_ATTRIBUTE{ "nodetype" };
static constexpr const pugi::char_t* KRA_INNER_LAYER_NODES{ "layers/layer" };
static constexpr const pugi::char_t* KRA_KEY_FRAME_NODES{ "keyframes/channel/keyframe" };
static constexpr const pugi::char_t* KRA_X_ATTRIBUTE{ "x" };
static constexpr const pugi::char_t* KRA_Y_ATTRIBUTE{ "y" };
static constexpr const pugi::char_t* KRA_FRAME_ATTRIBUTE{ "frame" };
static constexpr const pugi::char_t* KRA_TIME_ATTRIBUTE{ "time" };
static constexpr const pugi::char_t* KRA_OFFSET_NODE{ "offset" };
static constexpr const char KRA_EMPTY_CHAR{ '\0' };
static constexpr const char KRA_END{ '\n' };
static constexpr const char KRA_COMPRESSION_FLAG{ 1 };
static constexpr const char KRA_SEPARATOR{ ',' };
static constexpr const char* KRA_LAYERS_FILEPATH{ "/layers/" };
static constexpr const char* KRA_COMPRESSION_TYPE{ "LZF" };
static constexpr const unsigned char KRA_MAX_NAME_LENGTH{ 255 };
static constexpr const unsigned char KRA_LAYERS_FILEPATH_LENGTH{ 8 };

struct kkra_main_doc_t
{
	pugi::xml_document _xml{};
	char _image_name[KRA_MAX_NAME_LENGTH]{};
	char _color_space[KRA_MAX_NAME_LENGTH]{};
	unsigned long long _layers_count{ 0 };
	unsigned long long _height{ 0 };
	unsigned long long _width{ 0 };

};

struct kkra_key_frame_t
{
	int _x{ 0 };
	int _y{ 0 };
	unsigned int _time{ 0 };
	char _frame[KRA_MAX_NAME_LENGTH]{};
	unsigned long long _index{ ULLONG_MAX };
	unsigned long long _key_frames_count{ 0 };
	pugi::xml_document _xml{};

};

struct kkra_layer_t
{
	char _name[KRA_MAX_NAME_LENGTH]{};
	char _filename[KRA_MAX_NAME_LENGTH]{};
	char _frame_name[KRA_MAX_NAME_LENGTH]{};
	char _type{ -1 };
	unsigned long long _index{ ULLONG_MAX };
	unsigned long long _parent_index{ ULLONG_MAX };

};

struct kkra_archive_t
{
	zip_t* _archive{ nullptr };
	kkra_main_doc_t _main_doc{};
	kkra_layer_t _current_layer{};
	kkra_key_frame_t _current_key_frame{};

};

unsigned char kkra_to_layer_type(const std::string_view string)
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

	for (char i = 0; i < NODE_STRINGS.size(); ++i)
	{
		if (NODE_STRINGS[i].compare(string) == 0)
		{
			return i + 1;
		}
	}

	return KRA_UNKNOWN_LAYER_TYPE;
}

KRA_IMP_API kkra_archive_t* kkra_open_archive(const char* archive_data, const unsigned long long archive_data_size)
{
	if (archive_data == nullptr)
	{
		return nullptr;
	}

	static constexpr const char READ_MODE{ 'r' };
	zip_t* archive = zip_stream_open(archive_data, archive_data_size, 0, READ_MODE);
	if (archive == nullptr)
	{
		return nullptr;
	}

	kkra_archive_t* kra_archive = new kkra_archive_t;
	kra_archive->_archive = archive;
	return kra_archive;
}

KRA_IMP_API void kkra_close_archive(kkra_archive_t* archive)
{
	if (archive == nullptr)
	{
		return;
	}

	zip_stream_close(archive->_archive);
	delete archive;
}

KRA_IMP_API unsigned long long kkra_file_size(kkra_archive_t* archive, const char* file_name)
{
	if (archive == nullptr || zip_entry_open(archive->_archive, file_name) != 0)
	{
		return KRA_FAIL;
	}

	const unsigned long long entry_size = zip_entry_size(archive->_archive);
	if (zip_entry_close(archive->_archive) != 0)
	{
		return KRA_FAIL;
	}

	return entry_size;
}

KRA_IMP_API unsigned long long kkra_file_load(kkra_archive_t* archive, const char* file_name, char* buffer, const unsigned long long buffer_size)
{
	if (archive == nullptr || zip_entry_open(archive->_archive, file_name) != 0)
	{
		return KRA_FAIL;
	}

	const unsigned long long read_bytes = zip_entry_noallocread(archive->_archive, buffer, buffer_size);
	if (zip_entry_close(archive->_archive) != 0)
	{
		return KRA_FAIL;
	}

	return read_bytes;
}

KRA_IMP_API const char* kkra_main_doc_name()
{
	return KRA_MAIN_DOC_FILE_NAME;
}

KRA_IMP_API unsigned char kkra_main_doc_read(kkra_archive_t* archive, const char* buffer, const unsigned long long buffer_size)
{
	if (archive == nullptr)
	{
		return KRA_FAIL;
	}

	const pugi::xml_parse_result parse_result = archive->_main_doc._xml.load_buffer(buffer, buffer_size);
	if (!parse_result)
	{
		return KRA_FAIL;
	}
	
	const pugi::xpath_node node = archive->_main_doc._xml.select_node(KRA_DOC_IMAGE_NODE);
	std::memset(archive->_main_doc._image_name, KRA_EMPTY_CHAR, KRA_MAX_NAME_LENGTH);
	std::strcpy(archive->_main_doc._image_name, node.node().attribute(KRA_NAME_ATTRIBUTE).value());
	std::memset(archive->_main_doc._color_space, KRA_EMPTY_CHAR, KRA_MAX_NAME_LENGTH);
	std::strcpy(archive->_main_doc._color_space, node.node().attribute(KRA_COLOR_SPACE_NAME_ATTRIBUTE).value());
	archive->_main_doc._width = node.node().attribute(KRA_WIDTH_ATTRIBUTE).as_ullong();
	archive->_main_doc._height = node.node().attribute(KRA_HEIGHT_ATTRIBUTE).as_ullong();
	
	const pugi::xpath_node_set layer_nodes = archive->_main_doc._xml.select_nodes(KRA_LAYER_NODES);
	archive->_main_doc._layers_count = 0;
	std::function<void(const pugi::xml_node&, unsigned long long&)> layers_counter_recursive;
	layers_counter_recursive = [&layers_counter_recursive](const pugi::xml_node& xml_node, unsigned long long& layers_count)
	{
		const pugi::char_t* node_type_attribute = xml_node.attribute(KRA_NODE_TYPE_ATTRIBUTE).value();
		const char current_layer_type = kkra_to_layer_type(std::string_view(node_type_attribute));
		if (current_layer_type == KRA_GROUP_LAYER_TYPE)
		{
			const pugi::xpath_node_set layer_nodes = xml_node.select_nodes(KRA_INNER_LAYER_NODES);
			for (pugi::xpath_node_set::const_iterator it = layer_nodes.begin(); it != layer_nodes.end(); ++it)
			{
				layers_counter_recursive(it->node(), layers_count);
			}
		}
		++layers_count;
	};
	for (pugi::xpath_node_set::const_iterator it = layer_nodes.begin(); it != layer_nodes.end(); ++it)
	{
		layers_counter_recursive(it->node(), archive->_main_doc._layers_count);
	}

	return KRA_SUCCESS;
}

KRA_IMP_API const char* kkra_image_name(kkra_archive_t* archive)
{
	if (archive == nullptr)
	{
		return nullptr;
	}

	return archive->_main_doc._image_name;
}

KRA_IMP_API const char* kkra_color_space(kkra_archive_t* archive)
{
	if (archive == nullptr)
	{
		return nullptr;
	}

	return archive->_main_doc._color_space;
}

KRA_IMP_API unsigned long long kkra_image_height(kkra_archive_t* archive)
{
	if (archive == nullptr)
	{
		return KRA_FAIL;
	}

	return archive->_main_doc._height;
}

KRA_IMP_API unsigned long long kkra_image_width(kkra_archive_t* archive)
{
	if (archive == nullptr)
	{
		return KRA_FAIL;
	}

	return archive->_main_doc._width;
}

KRA_IMP_API unsigned long long kkra_layers_count(kkra_archive_t* archive)
{
	if (archive == nullptr)
	{
		return KRA_FAIL;
	}

	return archive->_main_doc._layers_count;
}

KRA_IMP_API unsigned char kkra_layer_read(kkra_archive_t* archive, const unsigned long long layer_index)
{
	if (archive == nullptr)
	{
		return KRA_FAIL;
	}

	archive->_current_layer._index = ULLONG_MAX;
	archive->_current_layer._type = KRA_UNKNOWN_LAYER_TYPE;
	std::memset(archive->_current_layer._frame_name, KRA_EMPTY_CHAR, KRA_MAX_NAME_LENGTH);
	std::memset(archive->_current_layer._name, KRA_EMPTY_CHAR, KRA_MAX_NAME_LENGTH);
	const pugi::xpath_node_set layer_nodes = archive->_main_doc._xml.select_nodes(KRA_LAYER_NODES);
	unsigned long long layers_count = 0;
	std::function<bool(const pugi::xml_node&, unsigned long long&, const unsigned long long, const unsigned long long)> layers_traverser_recursive;
	layers_traverser_recursive = [&layers_traverser_recursive, archive](const pugi::xml_node& xml_node, unsigned long long& layers_count, const unsigned long long layer_index, const unsigned long long parent_index)
	{
		const pugi::char_t* node_type_attribute = xml_node.attribute(KRA_NODE_TYPE_ATTRIBUTE).value();
		const char current_layer_type = kkra_to_layer_type(std::string_view(node_type_attribute));
		if (layers_count == layer_index)
		{
			archive->_current_layer._index = layer_index;
			archive->_current_layer._type = current_layer_type;
			archive->_current_layer._parent_index = parent_index;
			std::strcpy(archive->_current_layer._filename, xml_node.attribute(KRA_FILENAME_ATTRIBUTE).value());
			std::strcpy(archive->_current_layer._name, xml_node.attribute(KRA_NAME_ATTRIBUTE).value());
			std::strcpy(archive->_current_layer._frame_name, xml_node.attribute(KRA_KEY_FRAMES_ATTRIBUTE).value());
			return true;
		}

		const unsigned long long parent_idx = layers_count;
		++layers_count;
		if (current_layer_type == KRA_GROUP_LAYER_TYPE)
		{
			const pugi::xpath_node_set layer_nodes = xml_node.select_nodes(KRA_INNER_LAYER_NODES);
			for (pugi::xpath_node_set::const_iterator it = layer_nodes.begin(); it != layer_nodes.end(); ++it)
			{
				if (layers_traverser_recursive(it->node(), layers_count, layer_index, parent_idx))
				{
					return true;
				}
			}
		}
		return false;
	};
	for (pugi::xpath_node_set::const_iterator it = layer_nodes.begin(); it != layer_nodes.end(); ++it)
	{
		if (layers_traverser_recursive(it->node(), layers_count, layer_index, ULONG_MAX))
		{
			return KRA_SUCCESS;
		}
	}

	return KRA_FAIL;
}

KRA_IMP_API unsigned char kkra_current_layer_type(kkra_archive_t* archive)
{
	if (archive == nullptr)
	{
		return KRA_UNKNOWN_LAYER_TYPE;
	}

	return archive->_current_layer._type;
}

KRA_IMP_API unsigned long long kkra_current_layer_parent_index(kkra_archive_t* archive)
{
	if (archive == nullptr)
	{
		return ULONG_MAX;
	}

	return archive->_current_layer._parent_index;
}

KRA_IMP_API const char* kkra_current_layer_name(kkra_archive_t* archive)
{
	if (archive == nullptr)
	{
		return nullptr;
	}

	return archive->_current_layer._name;
}

KRA_IMP_API const char* kkra_current_layer_file_name(kkra_archive_t* archive)
{
	if (archive == nullptr)
	{
		return nullptr;
	}

	return archive->_current_layer._filename;
}

KRA_IMP_API const char* kkra_current_layer_key_frame_file_name(kkra_archive_t* archive)
{
	if (archive == nullptr)
	{
		return nullptr;
	}

	return archive->_current_layer._frame_name;
}

KRA_IMP_API unsigned char kkra_layer_file_path(char* layer_path_buffer, const unsigned char layer_path_buffer_size, const char* image_name, const char* file_name)
{
	const unsigned long long image_name_length = std::strlen(image_name);
	const unsigned long long file_name_length = std::strlen(file_name);
	if (layer_path_buffer_size < (image_name_length + file_name_length + KRA_LAYERS_FILEPATH_LENGTH))
	{
		return KRA_FAIL;
	}

	std::memset(layer_path_buffer, KRA_EMPTY_CHAR, layer_path_buffer_size);
	std::strcpy(layer_path_buffer, image_name);
	std::strcpy(layer_path_buffer + image_name_length, KRA_LAYERS_FILEPATH);
	std::strcpy(layer_path_buffer + image_name_length + KRA_LAYERS_FILEPATH_LENGTH, file_name);
	return KRA_SUCCESS;
}

KRA_IMP_API unsigned char kkra_key_frames_read(kkra_archive_t* archive, const char* buffer, const unsigned long long buffer_size)
{
	if (archive == nullptr)
	{
		return KRA_FAIL;
	}

	const pugi::xml_parse_result parse_result = archive->_current_key_frame._xml.load_buffer(buffer, buffer_size);
	if (!parse_result)
	{
		return KRA_FAIL;
	}

	const pugi::xpath_node_set key_frame_nodes = archive->_current_key_frame._xml.select_nodes(KRA_KEY_FRAME_NODES);
	archive->_current_key_frame._key_frames_count = key_frame_nodes.size();
	return KRA_SUCCESS;
}

KRA_IMP_API unsigned long long kkra_key_frames_count(kkra_archive_t* archive)
{
	if (archive == nullptr)
	{
		return KRA_FAIL;
	}

	return archive->_current_key_frame._key_frames_count;
}

KRA_IMP_API unsigned char kkra_current_key_frame_read(kkra_archive_t* archive, const unsigned long long key_frame_index)
{
	if (archive == nullptr)
	{
		return KRA_FAIL;
	}

	const pugi::xpath_node_set key_frame_nodes = archive->_current_key_frame._xml.select_nodes(KRA_KEY_FRAME_NODES);
	if (key_frame_nodes.size() <= key_frame_index)
	{
		return KRA_FAIL;
	}

	pugi::xpath_node_set::const_iterator it = key_frame_nodes.begin() + key_frame_index;
	const pugi::xml_node& xml_node = it->node();
	archive->_current_key_frame._index = key_frame_index;
	archive->_current_key_frame._time = xml_node.attribute(KRA_TIME_ATTRIBUTE).as_uint();
	const pugi::xml_node& offset_node = xml_node.child(KRA_OFFSET_NODE);
	archive->_current_key_frame._x = offset_node.attribute(KRA_X_ATTRIBUTE).as_int();
	archive->_current_key_frame._y = offset_node.attribute(KRA_Y_ATTRIBUTE).as_int();
	std::memset(archive->_current_key_frame._frame, KRA_EMPTY_CHAR, KRA_MAX_NAME_LENGTH);
	std::strcpy(archive->_current_key_frame._frame, xml_node.attribute(KRA_FRAME_ATTRIBUTE).value());
	return KRA_SUCCESS;
}

KRA_IMP_API const char* kkra_current_key_frame_name(kkra_archive_t* archive)
{
	if (archive == nullptr)
	{
		return nullptr;
	}

	return archive->_current_key_frame._frame;
}

KRA_IMP_API unsigned char kkra_layer_data_header_read(const char* buffer, const unsigned long long buffer_size, kkra_layer_data_header_t* layer_data_header)
{
	static constexpr const std::array<std::string_view, 5> KRA_HEADER
	{
		"VERSION ",
		"TILEWIDTH ",
		"TILEHEIGHT ",
		"PIXELSIZE ",
		"DATA "
	};

	layer_data_header->_buffer = buffer;
	layer_data_header->_buffer_size = buffer_size;
	for (unsigned int i = 0; i < KRA_HEADER.size(); ++i)
	{
		const std::string_view current_header(buffer + layer_data_header->_header_size, KRA_HEADER[i].size());
		if (KRA_HEADER[i].compare(current_header) != 0)
		{
			return KRA_FAIL;
		}

		layer_data_header->_header_size += KRA_HEADER[i].size();
		int end_position = layer_data_header->_header_size + 1;
		while (buffer_size > end_position && buffer[end_position] != KRA_END)
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
				return KRA_FAIL;
			}
			break;
		}
		case 1:
		{
			if (std::from_chars(current_hader_value.data(), current_hader_value.data() + current_hader_value.size(), layer_data_header->_layer_data_width).ec != std::errc())
			{
				return KRA_FAIL;
			}
			break;
		}
		case 2:
		{
			if (std::from_chars(current_hader_value.data(), current_hader_value.data() + current_hader_value.size(), layer_data_header->_layer_data_height).ec != std::errc())
			{
				return KRA_FAIL;
			}
			break;
		}
		case 3:
		{
			if (std::from_chars(current_hader_value.data(), current_hader_value.data() + current_hader_value.size(), layer_data_header->_layer_data_pixel_size).ec != std::errc())
			{
				return KRA_FAIL;
			}
			break;
		}
		case 4:
		{
			if (std::from_chars(current_hader_value.data(), current_hader_value.data() + current_hader_value.size(), layer_data_header->_layer_datas_count).ec != std::errc())
			{
				return KRA_FAIL;
			}
			break;
		}
		default:
		{
			return KRA_FAIL;
		}
		}
		layer_data_header->_header_size = end_position + 1;
	}
	return KRA_SUCCESS;
}

KRA_IMP_API unsigned char kkra_layer_data_tile_read(kkra_layer_data_header_t* layer_data_header, const unsigned long long layer_data_tile_index, char* buffer, const unsigned long long buffer_size, int* x_offset, int* y_offset)
{
	unsigned long long start_position = layer_data_header->_header_size;
	unsigned long long end_position = start_position + 1;

	auto separator_splitter = [layer_data_header](unsigned long long* end_position)
	{
		while(layer_data_header->_buffer_size > *end_position)
		{
			if (layer_data_header->_buffer[*end_position] == KRA_SEPARATOR || layer_data_header->_buffer[*end_position] == KRA_END)
			{
				return true;
			}
			++(*end_position);
		}

		return false;
	};
	for (unsigned long long i = 0; i < layer_data_header->_layer_datas_count; ++i)
	{
		if (!separator_splitter(&end_position))
		{
			return KRA_FAIL;
		}
		
		const std::string_view tile_x_offset(&layer_data_header->_buffer[start_position], end_position - start_position);
		if (std::from_chars(tile_x_offset.data(), tile_x_offset.data() + tile_x_offset.size(), *x_offset).ec != std::errc())
		{
			return KRA_FAIL;
		}

		start_position = end_position + 1;
		end_position = start_position + 1;
		if (!separator_splitter(&end_position))
		{
			return KRA_FAIL;
		}

		const std::string_view tile_y_offset(&layer_data_header->_buffer[start_position], end_position - start_position);
		if (std::from_chars(tile_y_offset.data(), tile_y_offset.data() + tile_y_offset.size(), *y_offset).ec != std::errc())
		{
			return KRA_FAIL;
		}

		start_position = end_position + 1;
		end_position = start_position + 1;
		if (!separator_splitter(&end_position))
		{
			return KRA_FAIL;
		}

		const std::string_view tile_compression_type(&layer_data_header->_buffer[start_position], end_position - start_position);
		if (tile_compression_type.compare(KRA_COMPRESSION_TYPE) != 0)
		{
			return KRA_FAIL;
		}

		start_position = end_position + 1;
		end_position = start_position + 1;
		if (!separator_splitter(&end_position))
		{
			return KRA_FAIL;
		}

		const std::string_view tile_compressed_size(&layer_data_header->_buffer[start_position], end_position - start_position);
		unsigned int compressed_size = 0;
		if (std::from_chars(tile_compressed_size.data(), tile_compressed_size.data() + tile_compressed_size.size(), compressed_size).ec != std::errc())
		{
			return KRA_FAIL;
		}

		start_position = end_position + 1;
		end_position = start_position + 1;
		if (i == layer_data_tile_index)
		{
			if (KRA_COMPRESSION_FLAG == layer_data_header->_buffer[start_position])
			{
				if (lzf_decompress(&layer_data_header->_buffer[start_position + 1], compressed_size - 1, buffer, buffer_size) != buffer_size)
				{
					return KRA_FAIL;
				}
			}
			else
			{
				std::memcpy(buffer, &layer_data_header->_buffer[start_position + 1], buffer_size);
			}

			return KRA_SUCCESS;
		}
		start_position += compressed_size;
		end_position = start_position + 1;
	}
	return KRA_FAIL;
}

KRA_IMP_API void kkra_delinearize_colors(const char* input, const unsigned long long buffer_size, char* output, const unsigned int x_offset, const unsigned int y_offset, const unsigned int input_width, const unsigned int output_width, const char pixel_size)
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
