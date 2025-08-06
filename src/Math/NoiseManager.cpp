#include "Math/NoiseManager.h"
#include "Math/Utility.h"

std::vector<Vector2> NoiseManager::poissonOffset8;
std::vector<Vector2> NoiseManager::poissonOffset12;
std::vector<Vector2> NoiseManager::poissonOffset16;

std::vector<Vector2> NoiseManager::getPoissonOffset8(){
    if(poissonOffset8.empty()){
        std::vector<Vector2> offsets = generatePoissonDisk( defaultSmallMinDistance, defaultMaxTries);
        if (offsets.size() > 8)
            offsets.resize(8);
        NoiseManager::poissonOffset8 = std::move(offsets);
    }
    return poissonOffset8;
}

std::vector<Vector2> NoiseManager::getPoissonOffset12(){
    if(poissonOffset12.empty()){
        std::vector<Vector2> offsets = generatePoissonDisk( defaultSmallMinDistance, defaultMaxTries);
        if (offsets.size() > 12)
            offsets.resize(12);
       NoiseManager::poissonOffset12 = std::move(offsets);
    }
    return poissonOffset12;
}
std::vector<Vector2> NoiseManager::getPoissonOffset16(){
    if(poissonOffset16.empty()){
        std::vector<Vector2> offsets = generatePoissonDisk( defaultSmallMinDistance, defaultMaxTries);
        if (offsets.size() > 16)
            offsets.resize(16);
        NoiseManager::poissonOffset16 = std::move(offsets);
    }
    return poissonOffset16;
}

//Bridson's algorithm
std::vector<Vector2> NoiseManager::generatePoissonDisk(double minDistanceBeetwenPoints, int maxTries){

    //constants initialization
    const double gridCellSize = minDistanceBeetwenPoints / sqrt(2);
    const double planeMin = -1.0;
    const double planeMax = 1.0;
    const int gridCellsAmount = std::ceil((planeMax - planeMin)/gridCellSize);
    std::queue<Vector2>activePoints;
    std::vector<Vector2>points;
    std::random_device rd;
    std::mt19937 gen(rd());
    struct GridCell{
        bool isFull = false;
        int pointId = -1;

        void fill(int id){
            isFull = true;
            pointId = id;
        }
    };


    //create grid
    //true - there's already a point there, false - there isn't one
    Buffer<GridCell> pointsGrid(gridCellsAmount,gridCellsAmount,GridCell());
    Vector2 centralPoint = Vector2( MathUt::randomInRangeHard(planeMin, planeMax, gen), MathUt::randomInRangeHard(planeMin, planeMax, gen) );

    activePoints.push(centralPoint);
    points.push_back(centralPoint);
    pointsGrid[(int)((centralPoint.y - planeMin)/gridCellSize)][(int)((centralPoint.x - planeMin)/gridCellSize)].fill(points.size()-1);

    while(!activePoints.empty()){
        centralPoint = activePoints.front();
        activePoints.pop();

        for(int step=0; step<maxTries; step++){
            //randomize Point in certain range from central point
            Vector2 randomizedPointRange = MathUt::randomPointInRing(centralPoint, minDistanceBeetwenPoints, gen);//r-2r from central
            //check if new Point can be added
            int randomizedPointGridXId = (int)((randomizedPointRange.x - planeMin)/gridCellSize);
            int randomizedPointGridYId = (int)((randomizedPointRange.y - planeMin)/gridCellSize);

            bool pointIsCorrect = true;
            for(int row = randomizedPointGridYId-1; row <= randomizedPointGridYId+1; row++){
                for(int col = randomizedPointGridXId-1; col <= randomizedPointGridXId+1; col++){
                    if(pointsGrid.exists(row , col) && pointsGrid[row][col].isFull &&
                        MathUt::euclideanDistance(randomizedPointRange , points[pointsGrid[row][col].pointId]) < minDistanceBeetwenPoints){
                        pointIsCorrect = false;
                        break;
                    }
                }
                if(!pointIsCorrect) break;
            }
            if(!pointIsCorrect) continue;

            activePoints.push(randomizedPointRange);
            points.push_back(randomizedPointRange);
            pointsGrid[(int)((randomizedPointRange.y - planeMin)/gridCellSize)][(int)((randomizedPointRange.x - planeMin)/gridCellSize)].fill(points.size()-1);
            break;
        }
    }

    return points;
}
