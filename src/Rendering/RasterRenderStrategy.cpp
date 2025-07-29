#include "Rendering/RasterRenderStrategy.h"
#include "Rendering/BasicRenderStrategy.h"
#include "Rendering/Renderer.h"
#include "Scene/RenderableObject3D.h"

/*
/* --+-- Lasciate ogni speranza, voi ch'entrate --+-- */
void RasterRenderStrategy::render(RenderableObject3D& obj,
                                  Renderer& renderer,
                                  int objId)
{

}

double RasterRenderStrategy::edgeFunction(const Vector3 &a, const Vector3 &b, const Vector3 &c) {
    return (c.x - a.x) * (b.y - a.y)
    - (c.y - a.y) * (b.x - a.x);
}
