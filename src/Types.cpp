/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */                                                                                                                                                                                \
#pragma once
#include <kra_imp/Types.hpp>
#include <array>

namespace kra_imp
{
ColorSpaceModel colorSpaceModelFromString(std::string_view colorSpaceName)
{
    static constexpr std::array<std::pair<std::string_view, ColorSpaceModel>, 6> colorSpaceModelMap = {
        { { "LABA", ColorSpaceModel::CIELAB },
         { "CMYK", ColorSpaceModel::CMYK },
         { "GRAYA", ColorSpaceModel::Graya },
         { "RGBA", ColorSpaceModel::RGBA },
         { "XYZA", ColorSpaceModel::Xyza },
         { "YCBCRA", ColorSpaceModel::YCbCrA } }
    };
    for (const auto& [name, model] : colorSpaceModelMap)
    {
        if (colorSpaceName == name)
        {
            return model;
        }
    }
    return ColorSpaceModel::Unknown;
}
LayerType layerTypeFromString(std::string_view layerTypeName)
{
    static constexpr std::array<std::pair<std::string_view, LayerType>, 7> layerTypeMap = {
        { { "grouplayer", LayerType::Group },
         { "paintlayer", LayerType::Paint },
         { "cloneLayer", LayerType::Clone },
         { "filelayer", LayerType::File },
         { "colorizemask", LayerType::ColorizeMask },
         { "transformmask", LayerType::TransformMask },
         { "transparencymask", LayerType::TransparencyMask } }
    };
    for (const auto& [name, type] : layerTypeMap)
    {
        if (layerTypeName == name)
        {
            return type;
        }
    }
    return LayerType::Unknown;
}
} // namespace kra_imp
