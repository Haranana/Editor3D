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

    enum class DiffuseModel{
        NONE,
        LAMBERT
    };

    enum class SpecularModel{
        NONE,
        PHONG,
        BLINN_PHONG,
        COOK_TORRANCE
    };

    bool colorWireframes = false;
    bool backFaceCulling = true;
    RenderMode renderMode = RenderMode::RASTER_COLOR;
    Shading shadingMode = Shading::PHONG;

    DiffuseModel diffuseModel = DiffuseModel::LAMBERT;
    SpecularModel specularModel = SpecularModel::NONE;

    void print() const {
        std::cout << "DisplaySettings:" << std::endl;
        std::cout << "  colorWireframes:      " << (colorWireframes ? "true" : "false") << std::endl;
        std::cout << "  renderMode:           " << renderModeToString(renderMode) << std::endl;
        std::cout << "  shadingMode:          " << shadingModeToString(shadingMode) << std::endl;
        std::cout << "  specModel:         " << SpecularModelToString(specularModel) << std::endl;
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
    static const char* DiffuseModelToString(DiffuseModel m) {
        switch(m) {
        case DiffuseModel::NONE:       return "NONE";
        case DiffuseModel::LAMBERT:    return "LAMBERT";
        default:                       return "UNKNOWN";
        }
    }
    static const char* SpecularModelToString(SpecularModel m) {
        switch(m) {
        case SpecularModel::NONE:       return "NONE";
        case SpecularModel::PHONG:    return "PHONG";
        case SpecularModel::BLINN_PHONG:    return "BLINN_PHONG";
        case SpecularModel::COOK_TORRANCE:    return "COOK_TORRANCE";
        default:                       return "UNKNOWN";
        }
    }
};

#endif // DISPLAYSETTINGS_H
