/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#pragma once
#include <kra_imp/config.hpp>
#include <optional>
#include <system_error>

namespace kra_imp
{
struct IArchive;
struct IXmlParser;
struct MainDoc;
class KRA_IMP_API Importer
{
  public:
    Importer(IArchive& archive);
    std::pair<std::error_code, std::optional<MainDoc>> parseMainDoc(const IXmlParser& parser);
  private:
    IArchive& _archive;
};
} // namespace kra_imp
