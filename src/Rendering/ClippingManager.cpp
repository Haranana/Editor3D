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
        return v.w - v.z;
        break;
    case FAR:
        return v.z + v.w;
        break;
    }
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

std::vector<Vector4> ClippingManager::clipTriangle(const std::vector<Vector4>& triangle){
    std::vector<Vector4> clippedPoly(triangle);
    for(const auto &curPlane : planes){
        //std::cout << "Plane " << ":\n";
        std::vector<Vector4> out;
        if(clippedPoly.empty()) break;
        for(size_t curVertexId = 0; curVertexId < clippedPoly.size(); curVertexId++){


            const Vector4& v1 = clippedPoly[curVertexId];
            const Vector4& v2 = clippedPoly[(curVertexId+1)%clippedPoly.size()];

            bool v1Inside = isVectorInsideScreen(curPlane, v1);
            bool v2Inside = isVectorInsideScreen(curPlane, v2);

            if (v1Inside && v2Inside){
                out.push_back(v2);
                //std::cout<<"BOTH"<<std::endl;
            }else if(v1Inside){
                out.push_back(intersect(v1,v2, curPlane));
                //std::cout<<"V1"<<std::endl;
            }else if(v2Inside){
                out.push_back(intersect(v1,v2, curPlane));
                out.push_back(v2);
                //std::cout<<"V2"<<std::endl;
            }else{
                //std::cout << "NONE" << std::endl;
            }


        }
        clippedPoly.swap(out);
    }
    return clippedPoly;
}
