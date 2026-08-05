#include <catch2/catch_test_macros.hpp>
#include <kra_imp/IArchive.hpp>
#include <kra_imp/Importer.hpp>
#include <kra_imp/Types.hpp>
#include <kra_imp/XmlParser.hpp>

class MockArchive : public kra_imp::IArchive
{
    constexpr static const std::string_view MAIN_DOC_XML = R"(
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE DOC PUBLIC '-//KDE//DTD krita 2.0//EN' 'http://www.calligra.org/DTD/krita-2.0.dtd'>
<DOC xmlns="http://www.calligra.org/DTD/krita" kritaVersion="5.3.3" editor="Krita" syntaxVersion="2.0">
 <IMAGE profile="sRGB-elle-V2-srgbtrc.icc" mime="application/x-kra" colorspacename="RGBA" width="254" y-res="240" x-res="240" height="240" name="example" description="">
  <layers>
   <layer y="0" opacity="255" x="0" uuid="{d7dd6e89-9042-462d-8012-74fda8c964d1}" channellockflags="" intimeline="1" colorlabel="0" selected="true" compositeop="normal" filename="layer2" channelflags="" name="Paint Layer 1" locked="0" visible="1" onionskin="0" collapsed="0" nodetype="paintlayer" colorspacename="RGBA"/>
   <layer opacity="255" y="0" colorlabel="0" nodetype="paintlayer" intimeline="1" colorspacename="RGBA" x="0" channellockflags="" compositeop="normal" channelflags="" uuid="{2f31cc84-3de0-4bd5-951f-89ef974bf7ca}" locked="1" collapsed="0" onionskin="0" name="Background" visible="1" filename="layer3"/>
  </layers>
  <ProjectionBackgroundColor ColorData="AAAAAA=="/>
  <GlobalAssistantsColor SimpleColorData="176,176,176,255"/>
  <MirrorAxis>
   <mirrorHorizontal type="value" value="0"/>
   <mirrorVertical type="value" value="0"/>
   <lockHorizontal type="value" value="0"/>
   <lockVertical type="value" value="0"/>
   <hideHorizontalDecoration type="value" value="0"/>
   <hideVerticalDecoration type="value" value="0"/>
   <handleSize type="value" value="32"/>
   <horizontalHandlePosition type="value" value="64"/>
   <verticalHandlePosition type="value" value="64"/>
   <axisPosition y="120" x="127" type="pointf"/>
  </MirrorAxis>
  <ColorHistory/>
  <Palettes/>
  <resources/>
  <animation>
   <framerate type="value" value="24"/>
   <range type="timerange" to="100" from="0"/>
   <currentTime type="value" value="0"/>
  </animation>
 </IMAGE>
</DOC>
)";
  public:
    ~MockArchive() override = default;

    std::error_code readFile(std::string_view filePath, std::span<std::byte> buffer) override
    {
        std::memcpy(buffer.data(), MAIN_DOC_XML.data(), MAIN_DOC_XML.size());
        return std::error_code{};
    }

    std::size_t getFileSize(std::string_view filePath) const override
    {
        return MAIN_DOC_XML.size();
    }
};

TEST_CASE("kra_imp Importer", "[importer test]")
{
    MockArchive archive;
    kra_imp::Importer importer{ archive };
    kra_imp::XmlParser parser;
    auto [result, mainDocOpt] = importer.parseMainDoc(parser);
    REQUIRE(!result);
}
