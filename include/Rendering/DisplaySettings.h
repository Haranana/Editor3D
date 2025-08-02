#ifndef DISPLAYSETTINGS_H
#define DISPLAYSETTINGS_H
#include <iostream>

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
    bool backFaceCulling = true;
    RenderMode renderMode = RenderMode::RASTER;
    RasterMode rasterMode = RasterMode::COLOR ;
    Shading shadingMode = Shading::NONE;
    LightingModel lightingMode = LightingModel::LAMBERT;
    void print() const {
        std::cout << "DisplaySettings:" << std::endl;
        std::cout << "  hideUnseenWireframes: " << (hideUnseenWireframes ? "true" : "false") << std::endl;
        std::cout << "  colorWireframes:      " << (colorWireframes ? "true" : "false") << std::endl;
        std::cout << "  renderMode:           " << renderModeToString(renderMode) << std::endl;
        std::cout << "  rasterMode:           " << rasterModeToString(rasterMode) << std::endl;
        std::cout << "  shadingMode:          " << shadingModeToString(shadingMode) << std::endl;
        std::cout << "  lightingMode:         " << lightingModeToString(lightingMode) << std::endl;
    }
private:
    static const char* renderModeToString(RenderMode m) {
        switch(m) {
        case RenderMode::NONE:      return "NONE";
        case RenderMode::WIREFRAME: return "WIREFRAME";
        case RenderMode::RASTER:    return "RASTER";
        default:                    return "UNKNOWN";
        }
    }
    static const char* rasterModeToString(RasterMode m) {
        switch(m) {
        case RasterMode::NONE:    return "NONE";
        case RasterMode::COLOR:   return "COLOR";
        case RasterMode::TEXTURE: return "TEXTURE";
        default:                  return "UNKNOWN";
        }
    }
    static const char* shadingModeToString(Shading m) {
        switch(m) {
        case Shading::NONE:    return "NONE";
        case Shading::FLAT:    return "FLAT";
        case Shading::GOURAUD: return "GOURAUD";
        case Shading::PHONG:   return "PHONG";
        default:               return "UNKNOWN";
        }
    }
    static const char* lightingModeToString(LightingModel m) {
        switch(m) {
        case LightingModel::NONE:       return "NONE";
        case LightingModel::FACE_RATIO: return "FACE_RATIO";
        case LightingModel::LAMBERT:    return "LAMBERT";
        default:                       return "UNKNOWN";
        }
    }
};

#endif // DISPLAYSETTINGS_H
