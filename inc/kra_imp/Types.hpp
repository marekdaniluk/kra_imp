/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#pragma once
#include <kra_imp/config.hpp>
#include <optional>
#include <string>
#include <vector>

namespace kra_imp
{
enum class ColorSpaceModel
{
    Unknown,
    CIELAB,
    CMYK,
    Graya,
    RGBA,
    Xyza,
    YCbCrA
};
ColorSpaceModel colorSpaceModelFromString(std::string_view colorSpaceName);
enum class LayerType
{
    Unknown,
    Group,
    Paint,
    Clone,
    File,
    ColorizeMask,
    TransformMask,
    TransparencyMask
};
LayerType layerTypeFromString(std::string_view layerTypeName);
enum class LayerVisibility
{
    Hidden,
    Visible
};
struct KRA_IMP_API LayerInfo
{
    LayerType type{ LayerType::Unknown };
    LayerVisibility visibility{ LayerVisibility::Hidden };
    std::byte opacity{ 0 };
    std::optional<std::size_t> parentIndex{ std::nullopt };
    std::string fileName{};
    std::string frameFileName{};
    std::string name{};
};
struct KRA_IMP_API MainDoc
{
    ColorSpaceModel colorSpaceModel{ ColorSpaceModel::Unknown };
    std::size_t animationFrameRate{ 0 };
    std::size_t animationFrom{ 0 };
    std::size_t animationTo{ 0 };
    std::size_t width{ 0 };
    std::size_t height{ 0 };
    std::string imageName{};
    std::vector<LayerInfo> layers;
};
} // namespace kra_imp
