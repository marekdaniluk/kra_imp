/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#include <kra_imp/XmlParser.hpp>
#include <kra_imp/IXmlNode.hpp>
#include <pugixml.hpp>

namespace kra_imp
{
struct XmlNode final : public IXmlNode
{
    explicit XmlNode(const pugi::xml_node& xmlNode)
        : _xmlNode(xmlNode)
    {
    }

    ~XmlNode() override = default;
    
    std::string_view getAttribute(std::string_view name) const override
    {
        const auto attribute = _xmlNode.attribute(name.data());
        return std::string_view(attribute.value());
    }
    
    std::string_view getTag() const override
    {
        return _xmlNode.name();
    }
    
    void traverseChildren(std::function<TraverseState(const IXmlNode&)> traverseCallback) const override
    {
        for (auto child : _xmlNode.children())
        {
            const XmlNode xmlChild(child);
            if (traverseCallback(xmlChild) == TraverseState::Break)
            {
                break;
            }
        }
    }
    const pugi::xml_node& _xmlNode;
};

void XmlParser::parseXmlDocument(std::span<std::byte> buffer, std::function<void(std::error_code, const IXmlNode*)> parseCallback) const
{
    pugi::xml_document xmlDocument;
    const pugi::xml_parse_result parseResult = xmlDocument.load_buffer(buffer.data(), buffer.size());
    if (!parseResult)
    {
        parseCallback(std::make_error_code(std::errc::invalid_argument), nullptr);
        return;
    }
    const XmlNode xmlDoc(xmlDocument);
    parseCallback({}, &xmlDoc);
}
} // namespace kra_imp
