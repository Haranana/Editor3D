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
        COLOR,
        TEXTURE,
    };

    enum class Shading{
        NONE,
        FLAT,
        GOURAUD,
        PHONG
    };

    enum class LightingModel{
        NONE,
        FACE_RATIO,
        LAMBERT,
    };

    bool hideUnseenWireframes = false; //not implemented!
    bool colorWireframes = false;
    RenderMode renderMode = RenderMode::WIREFRAME;
    RasterMode rasterMode = RasterMode::COLOR ;
    Shading shadingMode = Shading::PHONG;
    LightingModel lightingMode = LightingModel::FACE_RATIO;
};

#endif // DISPLAYSETTINGS_H
