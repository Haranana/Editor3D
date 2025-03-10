#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <vector>
#include <Math/Vector3.h>
#include "Math/Transform3D.h"
#include "Rendering/RenderMethod.h"
#include <memory>
#include <Rendering/ViewportDisplay.h>
class RenderStrategy;
class BasicRenderStrategy;

class Object3D{
public:

    virtual ~Object3D() = default;

    Transform3D transform;

    ViewportDisplay viewportDisplay;

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
