#include "Rendering/PaintTool.h"


PaintTool::PaintTool(std::shared_ptr<QImage> img, std::shared_ptr<Buffer<double>> zBuffer):
    img(img), zBuffer(zBuffer) {}

PaintTool::PaintTool(std::shared_ptr<QImage> img, std::shared_ptr<Buffer<double>> zBuffer, std::shared_ptr<Buffer<IdBufferElement>> idBuffer):
    img(img), zBuffer(zBuffer), idBuffer(idBuffer) {}

void PaintTool::drawLine3D(const Vector3& aScr, const Vector3& bScr,
                          IdBufferElement& idBufferElement, const Color& color)
{
    IdBufferElement element = idBufferElement;
    bool isMock = element.mock;

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

void PaintTool::drawLine3D(const Vector3& aScr, const Vector3& bScr, const Color& color)
{
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

void PaintTool::drawCircle3D(const Vector3& v, IdBufferElement& idBufferElement, int radius, const Color& color){
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

void PaintTool::drawCircle3D(const Vector3& v, int radius, const Color& color){
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

void PaintTool::drawSquare3D(const Vector3& v,int radius, const Color& color){
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

bool PaintTool::drawPixel(int x, int y, double depth, const Color& color)
{
    if (x < 0 || y < 0 || x >= img->width() || y >=img->height())
        return false;

    if (depth < (*zBuffer)[y][x]) {
        (*zBuffer)[y][x]   = depth;
        drawPixel(Vector2(x,y),color);

        return true;
    }
    return false;
}

void PaintTool::drawPixel(Vector2 v , Color color){
    int x = v.x;
    int y = v.y;

    if (x >= 0 && x < img->width() && y >= 0 && y < img->height()) {
        unsigned char *ptr = img->bits();
        int bytesPerLine = img->bytesPerLine();
        int idx = bytesPerLine * y + x * 4;
        ptr[idx] = color.B;
        ptr[idx + 1] = color.G;
        ptr[idx + 2] = color.R;
        ptr[idx + 3] = 255;
    }
}
