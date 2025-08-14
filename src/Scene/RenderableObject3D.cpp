#include "Scene/RenderableObject3D.h"

RenderableObject3D::RenderableObject3D() {

}


//do zrobienia dla wszystkic obiektow
RenderableObject3D::RenderableObject3D(const RenderableObject3D& otherObject) :
    vertices(otherObject.vertices),
    transformedVertices(otherObject.transformedVertices),
    faceVertexIndices(otherObject.faceVertexIndices),
    normals(otherObject.normals),
    textureCoords(otherObject.textureCoords)
{
    this->transform = otherObject.transform;
    displaySettings = std::make_unique<DisplaySettings>();

    vertexHasNormal = otherObject.vertexHasNormal;
    vertexNormals = otherObject.vertexNormals;
    vertexHasUV = otherObject.vertexHasUV;
    faceHasUV = otherObject.faceHasUV;
}
