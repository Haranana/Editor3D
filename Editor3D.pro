# My3DEditor.pro

######################################################################
# 1. Ustawienia Qt
######################################################################
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Kompilujemy z użyciem C++17:
CONFIG += c++17

######################################################################
# 2. Ścieżki do plików źródłowych (SOURCES) i nagłówkowych (HEADERS)
######################################################################
# Tutaj dodajemy wszystkie pliki .cpp, które chcemy skompilować.
# Możesz wymienić je jeden po drugim lub użyć wzorców (np. *.cpp).
# Poniżej jako przykład wpisano main.cpp i mainwindow.cpp
# - a także pliki z Math, Rendering, itp. jeśli je masz.
# W praktyce dołączysz wszystkie .cpp, które składają się na projekt.

SOURCES += \
    src/Math/Matrices.cpp \
    src/Math/Matrix3.cpp \
    src/Math/Matrix4.cpp \
    src/Math/Transform3D.cpp \
    src/Math/Vector2.cpp \
    src/Math/Vector3.cpp \
    src/Math/Vector4.cpp \
    src/Math/Vectors.cpp \
    src/Rendering/BasicRenderStrategy.cpp \
    src/Rendering/ClippingManager.cpp \
    src/Rendering/Color.cpp \
    src/Rendering/Colors.cpp \
    src/Rendering/FrontOnlyRenderStrategy.cpp \
    src/Rendering/GridRenderStrategy.cpp \
    src/Rendering/HitDetectionManager.cpp \
    src/Rendering/LinePainter.cpp \
    src/Rendering/PixelPainter.cpp \
    src/Rendering/RasterRenderStrategy.cpp \
    src/Rendering/Renderer.cpp \
    src/Rendering/RenderingSurface.cpp \
    src/Rendering/ShadingManager.cpp \
    src/Scene/Camera.cpp \
    src/Scene/Cube.cpp \
    src/Scene/Cylinder.cpp \
    src/Scene/Grid.cpp \
    src/Scene/Object3D.cpp \
    src/Scene/ObjectLoader.cpp \
    src/Scene/ObjectSaver.cpp \
    src/Scene/RenderableObject3D.cpp \
    src/Scene/Scene.cpp \
    src/UI/ColorPicker.cpp \
    src/UI/ColorPreview.cpp \
    src/UI/ImageLabel.cpp \
    src/UI/RenderWidget.cpp \
    src/UI/mainwindow.cpp \
    src/main.cpp


# Analogicznie do HEADERS dodajemy wszystkie pliki .h
HEADERS += \
    include/Math/Matrices.h \
    include/Math/Matrix3.h \
    include/Math/Matrix4.h \
    include/Math/Transform3D.h \
    include/Math/UVProjector.h \
    include/Math/Vector2.h \
    include/Math/Vector3.h \
    include/Math/Vector4.h \
    include/Math/Vectors.h \
    include/Rendering/BasicRenderStrategy.h \
    include/Rendering/Buffer.h \
    include/Rendering/ClippingManager.h \
    include/Rendering/Color.h \
    include/Rendering/Colors.h \
    include/Rendering/DisplaySettings.h \
    include/Rendering/FrontOnlyRenderStrategy.h \
    include/Rendering/GridRenderStrategy.h \
    include/Rendering/HitDetectionManager.h \
    include/Rendering/LinePainter.h \
    include/Rendering/PixelPainter.h \
    include/Rendering/RasterRenderStrategy.h \
    include/Rendering/RenderMethod.h \
    include/Rendering/RenderStrategy.h \
    include/Rendering/Renderer.h \
    include/Rendering/RenderingSurface.h \
    include/Rendering/ShadingManager.h \
    include/Rendering/Texture.h \
    include/Rendering/ViewportDisplay.h \
    include/Scene/Camera.h \
    include/Scene/Cube.h \
    include/Scene/Cylinder.h \
    include/Scene/DistantLight.h \
    include/Scene/Grid.h \
    include/Scene/Light.h \
    include/Scene/Object3D.h \
    include/Scene/ObjectLoader.h \
    include/Scene/ObjectSaver.h \
    include/Scene/PointLight.h \
    include/Scene/RenderableObject3D.h \
    include/Scene/Scene.h \
    include/Scene/SpotLight.h \
    include/UI/ColorPicker.h \
    include/UI/ColorPreview.h \
    include/UI/ImageLabel.h \
    include/UI/RenderWidget.h \
    include/UI/mainwindow.h

######################################################################
# 3. Ustawienia include
######################################################################
# Dodajemy katalog, w którym Qt (i kompilator) będzie szukał
# plików nagłówkowych. $$PWD oznacza bieżący katalog projektu
# (tam gdzie znajduje się .pro). Dzięki temu w kodzie możemy pisać:
#    #include "UI/mainwindow.h"
# zamiast "include/UI/mainwindow.h" lub ścieżek względnych.

INCLUDEPATH += $$PWD/include

######################################################################
# 4. Domyślne reguły dla deploymentu (opcjonalne, QNX / Unix / Android).
######################################################################
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

######################################################################
# 5. Pliki dodatkowe (np. pliki konfiguracyjne, README, ikony itp.)
######################################################################
DISTFILES +=
