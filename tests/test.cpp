/**
 * kraimp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#include <catch2/catch_test_macros.hpp>
#include <kra_imp/kra_imp.hpp>

TEST_CASE("kra_imp_get_version", "[default]")
{
    const unsigned int version = kra_imp_get_version();
    const unsigned int major = (version >> 16) & 0xff;
    const unsigned int minor = (version >> 8) & 0xff;
    const unsigned int patch = version & 0xff;
    REQUIRE(version == KRA_IMP_VERSION);
    REQUIRE(major == KRA_IMP_MAJOR);
    REQUIRE(minor == KRA_IMP_MINOR);
    REQUIRE(patch == KRA_IMP_PATCH);
}

TEST_CASE("kra_imp_get_main_doc_file_name", "[default]")
{
    static constexpr const char* KRA_IMP_MAIN_DOC_FILE_NAME{ "maindoc.xml" };

    const char* main_doc_name = kra_imp_get_main_doc_file_name();
    REQUIRE(std::strcmp(KRA_IMP_MAIN_DOC_FILE_NAME, main_doc_name) == 0);
}

TEST_CASE("kra_imp_get_layer_directory_name", "[default]")
{
    static constexpr const char* KRA_IMP_LAYERS_DIRECTORY_NAME{ "layers" };

    const char* layer_directory_name = kra_imp_get_layer_directory_name();
    REQUIRE(std::strcmp(KRA_IMP_LAYERS_DIRECTORY_NAME, layer_directory_name) == 0);
}
