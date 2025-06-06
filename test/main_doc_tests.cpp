/**
 * kraimp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#include <catch2/catch_test_macros.hpp>
#include <kra_imp/kra_imp.hpp>

constexpr const std::string_view EMPTY_XML = "";

constexpr const std::string_view INVALID_MAIN_DOC_XML = R"(
	<?xml version="1.0" encoding="UTF-8"?
	<!DOCTYPE DOC PUBLIC '-//KDE//DTD krita 2.0//EN' 'http://www.calligra.org/DTD/krita-2.0.dtd'
	<DOC xmlns="http://www.calligra.org/DTD/krita" kritaVersion="5.0.0" syntaxVersion="2.0" editor="Krita"
	</DOC>
	)";

constexpr const std::string_view NO_IMAGE_MAIN_DOC_XML = R"(
	<?xml version="1.0" encoding="UTF-8"?>
	<!DOCTYPE DOC PUBLIC '-//KDE//DTD krita 2.0//EN' 'http://www.calligra.org/DTD/krita-2.0.dtd'>
	<DOC xmlns="http://www.calligra.org/DTD/krita" kritaVersion="5.0.0" syntaxVersion="2.0" editor="Krita">
	</DOC>
	)";

constexpr const std::string_view NO_LAYERS_MAIN_DOC_XML = R"(
	<?xml version="1.0" encoding="UTF-8"?>
	<!DOCTYPE DOC PUBLIC '-//KDE//DTD krita 2.0//EN' 'http://www.calligra.org/DTD/krita-2.0.dtd'>
	<DOC xmlns="http://www.calligra.org/DTD/krita" kritaVersion="5.0.0" syntaxVersion="2.0" editor="Krita">
	 <IMAGE name="Example" colorspacename="RGBA" y-res="100" proofing-model="CMYKA" x-res="100" proofing-intent="3" mime="application/x-kra" width="256" proofing-depth="U8" description="" proofing-profile-name="Chemical proof" proofing-adaptation-state="1" height="128" profile="sRGB IEC61966-2.1">
	 </IMAGE>
	</DOC>
	)";

constexpr const std::string_view SINGLE_LAYER_MAIN_DOC_XML = R"(
	<?xml version="1.0" encoding="UTF-8"?>
	<!DOCTYPE DOC PUBLIC '-//KDE//DTD krita 2.0//EN' 'http://www.calligra.org/DTD/krita-2.0.dtd'>
	<DOC xmlns="http://www.calligra.org/DTD/krita" kritaVersion="5.0.0" syntaxVersion="2.0" editor="Krita">
	 <IMAGE name="Example" colorspacename="YCBCRAU16" y-res="100" proofing-model="CMYKA" x-res="100" proofing-intent="3" mime="application/x-kra" width="128" proofing-depth="U8" description="" proofing-profile-name="Chemical proof" proofing-adaptation-state="1" height="128" profile="ITU-R BT.709-6 YCbCr ICC V4 profile">
	  <layers>
	   <layer name="layer_1" colorspacename="YCBCRAU16" onionskin="0" x="0" nodetype="paintlayer" y="0" channellockflags="1111" visible="1" compositeop="normal" intimeline="1" locked="0" collapsed="0" colorlabel="0" opacity="255" filename="layer1" channelflags="" uuid="{683fcc00-dd23-4de2-9559-1ba917d53a7b}"/>
	  </layers>
	 </IMAGE>
	</DOC>
	)";

constexpr const std::string_view INVALID_COLOR_SPACE_MAIN_DOC_XML = R"(
	<?xml version="1.0" encoding="UTF-8"?>
	<!DOCTYPE DOC PUBLIC '-//KDE//DTD krita 2.0//EN' 'http://www.calligra.org/DTD/krita-2.0.dtd'>
	<DOC xmlns="http://www.calligra.org/DTD/krita" kritaVersion="5.0.0" syntaxVersion="2.0" editor="Krita">
	 <IMAGE name="Example" colorspacename="invalid" y-res="100" proofing-model="CMYKA" x-res="100" proofing-intent="3" mime="application/x-kra" width="128" proofing-depth="U8" description="" proofing-profile-name="Chemical proof" proofing-adaptation-state="1" height="128" profile="ITU-R BT.709-6 YCbCr ICC V4 profile">
	  <layers>
	   <layer name="layer_1" colorspacename="invalid" onionskin="0" x="0" nodetype="paintlayer" y="0" channellockflags="1111" visible="1" compositeop="normal" intimeline="1" locked="0" collapsed="0" colorlabel="0" opacity="255" filename="layer1" channelflags="" uuid="{683fcc00-dd23-4de2-9559-1ba917d53a7b}"/>
	  </layers>
	 </IMAGE>
	</DOC>
	)";

constexpr const std::string_view ANIMATION_MAIN_DOC_XML = R"(
	<?xml version="1.0" encoding="UTF-8"?>
    <!DOCTYPE DOC PUBLIC '-//KDE//DTD krita 2.0//EN' 'http://www.calligra.org/DTD/krita-2.0.dtd'>
    <DOC xmlns="http://www.calligra.org/DTD/krita" kritaVersion="5.2.6" syntaxVersion="2.0" editor="Krita">
     <IMAGE mime="application/x-kra" height="64" colorspacename="RGBA" x-res="100" name="animation" description="" y-res="100" width="64" profile="sRGB-elle-V2-srgbtrc.icc">
      <layers>
       <mask locked="0" x="0" name="Selection Mask" uuid="{77c997e3-3236-45aa-a363-a1404fd407d4}" visible="1" nodetype="selectionmask" filename="mask2" y="0" colorlabel="0" intimeline="0" active="1"/>
       <layer visible="1" name="Paint Layer 1" locked="0" onionskin="0" keyframes="layer3.keyframes.xml" filename="layer3" channelflags="" collapsed="0" channellockflags="" nodetype="paintlayer" uuid="{901c4f11-a703-44a2-81d5-a50b7728987d}" y="0" compositeop="normal" colorlabel="0" colorspacename="RGBA" opacity="255" x="0" intimeline="1"/>
       <layer visible="1" name="Background" locked="0" onionskin="0" keyframes="layer4.keyframes.xml" filename="layer4" channelflags="" collapsed="0" channellockflags="" nodetype="paintlayer" uuid="{b37e7cbe-400f-4c57-a3a6-b97bf425332c}" y="0" compositeop="normal" colorlabel="0" colorspacename="RGBA" opacity="255" x="0" selected="true" intimeline="1"/>
      </layers>
      <ProjectionBackgroundColor ColorData="AAAAAA=="/>
      <GlobalAssistantsColor SimpleColorData="176,176,176,255"/>
      <MirrorAxis>
       <mirrorHorizontal type="value" value="0"/>
       <mirrorVertical type="value" value="0"/>
       <lockHorizontal type="value" value="0"/>
       <lockVertical type="value" value="0"/>
       <hideHorizontalDecoration type="value" value="0"/>
       <hideVerticalDecoration type="value" value="0"/>
       <handleSize type="value" value="32"/>
       <horizontalHandlePosition type="value" value="64"/>
       <verticalHandlePosition type="value" value="64"/>
       <axisPosition type="pointf" x="32" y="32"/>
      </MirrorAxis>
      <Palettes/>
      <resources/>
      <animation>
       <framerate type="value" value="24"/>
       <range type="timerange" to="100" from="0"/>
       <currentTime type="value" value="72"/>
      </animation>
     </IMAGE>
    </DOC>
	)";

TEST_CASE("kra_imp_read_main_doc null buffer", "[main_doc]")
{
    kra_imp_main_doc_t main_doc;
    kra_imp_error_code_e result = kra_imp_read_main_doc(nullptr, 0ULL, &main_doc);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_read_main_doc xml_buffer_size=0", "[main_doc]")
{
    kra_imp_main_doc_t main_doc;
    kra_imp_error_code_e result = kra_imp_read_main_doc(INVALID_MAIN_DOC_XML.data(), 0ULL, &main_doc);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_read_main_doc null kra_imp_main_doc_t", "[main_doc]")
{
    kra_imp_error_code_e result = kra_imp_read_main_doc(INVALID_MAIN_DOC_XML.data(), INVALID_MAIN_DOC_XML.size(), nullptr);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_read_main_doc empty buffer", "[main_doc]")
{
    kra_imp_main_doc_t main_doc;
    kra_imp_error_code_e result = kra_imp_read_main_doc(EMPTY_XML.data(), EMPTY_XML.size(), &main_doc);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_read_main_doc invalid xml", "[main_doc]")
{
    kra_imp_main_doc_t main_doc;
    kra_imp_error_code_e result = kra_imp_read_main_doc(INVALID_MAIN_DOC_XML.data(), INVALID_MAIN_DOC_XML.size(), &main_doc);
    REQUIRE(result == KRA_IMP_PARSE_ERROR);
}

TEST_CASE("kra_imp_read_main_doc invalid xml - no IMAGE node", "[main_doc]")
{
    kra_imp_main_doc_t main_doc;
    kra_imp_error_code_e result = kra_imp_read_main_doc(NO_IMAGE_MAIN_DOC_XML.data(), NO_IMAGE_MAIN_DOC_XML.size(), &main_doc);
    REQUIRE(result == KRA_IMP_FAIL);
}

TEST_CASE("kra_imp_read_main_doc success", "[main_doc]")
{
    kra_imp_main_doc_t main_doc;
    kra_imp_error_code_e result = kra_imp_read_main_doc(NO_LAYERS_MAIN_DOC_XML.data(), NO_LAYERS_MAIN_DOC_XML.size(), &main_doc);
    REQUIRE(result == KRA_IMP_SUCCESS);
    REQUIRE(main_doc._width == 256U);
    REQUIRE(main_doc._height == 128U);
    REQUIRE(std::strcmp(main_doc._image_name, "Example") == 0);
    REQUIRE(main_doc._color_space_model == KRA_IMP_RGBA_COLOR_SPACE_MODEL);
    REQUIRE(main_doc._layers_count == 0U);
    const kra_imp_animation_t& animation = main_doc._animation;
    REQUIRE(animation._frame_rate == 0U);
    REQUIRE(animation._from == 0U);
    REQUIRE(animation._to == 0U);
}

TEST_CASE("kra_imp_read_main_doc with a layer", "[main_doc]")
{
    kra_imp_main_doc_t main_doc;
    kra_imp_error_code_e result = kra_imp_read_main_doc(SINGLE_LAYER_MAIN_DOC_XML.data(), SINGLE_LAYER_MAIN_DOC_XML.size(), &main_doc);
    REQUIRE(result == KRA_IMP_SUCCESS);
    REQUIRE(main_doc._width == 128U);
    REQUIRE(main_doc._height == 128U);
    REQUIRE(std::strcmp(main_doc._image_name, "Example") == 0);
    REQUIRE(main_doc._color_space_model == KRA_IMP_YCBCR_COLOR_SPACE_MODEL);
    REQUIRE(main_doc._layers_count == 1U);
    const kra_imp_animation_t& animation = main_doc._animation;
    REQUIRE(animation._frame_rate == 0U);
    REQUIRE(animation._from == 0U);
    REQUIRE(animation._to == 0U);
}

TEST_CASE("kra_imp_read_main_doc invalid image's space model", "[main_doc]")
{
    kra_imp_main_doc_t main_doc;
    kra_imp_error_code_e result = kra_imp_read_main_doc(INVALID_COLOR_SPACE_MAIN_DOC_XML.data(), INVALID_COLOR_SPACE_MAIN_DOC_XML.size(), &main_doc);
    REQUIRE(result == KRA_IMP_SUCCESS);
    REQUIRE(main_doc._width == 128U);
    REQUIRE(main_doc._height == 128U);
    REQUIRE(std::strcmp(main_doc._image_name, "Example") == 0);
    REQUIRE(main_doc._color_space_model == KRA_IMP_UNKNOWN_COLOR_SPACE_MODEL);
    REQUIRE(main_doc._layers_count == 1U);
    const kra_imp_animation_t& animation = main_doc._animation;
    REQUIRE(animation._frame_rate == 0U);
    REQUIRE(animation._from == 0U);
    REQUIRE(animation._to == 0U);
}

TEST_CASE("kra_imp_read_main_doc animation", "[main_doc]")
{
    kra_imp_main_doc_t main_doc;
    kra_imp_error_code_e result = kra_imp_read_main_doc(ANIMATION_MAIN_DOC_XML.data(), ANIMATION_MAIN_DOC_XML.size(), &main_doc);
    REQUIRE(result == KRA_IMP_SUCCESS);
    REQUIRE(main_doc._width == 64U);
    REQUIRE(main_doc._height == 64U);
    REQUIRE(std::strcmp(main_doc._image_name, "animation") == 0);
    REQUIRE(main_doc._color_space_model == KRA_IMP_RGBA_COLOR_SPACE_MODEL);
    REQUIRE(main_doc._layers_count == 2U);
    const kra_imp_animation_t& animation = main_doc._animation;
    REQUIRE(animation._frame_rate == 24U);
    REQUIRE(animation._from == 0U);
    REQUIRE(animation._to == 100U);
}
