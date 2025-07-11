#include "Rendering/Renderer.h"



Renderer::Renderer(
    //std::shared_ptr<RenderingSurface> renderingSurface,
    std::shared_ptr<QImage> img,
    std::shared_ptr<Scene> scene,
    std::shared_ptr<Camera> camera) :
    renderingSurface(std::make_shared<RenderingSurface>(img)) ,
    scene(scene),
    camera(camera)
{
    zBuffer = std::make_shared<Buffer<float>>(img->width(), img->height(), std::numeric_limits<float>::infinity());
    idBuffer = std::make_shared<Buffer<IdBufferElement>>(img->width(), img->height(), IdBufferElement{});

    pixelPainter = std::make_shared<PixelPainter>(renderingSurface->getImg());
    linePainter = std::make_shared<LinePainter>(renderingSurface->getImg());
    clippingManager = std::make_shared<ClippingManager>();
}

void Renderer::renderScene(){
    idBuffer->clear();
    zBuffer->clear();

    viewMatrix = camera->getViewMatrix();
    ProjectionMatrix = camera->getProjectionMatrix();
    viewProjectionMatrix = ProjectionMatrix * viewMatrix;

    renderSceneObjects();
    highlightObjectsSelectedElements();
}

Vector4 Renderer::modelToClip(const Vector3& v, const Matrix4& modelMatrix){
    Vector4 result = camera->getProjectionMatrix() * camera->getViewMatrix() * modelMatrix * Vector4(v.x,v.y,v.z, 1.0);
    return result;
}

Vector4 Renderer::worldToClip(const Vector3& v){
    Vector4 result = camera->getProjectionMatrix() * camera->getViewMatrix() * Vector4(v.x,v.y,v.z, 1.0);
    return result;
}

Vector3 Renderer::modelToWorld(const Vector3& v, const Matrix4& modelMatrix){
    Vector4 resultHom = modelMatrix * Vector4(v.x,v.y,v.z, 1.0);
    Vector3 result = Vector3(resultHom.x, resultHom.y, resultHom.z);
    return result;
}

Vector3 Renderer::worldToCamera(const Vector3& v){
    Vector4 resultHom = camera->getViewMatrix() * Vector4(v.x,v.y,v.z, 1.0);
    Vector3 result = Vector3(resultHom.x, resultHom.y, resultHom.z);
    return result;
}

Vector3 Renderer::clipToNdc(const Vector4& v){
    return Vector3(v.x/v.w, v.y/v.w, v.z/v.w);
}

Vector2 Renderer::ndcToScreen(const Vector3& v){
    return Vector2(
        (v.x + 1) * 0.5 * renderingSurface->getImg()->width(),
        (1 - (v.y + 1)*0.5) * renderingSurface->getImg()->height());
}

void Renderer::setRenderingSurface(std::shared_ptr<RenderingSurface> newRenderingSurface){
    renderingSurface = newRenderingSurface;
}

void Renderer::clearRenderingSurface(){
    pixelPainter->fillImage(Colors::Black);
}

void Renderer::setScene(std::shared_ptr<Scene> newScene){
    scene = newScene;
}

void Renderer::setCamera(std::shared_ptr<Camera> newCamera){
    camera = newCamera;
}

bool Renderer::drawPixel(int x, int y, double depth, const Color& color)
{
    //std::shared_ptr<std::vector<std::vector<float>>> zBuffer = getZBuffer();
    if (x < 0 || y < 0 || x >= renderingSurface->getImg()->width() || y >=renderingSurface->getImg()->height())
        return false;

    if (depth < (*zBuffer)[y][x]) {
        (*zBuffer)[y][x]   = depth;
        pixelPainter->drawPixel(Vector2(x,y),color);

        return true;
    }
    return false;
}


// debatowalne czy powinno byc to luzem w rendererze
void Renderer::drawLine3D(const Vector3& aScr, const Vector3& bScr,IdBufferElement& idBufferElement, const Color& color)
{

    IdBufferElement element = idBufferElement;
    bool isMock = element.mock;
    // aScr, bScr: x,y w pikselach, z = depth 0..1 (mniejsza = bliżej)
    int x0 = int(std::round(aScr.x));
    int y0 = int(std::round(aScr.y));
    int x1 = int(std::round(bScr.x));
    int y1 = int(std::round(bScr.y));

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    int steps = std::max(dx, dy);

    for (int i = 0, x = x0, y = y0; i <= steps; ++i)
    {
        double t = steps ? double(i) / steps : 0.0;
        double depth = aScr.z + t * (bScr.z - aScr.z);
        if( drawPixel(x, y, depth, color)){
            if(!isMock){
            if(i == 0 && element.edgeVertices.first != -1){
                element.vertexId = element.edgeVertices.first;
            }else if(i == steps && element.edgeVertices.second != -1){
                element.vertexId = element.edgeVertices.second;
            }else{
                element.vertexId = -1;
            }
            element.isEmpty = false;
            (*idBuffer)[y][x] = element;
            }
        }

        int e2 = err << 1;
        if (e2 > -dy) { err -= dy; x += sx; }
        if (e2 <  dx) { err += dx; y += sy; }
    }
}

void Renderer::drawCircle3D(const Vector3& v, IdBufferElement& idBufferElement, int radius, const Color& color){
    int curX = 0;
    int curY= 0;
    int loopEnd = radius;
    IdBufferElement element = idBufferElement;

    for (curX= 0; curX <= loopEnd;curX++) {
        curY = sqrt(radius * radius- curX * curX);

        if(drawPixel(curX + v.x, curY + v.y , v.z, color)){
            (*idBuffer)[static_cast<int>(curY + v.y)][static_cast<int>(curX + v.x)] = element;
        }
        if(drawPixel(curX + v.x, -curY + v.y , v.z, color)){
            (*idBuffer)[static_cast<int>(-curY + v.y)][static_cast<int>(curX + v.x)] = element;
        }
        if(drawPixel(-curX + v.x, curY + v.y , v.z, color)){
            (*idBuffer)[static_cast<int>(curY + v.y)][static_cast<int>(-curX + v.x)] = element;
        }
        if(drawPixel(-curX + v.x, -curY + v.y , v.z, color)){
            (*idBuffer)[static_cast<int>(-curY + v.y)][static_cast<int>(-curX + v.x)] = element;
        }

        if(drawPixel(curY + v.x, curX + v.y , v.z, color)){
            (*idBuffer)[static_cast<int>(curX + v.x)][static_cast<int>(curY + v.y)] = element;
        }
        if(drawPixel(curY + v.x, -curX + v.y , v.z, color)){
            (*idBuffer)[static_cast<int>(-curX + v.x)][static_cast<int>(curY + v.y)] = element;
        }
        if(drawPixel(-curY + v.x, curX + v.y , v.z, color)){
            (*idBuffer)[static_cast<int>(curX + v.x)][static_cast<int>(-curY + v.y)] = element;
        }
        if(drawPixel(-curY + v.x, -curX + v.y , v.z, color)){
            (*idBuffer)[static_cast<int>(-curX + v.x)][static_cast<int>(-curY + v.y)] = element;
        }
    }
}


// debatowalne czy powinno byc to luzem w rendererze
void Renderer::drawLine3D(const Vector3& aScr, const Vector3& bScr, const Color& color)
{

    // aScr, bScr: x,y w pikselach, z = depth 0..1 (mniejsza = bliżej)
    int x0 = int(std::round(aScr.x));
    int y0 = int(std::round(aScr.y));
    int x1 = int(std::round(bScr.x));
    int y1 = int(std::round(bScr.y));

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    int steps = std::max(dx, dy);

    for (int i = 0, x = x0, y = y0; i <= steps; ++i)
    {
        double t = steps ? double(i) / steps : 0.0;
        double depth = aScr.z + t * (bScr.z - aScr.z);
        drawPixel(x, y, depth, color);

        int e2 = err << 1;
        if (e2 > -dy) { err -= dy; x += sx; }
        if (e2 <  dx) { err += dx; y += sy; }
    }
}

void Renderer::drawCircle3D(const Vector3& v, int radius, const Color& color){
    int curX = 0;
    int curY= 0;
    int loopEnd = radius;

    for (curX= 0; curX <= loopEnd;curX++) {
        curY = sqrt(radius * radius- curX * curX);

        drawPixel(curX + v.x, curY + v.y , v.z, color);
        drawPixel(curX + v.x, -curY + v.y , v.z, color);
        drawPixel(-curX + v.x, curY + v.y , v.z, color);
        drawPixel(-curX + v.x, -curY + v.y , v.z, color);
        drawPixel(curY + v.x, curX + v.y , v.z, color);
        drawPixel(curY + v.x, -curX + v.y , v.z, color);
        drawPixel(-curY + v.x, curX + v.y , v.z, color);
        drawPixel(-curY + v.x, -curX + v.y , v.z, color);
    }
}

void Renderer::drawSquare3D(const Vector3& v,int radius, const Color& color){
    int startX = v.x - radius/2;
    int endX = v.x + radius/2;
    int startY = v.y - radius/2;
    int endY = v.y + radius/2;

    for(int curY = startY; curY < endY; curY++){
        for(int curX = startX; curX < endX; curX++){
            drawPixel(curX, curY, v.z, color);
        }
    }
}

std::shared_ptr<Camera> Renderer::getCamera(){
    return camera;
}

std::shared_ptr<Scene> Renderer::getScene(){
    return scene;
}

std::shared_ptr<RenderingSurface> Renderer::getRenderingSurface(){
    return renderingSurface;
}


void Renderer::renderSceneObjects(){
    LinePainter linePainter = LinePainter(renderingSurface->getImg());

    for(int objIt = scene->objectsAmount()-1 ; objIt >= 0 ; objIt-- ){

        std::shared_ptr<Object3D> object = scene->getObject(objIt);
        if(RenderableObject3D* curObject = dynamic_cast<RenderableObject3D*>(object.get())){
            if(curObject->renderStrategy){

                curObject->renderStrategy->render(*curObject , *this, objIt);
            }
        }
    }
}

void Renderer::highlightObjectsSelectedElements(){
    for(int objIt = scene->objectsAmount()-1 ; objIt >= 0 ; objIt-- ){

        std::shared_ptr<Object3D> object = scene->getObject(objIt);
        if(RenderableObject3D* curObject = dynamic_cast<RenderableObject3D*>(object.get())){

            ViewportDisplay::SelectMode selectedElement = curObject->viewportDisplay.selectedPart;
            if(selectedElement == ViewportDisplay::NONE){
                continue;
            }
            else if(selectedElement == ViewportDisplay::VERTICES){
                if(curObject->viewportDisplay.selectedVertex == -1) continue;
                Vector3 modelV = curObject->vertices[curObject->viewportDisplay.selectedVertex];
                Vector4 clipV = modelToClip(modelV , curObject->transform.getTransMatrix());
                if(!clippingManager->isVectorInsideScreen(clipV)) continue;
                Vector3 ndcV = clipToNdc(clipV);
                Vector2 screenV = ndcToScreen(ndcV);
                Vector3 screenDepthV = Vector3(screenV.x , screenV.y, ((clipV.z/clipV.w)+1.0)*0.5 -1e-4); //-1e-4 is for bias, please document future me
                //IdBufferElement element;
                //element.objectId = objIt;
                //element.vertexId = curObject->viewportDisplay.selectedVertex;
                drawSquare3D(screenDepthV , /*element*/ 5, curObject->viewportDisplay.selectColor);
                /*
                Vector3 renderedV = Vector3(
                ndcToScreen(clipToNdc(modelToClip(curObject->vertices[curObject->viewportDisplay.selectedVertex] , curObject->transform.getTransMatrix()))).x,
                ndcToScreen(clipToNdc(modelToClip(curObject->vertices[curObject->viewportDisplay.selectedVertex] , curObject->transform.getTransMatrix()))).y,
                ((modelToClip(curObject->vertices[curObject->viewportDisplay.selectedVertex] , curObject->transform.getTransMatrix()).z /
                      modelToClip(curObject->vertices[curObject->viewportDisplay.selectedVertex] , curObject->transform.getTransMatrix()).w)+1.0)*0.5
                    );
                */

            }else if(selectedElement == ViewportDisplay::EDGES){
                if(curObject->viewportDisplay.selectedEdge == std::pair<int,int>(-1,-1)) continue;
                std::pair<Vector3 ,Vector3>modelE = std::make_pair(curObject->vertices[curObject->viewportDisplay.selectedEdge.first],
                                                               curObject->vertices[curObject->viewportDisplay.selectedEdge.second]);
                std::pair<Vector4 ,Vector4>clipE = std::make_pair(modelToClip(modelE.first , curObject->transform.getTransMatrix()),
                                                                   modelToClip(modelE.second , curObject->transform.getTransMatrix()));
                std::pair<Vector4 , Vector4> clippedE = clippingManager->clipLine(clipE);
                if(clippedE.first == Vector4(-1,-1,-1,-1) && clippedE.second == Vector4(-1,-1,-1,-1)) continue;
                std::pair<Vector3, Vector3> ndcE = std::make_pair(clipToNdc(clippedE.first), clipToNdc(clippedE.second));
                std::pair<Vector2, Vector2> screenE = std::make_pair(ndcToScreen(ndcE.first), ndcToScreen(ndcE.second));
                std::pair<Vector3, Vector3> screenDepthE = std::make_pair(
                    Vector3(screenE.first.x , screenE.first.y, ((clippedE.first.z/clippedE.first.w)+1.0)*0.5 -1e-4),
                    Vector3(screenE.second.x , screenE.second.y, ((clippedE.second.z/clippedE.second.w)+1.0)*0.5 -1e-4)
                    );
                drawLine3D(screenDepthE.first , screenDepthE.second, curObject->viewportDisplay.selectColor);


            }else if(selectedElement == ViewportDisplay::FACES){
                if(curObject->viewportDisplay.selectedFace ==-1) continue;
                std::cout<<"HIGHLIGHTING FACE"<<std::endl;
                const int width  = getRenderingSurface()->getImg()->width();
                const int height = getRenderingSurface()->getImg()->height();

                std::vector<Vector4> clip;
                Matrix4 M = curObject->transform.getTransMatrix();

                    int selectedFace = curObject->viewportDisplay.selectedFace;
                    //original vertices in clip-spaces
                    Vector4 cv0 = modelToClip(curObject->vertices[curObject->faceVertexIndices[3*selectedFace]], M);
                    Vector4 cv1 = modelToClip(curObject->vertices[curObject->faceVertexIndices[3*selectedFace+1]], M);
                    Vector4 cv2 = modelToClip(curObject->vertices[curObject->faceVertexIndices[3*selectedFace+2]], M);

                    auto clippedPoly =
                        clippingManager->clipTriangle({cv0, cv1, cv2});
                    if (clippedPoly.size() < 3) continue;

                    // clip -> ndc
                    std::vector<Vector3> normalizedVertices;
                    normalizedVertices.reserve(clippedPoly.size());
                    for (auto& cv : clippedPoly)
                        normalizedVertices.push_back(clipToNdc(cv));


                    // ndc -> screen
                    std::vector<Vector2> screenVertices;
                    screenVertices.reserve(normalizedVertices.size());
                    for (auto& nv : normalizedVertices)
                        screenVertices.push_back(ndcToScreen(nv));


                    // preparing data
                    // screenDepthVertices = {screen.x , screen.y, normalized.z}; normalization in range [0,1]
                    std::vector<Vector3> screenDepthVertices;
                    std::vector<double> clipZ;
                    std::vector<double> invW;
                    screenDepthVertices.reserve(clippedPoly.size());

                    for (size_t i = 0; i < clippedPoly.size(); ++i) {
                        auto& sv = screenVertices[i];
                        auto& cv = clippedPoly[i];

                        clipZ.push_back(cv.z);
                        invW.push_back(1.0 / cv.w);

                        double normalizedDepth = ((cv.z/cv.w)+1.0)*0.5;
                        screenDepthVertices.push_back({ sv.x, sv.y, normalizedDepth });
                    }

                    // d) Fill-pass: Twoja oryginalna pętla barycentryczna
                    for (size_t k = 1; k + 1 < clippedPoly.size(); ++k)
                    {
                        // Fan-triangulation
                        auto& A = screenVertices[0];
                        auto& B = screenVertices[k];
                        auto& C = screenVertices[k+1];

                        // bounding box for rasterization
                        // beyond this rectangle there's no need to make further checks
                        int minX = std::max(0, int(std::floor(std::min({A.x,B.x,C.x}))));
                        int maxX = std::min(width-1, int(std::ceil(std::max({A.x,B.x,C.x}))));
                        int minY = std::max(0, int(std::floor(std::min({A.y,B.y,C.y}))));
                        int maxY = std::min(height-1, int(std::ceil(std::max({A.y,B.y,C.y}))));


                        double area = (B.x - A.x)*(C.y - A.y)
                                      - (B.y - A.y)*(C.x - A.x);
                        if (area == 0.0) continue;
                        double invArea = 1.0 / area;

                        for (int y = minY; y <= maxY; ++y) {
                            for (int x = minX; x <= maxX; ++x) {

                                double px = x + 0.5, py = y + 0.5;

                                double w0 = ((B.x-px)*(C.y-py) - (B.y-py)*(C.x-px)) * invArea;
                                double w1 = ((C.x-px)*(A.y-py) - (C.y-py)*(A.x-px)) * invArea;
                                double w2 = 1.0 - w0 - w1;
                                if (w0<0 || w1<0 || w2<0) continue;

                                double num   = w0 * clipZ[0] * invW[0]
                                             + w1 * clipZ[k] * invW[k]
                                             + w2 * clipZ[k+1] * invW[k+1];

                                double denom = w0 * invW[0]
                                               + w1 * invW[k]
                                               + w2 * invW[k+1];

                                double depth = num/denom;

                                drawPixel(x, y, depth-1e-4, curObject->viewportDisplay.selectColor);

                            }
                        }
                    }
            }
        }
    }

}
