#ifndef NOISEMANAGER_H
#define NOISEMANAGER_H
#include <vector>
#include "Math/Utility.h"
#include "Math/Vector2.h"
//no need for different seeds, in future remake to only do offset16, and use part of it for faster versions
class NoiseManager{
public:

    static std::vector<Vector2> getPoissonOffset8();
    static std::vector<Vector2> getPoissonOffset12();
    static std::vector<Vector2> getPoissonOffset16();

private:


    static std::vector<Vector2> generatePoissonDisk(double minDistanceBeetwenPoints=defaultMinDistance, int maxTries=defaultMaxTries);

    static std::vector<Vector2> poissonOffset8 ;
    static std::vector<Vector2> poissonOffset12 ;
    static std::vector<Vector2> poissonOffset16 ;

    constexpr static double defaultSmallMinDistance = 0.1;
    constexpr static double defaultMinDistance = 0.3;
    constexpr static int defaultMaxTries= 30;
};

#endif // NOISEMANAGER_H
