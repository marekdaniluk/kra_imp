/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#include <kra_imp/IArchive.hpp>
#include <kra_imp/IXmlParser.hpp>
#include <kra_imp/Importer.hpp>
#include "MainDocParser/MainDocParser.hpp"

namespace kra_imp
{
Importer::Importer(IArchive& archive)
    : _archive(archive)
{
}

std::pair<std::error_code, std::optional<MainDoc>> Importer::parseMainDoc(const IXmlParser& parser)
{
    static constexpr auto MAIN_DOC_FILENAME{ "maindoc.xml" };
    const auto mainDocFileSize = _archive.getFileSize(MAIN_DOC_FILENAME);
    if (mainDocFileSize == 0)
    {
        return { std::make_error_code(std::errc::no_such_file_or_directory), std::nullopt };
    }
    std::vector<std::byte> mainDocBuffer(mainDocFileSize);
    const auto readResult = _archive.readFile(MAIN_DOC_FILENAME, mainDocBuffer);
    if (readResult)
    {
        return { readResult, std::nullopt };
    }
    std::pair<std::error_code, std::optional<MainDoc>> result{ {}, std::nullopt };
    auto parseCallback = [&result](std::error_code errorCode, const IXmlNode* document)
    {
        if (errorCode)
        {
            result = { errorCode, std::nullopt };
            return;
        }
        result.second = MainDocParser::getMainDoc(*document);
    };
    parser.parseXmlDocument(mainDocBuffer, parseCallback);
    return result;
}
} // namespace kra_imp
