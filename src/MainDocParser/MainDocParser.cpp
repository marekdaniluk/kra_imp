/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#include "MainDocParser.hpp"
#include <kra_imp/IXmlNode.hpp>

namespace kra_imp::MainDocParser
{
void parseAnimation(const IXmlNode& animationNode, MainDoc& mainDoc)
{
    static constexpr auto FRAMERATE_NODE{ "framerate" };
    static constexpr auto RANGE_NODE{ "range" };
    static constexpr auto FROM_ATTRIBUTE{ "from" };
    static constexpr auto TO_ATTRIBUTE{ "to" };
    static constexpr auto VALUE_ATTRIBUTE{ "value" };
    animationNode.traverseChildren(
        [&](const IXmlNode& animationChildNode) -> IXmlNode::TraverseState
        {
            if (animationChildNode.getTag() == FRAMERATE_NODE)
            {
                mainDoc.animationFrameRate = std::stoul(std::string(animationChildNode.getAttribute(VALUE_ATTRIBUTE)));
            }
            else if (animationChildNode.getTag() == RANGE_NODE)
            {
                mainDoc.animationFrom = std::stoul(std::string(animationChildNode.getAttribute(FROM_ATTRIBUTE)));
                mainDoc.animationTo = std::stoul(std::string(animationChildNode.getAttribute(TO_ATTRIBUTE)));
            }
            return IXmlNode::TraverseState::Continue;
        });
}

void parseLayer(const IXmlNode& layerNode, MainDoc& mainDoc, std::optional<std::size_t> parentIndex)
{
    static constexpr auto LAYER_NODE{ "layer" };
    if (layerNode.getTag() != LAYER_NODE)
    {
        return;
    }
    LayerInfo layerInfo{};
    static constexpr auto NAME_ATTRIBUTE{ "name" };
    static constexpr auto OPACITY_ATTRIBUTE{ "opacity" };
    static constexpr auto VISIBLE_ATTRIBUTE{ "visible" };
    static constexpr auto FILENAME_ATTRIBUTE{ "filename" };
    static constexpr auto VISIBLE_VALUE{ "1" };
    layerInfo.type = layerTypeFromString(layerNode.getAttribute("nodetype"));
    layerInfo.visibility = (layerNode.getAttribute(VISIBLE_ATTRIBUTE) == VISIBLE_VALUE) ? LayerVisibility::Visible : LayerVisibility::Hidden;
    layerInfo.opacity = std::byte(std::stoul(std::string(layerNode.getAttribute(OPACITY_ATTRIBUTE))));
    layerInfo.name = std::string(layerNode.getAttribute(NAME_ATTRIBUTE));
    layerInfo.fileName = std::string(layerNode.getAttribute(FILENAME_ATTRIBUTE));
    layerInfo.parentIndex = parentIndex;
    std::size_t currentLayerIndex = mainDoc.layers.size();
    mainDoc.layers.push_back(std::move(layerInfo));
    layerNode.traverseChildren(
        [&mainDoc, currentLayerIndex](const IXmlNode& childLayerNode) -> IXmlNode::TraverseState
        {
            parseLayer(childLayerNode, mainDoc, currentLayerIndex);
            return IXmlNode::TraverseState::Continue;
        });
}

MainDoc getMainDoc(const IXmlNode& node)
{
    MainDoc mainDoc{};
    static constexpr auto DOC_NODE{ "DOC" };
    node.traverseChildren(
        [&](const IXmlNode& childNode) -> IXmlNode::TraverseState
        {
            if (childNode.getTag() != DOC_NODE)
            {
                return IXmlNode::TraverseState::Continue;
            }
            static constexpr auto IMAGE_NODE{ "IMAGE" };
            childNode.traverseChildren(
                [&](const IXmlNode& docChildNode) -> IXmlNode::TraverseState
                {
                    if (docChildNode.getTag() != IMAGE_NODE)
                    {
                        return IXmlNode::TraverseState::Continue;
                    }
                    static constexpr auto COLOR_SPACE_ATTRIBUTE{ "colorspacename" };
                    static constexpr auto HEIGHT_ATTRIBUTE{ "height" };
                    static constexpr auto WIDTH_ATTRIBUTE{ "width" };
                    static constexpr auto NAME_ATTRIBUTE{ "name" };
                    mainDoc.colorSpaceModel = colorSpaceModelFromString(docChildNode.getAttribute(COLOR_SPACE_ATTRIBUTE));
                    mainDoc.height = std::stoul(std::string(docChildNode.getAttribute(HEIGHT_ATTRIBUTE)));
                    mainDoc.width = std::stoul(std::string(docChildNode.getAttribute(WIDTH_ATTRIBUTE)));
                    mainDoc.imageName = std::string(docChildNode.getAttribute(NAME_ATTRIBUTE));
                    docChildNode.traverseChildren(
                        [&](const IXmlNode& imageChildNode) -> IXmlNode::TraverseState
                        {
                            static constexpr auto ANIMATION_NODE{ "animation" };
                            static constexpr auto LAYERS_NODE{ "layers" };
                            if (imageChildNode.getTag() == ANIMATION_NODE)
                            {
                                parseAnimation(imageChildNode, mainDoc);
                            }
                            else if (imageChildNode.getTag() == LAYERS_NODE)
                            {
                                imageChildNode.traverseChildren(
                                    [&](const IXmlNode& layerNode) -> IXmlNode::TraverseState
                                    {
                                        parseLayer(layerNode, mainDoc, std::nullopt);
                                        return IXmlNode::TraverseState::Continue;
                                    });
                            }
                            return IXmlNode::TraverseState::Continue;
                        });
                    return IXmlNode::TraverseState::Break;
                });
            return IXmlNode::TraverseState::Break;
        });
    return mainDoc;
}
} // namespace kra_imp::MainDocParser
