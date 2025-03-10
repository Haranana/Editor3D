#ifndef VIEWPORTDISPLAY_H
#define VIEWPORTDISPLAY_H

#include "Rendering/Color.h"

class ViewportDisplay{
public:
    enum DisplayMode{
      WIREFRAME,
      SOLID,
      TEXTURED
    };


    DisplayMode displayMode = WIREFRAME;
    Color color = Color(128,128,128);
    Color wireframeColor = Color(200,110,170);

};

#endif // VIEWPORTDISPLAY_H
