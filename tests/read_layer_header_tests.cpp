/**
 * kraimp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <kra_imp/kra_imp.hpp>

constexpr const std::string_view EMPTY_LAYER_DATA = "";
constexpr const std::string_view ONLY_HEADER_LAYER_DATA = "VERSION 2\nTILEWIDTH 64\nTILEHEIGHT 64\nPIXELSIZE 4\nDATA 0\n";
constexpr const std::string_view MISSING_EOL_LAYER_DATA = "VERSION 2\nTILEWIDTH 64\nTILEHEIGHT 64\nPIXELSIZE 4\nDATA 0";
constexpr const std::string_view MISSING_EOLS_LAYER_DATA = "VERSION 2 TILEWIDTH 64 TILEHEIGHT 64 PIXELSIZE 4 DATA 0\n";
constexpr const std::string_view MISSPELLED_LAYER_DATA = "VERSION 2\nTILEWIDTH 64\nTILEHEIGHT 64\nPIEXLSIZE 4\nDATA 0\n";
constexpr const std::string_view LOWERCASE_LAYER_DATA = "version 2\ntilewidth 64\ntileheight 64\npiexlsize 4\ndata 0\n";
constexpr const std::string_view MISSING_VERSION_HEADER_LAYER_DATA = "TILEWIDTH 64\nTILEHEIGHT 64\nPIXELSIZE 4\nDATA 0\n";
constexpr const std::string_view INVALID_VALUE_LAYER_DATA = "VERSION 2.2\nTILEWIDTH 64\nTILEHEIGHT 64\nPIXELSIZE 4\nDATA 0\n";

TEST_CASE("kra_imp_read_layer_data_header success", "[layer_data_header]")
{
    kra_imp_layer_data_header_t layer_data_header;
    const kra_imp_error_code_e result = kra_imp_read_layer_data_header(ONLY_HEADER_LAYER_DATA.data(), ONLY_HEADER_LAYER_DATA.size(), &layer_data_header);
    REQUIRE(result == KRA_IMP_SUCCESS);
    REQUIRE(layer_data_header._version == 2U);
    REQUIRE(layer_data_header._layer_data_height == 64U);
    REQUIRE(layer_data_header._layer_data_width == 64U);
    REQUIRE(layer_data_header._layer_data_pixel_size == 4U);
    REQUIRE(layer_data_header._layer_datas_count == 0U);
    REQUIRE(layer_data_header._header_size == ONLY_HEADER_LAYER_DATA.size());
}

TEST_CASE("kra_imp_read_layer_data_header null buffer", "[layer_data_header]")
{
    kra_imp_layer_data_header_t layer_data_header;
    const kra_imp_error_code_e result = kra_imp_read_layer_data_header(nullptr, 0ULL, &layer_data_header);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_read_layer_data_header null header", "[layer_data_header]")
{
    const kra_imp_error_code_e result = kra_imp_read_layer_data_header(ONLY_HEADER_LAYER_DATA.data(), ONLY_HEADER_LAYER_DATA.size(), nullptr);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_read_layer_data_header buffer_size=0", "[layer_data_header]")
{
    kra_imp_layer_data_header_t layer_data_header;
    const kra_imp_error_code_e result = kra_imp_read_layer_data_header(ONLY_HEADER_LAYER_DATA.data(), 0ULL, &layer_data_header);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_read_layer_data_header empty buffer", "[layer_data_header]")
{
    kra_imp_layer_data_header_t layer_data_header;
    const kra_imp_error_code_e result = kra_imp_read_layer_data_header(EMPTY_LAYER_DATA.data(), EMPTY_LAYER_DATA.size(), &layer_data_header);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_read_layer_data_header invalid buffer (missing eol)", "[layer_data_header]")
{
    kra_imp_layer_data_header_t layer_data_header;
    const kra_imp_error_code_e result = kra_imp_read_layer_data_header(MISSING_EOL_LAYER_DATA.data(), MISSING_EOL_LAYER_DATA.size(), &layer_data_header);
    REQUIRE(result == KRA_IMP_PARSE_ERROR);
}

TEST_CASE("kra_imp_read_layer_data_header invalid buffer (missing eols)", "[layer_data_header]")
{
    kra_imp_layer_data_header_t layer_data_header;
    const kra_imp_error_code_e result = kra_imp_read_layer_data_header(MISSING_EOLS_LAYER_DATA.data(), MISSING_EOLS_LAYER_DATA.size(), &layer_data_header);
    REQUIRE(result == KRA_IMP_PARSE_ERROR);
}

TEST_CASE("kra_imp_read_layer_data_header invalid buffer (misspelled)", "[layer_data_header]")
{
    kra_imp_layer_data_header_t layer_data_header;
    const kra_imp_error_code_e result = kra_imp_read_layer_data_header(MISSPELLED_LAYER_DATA.data(), MISSPELLED_LAYER_DATA.size(), &layer_data_header);
    REQUIRE(result == KRA_IMP_PARSE_ERROR);
}

TEST_CASE("kra_imp_read_layer_data_header invalid buffer (lowercase)", "[layer_data_header]")
{
    kra_imp_layer_data_header_t layer_data_header;
    const kra_imp_error_code_e result = kra_imp_read_layer_data_header(LOWERCASE_LAYER_DATA.data(), LOWERCASE_LAYER_DATA.size(), &layer_data_header);
    REQUIRE(result == KRA_IMP_PARSE_ERROR);
}

TEST_CASE("kra_imp_read_layer_data_header invalid buffer (missing version header)", "[layer_data_header]")
{
    kra_imp_layer_data_header_t layer_data_header;
    const kra_imp_error_code_e result = kra_imp_read_layer_data_header(MISSING_VERSION_HEADER_LAYER_DATA.data(), MISSING_VERSION_HEADER_LAYER_DATA.size(), &layer_data_header);
    REQUIRE(result == KRA_IMP_PARSE_ERROR);
}

TEST_CASE("kra_imp_read_layer_data_header invalid buffer (invalid value)", "[layer_data_header]")
{
    kra_imp_layer_data_header_t layer_data_header;
    const kra_imp_error_code_e result = kra_imp_read_layer_data_header(INVALID_VALUE_LAYER_DATA.data(), INVALID_VALUE_LAYER_DATA.size(), &layer_data_header);
    REQUIRE(result == KRA_IMP_PARSE_ERROR);
}
