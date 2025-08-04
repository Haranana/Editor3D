#ifndef UTILITY_H
#define UTILITY_H
#include <algorithm>
#include <vector>
#include <Math/Vectors.h>
#include <random>
#include <cmath>
#include "Rendering/Buffer.h"
#include <queue>

namespace MathUt{
    constexpr double degreeToRadian(double degree){
        return (M_PI/180)*degree;
    }

    constexpr double radianToDegree(double radian){
        return (180/M_PI)*radian;
    }

    constexpr double euclideanDistance(const Vector2& v1, const Vector2& v2){
        return sqrt(pow(v1.x - v2.x , 2) + pow(v1.y - v2.y , 2));
    }

    constexpr float smoothstep(float edge0, float edge1, float x) {
        float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }

    double randomInRangeHard(double min, double max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(min, std::nextafter(max, std::numeric_limits<double>::max()));
        return dist(gen);
    }

    double randomInRangeHard(double min, double max, std::mt19937& gen) {
        std::uniform_real_distribution<double> dist(min, std::nextafter(max, std::numeric_limits<double>::max()));
        return dist(gen);
    }

    double randomInRangeSoft(double min, double max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(std::nextafter(min, std::numeric_limits<double>::max()), max);
        return dist(gen);
    }

    Vector2 randomPointInDistance(Vector2 center, double minDist, double maxDist,
                                double minX, double maxX, double minY, double maxY)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<double> xdist(minX, maxX);
        std::uniform_real_distribution<double> ydist(minY, maxY);

        while (true) {
            double x = xdist(gen);
            double y = ydist(gen);
            double dist = std::hypot(x - center.x, y - center.y);
            if (dist >= minDist && dist <= maxDist)
                return {x, y};
        }
    }

    Vector2 randomPointInRing(const Vector2& center, double r, std::mt19937& gen) {
        std::uniform_real_distribution<double> angleDist(0.0, 2.0*M_PI);
        std::uniform_real_distribution<double> rad2Dist(r*r, 4*r*r);
        double theta = angleDist(gen);
        double radius = std::sqrt(rad2Dist(gen));
        return { center.x + radius*std::cos(theta), center.y + radius*std::sin(theta) };
    }

    //Bridson's algorithm
    std::vector<Vector2> getRandomPoints(double minDistanceBeetwenPoints, int maxTries){

        //constants initialization
        const double gridCellSize = minDistanceBeetwenPoints / sqrt(2);
        const double planeMin = 0.0;
        const double planeMax = 1.0;
        const int gridCellsAmount = std::ceil(1.0/gridCellSize);
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
        Vector2 centralPoint = Vector2( randomInRangeHard(planeMin, planeMax, gen), randomInRangeHard(planeMin, planeMax, gen) );

        activePoints.push(centralPoint);
        points.push_back(centralPoint);
        pointsGrid[(int)(centralPoint.y/gridCellSize)][(int)(centralPoint.x/gridCellSize)].fill(points.size()-1);

        while(!activePoints.empty()){
            centralPoint = activePoints.front();
            activePoints.pop();

            for(int step=0; step<maxTries; step++){
                //randomize Point in certain range from central point
                Vector2 randomizedPointRange = randomPointInRing(centralPoint, minDistanceBeetwenPoints, gen);//r-2r from central
                //check if new Point can be added
                int randomizedPointGridXId = (int)(randomizedPointRange.x/gridCellSize);
                int randomizedPointGridYId = (int)(randomizedPointRange.y/gridCellSize);

                bool pointIsCorrect = true;
                for(int row = randomizedPointGridYId-1; row <= randomizedPointGridYId+1; row++){
                    for(int col = randomizedPointGridXId-1; col <= randomizedPointGridXId+1; col++){
                        if(pointsGrid.exists(row , col) && pointsGrid[row][col].isFull &&
                            euclideanDistance(randomizedPointRange , points[pointsGrid[row][col].pointId]) < minDistanceBeetwenPoints){
                            pointIsCorrect = false;
                            break;
                        }
                    }
                    if(!pointIsCorrect) break;
                }
                if(!pointIsCorrect) continue;

                activePoints.push(randomizedPointRange);
                points.push_back(randomizedPointRange);
                pointsGrid[(int)(randomizedPointRange.y/gridCellSize)][(int)(randomizedPointRange.x/gridCellSize)].fill(points.size()-1);
                break;
            }
        }

        return points;
    }


};
#endif // UTILITY_H
