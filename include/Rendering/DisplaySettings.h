#ifndef DISPLAYSETTINGS_H
#define DISPLAYSETTINGS_H
#include <iostream>

class DisplaySettings{
public:
    enum class RenderMode{
        NONE,
        WIREFRAME,
        RASTER_COLOR,
        RASTER_TEXTURE
    };

    enum class Shading{
        FLAT,
        GOURAUD,
        PHONG
    };

    enum class LightingModel{
        UNLIT,
        LAMBERT,
        PHONG,
        BLINN_PHONG,
        COOK_TORRANCE
    };

    bool hideUnseenWireframes = false; //not implemented!
    bool colorWireframes = false;
    bool backFaceCulling = true;
    RenderMode renderMode = RenderMode::RASTER_COLOR;
    Shading shadingMode = Shading::PHONG;
    LightingModel lightingMode = LightingModel::LAMBERT;
    void print() const {
        std::cout << "DisplaySettings:" << std::endl;
        std::cout << "  hideUnseenWireframes: " << (hideUnseenWireframes ? "true" : "false") << std::endl;
        std::cout << "  colorWireframes:      " << (colorWireframes ? "true" : "false") << std::endl;
        std::cout << "  renderMode:           " << renderModeToString(renderMode) << std::endl;
        std::cout << "  shadingMode:          " << shadingModeToString(shadingMode) << std::endl;
        std::cout << "  lightingMode:         " << lightingModeToString(lightingMode) << std::endl;
    }
private:
    static const char* renderModeToString(RenderMode m) {
        switch(m) {
        case RenderMode::NONE:      return "NONE";
        case RenderMode::WIREFRAME: return "WIREFRAME";
        case RenderMode::RASTER_COLOR:    return "RASTER COLOR";
        case RenderMode::RASTER_TEXTURE:    return "RASTER TEXTURE";
        default:                    return "UNKNOWN";
        }
    }
    static const char* shadingModeToString(Shading m) {
        switch(m) {
        case Shading::FLAT:    return "FLAT";
        case Shading::GOURAUD: return "GOURAUD";
        case Shading::PHONG:   return "PHONG";
        default:               return "UNKNOWN";
        }
    }
    static const char* lightingModeToString(LightingModel m) {
        switch(m) {
        case LightingModel::UNLIT:       return "NONE";
        case LightingModel::LAMBERT:    return "LAMBERT";
        case LightingModel::PHONG:    return "PHONG";
        case LightingModel::BLINN_PHONG:    return "BLINN_PHONG";
        default:                       return "UNKNOWN";
        }
    }
};

#endif // DISPLAYSETTINGS_H
