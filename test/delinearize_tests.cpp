#include <array>
#include <catch2/catch_test_macros.hpp>
#include <kra_imp/kra_imp.hpp>

TEST_CASE("kra_imp_delinearize_to_bgra null input buffer", "[delinearize_to_bgra]")
{
    const unsigned int width = 64;
    const unsigned int pixel_size = 4;
    std::array<char, width * width * pixel_size> output_buffer;
    const kra_imp_error_code_e result = kra_imp_delinearize_to_bgra(nullptr, output_buffer.data(), output_buffer.size(), width);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_delinearize_to_bgra null output buffer", "[delinearize_to_bgra]")
{
    const unsigned int width = 64;
    const unsigned int pixel_size = 4;
    const std::array<char, width * width * pixel_size> input_buffer{ 0 };
    const kra_imp_error_code_e result = kra_imp_delinearize_to_bgra(input_buffer.data(), nullptr, input_buffer.size(), width);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_delinearize_to_bgra zero buffer size", "[delinearize_to_bgra]")
{
    const unsigned int width = 64;
    const unsigned int pixel_size = 4;
    const std::array<char, width * width * pixel_size> input_buffer{ 0 };
    std::array<char, width * width * pixel_size> output_buffer{ 0 };
    const kra_imp_error_code_e result = kra_imp_delinearize_to_bgra(input_buffer.data(), output_buffer.data(), 0, width);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_delinearize_to_bgra valid conversion", "[delinearize_to_bgra]")
{
    const unsigned int width = 2;
    const unsigned int pixel_size = 4;
    const std::array<char, width * width * pixel_size> input_buffer = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    std::array<char, width * width * pixel_size> output_buffer = { 0 };
    const std::array<char, width * width * pixel_size> expected_output_buffer = { 0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15 };
    const kra_imp_error_code_e result = kra_imp_delinearize_to_bgra(input_buffer.data(), output_buffer.data(), input_buffer.size(), width);
    REQUIRE(result == KRA_IMP_SUCCESS);
    REQUIRE(output_buffer == expected_output_buffer);
}

TEST_CASE("kra_imp_delinearize_to_bgra_with_offset too small output", "[delinearize_to_bgra]")
{
    const unsigned int width = 4;
    const unsigned int height = 4;
    const unsigned int pixel_size = 4;
    const std::array<char, width * height * pixel_size> input_buffer{ 0 };
    const unsigned int output_width = 2;
    const unsigned int output_height = 2;
    std::array<char, output_width * output_height * pixel_size> output_buffer = { 0 };
    const kra_imp_error_code_e result =
        kra_imp_delinearize_to_bgra_with_offset(input_buffer.data(), input_buffer.size(), width, output_buffer.data(), output_buffer.size(), output_width, 0);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_delinearize_to_bgra_with_offset too big offset", "[delinearize_to_bgra]")
{
    const unsigned int width = 4;
    const unsigned int height = 4;
    const unsigned int pixel_size = 4;
    const std::array<char, width * height * pixel_size> input_buffer{ 0 };
    std::array<char, width * height * pixel_size> output_buffer = { 0 };
    const kra_imp_error_code_e result =
        kra_imp_delinearize_to_bgra_with_offset(input_buffer.data(), input_buffer.size(), width, output_buffer.data(), output_buffer.size(), width, pixel_size);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_delinearize_to_bgra_with_offset with offset", "[delinearize_to_bgra]")
{
    const unsigned int width = 2;
    const unsigned int height = 2;
    const unsigned int pixel_size = 4;
    const std::array<char, width * height * pixel_size> input_buffer = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    const unsigned int output_width = width + 2;
    const unsigned int output_height = height + 2;
    const unsigned int output_offset = (output_width + 1) * pixel_size;
    std::array<char, output_width * output_height * pixel_size> output_buffer = { 0 };
    const std::array<char, output_width * output_height * pixel_size> expected_output_buffer = { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                                                                                                 0,  0,  0,  0,  0,  4,  8, 12,  1,  5,  9, 13,  0,  0,  0,  0,
                                                                                                 0,  0,  0,  0,  2,  6, 10, 14,  3,  7, 11, 15,  0,  0,  0,  0,  
                                                                                                 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 };
    const kra_imp_error_code_e result =
        kra_imp_delinearize_to_bgra_with_offset(input_buffer.data(), input_buffer.size(), width, output_buffer.data(), output_buffer.size(), output_width, output_offset);
    REQUIRE(result == KRA_IMP_SUCCESS);
    REQUIRE(output_buffer == expected_output_buffer);
}
