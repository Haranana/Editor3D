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
    addCubeButton = new QPushButton("Add Cube", leftPanel);

    leftLayout->addWidget(objectsList);
    leftLayout->addWidget(addCubeButton);
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
    //rightLayout->addStretch();

    propertiesWidgetScrollArea->setWidget(rightPanel);
    mainLayout->addWidget(propertiesWidgetScrollArea, 2);

    connect(addCubeButton, &QPushButton::clicked, this, &MainWindow::onAddCubeClicked);
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
    QMenu *selectMenu = menuBar->addMenu("Select");

    QActionGroup *selectActionGroup = new QActionGroup(this);
    QAction *importObjectAction = new QAction("&Import object");
    QAction *saveObjectAction = new QAction("&Save object");
    QAction *createObjectCube = new QAction("&Cube");
    QAction *createObjectCylinder = new QAction("&Cylinder");
    QAction *deleteSelectedObject = new QAction("&Delete");
    QAction *assignTectureToObject = new QAction("&Assign texture");
    QAction *selectObjects = new QAction("&Objects");
    QAction *selectFaces = new QAction("&Faces");
    QAction *selectEdges = new QAction("&Edges");
    QAction *selectVertices = new QAction("&Vertices");
    QAction *createDistantLight = new QAction("&Distant");
    QAction *createPointLight = new QAction("&Point");
    QAction *createSpotLight = new QAction("&Spot");

    selectActionGroup->setExclusive(true);
    selectObjects->setCheckable(true);
    selectFaces->setCheckable(true);
    selectEdges->setCheckable(true);
    selectVertices->setCheckable(true);
    selectActionGroup->addAction(selectObjects);
    selectActionGroup->addAction(selectFaces);
    selectActionGroup->addAction(selectEdges);
    selectActionGroup->addAction(selectVertices);
    //selectObjects->setChecked(true);

    selectMenu->addAction(selectObjects);
    selectMenu->addAction(selectFaces);
    selectMenu->addAction(selectEdges);
    selectMenu->addAction(selectVertices);

    fileMenu->addAction(importObjectAction);
    fileMenu->addSeparator();
    fileMenu->addAction(saveObjectAction);
    //objectsMenu->addMenu(createObjectsMenu);

    createObjectsMenu->addAction(createObjectCube);
    createObjectsMenu->addAction(createObjectCylinder);
    createObjectsLightMenu->addAction(createDistantLight);
    createObjectsLightMenu->addAction(createPointLight);
    createObjectsLightMenu->addAction(createSpotLight);
    //createObjectsMenu->addMenu(createObjectCylinder);
    selectedObjectMenu->addAction(deleteSelectedObject);
    selectedObjectMenu->addAction(assignTectureToObject);

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

    QObject::connect(saveObjectAction, &QAction::triggered, [&](){
        onFileMenuSaveObject();
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

    QObject::connect(assignTectureToObject, &QAction::triggered, [&](){
        onSceneMenuAssignTectureToObject();
    });

    QObject::connect(selectObjects, &QAction::triggered, [&](){
        onSelectMenuChangeSelectMode(SelectMode::OBJECTS);
    });

    QObject::connect(selectFaces, &QAction::triggered, [&](){
        onSelectMenuChangeSelectMode(SelectMode::FACES);
    });


    QObject::connect(selectEdges, &QAction::triggered, [&](){
        onSelectMenuChangeSelectMode(SelectMode::EDGES);
    });


    QObject::connect(selectVertices, &QAction::triggered, [&](){
        onSelectMenuChangeSelectMode(SelectMode::VERTICES);
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
    //TestScenes::loadOnlyDistantLight(*scene , *objectsList);
    TestScenes::loadRotatedCubeAndLight(*scene, *objectsList, Light::LightType::POINT);

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

void MainWindow::onAddCubeClicked()
{

    auto newObj = std::make_shared<Cylinder>();
    scene->addObject(newObj);
    QString itemText = QString("Cylinder");
    objectsList->addItem(itemText);
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
    QString itemText = QString("Cube");
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

void MainWindow::onSelectMenuChangeSelectMode(SelectMode newSelectMode){
    curSelectMode = newSelectMode;
}

void MainWindow::onSceneDisplayClicked(int x, int y){

    IdBufferElement el = (*renderer->getIdBuffer())[y][x];

    std::cout<<"X: "<<x<<"  :  Y: "<<y;
    std::cout<<"    | vertexID: "<<el.vertexId<<" | edge: "<<el.edgeVertices.first<<" : "<<el.edgeVertices.second
              <<" | face: "<<el.faceId<<" | object: "<<el.objectId<<std::endl;

    objectsList->setCurrentRow(el.objectId);
    emit onObjectSelected();
        /*
    if(el.objectId!=-1){
        switch(curSelectMode){
        case OBJECTS:
            scene->getObject(el.objectId)->viewportDisplay.selectObject();
            break;
        case EDGES:
            scene->getObject(el.objectId)->viewportDisplay.selectEdge(el.edgeVertices.first, el.edgeVertices.second);
            break;
        case VERTICES:
            scene->getObject(el.objectId)->viewportDisplay.selectVertex(el.vertexId);
            break;
        case FACES:
            scene->getObject(el.objectId)->viewportDisplay.selectFace(el.faceId);
            break;
        case NONE:
            break;
        default:
            break;
        }
    }*/
}

void MainWindow::onSceneMenuAssignTectureToObject(){
    qDebug("onSceneMenuAssignTectureToObject");
    if (!currentObject) return;
    QString path = QFileDialog::getOpenFileName(
        this,"Select texture","","Images (*.png *.jpg *.bmp)");
    if (path.isEmpty()) return;

    auto tex = std::make_shared<Texture>();
    tex->path  = path;
    tex->image = QImage(path).convertToFormat(QImage::Format_RGBA8888);
    if (tex->image.isNull()) { qDebug()<<"Bad image"; return; }

    if (auto rend = dynamic_cast<RenderableObject3D*>(currentObject.get()))
        rend->texture = tex;

    // krok 3 ➜ generujemy UV dla zaznaczonej / całej siatki
    generatePlanarUV(*dynamic_cast<RenderableObject3D*>(currentObject.get()),
                     Plane::XY);   // na razie XY

    refreshScene();
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
