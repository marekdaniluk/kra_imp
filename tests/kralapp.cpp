/**
 * kraimp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#include <kra_imp/kra_imp.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <cassert>
#include <format>
#include <ranges>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void ReadFile(const char* filename, std::vector<char>& data)
{
	std::ifstream fin(filename, std::ios::binary);
	fin.seekg(0, std::ios_base::end);
	auto length = fin.tellg();
	data.resize(length);
	fin.seekg(0, std::ios_base::beg);
	fin.read(reinterpret_cast<char*>(data.data()), length);
	fin.close();
}

struct STile
{
	int xOffset{ 0 };
	int yOffset{ 0 };
	std::vector<char> tileBuffer;
};

void read_layer(const std::string_view filename, const std::vector<char>& layerBuffer)
{
	int errorCode = 0;
	const unsigned long long layerSize = layerBuffer.size();

	kra_imp_layer_data_header_t header;
	kra_imp_read_layer_data_header(layerBuffer.data(), layerSize, &header);
	
	int Bottom{ 0 };
	int Left{ 0 };
	int Right{ 0 };
	int Top{ 0 };
	const unsigned long long tile_data_size = header._layer_data_width * header._layer_data_height * header._layer_data_pixel_size;
	std::vector<STile> tiles(header._layer_datas_count);
	for (int i = 0; i < header._layer_datas_count; ++i)
	{
		STile& tile = tiles[i];
		tile.tileBuffer.resize(tile_data_size);
		kra_imp_read_layer_data_tile(layerBuffer.data() + header._header_size, layerBuffer.size() - header._header_size, i, tile.tileBuffer.data(), tile_data_size, &tile.xOffset, &tile.yOffset);
		if (i == 0 || Top > tile.yOffset)
		{
			Top = tile.yOffset;
		}
		if (i == 0 || Bottom < (tile.yOffset + header._layer_data_height))
		{
			Bottom = tile.yOffset + header._layer_data_height;
		}
		if (i == 0 || Left > tile.xOffset)
		{
			Left = tile.xOffset;
		}
		if (i == 0 || Right < (tile.xOffset + header._layer_data_width))
		{
			Right = tile.xOffset + header._layer_data_width;
		}
	}

	const unsigned short width = Right - Left;
	const unsigned short height = Bottom - Top;
	const unsigned int layer_data_size = width * height * header._layer_data_pixel_size;
	std::vector<char> mergedLayer(layer_data_size);
	for (const STile& tile : tiles)
	{
		const unsigned int xOffset = tile.xOffset - Left;
		const unsigned int yOffset = tile.yOffset - Top;
		kra_imp_delinearize_to_bgra(tile.tileBuffer.data(), tile_data_size, mergedLayer.data(), xOffset, yOffset, header._layer_data_width, width, header._layer_data_pixel_size);
	}
	//stbi_write_bmp(filename.data(), width, height, 4, mergedLayer.data());
	//const char version = 0;
	//std::ofstream fout(filename.data(), std::ios::binary);
	//fout.write(&version, sizeof(char));
	//fout.write(reinterpret_cast<const char*>(&width), sizeof(unsigned short));
	//fout.write(reinterpret_cast<const char*>(&height), sizeof(unsigned short));
	//fout.write(&header._layer_data_pixel_size, sizeof(char));
	//fout.write(mergedLayer.data(), mergedLayer.size() * sizeof(char));
	//fout.close();
}

static constexpr const std::string_view ARCHIVE_PATH{ "C:\\Users\\marek\\source\\repos\\kra_imp\\test\\example_RGBA.kra" };

int main()
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now(); int errorCode = 0;
	for (int i = 0; i < 200; ++i)
	{
		std::vector<char> zipfile;
		ReadFile(ARCHIVE_PATH.data(), zipfile);
		kra_imp_archive_t* archive = kra_imp_open_archive(zipfile.data(), zipfile.size());
		const unsigned long long maindocSize = kra_imp_get_file_size(archive, kra_imp_get_main_doc_name());
		std::vector<char> maindocBuffer(maindocSize);
		kra_imp_load_file(archive, kra_imp_get_main_doc_name(), maindocBuffer.data(), maindocSize);
		kra_imp_main_doc_t main_doc;
		kra_imp_read_main_doc(maindocBuffer.data(), maindocSize, &main_doc);
		kra_imp_image_layer_t image_layer;
		for (std::size_t j = 0; j < main_doc._layers_count; ++j)
		{
			kra_imp_read_image_layer(maindocBuffer.data(), maindocSize, j, &image_layer);
			if (image_layer._type != KRA_IMP_PAINT_LAYER_TYPE)
			{
				continue;
			}

			if (std::strlen(image_layer._frame_name) == 0)
			{
				const std::string layer_file_path = std::format("{}/{}/{}", main_doc._image_name, kra_imp_get_layer_directory_name(), image_layer._file_name);
				const unsigned long long layerSize = kra_imp_get_file_size(archive, layer_file_path.data());
				std::vector<char> layerBuffer(layerSize);
				kra_imp_load_file(archive, layer_file_path.data(), layerBuffer.data(), layerSize);

				std::string filename = std::format("{}_{}.bmp", main_doc._image_name, image_layer._name);
				read_layer(filename, layerBuffer);
			}
			else
			{
				const std::string frame_file_path = std::format("{}/{}/{}", main_doc._image_name, kra_imp_get_layer_directory_name(), image_layer._frame_name);
				const unsigned long long keyFrameSize = kra_imp_get_file_size(archive, frame_file_path.data());
				std::vector<char> keyFrameBuffer(keyFrameSize);
				kra_imp_load_file(archive, frame_file_path.data(), keyFrameBuffer.data(), keyFrameSize);
				const unsigned long long frames = kra_imp_get_image_key_frames_count(keyFrameBuffer.data(), keyFrameBuffer.size());
				kra_imp_image_key_frame_t image_key_frame;
				for (int k = 0; k < frames; ++k)
				{
					kra_imp_read_image_key_frame(keyFrameBuffer.data(), keyFrameBuffer.size(), k, &image_key_frame);
					const std::string key_frame_file_path = std::format("{}/{}/{}", main_doc._image_name, kra_imp_get_layer_directory_name(), image_key_frame._frame);
					const unsigned long long layerSize = kra_imp_get_file_size(archive, key_frame_file_path.data());
					std::vector<char> layerBuffer(layerSize);
					kra_imp_load_file(archive, key_frame_file_path.data(), layerBuffer.data(), layerSize);

					std::string filename = std::format("{}_{}.bmp", main_doc._image_name, image_key_frame._frame);
					read_layer(filename, layerBuffer);
				}
			}
		}
		kra_imp_close_archive(archive);
	}
	const std::chrono::microseconds duration = duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << std::format("{} us", duration.count()) << std::endl;
}
