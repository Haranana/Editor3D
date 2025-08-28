#include "Rendering/ClippingManager.h"

double ClippingManager::planeValue(ClippingManager::PlaneType planeType, const Vector4& v){
    switch(planeType){
    case LEFT:
        return v.x + v.w;
        break;
    case RIGHT:
        return v.w - v.x;
        break;
    case TOP:
        return v.w - v.y;
        break;
    case BOTTOM:
        return v.y + v.w;
        break;
    case NEAR:
        return v.w + v.z;
        break;
    case FAR:
        return v.w - v.z;
        break;
    }
}

ClippingManager::ClippedVertex ClippingManager::interpolateClippedVertices(const ClippedVertex& v1, const ClippedVertex& v2, PlaneType planeType) {

    double za = planeValue(planeType,v1.clip);
    double zb = planeValue(planeType,v2.clip);
    double t = za / (za - zb);

    return {
        v1.clip      + (v2.clip      - v1.clip)      * t,
        v1.invW      + (v2.invW      - v1.invW)      * t,
        v1.worldSpaceVertexOverW  + (v2.worldSpaceVertexOverW  - v1.worldSpaceVertexOverW)  * t,
        v1.worldSpaceNormalOverW   + (v2.worldSpaceNormalOverW   - v1.worldSpaceNormalOverW)   * t,
        v1.colorOverW + (v2.colorOverW   - v1.colorOverW)   * t,
        v1.uvOverW + (v2.uvOverW - v1.uvOverW)*t,
        v1.hasUV && v2.hasUV // jeżeli oba końce miały UV, punkt pośredni też ma
    };
}

bool ClippingManager::isVectorInsideScreen(const Vector4& v){
    for(const auto &curPlane : planes){
        if( planeValue(curPlane, v) < 0.0){
            return false;
        }
    }
    return true;
}

bool ClippingManager::isVectorInsideScreen(ClippingManager::PlaneType planeType, const Vector4& v){
    return planeValue(planeType, v) >= 0.0;
}

Vector4 ClippingManager::intersect(const Vector4& v1, const Vector4& v2, ClippingManager::PlaneType planeType)
{
    double fa = planeValue(planeType,v1);
    double fb = planeValue(planeType,v2);
    double t  = fa / (fa - fb);

    return Vector4{ v1.x + t*(v2.x - v1.x),
                   v1.y + t*(v2.y - v1.y),
                   v1.z + t*(v2.z - v1.z),
                   v1.w + t*(v2.w - v1.w) };
}

std::pair<Vector4, Vector4> ClippingManager::clipLine(const std::pair<Vector4, Vector4>& e){
    std::pair<Vector4, Vector4> result = e;

    for(const auto &curPlane : planes){
        const Vector4& v1 = result.first;
        const Vector4& v2 = result.second;

        bool v1Inside = isVectorInsideScreen(curPlane, v1);
        bool v2Inside = isVectorInsideScreen(curPlane, v2);

        if (v1Inside && v2Inside){
           // return std::make_pair(v1, v2);
            result = std::make_pair(v1, v2);
        }else if(v1Inside){
           // return std::make_pair(v1, intersect(v1,v2, curPlane));
            result = std::make_pair(v1, intersect(v1,v2, curPlane));
        }else if(v2Inside){
           // return std::make_pair(intersect(v1,v2, curPlane), v2);
            result = std::make_pair(intersect(v1,v2, curPlane), v2);
        }else{
            return std::make_pair(Vector4(-1,-1,-1,-1), Vector4(-1,-1,-1,-1));
        }

    }
    return result;
}

std::vector<Vector4> ClippingManager::clipTriangle(const std::vector<Vector4>& triangle){
    std::vector<Vector4> input(triangle);
    std::vector<Vector4> result;
    for(const auto &curPlane : planes){
        std::vector<Vector4> out;
        if(input.empty()) break;
        for(size_t curVertexId = 0; curVertexId < input.size(); curVertexId++){
            const Vector4& v1 = input[curVertexId];
            const Vector4& v2 = input[(curVertexId+1)%input.size()];

            bool v1Inside = isVectorInsideScreen(curPlane, v1);
            bool v2Inside = isVectorInsideScreen(curPlane, v2);

            if (v1Inside && v2Inside){
                out.push_back(v2);
            }else if(v1Inside){
                out.push_back(intersect(v1,v2, curPlane));
            }else if(v2Inside){
                out.push_back(intersect(v1,v2, curPlane));
                out.push_back(v2);

            }else{
            }

        }
        input = out;
    }
    return input;
}

//Sutheran-hodgman
std::vector<ClippingManager::ClippedVertex> ClippingManager::clipTriangle(const std::vector<ClippedVertex>& triangle){
    std::vector<ClippedVertex> input(triangle);
    std::vector<ClippedVertex> result;
    for(const auto &curPlane : planes){
        std::vector<ClippedVertex> out;
        if(input.empty()) break;
        for(size_t curVertexId = 0; curVertexId < input.size(); curVertexId++){
            const ClippedVertex& v1 = input[curVertexId];
            const ClippedVertex& v2 = input[(curVertexId+1)%input.size()];

            bool v1Inside = isVectorInsideScreen(curPlane, v1.clip);
            bool v2Inside = isVectorInsideScreen(curPlane, v2.clip);

            if (v1Inside && v2Inside){
                out.push_back(v2);
            }else if(v1Inside){
                out.push_back(interpolateClippedVertices(v1,v2, curPlane));
            }else if(v2Inside){
                out.push_back(interpolateClippedVertices(v1,v2, curPlane));
                out.push_back(v2);

            }else{
            }

        }
        input = out;
    }
    return input;
}
