#include "UI/mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QPixmap>
#include <QTimer>
#include "UI/CameraPropertiesWidget.h"
#include "Math/Utility.h"
#include "Scene/TestScenes.h"
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    setupUI();
    setupScene();
    loadTestScene();


    QTimer* timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &MainWindow::refreshScene);
    connect(sceneDisplay, &ImageLabel::pixelClicked,
            this, [&](int x, int y){
            onSceneDisplayClicked(x,y);
    });
    timer->start(30);
}

MainWindow::~MainWindow(){}

void MainWindow::setupUI()
{

    centralWidgetContainer = new QWidget(this);
    setCentralWidget(centralWidgetContainer);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidgetContainer);


    menuBar = new QMenuBar(this);
    QHBoxLayout* menuBarLayout = new QHBoxLayout(menuBar);
    QVBoxLayout* menuAndMainLayout = new QVBoxLayout();

    menuAndMainLayout->addLayout(menuBarLayout);
    menuAndMainLayout->addLayout(mainLayout);

    setupMenuBar();

    QWidget* leftPanel = new QWidget(this);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);

    objectsList = new QListWidget(leftPanel);

    leftLayout->addWidget(objectsList);
    leftPanel->setLayout(leftLayout);
    mainLayout->addWidget(leftPanel, 1);

    sceneDisplay = new ImageLabel(this);
    sceneDisplay->setFixedSize(800,600);
    sceneDisplay->setScaledContents(false);

    QWidget* centerPanel = new QWidget(this);
    QVBoxLayout* centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->addWidget(sceneDisplay);
    centerPanel->setLayout(centerLayout);
    mainLayout->addWidget(centerPanel, 3);

    propertiesWidgetScrollArea = new QScrollArea(this);
    propertiesWidgetScrollArea->setWidgetResizable(true);
    propertiesWidgetScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    propertiesWidgetScrollArea->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    rightLayout = new QVBoxLayout(propertiesWidgetScrollArea);

    rightPanel = new QWidget(propertiesWidgetScrollArea);
    rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(12,12,12,12);
    rightLayout->setSpacing(8);

    propertiesWidgetScrollArea->setWidget(rightPanel);
    mainLayout->addWidget(propertiesWidgetScrollArea, 2);

    connect(objectsList, &QListWidget::itemSelectionChanged, this, &MainWindow::onObjectSelected);
}

void MainWindow::setupMenuBar(){

    setMenuBar(menuBar);

    QMenu *fileMenu = menuBar->addMenu("File");
    QMenu *objectsMenu = menuBar->addMenu("Objects");
    QMenu *sceneMenu = menuBar->addMenu("Scene");
    QMenu *selectedObjectMenu = sceneMenu->addMenu("Sel. Object");
    QMenu *createObjectsMenu = objectsMenu->addMenu("Create");
    QMenu *createObjectsLightMenu = createObjectsMenu->addMenu("Light");

    QAction *openSceneProperties = new QAction("&Properties");
    QAction *importObjectAction = new QAction("&Import object");
    QAction *createObjectCube = new QAction("&Cube");
    QAction *createObjectCylinder = new QAction("&Cylinder");
    QAction *deleteSelectedObject = new QAction("&Delete");
    QAction *createDistantLight = new QAction("&Distant");
    QAction *createPointLight = new QAction("&Point");
    QAction *createSpotLight = new QAction("&Spot");

    fileMenu->addAction(importObjectAction);
     sceneMenu->addAction(openSceneProperties);

    createObjectsMenu->addAction(createObjectCube);
    createObjectsMenu->addAction(createObjectCylinder);
    createObjectsLightMenu->addAction(createDistantLight);
    createObjectsLightMenu->addAction(createPointLight);
    createObjectsLightMenu->addAction(createSpotLight);
    selectedObjectMenu->addAction(deleteSelectedObject);

    QObject::connect(openSceneProperties, &QAction::triggered, [&](){
        onOpenSceneProperties();
    });

    QObject::connect(importObjectAction, &QAction::triggered, [&](){
        onFileMenuImportObject();
    });

    QObject::connect(createDistantLight , &QAction::triggered , [&](){
        onObjectMenuCreateDistantLight();
    });

    QObject::connect(createPointLight , &QAction::triggered , [&](){
        onObjectMenuCreatePointLight();
    });

    QObject::connect(createSpotLight , &QAction::triggered , [&](){
        onObjectMenuCreateSpotLight();
    });

    QObject::connect(createObjectCube, &QAction::triggered, [&](){
        onObjectMenuCreateCube();
    });

    QObject::connect(createObjectCylinder, &QAction::triggered, [&](){
        onObjectMenuCreateCylinder();
    });

    QObject::connect(deleteSelectedObject, &QAction::triggered, [&](){
        onSceneMenuDeleteSelectedObject();
    });

}

void MainWindow::setupScene()
{
    scene = std::make_shared<Scene>();
    objectLoader = std::make_shared<ObjectLoader>();
    objectSaver = std::make_shared<ObjectSaver>();
    camera = std::make_shared<Camera>(800,800);

    auto img = std::make_shared<QImage>(800, 800, QImage::Format_ARGB32);
    PixelPainter pixelPainter = PixelPainter(img);
    pixelPainter.fillImage(Colors::Black);

    renderer = std::make_shared<Renderer>(img, scene, camera);

    std::shared_ptr<Camera> defaultCamera = std::make_shared<Camera>(800,800);
    defaultCamera->transform.setPosition({ 0.0, 0.0, 0.0 });
    defaultCamera->visibleInScene = false;

    camera->transform.setPosition({ 0.0, 0.0, 300 });
    scene->addObject(camera);
    QListWidgetItem* item1 = new QListWidgetItem("Main Camera");
    objectsList->addItem(item1);

    refreshScene();
}

void MainWindow::refreshScene()
{
    renderer->clearRenderingSurface();
    renderer->renderScene();


    auto imgPtr = renderer->getRenderingSurface()->getImg();
    if (!imgPtr) return;

    drawOverlayPerFrame();

    sceneDisplay->setImage(*imgPtr);
}

void MainWindow::loadTestScene(){
    //TestScenes::loadDistantLightTestScene1(*scene , *objectsList);
    //TestScenes::loadSpotLightTestScene2(*scene, *objectsList);
    //TestScenes::loadPointLightTestScene3(*scene, *objectsList);
    //TestScenes::loadSpotLightOnEmptyWall(*scene, *objectsList);
    TestScenes::loadOnlyDistantLight(*scene , *objectsList);
    //TestScenes::loadRotatedCubeAndLight(*scene, *objectsList, Light::LightType::POINT);

    refreshScene();
}

void MainWindow::drawOverlayPerFrame(){
    fpsCounter.frame();
    drawFpsOverlay(*renderer->getRenderingSurface()->getImg(), fpsCounter.text(), Qt::TopLeftCorner);
    drawRenderStatsOverlay(*renderer->getRenderingSurface()->getImg() , renderer->stats.toQString());
    if(RenderableObject3D* obj = dynamic_cast<RenderableObject3D*>(currentObject.get())){
        drawObjectStatsOverlay(*renderer->getRenderingSurface()->getImg() , obj->toQString());
    }
}

void MainWindow::addImportedObjectsToScene(const ImportResult& import, const QString& sourcePath){

    for(const auto& obj : import.objects){
        obj->material.loadTexturesFromPaths(sourcePath);
        scene->addObject(obj);
        QString itemTextLight = QString::fromStdString(obj->name);
        objectsList->addItem(itemTextLight);
        //obj->debugStats();
    }


    refreshScene();
}

void MainWindow::onObjectSelected(){

    int objectId = objectsList->currentRow();
    if (objectId < 0 || objectId >= scene->objectsAmount()) {
        currentObject = nullptr;
        return;
    }

    currentObject = scene->getObject(objectId);
    if (!currentObject) return;

    if (currentPropertiesWidget) {
        rightLayout->removeWidget(currentPropertiesWidget);
        delete currentPropertiesWidget;
        currentPropertiesWidget = nullptr;
    }

    if (dynamic_cast<RenderableObject3D*>(currentObject.get())) {
        currentPropertiesWidget = new RenderableObjectPropertiesWidget(rightPanel);
    }else if(dynamic_cast<DistantLight*>(currentObject.get())){
        currentPropertiesWidget = new DistantLightPropertiesWidget(rightPanel);
    }else if(dynamic_cast<SpotLight*>(currentObject.get())){
        currentPropertiesWidget = new SpotLightPropertiesWidget(rightPanel);
    }else if(dynamic_cast<PointLight*>(currentObject.get())){
        currentPropertiesWidget = new PointLightPropertiesWidget(rightPanel);
    }else if(dynamic_cast<Camera*>(currentObject.get())){
        currentPropertiesWidget = new CameraPropertiesWidget(rightPanel);
    }

    if (currentPropertiesWidget) {
        currentPropertiesWidget->setObject(currentObject);
        rightLayout->addWidget(currentPropertiesWidget);
        connect(currentPropertiesWidget, &ObjectPropertiesWidget::objectChanged,
                this, &MainWindow::refreshScene);
    }

}

void MainWindow::onFileMenuImportObject(){
    QString fileName = QFileDialog::getOpenFileName(this, tr("Import .obj file"),"",tr("OBJ Files (*.obj);;Text Files (*.txt);;All Files (*)"));
    if (!fileName.isEmpty()) {
        qDebug() << "chosen file:" << fileName;
        ImportResult importResult = objImporter.load(fileName.toStdString());
        addImportedObjectsToScene(importResult , fileName);
    }
}

void MainWindow::onFileMenuSaveObject(){
    if(!currentObject){
        qDebug() << "Object to save was not selected!";
        return;
    }

    QString savePath = QFileDialog::getSaveFileName(this , tr("Save file") , "" , tr("OBJ Files (*.obj);;Text Files (*.txt);;All Files (*)"));
    if (!savePath.isEmpty()) {
        qDebug() << "saving:" << savePath;
        if(RenderableObject3D* currentRenderableObject = dynamic_cast<RenderableObject3D*>(currentObject.get())){
            objectSaver->saveObject(std::make_shared<RenderableObject3D>(*currentRenderableObject) , savePath.toStdString());
        }
    }
}

void MainWindow::onObjectMenuCreateCube(){
    std::shared_ptr<Cube> newObject = std::make_shared<Cube>();

    scene->addObject(newObject);
    QString itemText = QString("Cube");
    objectsList->addItem(itemText);

    refreshScene();
}

void MainWindow::onObjectMenuCreateCylinder(){
    std::shared_ptr<Cylinder> newObject = std::make_shared<Cylinder>();

    scene->addObject(newObject);
    QString itemText = QString("Cylinder");
    objectsList->addItem(itemText);

    refreshScene();
}

void MainWindow::onSceneMenuDeleteSelectedObject(){
    std::shared_ptr<Object3D> objToDelete = currentObject;
    currentObject = nullptr;
    scene->removeObject(objToDelete);
    QListWidgetItem* itemToDelete = objectsList->currentItem();
    if (itemToDelete) {
        delete objectsList->takeItem(objectsList->row(itemToDelete));
    }

    refreshScene();
}

void MainWindow::onOpenSceneProperties(){
    currentObject = nullptr;
    objectsList->clearSelection();

    if (currentPropertiesWidget) {
        rightLayout->removeWidget(currentPropertiesWidget);
        delete currentPropertiesWidget;
        currentPropertiesWidget = nullptr;
    }

     currentPropertiesWidget = new ScenePropertiesWidget(rightPanel);


     if (currentPropertiesWidget) {
         ScenePropertiesWidget* currentScenePropertiesWidget = static_cast<ScenePropertiesWidget*>(currentPropertiesWidget);
         currentScenePropertiesWidget->setScene(renderer->getScene());
         rightLayout->addWidget(currentScenePropertiesWidget);
         connect(currentScenePropertiesWidget, &ObjectPropertiesWidget::objectChanged,
                 this, &MainWindow::refreshScene);
     }

}


void MainWindow::onSceneDisplayClicked(int x, int y){

    IdBufferElement el = (*renderer->getIdBuffer())[y][x];

    std::cout<<"X: "<<x<<"  :  Y: "<<y;
    std::cout<<"    | vertexID: "<<el.vertexId<<" | edge: "<<el.edgeVertices.first<<" : "<<el.edgeVertices.second
              <<" | face: "<<el.faceId<<" | object: "<<el.objectId<<std::endl;

    objectsList->setCurrentRow(el.objectId);
    emit onObjectSelected();
}

void MainWindow::onObjectMenuCreateDistantLight(){
    std::shared_ptr<DistantLight> light = std::make_shared<DistantLight>(Vector3(1,0,0));
    light->color = Colors::White;
    light->castsShadow = true;

    scene->addObject(light);
    QString itemText = QString("Distant Light");
    objectsList->addItem(itemText);

    refreshScene();
}

void MainWindow::onObjectMenuCreatePointLight(){
    std::shared_ptr<PointLight> light = std::make_shared<PointLight>();
    light->color = Colors::White;
    light->castsShadow = true;

    scene->addObject(light);
    QString itemText = QString("Point Light");
    objectsList->addItem(itemText);

    refreshScene();
}

void MainWindow::onObjectMenuCreateSpotLight(){
    std::shared_ptr<SpotLight> light = std::make_shared<SpotLight>( );
    light->color = Colors::White;
    light->castsShadow = true;

    scene->addObject(light);
    QString itemText = QString("Spot Light");
    objectsList->addItem(itemText);

    refreshScene();
}
