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

	kkra_layer_data_header_t header;
	kkra_layer_data_header_read(layerBuffer.data(), layerSize, &header);
	
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
		kkra_layer_data_tile_read(&header, i, tile.tileBuffer.data(), tile_data_size, &tile.xOffset, &tile.yOffset);
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
		kkra_delinearize_colors(tile.tileBuffer.data(), tile_data_size, mergedLayer.data(), xOffset, yOffset, header._layer_data_width, width, header._layer_data_pixel_size);
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

static constexpr const std::string_view ARCHIVE_PATH{ "C:\\Users\\marek\\source\\repos\\kral\\examples\\example_RGBA.kra" };

int main()
{
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now(); int errorCode = 0;
	//for (int i = 0; i < 200; ++i)
	{
		std::vector<char> zipfile;
		ReadFile(ARCHIVE_PATH.data(), zipfile);
		kkra_archive_t* kra_archive = kkra_open_archive(zipfile.data(), zipfile.size());
		{
			const unsigned long long maindocSize = kkra_file_size(kra_archive, kkra_main_doc_name());
			std::vector<char> maindocBuffer(maindocSize);
			kkra_file_load(kra_archive, kkra_main_doc_name(), maindocBuffer.data(), maindocSize);
			kkra_main_doc_read(kra_archive, maindocBuffer.data(), maindocSize);
		}
		const unsigned long long layersCount = kkra_layers_count(kra_archive);
		for (std::size_t j = 0; j < layersCount; ++j)
		{
			kkra_layer_read(kra_archive, j);
			if (kkra_current_layer_type(kra_archive) != KRA_PAINT_LAYER_TYPE)
			{
				continue;
			}
			const char* frame_name = kkra_current_layer_key_frame_file_name(kra_archive);
			if (std::strlen(frame_name) == 0)
			{
				std::vector<char> layerPath(255);
				kkra_layer_file_path(layerPath.data(), layerPath.size(), kkra_image_name(kra_archive), kkra_current_layer_file_name(kra_archive));
				const unsigned long long layerSize = kkra_file_size(kra_archive, layerPath.data());
				std::vector<char> layerBuffer(layerSize);
				kkra_file_load(kra_archive, layerPath.data(), layerBuffer.data(), layerSize);

				std::string filename = std::format("{}_{}.bmp", kkra_image_name(kra_archive), kkra_current_layer_name(kra_archive));
				read_layer(filename, layerBuffer);
			}
			else
			{
				std::vector<char> framePath(255);
				kkra_layer_file_path(framePath.data(), framePath.size(), kkra_image_name(kra_archive), frame_name);
				const unsigned long long keyFrameSize = kkra_file_size(kra_archive, framePath.data());
				std::vector<char> keyFrameBuffer(keyFrameSize);
				kkra_file_load(kra_archive, framePath.data(), keyFrameBuffer.data(), keyFrameSize);
				kkra_key_frames_read(kra_archive, keyFrameBuffer.data(), keyFrameSize);
				const unsigned long long frames = kkra_key_frames_count(kra_archive);
				for (int k = 0; k < frames; ++k)
				{
					kkra_current_key_frame_read(kra_archive, k);
					kkra_layer_file_path(framePath.data(), framePath.size(), kkra_image_name(kra_archive), kkra_current_key_frame_name(kra_archive));
					const unsigned long long layerSize = kkra_file_size(kra_archive, framePath.data());
					std::vector<char> layerBuffer(layerSize);
					kkra_file_load(kra_archive, framePath.data(), layerBuffer.data(), layerSize);

					std::string filename = std::format("{}_{}.bmp", kkra_image_name(kra_archive), kkra_current_key_frame_name(kra_archive));
					read_layer(filename, layerBuffer);
				}
			}
		}
		kkra_close_archive(kra_archive);
	}
	const std::chrono::microseconds duration = duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << std::format("{} us", duration.count()) << std::endl;
}
