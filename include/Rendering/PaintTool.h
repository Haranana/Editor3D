#ifndef PAINTTOOL_H
#define PAINTTOOL_H
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Colors.h"
#include "Buffer.h"
#include "IdBufferElement.h"
#include <QImage>

class PaintTool{
public:

    PaintTool(std::shared_ptr<QImage> img, std::shared_ptr<Buffer<double>> zBuffer);
    PaintTool(std::shared_ptr<QImage> img, std::shared_ptr<Buffer<double>> zBuffer, std::shared_ptr<Buffer<IdBufferElement>>);

    void drawLine3D(const Vector3& v1,  const Vector3& c2, IdBufferElement& idBufferElement, const Color& color = defaultDrawColor);
    void drawLine3D(const Vector3& v1,  const Vector3& v2, const Color& color = defaultDrawColor);
    void drawCircle3D(const Vector3& v, IdBufferElement& idBufferElement,int radius, const Color& color = defaultDrawColor);
    void drawCircle3D(const Vector3& v,int radius, const Color& color = defaultDrawColor);
    void drawSquare3D(const Vector3& v,int radius, const Color& color = defaultDrawColor);
    bool drawPixel(int x, int y, double z, const Color& color = defaultDrawColor); //returns false if pixel couldn't be overwritten do to zBuffer, return true otherwise
    void drawPixel(Vector2 v , Color color = defaultDrawColor);

    void fillImage(Color color);
    void copyImg(std::shared_ptr<QImage>source , std::shared_ptr<QImage>dest);

private:

    Color getColorFromPoint(const Vector2& vector);

    static constexpr Color defaultDrawColor = Colors::White;

    std::shared_ptr<QImage> img;
    std::shared_ptr<Buffer<double>> zBuffer;
    std::shared_ptr<Buffer<IdBufferElement>> idBuffer;

};

#endif // PAINTTOOL_H
