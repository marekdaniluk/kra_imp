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

constexpr const std::string_view OPACITY_MAIN_DOC_XML = R"(
	<?xml version="1.0" encoding="UTF-8"?>
	<!DOCTYPE DOC PUBLIC '-//KDE//DTD krita 2.0//EN' 'http://www.calligra.org/DTD/krita-2.0.dtd'>
	<DOC xmlns="http://www.calligra.org/DTD/krita" kritaVersion="5.0.0" syntaxVersion="2.0" editor="Krita">
	 <IMAGE name="Example" colorspacename="RGBA" y-res="100" proofing-model="CMYKA" x-res="100" proofing-intent="3" mime="application/x-kra" width="128" proofing-depth="U8" description="" proofing-profile-name="Chemical proof" proofing-adaptation-state="1" height="128" profile="sRGB IEC61966-2.1">
	  <layers>
	   <layer name="layer_1" colorspacename="RGBA" onionskin="0" x="0" nodetype="paintlayer" y="0" channellockflags="1111" visible="1" compositeop="normal" intimeline="1" locked="0" collapsed="0" colorlabel="0" opacity="127" filename="layer1" channelflags="" uuid="{683fcc00-dd23-4de2-9559-1ba917d53a7b}"/>
	  </layers>
	 </IMAGE>
	</DOC>
	)";

constexpr const std::string_view GROUP_MAIN_DOC_XML = R"(
	<?xml version="1.0" encoding="UTF-8"?>
	<!DOCTYPE DOC PUBLIC '-//KDE//DTD krita 2.0//EN' 'http://www.calligra.org/DTD/krita-2.0.dtd'>
	<DOC xmlns="http://www.calligra.org/DTD/krita" kritaVersion="5.0.0" syntaxVersion="2.0" editor="Krita">
	 <IMAGE name="Example" colorspacename="RGBA" y-res="100" proofing-model="CMYKA" x-res="100" proofing-intent="3" mime="application/x-kra" width="128" proofing-depth="U8" description="" proofing-profile-name="Chemical proof" proofing-adaptation-state="1" height="128" profile="sRGB IEC61966-2.1">
	  <layers>
	   <layer name="group" x="0" passthrough="0" nodetype="grouplayer" y="0" visible="0" compositeop="normal" intimeline="0" locked="0" collapsed="0" colorlabel="0" opacity="255" filename="layer1" channelflags="" uuid="{2a16ae40-0a0b-4f13-b470-8a43c1ad1265}">
	    <layers>
	     <layer name="sublayer" colorspacename="RGBA" onionskin="0" x="0" nodetype="paintlayer" y="0" channellockflags="1111" visible="1" compositeop="normal" intimeline="1" locked="0" collapsed="0" colorlabel="0" opacity="255" filename="layer2" channelflags="" uuid="{5ff215e2-855a-44c5-916d-9e06d53cff4d}"/>
	    </layers>
	   </layer>
	  </layers>
	 </IMAGE>
	</DOC>
	)";

TEST_CASE("kra_imp_read_image_layer null buffer", "[image_layer]")
{
    kra_imp_image_layer_t image_layer;
    kra_imp_error_code_e result = kra_imp_read_image_layer(nullptr, 0ULL, 0U, &image_layer);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_read_image_layer xml_buffer_size=0", "[image_layer]")
{
    kra_imp_image_layer_t image_layer;
    kra_imp_error_code_e result = kra_imp_read_image_layer(NO_IMAGE_MAIN_DOC_XML.data(), 0ULL, 0U, &image_layer);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_read_image_layer null kra_imp_image_layer_t", "[image_layer]")
{
    kra_imp_error_code_e result = kra_imp_read_image_layer(INVALID_MAIN_DOC_XML.data(), INVALID_MAIN_DOC_XML.size(), 0U, nullptr);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_read_image_layer empty buffer", "[image_layer]")
{
    kra_imp_image_layer_t image_layer;
    kra_imp_error_code_e result = kra_imp_read_image_layer(EMPTY_XML.data(), EMPTY_XML.size(), 0U, &image_layer);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_read_image_layer invalid xml", "[image_layer]")
{
    kra_imp_image_layer_t image_layer;
    kra_imp_error_code_e result = kra_imp_read_image_layer(INVALID_MAIN_DOC_XML.data(), INVALID_MAIN_DOC_XML.size(), 0U, &image_layer);
    REQUIRE(result == KRA_IMP_PARSE_ERROR);
}

TEST_CASE("kra_imp_read_image_layer invalid xml - no IMAGE node", "[image_layer]")
{
    kra_imp_image_layer_t image_layer;
    kra_imp_error_code_e result = kra_imp_read_image_layer(NO_IMAGE_MAIN_DOC_XML.data(), NO_IMAGE_MAIN_DOC_XML.size(), 0U, &image_layer);
    REQUIRE(result == KRA_IMP_FAIL);
}

TEST_CASE("kra_imp_read_image_layer invalid layer_index (no layers)", "[image_layer]")
{
    kra_imp_image_layer_t image_layer;
    kra_imp_error_code_e result = kra_imp_read_image_layer(NO_LAYERS_MAIN_DOC_XML.data(), NO_LAYERS_MAIN_DOC_XML.size(), 0U, &image_layer);
    REQUIRE(result == KRA_IMP_FAIL);
}

TEST_CASE("kra_imp_read_image_layer invalid layer_index", "[image_layer]")
{
    kra_imp_image_layer_t image_layer;
    kra_imp_error_code_e result = kra_imp_read_image_layer(SINGLE_LAYER_MAIN_DOC_XML.data(), SINGLE_LAYER_MAIN_DOC_XML.size(), 1U, &image_layer);
    REQUIRE(result == KRA_IMP_FAIL);
}

TEST_CASE("kra_imp_read_image_layer success", "[image_layer]")
{
    kra_imp_image_layer_t image_layer;
    kra_imp_error_code_e result = kra_imp_read_image_layer(SINGLE_LAYER_MAIN_DOC_XML.data(), SINGLE_LAYER_MAIN_DOC_XML.size(), 0U, &image_layer);
    REQUIRE(result == KRA_IMP_SUCCESS);
    REQUIRE(image_layer._type == KRA_IMP_PAINT_LAYER_TYPE);
    REQUIRE(image_layer._parent_index == -1L);
    REQUIRE(image_layer._opacity == 255U);
    REQUIRE(image_layer._visibility == KRA_IMP_VISIBLE);
    REQUIRE(std::strcmp(image_layer._file_name, "layer1") == 0);
    REQUIRE(std::strcmp(image_layer._frame_file_name, "") == 0);
    REQUIRE(std::strcmp(image_layer._name, "layer_1") == 0);
}

TEST_CASE("kra_imp_read_image_layer opacity", "[image_layer]")
{
    kra_imp_image_layer_t image_layer;
    kra_imp_error_code_e result = kra_imp_read_image_layer(OPACITY_MAIN_DOC_XML.data(), OPACITY_MAIN_DOC_XML.size(), 0U, &image_layer);
    REQUIRE(result == KRA_IMP_SUCCESS);
    REQUIRE(image_layer._type == KRA_IMP_PAINT_LAYER_TYPE);
    REQUIRE(image_layer._parent_index == -1L);
    REQUIRE(image_layer._opacity == 127U);
    REQUIRE(image_layer._visibility == KRA_IMP_VISIBLE);
    REQUIRE(std::strcmp(image_layer._file_name, "layer1") == 0);
    REQUIRE(std::strcmp(image_layer._frame_file_name, "") == 0);
    REQUIRE(std::strcmp(image_layer._name, "layer_1") == 0);
}

TEST_CASE("kra_imp_read_image_layer with a grouped layers", "[image_layer]")
{
    kra_imp_image_layer_t image_layer;
    kra_imp_error_code_e result = kra_imp_read_image_layer(GROUP_MAIN_DOC_XML.data(), GROUP_MAIN_DOC_XML.size(), 0U, &image_layer);
    REQUIRE(result == KRA_IMP_SUCCESS);
    REQUIRE(image_layer._type == KRA_IMP_GROUP_LAYER_TYPE);
    REQUIRE(image_layer._parent_index == -1L);
    REQUIRE(image_layer._opacity == 255U);
    REQUIRE(image_layer._visibility == KRA_IMP_HIDDEN);
    result = kra_imp_read_image_layer(GROUP_MAIN_DOC_XML.data(), GROUP_MAIN_DOC_XML.size(), 1U, &image_layer);
    REQUIRE(result == KRA_IMP_SUCCESS);
    REQUIRE(image_layer._type == KRA_IMP_PAINT_LAYER_TYPE);
    REQUIRE(image_layer._parent_index == 0L);
    REQUIRE(image_layer._opacity == 255U);
    REQUIRE(image_layer._visibility == KRA_IMP_VISIBLE);
    REQUIRE(std::strcmp(image_layer._file_name, "layer2") == 0);
    REQUIRE(std::strcmp(image_layer._frame_file_name, "") == 0);
    REQUIRE(std::strcmp(image_layer._name, "sublayer") == 0);
}
