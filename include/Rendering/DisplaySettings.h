#ifndef DISPLAYSETTINGS_H
#define DISPLAYSETTINGS_H

class DisplaySettings{
public:
    enum class RenderMode{
        NONE,
        WIREFRAME,
        RASTER,

    };

    enum class RasterMode{
        NONE,
        RASTER_COLOR,
        RASTER_TEXTURE,
    };

    enum class ShadingMode{
        NONE,
        FLAT_SHADING,
        SMOOT_SHADING
    };

    bool hideUnseenWireframes = false; //not implemented!
    bool colorWireframes = true;
    RenderMode renderMode = RenderMode::WIREFRAME;
    RasterMode rasterMode = RasterMode::NONE ;
    ShadingMode shadingMode = ShadingMode::FLAT_SHADING;
};

#endif // DISPLAYSETTINGS_H
