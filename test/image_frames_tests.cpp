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
