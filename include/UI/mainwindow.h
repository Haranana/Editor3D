#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QSpinBox>
#include <memory>

#include "Scene/Scene.h"
#include "Scene/Object3D.h"
#include "Scene/Cube.h"
#include "Rendering/Renderer.h"
#include "Rendering/RenderingSurface.h"
#include "Scene/Camera.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Rendering/PixelPainter.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:

    void setupUI();
    void setupScene();
    void refreshScene();

    std::shared_ptr<Scene> scene;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<RenderingSurface> renderingSurface;
    std::shared_ptr<Renderer> renderer;

    std::shared_ptr<Object3D> currentObject = nullptr;

    QWidget* centralWidgetContainer = nullptr;
    QListWidget* objectsList = nullptr;
    QLabel* sceneDisplay = nullptr;

    QSlider* sceneObjectSliderPosX = nullptr;
    QSlider* sceneObjectSliderPosY = nullptr;
    QSlider* sceneObjectSliderPosZ = nullptr;
    QSpinBox* sceneObjectSpinPosX = nullptr;
    QSpinBox* sceneObjectSpinPosY = nullptr;
    QSpinBox* sceneObjectSpinPosZ = nullptr;

    QSlider* sceneObjectSliderScaleX = nullptr;
    QSlider* sceneObjectSliderScaleY = nullptr;
    QSlider* sceneObjectSliderScaleZ = nullptr;
    QDoubleSpinBox* sceneObjectSpinScaleX = nullptr;
    QDoubleSpinBox* sceneObjectSpinScaleY = nullptr;
    QDoubleSpinBox* sceneObjectSpinScaleZ = nullptr;

    QSlider* sceneObjectSliderRotX = nullptr;
    QSlider* sceneObjectSliderRotY = nullptr;
    QSlider* sceneObjectSliderRotZ = nullptr;
    QSpinBox* sceneObjectSpinRotX = nullptr;
    QSpinBox* sceneObjectSpinRotY = nullptr;
    QSpinBox* sceneObjectSpinRotZ = nullptr;

    QPushButton* addCubeButton = nullptr;

private slots:
    void onAddCubeClicked();
    void onObjectSelected();

    void onSceneObjectPosChangedSpin();
    void onSceneObjectPosChangedSlider();

    void onSceneObjectScaleChangedSpin();
    void onSceneObjectScaleChangedSlider();

    void onSCeneObjectRotChangedSpin();
    void onSCeneObjectRotChangedSlider();
};



#endif // MAINWINDOW_H
