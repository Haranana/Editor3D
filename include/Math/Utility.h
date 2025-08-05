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
        return (M_PI/180.0)*degree;
    }

    constexpr double radianToDegree(double radian){
        return (180.0/M_PI)*radian;
    }

    constexpr double euclideanDistance(const Vector2& v1, const Vector2& v2){
        return std::sqrt( (v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y));
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
};
#endif // UTILITY_H
