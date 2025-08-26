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
    src/Math/NoiseManager.cpp \
    src/Math/Transform3D.cpp \
    src/Math/Triangle3.cpp \
    src/Math/Utility.cpp \
    src/Math/Vector2.cpp \
    src/Math/Vector3.cpp \
    src/Math/Vector4.cpp \
    src/Math/Vectors.cpp \
    src/Rendering/ClippingManager.cpp \
    src/Rendering/Color.cpp \
    src/Rendering/HitDetectionManager.cpp \
    src/Rendering/LinePainter.cpp \
    src/Rendering/PaintTool.cpp \
    src/Rendering/PixelPainter.cpp \
    src/Rendering/Renderer.cpp \
    src/Rendering/RenderingSurface.cpp \
    src/Rendering/ShadingManager.cpp \
    src/Scene/Camera.cpp \
    src/Scene/Cube.cpp \
    src/Scene/Cylinder.cpp \
    src/Scene/Object3D.cpp \
    src/Scene/ObjectImporter.cpp \
    src/Scene/ObjectLoader.cpp \
    src/Scene/ObjectSaver.cpp \
    src/Scene/RenderableObject3D.cpp \
    src/Scene/Scene.cpp \
    src/UI/CameraPropertiesWidget.cpp \
    src/UI/ColorPicker.cpp \
    src/UI/ColorPreview.cpp \
    src/UI/DisplaySettingsPropertiesWidget.cpp \
    src/UI/DistantLightPropertiesWidget.cpp \
    src/UI/ImageLabel.cpp \
    src/UI/LightFilteringPropertiesWidget.cpp \
    src/UI/LightPropertiesWidget.cpp \
    src/UI/MaterialPropertiesWidget.cpp \
    src/UI/PointLightPropertiesWidget.cpp \
    src/UI/RenderWidget.cpp \
    src/UI/RenderableObjectPropertiesWidget.cpp \
    src/UI/SpotLightPropertiesWidget.cpp \
    src/UI/Transform3DPropertiesWidget.cpp \
    src/UI/Vector3PropertiesWIdget.cpp \
    src/UI/mainwindow.cpp \
    src/main.cpp


# Analogicznie do HEADERS dodajemy wszystkie pliki .h
HEADERS += \
    include/Math/Matrices.h \
    include/Math/Matrix3.h \
    include/Math/Matrix4.h \
    include/Math/NoiseManager.h \
    include/Math/Transform3D.h \
    include/Math/Triangle.h \
    include/Math/Triangle3.h \
    include/Math/UVProjector.h \
    include/Math/Utility.h \
    include/Math/Vector2.h \
    include/Math/Vector3.h \
    include/Math/Vector4.h \
    include/Math/Vectors.h \
    include/Rendering/BiasManager.h \
    include/Rendering/Buffer.h \
    include/Rendering/ClippingManager.h \
    include/Rendering/Color.h \
    include/Rendering/Colors.h \
    include/Rendering/DisplaySettings.h \
    include/Rendering/FilteringManager.h \
    include/Rendering/HitDetectionManager.h \
    include/Rendering/IdBufferElement.h \
    include/Rendering/LinePainter.h \
    include/Rendering/PaintTool.h \
    include/Rendering/PixelPainter.h \
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
    include/Scene/Light.h \
    include/Scene/Material.h \
    include/Scene/MeshBuilder.h \
    include/Scene/MtlParser.h \
    include/Scene/Object3D.h \
    include/Scene/ObjectImporter.h \
    include/Scene/ObjectLoader.h \
    include/Scene/ObjectSaver.h \
    include/Scene/PointLight.h \
    include/Scene/RenderableObject3D.h \
    include/Scene/Scene.h \
    include/Scene/SpotLight.h \
    include/UI/CameraPropertiesWidget.h \
    include/UI/ColorPicker.h \
    include/UI/ColorPreview.h \
    include/UI/DisplaySettingsPropertiesWidget.h \
    include/UI/DistantLightPropertiesWidget.h \
    include/UI/FpsCounter.h \
    include/UI/HudOverlay.h \
    include/UI/ImageLabel.h \
    include/UI/LightFilteringPropertiesWidget.h \
    include/UI/LightPropertiesWidget.h \
    include/UI/MaterialPropertiesWidget.h \
    include/UI/ObjectPropertiesWidget.h \
    include/UI/PointLightPropertiesWidget.h \
    include/UI/RenderWidget.h \
    include/UI/RenderableObjectPropertiesWidget.h \
    include/UI/SpotLightPropertiesWidget.h \
    include/UI/Transform3DPropertiesWidget.h \
    include/UI/Vector3PropertiesWidget.h \
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
