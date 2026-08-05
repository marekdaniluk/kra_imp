/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#include <kra_imp/IXmlParser.hpp>

namespace kra_imp
{
class KRA_IMP_API XmlParser final : public IXmlParser
{
  public:
    XmlParser() = default;
    ~XmlParser() override = default;
    void parseXmlDocument(std::span<std::byte> buffer, std::function<void(std::error_code, const IXmlNode*)> parseCallback) const override;
};
} // namespace kra_imp
