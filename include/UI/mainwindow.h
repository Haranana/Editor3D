#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QSpinBox>
#include <memory>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include <QVariant>
#include <QHeaderView>
#include <QColorDialog>
#include <QMenuBar>
#include <QMenu>
#include <QActionGroup>
#include <QFileDialog>
#include "UI/ImageLabel.h"

#include "Math/UVProjector.h"
#include "Scene/Scene.h"
#include "Scene/Object3D.h"
#include "Scene/Cube.h"
#include "Scene/Cylinder.h"
#include "Rendering/Renderer.h"
#include "Rendering/RenderingSurface.h"
#include "Scene/Camera.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Rendering/PixelPainter.h"
#include "UI/ColorPicker.h"
#include "Scene/ObjectLoader.h"
#include "Scene/ObjectSaver.h"
#include "ObjectPropertiesWidget.h"
#include "RenderableObjectPropertiesWidget.h"
#include "DistantLightPropertiesWidget.h"
#include "PointLightPropertiesWidget.h"
#include <QScrollArea>
#include "SpotLightPropertiesWidget.h"
#include "Scene/ObjectImporter.h"
#include "FpsCounter.h"
#include "HudOverlay.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:

    enum SelectMode{
        OBJECTS,
        FACES,
        EDGES,
        VERTICES,
        NONE
    };

    static constexpr double transUpLimit = 500;
    static constexpr double transDownLimit = -500;


    void setupUI();
    void setupMenuBar();


    void setupScene();
    void refreshScene();
    void loadTestScene();

    void distantLightTestScene();
    void pointLightTestScene();
    void simpleTestScene(); //single cube with no lighting
    void pointLightRoomTestScene();
    void pointLightShadowTestScene();
    void drawOverlayPerFrame();

    void addImportedObjectsToScene(const ImportResult& import, const QString& sourcePath);

    QVBoxLayout* rightLayout;
    QWidget* rightPanel;
    FpsCounter fpsCounter;

    std::shared_ptr<Scene> scene;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Renderer> renderer;
    std::shared_ptr<ObjectLoader> objectLoader;
    std::shared_ptr<ObjectSaver> objectSaver;
    std::shared_ptr<Object3D> currentObject = nullptr;

    ObjectPropertiesWidget* currentPropertiesWidget = nullptr;

    QScrollArea* propertiesWidgetScrollArea;

    QWidget* centralWidgetContainer = nullptr;
    QListWidget* objectsList = nullptr;
    ImageLabel* sceneDisplay = nullptr;
    ColorPicker* colorPicker = nullptr;
    QTreeWidget* objectParametersPropertyTree = nullptr;
    QComboBox* objectDisplayModeComboBox = nullptr;
    QColorDialog* objectDisplayColor = nullptr;

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

    QMenuBar* menuBar = nullptr;

    QPushButton* addCubeButton = nullptr;
    SelectMode curSelectMode = NONE;
    ObjImporter objImporter;

private slots:

    void onAddCubeClicked();
    void onObjectSelected();

    void onFileMenuSaveObject();
    void onFileMenuImportObject();

    void onObjectMenuCreateCube();
    void onObjectMenuCreateCylinder();
    void onObjectMenuCreateDistantLight();
    void onObjectMenuCreateSpotLight();
    void onObjectMenuCreatePointLight();

    void onSceneMenuDeleteSelectedObject();
    void onSceneMenuAssignTectureToObject();

    void onSelectMenuChangeSelectMode(SelectMode newSelectMode);
    void onSceneDisplayClicked(int x, int y);
};



#endif // MAINWINDOW_H
