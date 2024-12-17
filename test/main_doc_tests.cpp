/**
 * kraimp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#include <catch2/catch_test_macros.hpp>
#include <kra_imp/kra_imp.hpp>

TEST_CASE("kra_imp_read_main_doc null buffer", "[main_doc]")
{
    kra_imp_main_doc_t main_doc;
    kra_imp_error_code_e result = kra_imp_read_main_doc(nullptr, 0ULL, &main_doc);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_read_main_doc xml_buffer_size=0", "[main_doc]")
{
    constexpr const std::string_view MAIN_DOC_XML = R"(
	<?xml version="1.0" encoding="UTF-8"?
	<!DOCTYPE DOC PUBLIC '-//KDE//DTD krita 2.0//EN' 'http://www.calligra.org/DTD/krita-2.0.dtd'
	<DOC xmlns="http://www.calligra.org/DTD/krita" kritaVersion="5.0.0" syntaxVersion="2.0" editor="Krita"
	</DOC>
	)";

    kra_imp_main_doc_t main_doc;
    kra_imp_error_code_e result = kra_imp_read_main_doc(MAIN_DOC_XML.data(), 0ULL, &main_doc);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_read_main_doc null kra_imp_main_doc_t", "[main_doc]")
{
    constexpr const std::string_view MAIN_DOC_XML = R"(
	<?xml version="1.0" encoding="UTF-8"?
	<!DOCTYPE DOC PUBLIC '-//KDE//DTD krita 2.0//EN' 'http://www.calligra.org/DTD/krita-2.0.dtd'
	<DOC xmlns="http://www.calligra.org/DTD/krita" kritaVersion="5.0.0" syntaxVersion="2.0" editor="Krita"
	</DOC>
	)";

    kra_imp_error_code_e result = kra_imp_read_main_doc(MAIN_DOC_XML.data(), MAIN_DOC_XML.size(), nullptr);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_read_main_doc empty buffer", "[main_doc]")
{
    constexpr const std::string_view MAIN_DOC_XML = "";

    kra_imp_main_doc_t main_doc;
    kra_imp_error_code_e result = kra_imp_read_main_doc(MAIN_DOC_XML.data(), MAIN_DOC_XML.size(), &main_doc);
    REQUIRE(result == KRA_IMP_PARAMS_ERROR);
}

TEST_CASE("kra_imp_read_main_doc invalid xml", "[main_doc]")
{
    constexpr const std::string_view MAIN_DOC_XML = R"(
	<?xml version="1.0" encoding="UTF-8"?
	<!DOCTYPE DOC PUBLIC '-//KDE//DTD krita 2.0//EN' 'http://www.calligra.org/DTD/krita-2.0.dtd'
	<DOC xmlns="http://www.calligra.org/DTD/krita" kritaVersion="5.0.0" syntaxVersion="2.0" editor="Krita"
	</DOC>
	)";

    kra_imp_main_doc_t main_doc;
    kra_imp_error_code_e result = kra_imp_read_main_doc(MAIN_DOC_XML.data(), MAIN_DOC_XML.size(), &main_doc);
    REQUIRE(result == KRA_IMP_PARSE_ERROR);
}

TEST_CASE("kra_imp_read_main_doc invalid xml - no IMAGE node", "[main_doc]")
{
    constexpr const std::string_view MAIN_DOC_XML = R"(
	<?xml version="1.0" encoding="UTF-8"?>
	<!DOCTYPE DOC PUBLIC '-//KDE//DTD krita 2.0//EN' 'http://www.calligra.org/DTD/krita-2.0.dtd'>
	<DOC xmlns="http://www.calligra.org/DTD/krita" kritaVersion="5.0.0" syntaxVersion="2.0" editor="Krita">
	</DOC>
	)";

    kra_imp_main_doc_t main_doc;
    kra_imp_error_code_e result = kra_imp_read_main_doc(MAIN_DOC_XML.data(), MAIN_DOC_XML.size(), &main_doc);
    REQUIRE(result == KRA_IMP_FAIL);
}

TEST_CASE("kra_imp_read_main_doc success", "[main_doc]")
{
    constexpr const std::string_view MAIN_DOC_XML = R"(
	<?xml version="1.0" encoding="UTF-8"?>
	<!DOCTYPE DOC PUBLIC '-//KDE//DTD krita 2.0//EN' 'http://www.calligra.org/DTD/krita-2.0.dtd'>
	<DOC xmlns="http://www.calligra.org/DTD/krita" kritaVersion="5.0.0" syntaxVersion="2.0" editor="Krita">
	 <IMAGE name="Example" colorspacename="RGBA" y-res="100" proofing-model="CMYKA" x-res="100" proofing-intent="3" mime="application/x-kra" width="256" proofing-depth="U8" description="" proofing-profile-name="Chemical proof" proofing-adaptation-state="1" height="128" profile="sRGB IEC61966-2.1">
	 </IMAGE>
	</DOC>
	)";

    kra_imp_main_doc_t main_doc;
    kra_imp_error_code_e result = kra_imp_read_main_doc(MAIN_DOC_XML.data(), MAIN_DOC_XML.size(), &main_doc);
    REQUIRE(result == KRA_IMP_SUCCESS);
    REQUIRE(main_doc._width == 256U);
    REQUIRE(main_doc._height == 128U);
    REQUIRE(std::strcmp(main_doc._image_name, "Example") == 0);
    REQUIRE(main_doc._color_space_model == KRA_IMP_RGBA_COLOR_SPACE_MODEL);
    REQUIRE(main_doc._layers_count == 0U);
}

TEST_CASE("kra_imp_read_main_doc with a layer", "[main_doc]")
{
    constexpr const std::string_view MAIN_DOC_XML = R"(
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

    kra_imp_main_doc_t main_doc;
    kra_imp_error_code_e result = kra_imp_read_main_doc(MAIN_DOC_XML.data(), MAIN_DOC_XML.size(), &main_doc);
    REQUIRE(result == KRA_IMP_SUCCESS);
    REQUIRE(main_doc._width == 128U);
    REQUIRE(main_doc._height == 128U);
    REQUIRE(std::strcmp(main_doc._image_name, "Example") == 0);
    REQUIRE(main_doc._color_space_model == KRA_IMP_YCBCR_COLOR_SPACE_MODEL);
    REQUIRE(main_doc._layers_count == 1U);
}

TEST_CASE("kra_imp_read_main_doc invalid image's space model", "[main_doc]")
{
    constexpr const std::string_view MAIN_DOC_XML = R"(
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

    kra_imp_main_doc_t main_doc;
    kra_imp_error_code_e result = kra_imp_read_main_doc(MAIN_DOC_XML.data(), MAIN_DOC_XML.size(), &main_doc);
    REQUIRE(result == KRA_IMP_SUCCESS);
    REQUIRE(main_doc._width == 128U);
    REQUIRE(main_doc._height == 128U);
    REQUIRE(std::strcmp(main_doc._image_name, "Example") == 0);
    REQUIRE(main_doc._color_space_model == KRA_IMP_UNKNOWN_COLOR_SPACE_MODEL);
    REQUIRE(main_doc._layers_count == 1U);
}
