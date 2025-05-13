#include "Rendering/BasicRenderStrategy.h"
#include "Scene/Object3D.h"
#include "Rendering/Renderer.h"
#include "Scene/Camera.h"
#include "Rendering/RenderingSurface.h"
#include "Rendering/LinePainter.h"
#include "Math/Vectors.h"
#include "Rendering/BasicRenderStrategy.h"
#include "Rendering/Renderer.h"
#include "Scene/RenderableObject3D.h"

void BasicRenderStrategy::render(RenderableObject3D& object, Renderer& renderer)
{
    /*
    std::shared_ptr<Camera> camera = renderer.getCamera();
    std::shared_ptr<RenderingSurface> renderingSurface = renderer.getRenderingSurface();

    LinePainter linePainter(renderingSurface->getImg());

    Matrix4 modelMatrix = object.transform.getTransMatrix();
    Matrix4 viewMatrix  = camera->getViewMatrix();
    Matrix4 modelView   = viewMatrix * modelMatrix;

    // obliczanie transformedVertices obiektu
    for(int vertexIt = 0; vertexIt < static_cast<int>(object.vertices.size()); vertexIt++){
        // Mnożenie przez macierz transformacji (pozycja, rotacja, skalowanie)
        auto vert4 = Vectors::vector3to4(object.vertices[vertexIt]);
        Vector4 transformed4 = modelView * vert4;
        object.transformedVertices[vertexIt] = Vectors::vector4to3(transformed4);
    }

    // rysowanie krawedzi dla kazdej trojki wierzcholkow
    for(int i = 0; i < static_cast<int>(object.faceVertexIndices.size()); i += 3){

        int i1 = object.faceVertexIndices[i];
        int i2 = object.faceVertexIndices[i+1];
        int i3 = object.faceVertexIndices[i+2];

        Vector3 v1 = object.transformedVertices[i1];
        Vector3 v2 = object.transformedVertices[i2];
        Vector3 v3 = object.transformedVertices[i3];

        // projekcja perspektywiczna
        Vector2 p1(
            (v1.x * camera->getFov()) / (camera->getFov() + v1.z),
            (v1.y * camera->getFov()) / (camera->getFov() + v1.z)
            );
        Vector2 p2(
            (v2.x * camera->getFov()) / (camera->getFov() + v2.z),
            (v2.y * camera->getFov()) / (camera->getFov() + v2.z)
            );
        Vector2 p3(
            (v3.x * camera->getFov()) / (camera->getFov() + v3.z),
            (v3.y * camera->getFov()) / (camera->getFov() + v3.z)
            );

        // przesuniecie do środka ekranu
        p1 = p1 + renderingSurface->getMiddle();
        p2 = p2 + renderingSurface->getMiddle();
        p3 = p3 + renderingSurface->getMiddle();

        // rysowanie trojkata
        Vector3 p1_3(p1.x, p1.y, v1.z);
        Vector3 p2_3(p2.x, p2.y, v2.z);
        Vector3 p3_3(p3.x, p3.y, v3.z);

        //renderer.drawLine3D(p1_3,p2_3);
        //renderer.drawLine3D(p2_3,p3_3);
        //renderer.drawLine3D(p3_3,p1_3);

        renderer.drawLine3D(v1, v2, object.viewportDisplay.color);
        renderer.drawLine3D(v2, v3, object.viewportDisplay.color);
        renderer.drawLine3D(v3, v1, object.viewportDisplay.color);

        //linePainter.drawLine(p1, p2);
       // linePainter.drawLine(p2, p3);
        //linePainter.drawLine(p3, p1);
    }
    */

    std::vector<Vector4> clipVerts;
    clipVerts.reserve(object.vertices.size());
    for (auto& v : object.vertices)
        clipVerts.push_back(renderer.toClip(v, object.transform.getTransMatrix()));

    for (size_t i = 0; i < object.faceVertexIndices.size(); i += 3)
    {
        Vector4 c1 = clipVerts[object.faceVertexIndices[i]];
        Vector4 c2 = clipVerts[object.faceVertexIndices[i+1]];
        Vector4 c3 = clipVerts[object.faceVertexIndices[i+2]];

        // trivial‑reject poza frustum  (-w > x etc.)
        // lewa płaszczyzna
        if (c1.x < -c1.w && c2.x < -c2.w && c3.x < -c3.w) continue;
        // prawa
        if (c1.x >  c1.w && c2.x >  c2.w && c3.x >  c3.w) continue;
        // dół
        if (c1.y < -c1.w && c2.y < -c2.w && c3.y < -c3.w) continue;
        // góra
        if (c1.y >  c1.w && c2.y >  c2.w && c3.y >  c3.w) continue;
        // near (z> w)  & far (z< -w)
        if (c1.z >  c1.w && c2.z >  c2.w && c3.z >  c3.w) continue;
        if (c1.z < -c1.w && c2.z < -c2.w && c3.z < -c3.w) continue;

        // 3) divide by w  →  NDC
        auto ndc = [](const Vector4& c){ return Vector3(c.x/c.w, c.y/c.w, c.z/c.w); };
        Vector3 n1 = ndc(c1);
        Vector3 n2 = ndc(c2);
        Vector3 n3 = ndc(c3);

        // 4) viewport‑map - konwersja znormalizowanych dane na prawdziwe
        auto vp = [&](const Vector3& n){
            return Vector2(
                (n.x + 1) * 0.5 * renderer.getRenderingSurface()->getImg()->width(),
                (1 - (n.y + 1)*0.5) * renderer.getRenderingSurface()->getImg()->height() // y flip
                );
        };
        Vector2 s1 = vp(n1);
        Vector2 s2 = vp(n2);
        Vector2 s3 = vp(n3);

        // 5) rysuj linie (lub raster) – tutaj linie
        Vector3 l1(s1.x, s1.y, -c1.z / c1.w);   // depth = linear −z/w
        Vector3 l2(s2.x, s2.y, -c2.z / c2.w);
        Vector3 l3(s3.x, s3.y, -c3.z / c3.w);

        renderer.drawLine3D(l1, l2, object.viewportDisplay.color);
        renderer.drawLine3D(l2, l3, object.viewportDisplay.color);
        renderer.drawLine3D(l3, l1, object.viewportDisplay.color);
    }
}





