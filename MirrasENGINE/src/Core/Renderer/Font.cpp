#include "Core/Renderer/Font.h"

#include "Core/Renderer/Texture.h"
#include "Core/Log.h"

#include <msdf/msdf-atlas-gen.h>

namespace mirras
{
    template <typename T, int N>
    static single_ref<Texture> createAtlasTexture(msdfgen::BitmapConstRef<T, N> bitmap)
    {
        TextureSpecs specs {
            .data = bitmap.pixels,
            .width = bitmap.width,
            .height = bitmap.height,
            .channels = N,
            .mipmaps = 1
        };

        return Texture::createFrom(specs);
    }

    Font::Font(const fs::path& fontFilepath)
    {
        msdfgen::FreetypeHandle* ftHandle = msdfgen::initializeFreetype();

        if(!ftHandle) // Not critical to the engine, so we don't need to terminate
        {
            ENGINE_LOG_ERROR("Could not initialize FreeType");
            return;
        }

        msdfgen::FontHandle* font = msdfgen::loadFont(ftHandle, fontFilepath.string().c_str());

        if(!font)
        {
            ENGINE_LOG_ERROR("Failed to load font: {}", fontFilepath.string());
            return;
        }

        // Unicode blocks can be found at https://en.wikipedia.org/wiki/Unicode_block#List_of_blocks
        static const std::pair<uint32, uint32> availableRanges[]
        {
            {0x0020, 0x00FF}, // Basic Latin & Latin-1 Supplement
            //{0x0370, 0x03FF}, // Greek & Coptic
        };

        msdf_atlas::Charset charset;

        for(auto[begin, end] : availableRanges)
        {
            for(uint32 codepoint = begin; codepoint <= end; ++codepoint)
                charset.add(codepoint);
        }

        geometry = instantiate<msdf_atlas::FontGeometry>();

        const double fontScale = 1.0;
        const double maxCornerAngle = 3.0;

        geometry->loadCharset(font, fontScale, charset);
        auto& glyphs = geometry->getGlyphStorage();

        // Apply MSDF edge coloring
        for(msdf_atlas::GlyphGeometry& glyph : glyphs)
        {
            glyph.getShape().inverseYAxis = true;
            glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);
        }

        msdf_atlas::TightAtlasPacker packer;
        packer.setDimensionsConstraint(msdf_atlas::DimensionsConstraint::SQUARE);
        packer.setScale(50.0);
        //packer.setMinimumScale(24.0);
        packer.setPixelRange(2.0);
        packer.setMiterLimit(1.0);
        
        int32 result = packer.pack(glyphs.data(), glyphs.size()); // Compute atlas layout and pack glyphs

        int32 width{}, height{}; // Before the bugfix in the FontGeometry constructor, sometimes result was equal 0 (success),
        packer.getDimensions(width, height); // but width and height were still 0, so we do this extra check just in case

        if(result != 0 || !(width > 0 && height > 0))
        {
            ENGINE_LOG_ERROR("Failed to compute the font atlas layout and pack the glyphs");
            geometry.reset();
            return;
        }

        constexpr int32 channels = 3; // MSDF atlas type has 3 channels

        msdf_atlas::ImmediateAtlasGenerator<float, channels,
            msdf_atlas::msdfGenerator,
            msdf_atlas::BitmapAtlasStorage<msdfgen::byte, channels>
        > generator(width, height);
        
        msdf_atlas::GeneratorAttributes attributes;
        attributes.config.overlapSupport = true;
        attributes.scanlinePass = true;

        generator.setAttributes(attributes);
        generator.setThreadCount(4);

        // Generate atlas bitmap
        generator.generate(glyphs.data(), glyphs.size());

        atlasTexture = createAtlasTexture<msdfgen::byte, channels>(generator.atlasStorage());

        // TODO: save atlas with the font file name, and load it if available, instead of regenerating it all over
        msdfgen::savePng(generator.atlasStorage(), "fontAtlas.png");

        msdfgen::destroyFont(font);
        msdfgen::deinitializeFreetype(ftHandle);
    }

    Font::~Font() = default;
} // namespace mirras
