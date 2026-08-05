/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */                                                                                                                                                                                \
#pragma once
#include <kra_imp/config.hpp>
#include <functional>
#include <string_view>

namespace kra_imp
{
struct KRA_IMP_API IXmlNode
{
    enum class TraverseState
    {
        Continue,
        Break
    };
    virtual ~IXmlNode() = default;
    virtual std::string_view getAttribute(std::string_view name) const = 0;
    virtual std::string_view getTag() const = 0;
    virtual void traverseChildren(std::function<TraverseState(const IXmlNode&)> traverseCallback) const = 0;
};
} // namespace kra_imp
