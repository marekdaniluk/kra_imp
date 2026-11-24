/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */                                                                                                                                                                                \
#pragma once
#include <kra_imp/export.h>
#include <span>
#include <string_view>
#include <system_error>

namespace kra_imp
{
struct KRA_IMP_API IArchive
{
    virtual ~IArchive() = default;
    virtual std::size_t getFileSize(std::string_view filePath) const = 0;
    virtual std::error_code readFile(std::string_view filePath, std::span<std::byte> outBuffer) = 0;
};
} // namespace kra_imp
