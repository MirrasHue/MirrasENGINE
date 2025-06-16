#include "Core/Renderer/Font.h"

#include "Core/Renderer/Texture.h"
#include "Core/Log.h"

#include <msdf/msdf-atlas-gen.h>

#include <format>

namespace mirras
{
    Font::Font() = default;

    Font::Font(const fs::path& fontFilepath)
    {
        loadFrom(fontFilepath);
    }

    template <typename T, int N>
    static single_ref<Texture> createAtlasTexture(const msdfgen::BitmapConstRef<T, N>& bitmap)
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

    struct FreetypeHandleDeleter
    {
        void operator()(msdfgen::FreetypeHandle* handle) const
        {
            msdfgen::deinitializeFreetype(handle);
        }
    };

    struct FontHandleDeleter
    {
        void operator()(msdfgen::FontHandle* handle) const
        {
            msdfgen::destroyFont(handle);
        }
    };

    // Use std::unique_ptr instead of single_ref when a custom deleter is needed
    using FreetypeHandle = std::unique_ptr<msdfgen::FreetypeHandle, FreetypeHandleDeleter>;
    using FontHandle     = std::unique_ptr<msdfgen::FontHandle, FontHandleDeleter>;

    bool Font::loadFrom(const fs::path& fontFilepath)
    {
        FreetypeHandle ftHandle{msdfgen::initializeFreetype()};

        if(!ftHandle) // Not critical to the engine, so we don't need to terminate
        {
            ENGINE_LOG_ERROR("Could not initialize FreeType");
            return false;
        }

        FontHandle fontHandle{msdfgen::loadFont(ftHandle.get(), fontFilepath.string().c_str())};

        if(!fontHandle)
        {
            ENGINE_LOG_ERROR("Failed to load font: {}", fontFilepath.string());
            return false;
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
        atlasTexture.reset();

        const double fontScale = 1.0;
        const double maxCornerAngle = 3.0;

        int32 loadedGlyphs = geometry->loadCharset(fontHandle.get(), fontScale, charset);

        if(loadedGlyphs <= 0) // -1 is a valid return value
        {
            ENGINE_LOG_ERROR("Failed to load glyphs specified in the charset");
            geometry.reset();

            return false;
        }

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

        int32 width{}, height{};
        packer.getDimensions(width, height);

        // Before the bugfix in the FontGeometry constructor, sometimes result was equal 0 (success), but width and
        if(result != 0 || !(width > 0 && height > 0)) // height were still 0, so we do this extra check just in case
        {
            ENGINE_LOG_ERROR("Failed to compute the font atlas layout when packing the glyphs");
            geometry.reset();

            return false;
        }

        auto atlasPath = fontFilepath;
        atlasPath.replace_extension();
        atlasPath += std::format("-{}x{}_{}glyphs.png", width, height, loadedGlyphs);

        if(fs::exists(atlasPath))
        {
            atlasTexture = Texture::loadFrom(atlasPath);

            if(atlasTexture->id)
                return true;
            else
            {
                ENGINE_LOG_ERROR("Failed to load existing font atlas texture from {}. Generating a new one...", atlasPath.string());
            }
        }

        ENGINE_LOG_INFO("Generating font atlas texture...");
        
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

        if(atlasTexture->id == 0)
        {
            ENGINE_LOG_ERROR("Failed to create font atlas texture from the generated bitmap");
            geometry.reset();
            atlasTexture.reset();

            return false;
        }

        if(!msdfgen::savePng(generator.atlasStorage(), atlasPath.string().c_str(), /*flipVertically =*/ true))
        {
            ENGINE_LOG_ERROR("Failed to save the font atlas texture to {}", atlasPath.string());
        }

        return true;
    }

    Font::~Font() = default;
} // namespace mirras
