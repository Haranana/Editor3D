#include "Scene/RenderableObject3D.h"

RenderableObject3D::RenderableObject3D() {
    displaySettings = std::make_unique<DisplaySettings>();
    material = Material();
}


//do zrobienia dla wszystkic obiektow
RenderableObject3D::RenderableObject3D(const RenderableObject3D& otherObject) :
    vertices(otherObject.vertices),
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

void RenderableObject3D::sanitize(){
    removeDegenerateFaces();
    fixFaceNormals();
    fixVertexNormals();
}


void RenderableObject3D::removeDegenerateFaces(){
    const size_t faceCount = faceVertexIndices.size() / 3;
    if (faceCount == 0) return;

    std::vector<char> isDeg(faceCount, 0);
    size_t degCount = 0;

    for (size_t fi = 0; fi < faceCount; ++fi) {
        const int i0 = faceVertexIndices[3*fi + 0];
        const int i1 = faceVertexIndices[3*fi + 1];
        const int i2 = faceVertexIndices[3*fi + 2];

        if (i0 < 0 || i1 < 0 || i2 < 0 ||
            i0 >= (int)vertices.size() ||
            i1 >= (int)vertices.size() ||
            i2 >= (int)vertices.size()) {
            isDeg[fi] = 1; ++degCount; continue;
        }

        if (i0 == i1 || i1 == i2 || i2 == i0) {
            isDeg[fi] = 1; ++degCount; continue;
        }

        const Vector3& p0 = vertices[i0];
        const Vector3& p1 = vertices[i1];
        const Vector3& p2 = vertices[i2];

        const Vector3 e1 = p1 - p0;
        const Vector3 e2 = p2 - p0;
        const Vector3 n  = e1.crossProduct(e2);
        const double len2 = n.dotProduct(n);

        if (MathUt::equal(len2 , 0.0)){
            isDeg[fi] = 1;
            degCount++;
        }
    }

    if (degCount == 0)  return;


    const size_t goodFacesAmount = faceCount - degCount;
    std::vector<int>  newFaceVertexIndices;
    std::vector<bool> newFaceHasUV;

    newFaceVertexIndices.reserve(3 * goodFacesAmount);
    newFaceHasUV.reserve(goodFacesAmount);

    for (size_t fi = 0; fi < faceCount; ++fi) {
        if (isDeg[fi]) continue;
        newFaceVertexIndices.push_back(faceVertexIndices[3*fi + 0]);
        newFaceVertexIndices.push_back(faceVertexIndices[3*fi + 1]);
        newFaceVertexIndices.push_back(faceVertexIndices[3*fi + 2]);
        if (fi < faceHasUV.size()){
            newFaceHasUV.push_back(faceHasUV[fi]);
        }
    }

    faceVertexIndices.swap(newFaceVertexIndices);
    if (!faceHasUV.empty()){
        faceHasUV.swap(newFaceHasUV);
    }

    normals.clear();
    vertexToFaceNormals.clear();
}

void RenderableObject3D::fixFaceNormals(){
    const size_t facesAmount = faceVertexIndices.size() / 3;

    normals.resize(facesAmount);
    vertexToFaceNormals.assign(vertices.size(), {});
    faceHasUV.assign(facesAmount, false);

    for (size_t face = 0; face < facesAmount; face++) {

        int i0 = faceVertexIndices[3*face+0];
        int i1 = faceVertexIndices[3*face+1];
        int i2 = faceVertexIndices[3*face+2];
        const Vector3& p0 = vertices[i0];
        const Vector3& p1 = vertices[i1];
        const Vector3& p2 = vertices[i2];

        Vector3 fn = (p1 - p0).crossProduct(p2 - p0);
        double len2 = fn.dotProduct(fn);
        normals[face] = !MathUt::equal(len2, 0.0, 1e-20)  ? (fn / std::sqrt(len2)) : Vector3{0,0,0};

        vertexToFaceNormals[i0].push_back((int)face);
        vertexToFaceNormals[i1].push_back((int)face);
        vertexToFaceNormals[i2].push_back((int)face);

        bool uv0 = vertexHasUV[i0], uv1 = vertexHasUV[i1], uv2 = vertexHasUV[i2];
        faceHasUV[face] = (uv0 && uv1 && uv2);
    }
}

void RenderableObject3D::fixVertexNormals(){
    const double creaseDeg = 180.0;
    const bool useCrease = (creaseDeg < 179.9);
    const double cosThresh = std::cos(creaseDeg * M_PI / 180.0);

    std::vector<Vector3> accum(vertices.size(), Vector3{0,0,0});

    for (size_t f = 0; f < normals.size(); f++) {
        int i0 = faceVertexIndices[3*f+0];
        int i1 = faceVertexIndices[3*f+1];
        int i2 = faceVertexIndices[3*f+2];
        const Vector3& fn = normals[f];

        auto addIf = [&](int vi){
            if (!vertexHasNormal[vi]) {
                if (!useCrease) { accum[vi] = accum[vi] + fn; }
                else {

                    if (accum[vi].dotProduct(accum[vi]) < 1e-20 || (accum[vi].normalize().dotProduct(fn) >= cosThresh))
                        accum[vi] = accum[vi] + fn;
                }
            }
        };
        addIf(i0); addIf(i1); addIf(i2);
    }

    vertexNormals.resize(vertices.size());
    for (size_t i=0; i<vertices.size(); ++i) {
        if (!vertexHasNormal[i]) {
            vertexNormals[i] = accum[i].normalize();
            vertexHasNormal[i] = true;
        } else {
            vertexNormals[i] = vertexNormals[i].normalize();
        }
    }
}
