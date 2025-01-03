#ifndef RENDERINGSURFACE_H
#define RENDERINGSURFACE_H

#include <QImage>
#include <Math/Vector2.h>
#include <memory>
/* Wrapper class for main QImage that's used to draw objects
 * Anything that's not 3D object should be written on some other surface
*/
class RenderingSurface{
public:
    RenderingSurface(std::shared_ptr<QImage> img);
    std::shared_ptr<QImage> getImg();
    Vector2 getMiddle();

private:
    std::shared_ptr<QImage> img;
};

#endif // RENDERINGSURFACE_H
