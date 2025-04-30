/**
 * kraimp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#include <catch2/catch_test_macros.hpp>
#include <kra_imp/kra_imp.hpp>

constexpr const std::string_view EMPTY_XML = "";

constexpr const std::string_view INVALID_KEY_FRAMES_XML = R"(
	<?xml version="1.0" encoding="UTF-8"?>
    <!DOCTYPE keyframes PUBLIC '-//KDE//DTD krita-keyframes 1.0//EN' 'http://www.calligra.org/DTD/krita-keyframes-1.0.dtd'
	)";

constexpr const std::string_view NO_KEY_FRAMES_XML = R"(
	<?xml version="1.0" encoding="UTF-8"?>
    <!DOCTYPE keyframes PUBLIC '-//KDE//DTD krita-keyframes 1.0//EN' 'http://www.calligra.org/DTD/krita-keyframes-1.0.dtd'>
    <keyframes xmlns="http://www.calligra.org/DTD/krita-keyframes">
     <channel name="content">
     </channel>
    </keyframes>
	)";

constexpr const std::string_view KEY_FRAMES_XML = R"(
	<?xml version="1.0" encoding="UTF-8"?>
    <!DOCTYPE keyframes PUBLIC '-//KDE//DTD krita-keyframes 1.0//EN' 'http://www.calligra.org/DTD/krita-keyframes-1.0.dtd'>
    <keyframes xmlns="http://www.calligra.org/DTD/krita-keyframes">
     <channel name="content">
      <keyframe color-label="0" time="0" frame="layer3">
       <offset type="point" x="0" y="0"/>
      </keyframe>
      <keyframe color-label="0" time="24" frame="layer3.f1">
       <offset type="point" x="0" y="0"/>
      </keyframe>
      <keyframe color-label="0" time="48" frame="layer3.f2">
       <offset type="point" x="0" y="0"/>
      </keyframe>
      <keyframe color-label="0" time="72" frame="layer3.f3">
       <offset type="point" x="0" y="0"/>
      </keyframe>
     </channel>
    </keyframes>
	)";

TEST_CASE("kra_imp_get_image_key_frames_count null buffer", "[image_frames]")
{
    unsigned int frames_count = kra_imp_get_image_key_frames_count(nullptr, 0ULL);
    REQUIRE(frames_count == 0U);
}

TEST_CASE("kra_imp_get_image_key_frames_count xml_buffer_size=0", "[image_frames]")
{
    unsigned int frames_count = kra_imp_get_image_key_frames_count(KEY_FRAMES_XML.data(), 0ULL);
    REQUIRE(frames_count == 0U);
}

TEST_CASE("kra_imp_get_image_key_frames_count empty buffer", "[image_frames]")
{
    unsigned int frames_count = kra_imp_get_image_key_frames_count(EMPTY_XML.data(), EMPTY_XML.size());
    REQUIRE(frames_count == 0U);
}

TEST_CASE("kra_imp_get_image_key_frames_count invalid xml", "[image_frames]")
{
    unsigned int frames_count = kra_imp_get_image_key_frames_count(INVALID_KEY_FRAMES_XML.data(), INVALID_KEY_FRAMES_XML.size());
    REQUIRE(frames_count == 0U);
}

TEST_CASE("kra_imp_get_image_key_frames_count no frames", "[image_frames]")
{
    unsigned int frames_count = kra_imp_get_image_key_frames_count(NO_KEY_FRAMES_XML.data(), NO_KEY_FRAMES_XML.size());
    REQUIRE(frames_count == 0U);
}

TEST_CASE("kra_imp_get_image_key_frames_count success", "[image_frames]")
{
    unsigned int frames_count = kra_imp_get_image_key_frames_count(KEY_FRAMES_XML.data(), KEY_FRAMES_XML.size());
    REQUIRE(frames_count == 4U);
}

TEST_CASE("kra_imp_read_image_key_frame null buffer", "[image_frames]")
{
    kra_imp_image_key_frame_t frame;
    kra_imp_error_code_e result = kra_imp_read_image_key_frame(nullptr, 0ULL, 0U, &frame);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_read_image_key_frame xml_buffer_size=0", "[image_frames]")
{
    kra_imp_image_key_frame_t frame;
    kra_imp_error_code_e result = kra_imp_read_image_key_frame(KEY_FRAMES_XML.data(), 0ULL, 0U, &frame);
    REQUIRE(result == KRA_IMP_PARSE_ERROR);
}

TEST_CASE("kra_imp_read_image_key_frame empty buffer", "[image_frames]")
{
    kra_imp_image_key_frame_t frame;
    kra_imp_error_code_e result = kra_imp_read_image_key_frame(EMPTY_XML.data(), EMPTY_XML.size(), 0U, &frame);
    REQUIRE(result == KRA_IMP_PARSE_ERROR);
}

TEST_CASE("kra_imp_read_image_key_frame invalid xml", "[image_frames]")
{
    kra_imp_image_key_frame_t frame;
    kra_imp_error_code_e result = kra_imp_read_image_key_frame(INVALID_KEY_FRAMES_XML.data(), INVALID_KEY_FRAMES_XML.size(), 0U, &frame);
    REQUIRE(result == KRA_IMP_PARSE_ERROR);
}

TEST_CASE("kra_imp_read_image_key_frame invalid key_frame_index no frames", "[image_frames]")
{
    kra_imp_image_key_frame_t frame;
    kra_imp_error_code_e result = kra_imp_read_image_key_frame(NO_KEY_FRAMES_XML.data(), NO_KEY_FRAMES_XML.size(), 0U, &frame);
    REQUIRE(result == KRA_IMP_FAIL);
}

TEST_CASE("kra_imp_read_image_key_frame invalid key_frame_index", "[image_frames]")
{
    kra_imp_image_key_frame_t frame;
    kra_imp_error_code_e result = kra_imp_read_image_key_frame(NO_KEY_FRAMES_XML.data(), NO_KEY_FRAMES_XML.size(), 4U, &frame);
    REQUIRE(result == KRA_IMP_FAIL);
}

TEST_CASE("kra_imp_read_image_key_frame success", "[image_frames]")
{
    kra_imp_image_key_frame_t frame;
    kra_imp_error_code_e result = kra_imp_read_image_key_frame(KEY_FRAMES_XML.data(), KEY_FRAMES_XML.size(), 0U, &frame);
    REQUIRE(result == KRA_IMP_SUCCESS);
    REQUIRE(frame._time == 0);
    REQUIRE(std::strcmp(frame._frame, "layer3") == 0);
    REQUIRE(frame._x == 0);
    REQUIRE(frame._y == 0);
}

TEST_CASE("kra_imp_read_image_key_frame success with offset", "[image_frames]")
{
    kra_imp_image_key_frame_t frame;
    kra_imp_error_code_e result = kra_imp_read_image_key_frame(KEY_FRAMES_XML.data(), KEY_FRAMES_XML.size(), 1U, &frame);
    REQUIRE(result == KRA_IMP_SUCCESS);
    REQUIRE(frame._time == 24U);
    REQUIRE(std::strcmp(frame._frame, "layer3.f1") == 0);
    REQUIRE(frame._x == 0);
    REQUIRE(frame._y == 0);
}
