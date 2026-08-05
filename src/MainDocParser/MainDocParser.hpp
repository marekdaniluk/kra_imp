/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#include <kra_imp/Types.hpp>

namespace kra_imp
{
struct IXmlNode;
namespace MainDocParser
{
MainDoc getMainDoc(const IXmlNode& node);
} // namespace MainDocParser
} // namespace kra_imp
