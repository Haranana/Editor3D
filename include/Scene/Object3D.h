#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <vector>
#include <unordered_set>
#include <Math/Vector3.h>
#include "Math/Transform3D.h"

class Object3D{
public:

    Transform3D transform;
    std::vector<Vector3>vertices;
    std::vector< std::pair< Vector3 , Vector3> >edges;

protected:

    /*refers to original position of some anchor Point of the object
    *(for symmetrical Objects  it will probably refer to its middle)
    * For almost all cases it will remain as (0,0,0)
    */
    Vector3 truePosition = Vector3(0,0,0);

    /*refers to position in which the object will be created at
     * in other words its default parameters of Translation matrix of object's Transform3D object
     * For most cases it will probably be set as middle of RenderingSurface
     * Set as (0,0,0) as a placeholder, to be changed later in code production
     */
    const Vector3 startingPosition = Vector3(0,0,0);

};

#endif // OBJECT3D_H
