#ifndef FILTERINGMANAGER_H
#define FILTERINGMANAGER_H
#include "Rendering/Buffer.h"
#include "Math/Vector2.h"
#include "Math/NoiseManager.h"
#include <algorithm>
#include <cmath>

class FilteringManager{

    static double pcf3x3(const Buffer<double>&shadowMap, const Vector2& shadowMapUV, double distance, double bias){
        int shadowedTexels = 0;
        int texelsInRange = 0;

        int begY = std::max(0.0, std::floor(shadowMapUV.y)-1);
        int begX = std::max(0.0, std::floor(shadowMapUV.x)-1);
        int endY = std::min((int)shadowMap.getRows()-1 ,(int)std::floor(shadowMapUV.y)+1);
        int endX = std::min((int)shadowMap.getCols()-1 ,(int)std::floor(shadowMapUV.x)+1);

        for(int shadowMapY = begY ; shadowMapY <= endY; shadowMapY++){
            for(int shadowMapX = begX ; shadowMapX <= endX; shadowMapX++){
                texelsInRange++;
                if(shadowMap[shadowMapY][shadowMapX] + bias < distance) shadowedTexels++;
            }
        }
        return texelsInRange == 0 ? 0.0 : static_cast<double>(shadowedTexels) / texelsInRange;
    }

    static double pcf5x5(const Buffer<double>&shadowMap, const Vector2& shadowMapUV, double distance, double bias){
        int shadowedTexels = 0;
        int texelsInRange = 0;

        int begY = std::max(0.0, std::floor(shadowMapUV.y)-2);
        int begX = std::max(0.0, std::floor(shadowMapUV.x)-2);
        int endY = std::min((int)shadowMap.getRows()-1 ,(int)std::floor(shadowMapUV.y)+2);
        int endX = std::min((int)shadowMap.getCols()-1 ,(int)std::floor(shadowMapUV.x)+2);

        for(int shadowMapY = begY ; shadowMapY <= endY; shadowMapY++){
            for(int shadowMapX = begX ; shadowMapX <= endX; shadowMapX++){
                texelsInRange++;
                if(shadowMap[shadowMapY][shadowMapX] + bias < distance) shadowedTexels++;
            }
        }
        return texelsInRange == 0 ? 0.0 : static_cast<double>(shadowedTexels) / texelsInRange;
    }

    static bool bilinearFiltering(const Buffer<double>&shadowMap, const Vector2& shadowMapUV, double distance, double bias){
        int shadowMapLastRow = shadowMap.getRows()-1;
        int shadowMapLastCol = shadowMap.getCols()-1;
        Vector2 leftUpTexel = Vector2(std::max(0.0, std::floor(shadowMapUV.x)) , std::max(0.0, std::floor(shadowMapUV.y)) );
        Vector2 rightUpTexel = Vector2(std::min(shadowMapLastCol, (int)std::ceil(shadowMapUV.x)) , std::max(0.0, std::floor(shadowMapUV.y)) );
        Vector2 leftDownTexel = Vector2(std::max(0.0, std::floor(shadowMapUV.x)) , std::min(shadowMapLastCol, (int)std::ceil(shadowMapUV.y)) );
        Vector2 rightDownTexel = Vector2(std::max(shadowMapLastCol, (int)std::ceil(shadowMapUV.x)) ,std::max(shadowMapLastRow, (int)std::ceil(shadowMapUV.y)) );

        const double u = shadowMapUV.x - std::floor(shadowMapUV.x);
        const double v = shadowMapUV.y - std::floor(shadowMapUV.y);
        double interpolatedUVDepthValue = (1-u)*(1-v)*shadowMap[(int)leftUpTexel.y][(int)leftUpTexel.x]
                                          + u*(1-v)*shadowMap[(int)rightUpTexel.y][(int)rightUpTexel.x]
                                          + u*v*shadowMap[(int)rightDownTexel.y][(int)rightDownTexel.x]
                                          + (1-u)*v*shadowMap[(int)leftDownTexel.y][(int)leftDownTexel.x];

        return (interpolatedUVDepthValue + bias < distance);

    }

    static double pcfPoisson(const Buffer<double>&shadowMap, const Vector2& shadowMapUV, double distance, double bias,
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
            Vector2 testUV = Vector2(offset[i].x * texelSize * kernelRadius,
                                     offset[i].y * texelSize * kernelRadius);

            double u = ((testUV.x + shadowMapUV.x)+1.0)/2.0;
            double v = ((testUV.y + shadowMapUV.y)+1.0)/2.0;

            int shadowMapCoordX = u * shadowMap.getCols();
            int shadowMapCoordY = v * shadowMap.getRows();
            if(shadowMap.exists(shadowMapCoordY,shadowMapCoordX)){
                texelsInRange++;
                if(shadowMap[shadowMapCoordY][shadowMapCoordX] + bias < distance){
                    shadowedTexels++;
                }
            }
        }

        return texelsInRange == 0 ? 0.0 : static_cast<double>(shadowedTexels) / texelsInRange;
    }
};

#endif // FILTERINGMANAGER_H
