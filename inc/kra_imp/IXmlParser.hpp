/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */                                                                                                                                                                                \
#pragma once
#include <kra_imp/export.h>
#include <functional>
#include <span>
#include <system_error>

namespace kra_imp
{
struct IXmlNode;
struct KRA_IMP_API IXmlParser
{
    virtual ~IXmlParser() = default;
    virtual void parseXmlDocument(std::span<std::byte> buffer, std::function<void(std::error_code, const IXmlNode*)> parseCallback) const = 0;
};
} // namespace kra_imp
