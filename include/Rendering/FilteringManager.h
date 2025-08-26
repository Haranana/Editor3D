#ifndef FILTERINGMANAGER_H
#define FILTERINGMANAGER_H
#include "Rendering/Buffer.h"
#include "Math/Vector2.h"
#include "Math/NoiseManager.h"
#include <algorithm>
#include <cmath>
#include "Scene/Light.h"

class FilteringManager{
public:
    static double pcf3x3(const Buffer<double>&shadowMap, const Vector2& shadowMapCoords, double distance, double bias){
        int shadowedTexels = 0;
        int texelsInRange = 0;

        int begY = std::max(0.0, std::floor(shadowMapCoords.y)-1);
        int begX = std::max(0.0, std::floor(shadowMapCoords.x)-1);
        int endY = std::min((int)shadowMap.getRows()-1 ,(int)std::floor(shadowMapCoords.y)+1);
        int endX = std::min((int)shadowMap.getCols()-1 ,(int)std::floor(shadowMapCoords.x)+1);

        for(int shadowMapY = begY ; shadowMapY <= endY; shadowMapY++){
            for(int shadowMapX = begX ; shadowMapX <= endX; shadowMapX++){
                texelsInRange++;
                if(shadowMap[shadowMapY][shadowMapX] + bias < distance) shadowedTexels++;
            }
        }
        return texelsInRange == 0 ? 0.0 : static_cast<double>(shadowedTexels) / texelsInRange;
    }

    static double pcf5x5(const Buffer<double>&shadowMap, const Vector2& shadowMapCoords, double distance, double bias){
        int shadowedTexels = 0;
        int texelsInRange = 0;

        int begY = std::max(0.0, std::floor(shadowMapCoords.y)-2);
        int begX = std::max(0.0, std::floor(shadowMapCoords.x)-2);
        int endY = std::min((int)shadowMap.getRows()-1 ,(int)std::floor(shadowMapCoords.y)+2);
        int endX = std::min((int)shadowMap.getCols()-1 ,(int)std::floor(shadowMapCoords.x)+2);

        for(int shadowMapY = begY ; shadowMapY <= endY; shadowMapY++){
            for(int shadowMapX = begX ; shadowMapX <= endX; shadowMapX++){
                texelsInRange++;
                if(shadowMap[shadowMapY][shadowMapX] + bias < distance) shadowedTexels++;
            }
        }
        return texelsInRange == 0 ? 0.0 : static_cast<double>(shadowedTexels) / texelsInRange;
    }

    static double bilinearFiltering(const Buffer<double>&shadowMap, const Vector2& shadowMapCoords, double distance, double bias){

        const int leftUpX = std::clamp(int(std::floor(shadowMapCoords.x)), 0, (int)shadowMap.width()-1);
        const int leftUpY = std::clamp(int(std::floor(shadowMapCoords.y)), 0, (int)shadowMap.height()-1);
        const int rightDownX = std::min(leftUpX + 1, (int)shadowMap.width()-1);
        const int rightDownY = std::min(leftUpY + 1, (int)shadowMap.height()-1);

        const double u = shadowMapCoords.x - leftUpX;
        const double v = shadowMapCoords.y - leftUpY;

        auto depthCheck = [&](int cx,int cy){
            return (shadowMap[cy][cx] + bias < distance) ? 1.0 : 0.0;
        };

        const double result00 = depthCheck(leftUpX,leftUpY);
        const double result10 = depthCheck(rightDownX,leftUpY);
        const double result11 = depthCheck(rightDownX,rightDownY);
        const double result01 = depthCheck(leftUpX,rightDownY);

        return (1-u)*(1-v)*result00 + u*(1-v)*result10 + u*v*result11 + (1-u)*v*result01;

    }

    static double pcfPoisson(const Buffer<double>&shadowMap, const Vector2& shadowMapCoords, double distance, double bias,
                             int offsetSize = 8, double texelSize = 1.0/2048.0, double kernelRadius = 1.5){


        std::vector<Vector2> offset;
        if(offsetSize<=8){
            offset = NoiseManager::getPoissonOffset8();
        }else if(offsetSize<=12){
            offset = NoiseManager::getPoissonOffset12();
        }else{
            offset = NoiseManager::getPoissonOffset16();
        }

        int shadowedTexels = 0;
        int texelsInRange = 0;
        for(size_t i = 0; i < offset.size(); i++){
            /*
            Vector2 testUV = Vector2(offset[i].x * texelSize * kernelRadius,
                                     offset[i].y * texelSize * kernelRadius);
            */

            const int shadowMapCoordX = std::clamp(int(std::floor(shadowMapCoords.x + offset[i].x * kernelRadius)), 0, (int)shadowMap.width()-1);
            const int shadowMapCoordY = std::clamp(int(std::floor(shadowMapCoords.y + offset[i].y * kernelRadius)), 0, (int)shadowMap.height()-1);

            if(shadowMap.exists(shadowMapCoordY,shadowMapCoordX)){
                texelsInRange++;
                if(shadowMap[shadowMapCoordY][shadowMapCoordX] + bias < distance){
                    shadowedTexels++;
                }
            }
        }

        return texelsInRange == 0 ? 0.0 : static_cast<double>(shadowedTexels) / texelsInRange;
    }

    static inline int kernelSideFor(const Light& light){
        using FT = Light::FilteringType;
        switch (light.filteringType){
        case FT::BILINEAR:    return 2;             // 2x2
        case FT::PCF_3x3:     return 3;
        case FT::PCF_5x5:     return 5;
        case FT::PCF_POISSON: {
            int r = std::max(1, (int)std::ceil(light.pcfPoissonRadiusTexels));
            return 2*r + 1;  //
        }
        case FT::PCSS:        return 5;             // placeholder
        case FT::NONE:
        default:              return 1;             // 1x1 single-tap
        }
    }

private:

    std::pair<int,int>  ndcToPixel(const Vector2& ndc, int width, int height){
        int px = (ndc.x*0.5 + 0.5) * (width - 1);
        int py = (1.0 - (ndc.y*0.5 + 0.5)) * (height - 1); // flip Y

        return std::pair<int,int>(px,py);
    }
};

#endif // FILTERINGMANAGER_H
