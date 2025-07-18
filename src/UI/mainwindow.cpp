#include "UI/mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QPixmap>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUI();
    setupScene();

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::refreshScene);
    connect(sceneDisplay, &ImageLabel::pixelClicked,
            this, [&](int x, int y){
            onSceneDisplayClicked(x,y);
    });
    timer->start(30);
}

MainWindow::~MainWindow()
{

}

void MainWindow::setupUI()
{
    // Główny kontener i layout
    centralWidgetContainer = new QWidget(this);
    setCentralWidget(centralWidgetContainer);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidgetContainer);

    // Najbardziej zewnetrzny layout dzielacy aplikacje na czesc glowna i pasek menu
    menuBar = new QMenuBar(this);
    QHBoxLayout* menuBarLayout = new QHBoxLayout(menuBar);
    QVBoxLayout* menuAndMainLayout = new QVBoxLayout();

    menuAndMainLayout->addLayout(menuBarLayout);
    menuAndMainLayout->addLayout(mainLayout);

    setupMenuBar();

    // --- Lewy panel: lista obiektów i przycisk "Add Cube" ---
    QWidget* leftPanel = new QWidget(this);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);

    objectsList = new QListWidget(leftPanel);
    addCubeButton = new QPushButton("Add Cube", leftPanel);

    leftLayout->addWidget(objectsList);
    leftLayout->addWidget(addCubeButton);
    leftPanel->setLayout(leftLayout);
    mainLayout->addWidget(leftPanel, 1);

    // --- Środkowy panel: podgląd sceny ---
    sceneDisplay = new ImageLabel(this);
    sceneDisplay->setFixedSize(800,600);
    sceneDisplay->setScaledContents(false);

    QWidget* centerPanel = new QWidget(this);
    QVBoxLayout* centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->addWidget(sceneDisplay);
    centerPanel->setLayout(centerLayout);
    mainLayout->addWidget(centerPanel, 3);

    // --- Prawy panel: drzewko właściwości ---
    QWidget* rightPanel = new QWidget(this);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);

    setupUIPropertyTree(rightPanel , rightLayout);

    objectParametersPropertyTree->expandAll();
    rightLayout->addWidget(objectParametersPropertyTree);
    rightPanel->setLayout(rightLayout);
    mainLayout->addWidget(rightPanel, 2);

    // --- Łączenie sygnałów/slotów ---
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
    selectedObjectMenu->addAction(deleteSelectedObject);
    selectedObjectMenu->addAction(assignTectureToObject);

    QObject::connect(importObjectAction, &QAction::triggered, [&](){
        onFileMenuImportObject();
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

void MainWindow::setupUIPropertyTree(QWidget *rightPanel , QVBoxLayout *rightLayout){
    objectParametersPropertyTree = new QTreeWidget(rightPanel);
    objectParametersPropertyTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    objectParametersPropertyTree->setColumnCount(2);
    objectParametersPropertyTree->setHeaderHidden(true);

    setupUIPropertyTreeTransform();
    setupUIPropertyTreeViewportDisplay();
}

void MainWindow::setupUIPropertyTreeTransform(){

    QTreeWidgetItem* transformCategory = new QTreeWidgetItem();
    transformCategory->setText(0, "Transform");
    objectParametersPropertyTree->addTopLevelItem(transformCategory);

    setupUIPropertyTreeTransformPos(transformCategory);
    setupUIPropertyTreeTransformScale(transformCategory);
    setupUIPropertyTreeTransformRot(transformCategory);
}

void MainWindow::setupUIPropertyTreeTransformPos(QTreeWidgetItem* transformCategory){

    QTreeWidgetItem* positionNode = new QTreeWidgetItem(transformCategory);
    positionNode->setText(0, "Position");

    // Position X
    QTreeWidgetItem* posXItem = new QTreeWidgetItem(positionNode);
    posXItem->setText(0, "Pos X");
    QWidget* posXWidget = new QWidget();
    QHBoxLayout* posXLayout = new QHBoxLayout(posXWidget);
    posXLayout->setContentsMargins(0, 0, 0, 0);
    sceneObjectSliderPosX = new QSlider(Qt::Horizontal, posXWidget);
    sceneObjectSliderPosX->setRange(transDownLimit, transUpLimit);
    sceneObjectSpinPosX = new QSpinBox(posXWidget);
    sceneObjectSpinPosX->setRange(transDownLimit, transUpLimit);
    posXLayout->addWidget(sceneObjectSliderPosX);
    posXLayout->addWidget(sceneObjectSpinPosX);
    posXWidget->setLayout(posXLayout);
    objectParametersPropertyTree->setItemWidget(posXItem, 1, posXWidget);

    // Position Y
    QTreeWidgetItem* posYItem = new QTreeWidgetItem(positionNode);
    posYItem->setText(0, "Pos Y");
    QWidget* posYWidget = new QWidget();
    QHBoxLayout* posYLayout = new QHBoxLayout(posYWidget);
    posYLayout->setContentsMargins(0, 0, 0, 0);
    sceneObjectSliderPosY = new QSlider(Qt::Horizontal, posYWidget);
    sceneObjectSliderPosY->setRange(transDownLimit, transUpLimit);
    sceneObjectSpinPosY = new QSpinBox(posYWidget);
    sceneObjectSpinPosY->setRange(transDownLimit, transUpLimit);
    posYLayout->addWidget(sceneObjectSliderPosY);
    posYLayout->addWidget(sceneObjectSpinPosY);
    posYWidget->setLayout(posYLayout);
    objectParametersPropertyTree->setItemWidget(posYItem, 1, posYWidget);

    // Position Z
    QTreeWidgetItem* posZItem = new QTreeWidgetItem(positionNode);
    posZItem->setText(0, "Pos Z");
    QWidget* posZWidget = new QWidget();
    QHBoxLayout* posZLayout = new QHBoxLayout(posZWidget);
    posZLayout->setContentsMargins(0, 0, 0, 0);
    sceneObjectSliderPosZ = new QSlider(Qt::Horizontal, posZWidget);
    sceneObjectSliderPosZ->setRange(transDownLimit, transUpLimit);
    sceneObjectSpinPosZ = new QSpinBox(posZWidget);
    sceneObjectSpinPosZ->setRange(transDownLimit, transUpLimit);
    posZLayout->addWidget(sceneObjectSliderPosZ);
    posZLayout->addWidget(sceneObjectSpinPosZ);
    posZWidget->setLayout(posZLayout);
    objectParametersPropertyTree->setItemWidget(posZItem, 1, posZWidget);

    // Connections
    connect(sceneObjectSliderPosX, &QSlider::valueChanged, this, &MainWindow::onSceneObjectPosChangedSlider);
    connect(sceneObjectSpinPosX,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSceneObjectPosChangedSpin);
    connect(sceneObjectSliderPosY, &QSlider::valueChanged, this, &MainWindow::onSceneObjectPosChangedSlider);
    connect(sceneObjectSpinPosY,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSceneObjectPosChangedSpin);
    connect(sceneObjectSliderPosZ, &QSlider::valueChanged, this, &MainWindow::onSceneObjectPosChangedSlider);
    connect(sceneObjectSpinPosZ,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSceneObjectPosChangedSpin);
}

void MainWindow::setupUIPropertyTreeTransformScale(QTreeWidgetItem* transformCategory){

    QTreeWidgetItem* scaleNode = new QTreeWidgetItem(transformCategory);
    scaleNode->setText(0, "Scale");

    // Scale X
    QTreeWidgetItem* scaleXItem = new QTreeWidgetItem(scaleNode);
    scaleXItem->setText(0, "Scale X");
    QWidget* scaleXWidget = new QWidget();
    QHBoxLayout* scaleXLayout = new QHBoxLayout(scaleXWidget);
    scaleXLayout->setContentsMargins(0, 0, 0, 0);
    sceneObjectSliderScaleX = new QSlider(Qt::Horizontal, scaleXWidget);
    sceneObjectSliderScaleX->setRange(0, 3000);
    sceneObjectSliderScaleX->setValue(100);
    sceneObjectSpinScaleX = new QDoubleSpinBox(scaleXWidget);
    sceneObjectSpinScaleX->setRange(0.0, 30.0);
    sceneObjectSpinScaleX->setValue(1.0);
    scaleXLayout->addWidget(sceneObjectSliderScaleX);
    scaleXLayout->addWidget(sceneObjectSpinScaleX);
    scaleXWidget->setLayout(scaleXLayout);
    objectParametersPropertyTree->setItemWidget(scaleXItem, 1, scaleXWidget);

    // Scale Y
    QTreeWidgetItem* scaleYItem = new QTreeWidgetItem(scaleNode);
    scaleYItem->setText(0, "Scale Y");
    QWidget* scaleYWidget = new QWidget();
    QHBoxLayout* scaleYLayout = new QHBoxLayout(scaleYWidget);
    scaleYLayout->setContentsMargins(0, 0, 0, 0);
    sceneObjectSliderScaleY = new QSlider(Qt::Horizontal, scaleYWidget);
    sceneObjectSliderScaleY->setRange(0, 3000);
    sceneObjectSliderScaleY->setValue(100);
    sceneObjectSpinScaleY = new QDoubleSpinBox(scaleYWidget);
    sceneObjectSpinScaleY->setRange(0.0, 30.0);
    sceneObjectSpinScaleY->setValue(1.0);
    scaleYLayout->addWidget(sceneObjectSliderScaleY);
    scaleYLayout->addWidget(sceneObjectSpinScaleY);
    scaleYWidget->setLayout(scaleYLayout);
    objectParametersPropertyTree->setItemWidget(scaleYItem, 1, scaleYWidget);

    // Scale Z
    QTreeWidgetItem* scaleZItem = new QTreeWidgetItem(scaleNode);
    scaleZItem->setText(0, "Scale Z");
    QWidget* scaleZWidget = new QWidget();
    QHBoxLayout* scaleZLayout = new QHBoxLayout(scaleZWidget);
    scaleZLayout->setContentsMargins(0, 0, 0, 0);
    sceneObjectSliderScaleZ = new QSlider(Qt::Horizontal, scaleZWidget);
    sceneObjectSliderScaleZ->setRange(0, 3000);
    sceneObjectSliderScaleZ->setValue(100);
    sceneObjectSpinScaleZ = new QDoubleSpinBox(scaleZWidget);
    sceneObjectSpinScaleZ->setRange(0.0, 30.0);
    sceneObjectSpinScaleZ->setValue(1.0);
    scaleZLayout->addWidget(sceneObjectSliderScaleZ);
    scaleZLayout->addWidget(sceneObjectSpinScaleZ);
    scaleZWidget->setLayout(scaleZLayout);
    objectParametersPropertyTree->setItemWidget(scaleZItem, 1, scaleZWidget);

    // Connections
    connect(sceneObjectSliderScaleX, &QSlider::valueChanged, this, &MainWindow::onSceneObjectScaleChangedSlider);
    connect(sceneObjectSpinScaleX,  QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onSceneObjectScaleChangedSpin);
    connect(sceneObjectSliderScaleY, &QSlider::valueChanged, this, &MainWindow::onSceneObjectScaleChangedSlider);
    connect(sceneObjectSpinScaleY,  QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onSceneObjectScaleChangedSpin);
    connect(sceneObjectSliderScaleZ, &QSlider::valueChanged, this, &MainWindow::onSceneObjectScaleChangedSlider);
    connect(sceneObjectSpinScaleZ,  QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onSceneObjectScaleChangedSpin);
}

void MainWindow::setupUIPropertyTreeTransformRot(QTreeWidgetItem* transformCategory){
    QTreeWidgetItem* rotationNode = new QTreeWidgetItem(transformCategory);
    rotationNode->setText(0, "Rotation");

    // Rotation X
    QTreeWidgetItem* rotXItem = new QTreeWidgetItem(rotationNode);
    rotXItem->setText(0, "Rot X");
    QWidget* rotXWidget = new QWidget();
    QHBoxLayout* rotXLayout = new QHBoxLayout(rotXWidget);
    rotXLayout->setContentsMargins(0, 0, 0, 0);
    sceneObjectSliderRotX = new QSlider(Qt::Horizontal, rotXWidget);
    sceneObjectSliderRotX->setRange(0, 360);
    sceneObjectSpinRotX = new QSpinBox(rotXWidget);
    sceneObjectSpinRotX->setRange(0, 360);
    rotXLayout->addWidget(sceneObjectSliderRotX);
    rotXLayout->addWidget(sceneObjectSpinRotX);
    rotXWidget->setLayout(rotXLayout);
    objectParametersPropertyTree->setItemWidget(rotXItem, 1, rotXWidget);

    // Rotation Y
    QTreeWidgetItem* rotYItem = new QTreeWidgetItem(rotationNode);
    rotYItem->setText(0, "Rot Y");
    QWidget* rotYWidget = new QWidget();
    QHBoxLayout* rotYLayout = new QHBoxLayout(rotYWidget);
    rotYLayout->setContentsMargins(0, 0, 0, 0);
    sceneObjectSliderRotY = new QSlider(Qt::Horizontal, rotYWidget);
    sceneObjectSliderRotY->setRange(0, 360);
    sceneObjectSpinRotY = new QSpinBox(rotYWidget);
    sceneObjectSpinRotY->setRange(0, 360);
    rotYLayout->addWidget(sceneObjectSliderRotY);
    rotYLayout->addWidget(sceneObjectSpinRotY);
    rotYWidget->setLayout(rotYLayout);
    objectParametersPropertyTree->setItemWidget(rotYItem, 1, rotYWidget);

    // Rotation Z
    QTreeWidgetItem* rotZItem = new QTreeWidgetItem(rotationNode);
    rotZItem->setText(0, "Rot Z");
    QWidget* rotZWidget = new QWidget();
    QHBoxLayout* rotZLayout = new QHBoxLayout(rotZWidget);
    rotZLayout->setContentsMargins(0, 0, 0, 0);
    sceneObjectSliderRotZ = new QSlider(Qt::Horizontal, rotZWidget);
    sceneObjectSliderRotZ->setRange(0, 360);
    sceneObjectSpinRotZ = new QSpinBox(rotZWidget);
    sceneObjectSpinRotZ->setRange(0, 360);
    rotZLayout->addWidget(sceneObjectSliderRotZ);
    rotZLayout->addWidget(sceneObjectSpinRotZ);
    rotZWidget->setLayout(rotZLayout);
    objectParametersPropertyTree->setItemWidget(rotZItem, 1, rotZWidget);

    // Connections
    connect(sceneObjectSliderRotX, &QSlider::valueChanged, this, &MainWindow::onSCeneObjectRotChangedSlider);
    connect(sceneObjectSpinRotX,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSCeneObjectRotChangedSpin);
    connect(sceneObjectSliderRotY, &QSlider::valueChanged, this, &MainWindow::onSCeneObjectRotChangedSlider);
    connect(sceneObjectSpinRotY,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSCeneObjectRotChangedSpin);
    connect(sceneObjectSliderRotZ, &QSlider::valueChanged, this, &MainWindow::onSCeneObjectRotChangedSlider);
    connect(sceneObjectSpinRotZ,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSCeneObjectRotChangedSpin);

}

void MainWindow::setupUIPropertyTreeViewportDisplay(){

    QTreeWidgetItem* viewportDisplayCategory = new QTreeWidgetItem();
    viewportDisplayCategory->setText(0 , "Viewport display");
    objectParametersPropertyTree->addTopLevelItem(viewportDisplayCategory);

    QTreeWidgetItem* displayModeItem = new QTreeWidgetItem(viewportDisplayCategory);
    displayModeItem->setText(0 , "display as");
    objectDisplayModeComboBox = new QComboBox(this);
    QVBoxLayout* displayModeComboBoxLayput = new QVBoxLayout(objectDisplayModeComboBox);
    objectDisplayModeComboBox->insertItem(0 , "wireframe" , QVariant(static_cast<int>(DisplaySettings::RenderMode::WIREFRAME)));
    objectDisplayModeComboBox->insertItem(1 , "solid" , QVariant(static_cast<int>(DisplaySettings::RenderMode::RASTER)));
    //objectDisplayModeComboBox->insertItem(2 , "textured" , QVariant(ViewportDisplay::DisplayMode::TEXTURED));
    objectDisplayModeComboBox->setLayout(displayModeComboBoxLayput); //Jaki jest sens dodawania widget w konstruktorze layouta jesli tutaj go znowu przydzielam?
    objectParametersPropertyTree->setItemWidget(displayModeItem, 1, objectDisplayModeComboBox);

    /*
    QTreeWidgetItem* displayColorItem = new QTreeWidgetItem(viewportDisplayCategory);
    displayColorItem->setText(0 , "Color");
    objectDisplayColor = new QColorDialog(this);
    QVBoxLayout* displayColorLayput = new QVBoxLayout(objectDisplayColor);
    objectDisplayColor->setLayout(displayColorLayput);
    objectParametersPropertyTree->setItemWidget(displayColorItem, 1, objectDisplayColor);
    */

    QTreeWidgetItem* displayColorItem = new QTreeWidgetItem(viewportDisplayCategory);
    displayColorItem->setText(0 , "Color");
    colorPicker = new ColorPicker(this);
    QVBoxLayout* colorPickerLayout = new QVBoxLayout(colorPicker);
    colorPicker->setLayout(colorPickerLayout);
    objectParametersPropertyTree->setItemWidget(displayColorItem, 1, colorPicker);

    connect(objectDisplayModeComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::onDisplayModeCurIndexChanged);
    connect(colorPicker, &ColorPicker::colorChanged, this, &MainWindow::onDisplayColorPickerValueChanged);
}

void MainWindow::setupScene()
{
    scene = std::make_shared<Scene>();
    objectLoader = std::make_shared<ObjectLoader>();
    objectSaver = std::make_shared<ObjectSaver>();
    camera = std::make_shared<Camera>();

    auto img = std::make_shared<QImage>(800, 800, QImage::Format_ARGB32);
    //img->fill(Qt::black);
    PixelPainter pixelPainter = PixelPainter(img);
    pixelPainter.fillImage(Colors::Black);

    //renderingSurface = std::make_shared<RenderingSurface>(img);
    renderer = std::make_shared<Renderer>(img, scene, camera);

    std::shared_ptr<Camera> defaultCamera = std::make_shared<Camera>();
    defaultCamera->transform.setPosition({ 0.0, 0.0, 0.0 });
    //defaultCamera->setFov(200.0);
    defaultCamera->visibleInScene = false;
    scene->specialSceneObjects.defaultCamera = defaultCamera;

    //add Vertical grid
    /*
    std::shared_ptr<Grid> verticalGrid = std::make_shared<Grid>(Grid::Orientation::VERTICAL);
    //scene->specialSceneObjects.VerticalGrid =
    scene->addObject(verticalGrid);
    QListWidgetItem* vGridItem = new QListWidgetItem("Vertical Grid");
    objectsList->addItem(vGridItem);
    */

    /*
    std::shared_ptr<Grid> horizontalGrid = std::make_shared<Grid>(Grid::Orientation::HORIZONTAL);
    scene->addObject(horizontalGrid);
    QListWidgetItem* hGridItem = new QListWidgetItem("Horizontal Grid");
    objectsList->addItem(hGridItem);
    */

    camera->transform.setPosition({ 0.0, 0.0, 300 });
    //camera->setFov(200.0);
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

    /*
    QPixmap pix = QPixmap::fromImage(*imgPtr);
    sceneDisplay->setPixmap(pix);
    */
    sceneDisplay->setImage(*imgPtr);
}

void MainWindow::onAddCubeClicked()
{

    auto newObj = std::make_shared<Cylinder>();
    scene->addObject(newObj);
    QString itemText = QString("Cylinder");
    objectsList->addItem(itemText);
    /*
    static int cubeCount = 2;
    auto newCube = std::make_shared<Cube>(30);

    scene->addObject(newCube);

    QString itemText = QString("Cube #%1 (side=30)").arg(cubeCount++);
    objectsList->addItem(itemText);

    refreshScene();
    */


}

void MainWindow::onObjectSelected()
{


    int objectId = objectsList->currentRow();
    std::cout<<"Object Selected: "<<objectId<<" : "<<scene->objectsAmount()<<std::endl;
    if (objectId < 0 || objectId >= scene->objectsAmount()) {
        currentObject = nullptr;
        return;
    }

    currentObject = scene->getObject(objectId);
    if (!currentObject) return;

    Vector3 pos = currentObject->transform.getPosition();
    Vector3 rot = currentObject->transform.getAngles();
    Vector3 scale = currentObject->transform.getScales();
    //std::cout<<pos<<std::endl;
    //std::cout<<scene->objectsAmount()<<std::endl;

    sceneObjectSliderPosX->setValue(pos.x);
    sceneObjectSliderPosY->setValue(pos.y);
    sceneObjectSliderPosZ->setValue(pos.z);

    sceneObjectSpinPosX->setValue(pos.x);
    sceneObjectSpinPosY->setValue(pos.y);
    sceneObjectSpinPosZ->setValue(pos.z);

    sceneObjectSliderRotX->setValue(rot.x);
    sceneObjectSliderRotY->setValue(rot.y);
    sceneObjectSliderRotZ->setValue(rot.z);

    sceneObjectSpinRotX->setValue(rot.x);
    sceneObjectSpinRotY->setValue(rot.y);
    sceneObjectSpinRotZ->setValue(rot.z);

    sceneObjectSliderScaleX->setValue(scale.x);
    sceneObjectSliderScaleY->setValue(scale.y);
    sceneObjectSliderScaleZ->setValue(scale.z);

    sceneObjectSpinScaleX->setValue(scale.x);
    sceneObjectSpinScaleY->setValue(scale.y);
    sceneObjectSpinScaleZ->setValue(scale.z);

    //std::cout<<"Before changing colorPicker values object had colors: "<<currentObject->viewportDisplay.color<<std::endl;
    if(RenderableObject3D* currentRenderableObject = dynamic_cast<RenderableObject3D*>(currentObject.get())){
        colorPicker->setColor(currentRenderableObject->viewportDisplay.color);
    }

    //std::cout<<"After changing colorPicker values object had colors: "<<currentObject->viewportDisplay.color<<std::endl;
    //to fill

}

void MainWindow::onSceneObjectPosChangedSpin(){

    if (!currentObject) return;

    double sliderValueX = sceneObjectSliderPosX->value();
    double sliderValueY = sceneObjectSliderPosY->value();
    double sliderValueZ = sceneObjectSliderPosZ->value();

    double spinValueX = sceneObjectSpinPosX->value();
    double spinValueY = sceneObjectSpinPosY->value();
    double spinValueZ = sceneObjectSpinPosZ->value();

    if (static_cast<int>(sliderValueX) != spinValueX) {
        sceneObjectSliderPosX->setValue(sceneObjectSpinPosX->value());
        sliderValueX = sceneObjectSpinPosX->value();
    }

    if (static_cast<int>(sliderValueY) != spinValueY) {
        sceneObjectSliderPosY->setValue(sceneObjectSpinPosY->value());
        sliderValueY = sceneObjectSpinPosY->value();
    }

    if (static_cast<int>(sliderValueZ) != spinValueZ) {
        sceneObjectSliderPosZ->setValue(sceneObjectSpinPosZ->value());
        sliderValueZ = sceneObjectSpinPosZ->value();
    }

    currentObject->transform.setPosition(Vector3(spinValueX, spinValueY, spinValueZ));

    refreshScene();
}

void MainWindow::onSceneObjectPosChangedSlider(){

    if (!currentObject) return;

    double sliderValueX = sceneObjectSliderPosX->value();
    double sliderValueY = sceneObjectSliderPosY->value();
    double sliderValueZ = sceneObjectSliderPosZ->value();

    double spinValueX = sceneObjectSpinPosX->value();
    double spinValueY = sceneObjectSpinPosY->value();
    double spinValueZ = sceneObjectSpinPosZ->value();

    if (static_cast<int>(sliderValueX) != spinValueX) {
        sceneObjectSpinPosX->setValue(sliderValueX);
        spinValueX = sliderValueX;
    }

    if (static_cast<int>(sliderValueY) != spinValueY) {
        sceneObjectSpinPosY->setValue(sliderValueY);
        spinValueY = sliderValueY;
    }

    if (static_cast<int>(sliderValueZ) != spinValueZ) {
        sceneObjectSpinPosZ->setValue(sliderValueZ);
        spinValueZ = sliderValueZ;
    }

    currentObject->transform.setPosition(Vector3(spinValueX, spinValueY, spinValueZ));

    refreshScene();
}

void MainWindow::onSceneObjectScaleChangedSpin()
{
    if (!currentObject) return;

    double sliderValueX = sceneObjectSliderScaleX->value();
    double sliderValueY = sceneObjectSliderScaleY->value();
    double sliderValueZ = sceneObjectSliderScaleZ->value();

    double spinValueX = sceneObjectSpinScaleX->value();
    double spinValueY = sceneObjectSpinScaleY->value();
    double spinValueZ = sceneObjectSpinScaleZ->value();

    if (static_cast<int>(sliderValueX) != spinValueX) {
        sceneObjectSliderScaleX->setValue(sceneObjectSpinScaleX->value()*100);
        sliderValueX = sceneObjectSpinScaleX->value()*100;
    }
    if (static_cast<int>(sliderValueY) != spinValueY) {
        sceneObjectSliderScaleY->setValue(sceneObjectSpinScaleY->value()*100);
        sliderValueY = sceneObjectSpinScaleY->value()*100;
    }
    if (static_cast<int>(sliderValueZ) != spinValueZ) {
        sceneObjectSliderScaleZ->setValue(sceneObjectSpinScaleZ->value()*100);
        sliderValueZ = sceneObjectSpinScaleZ->value()*100;
    }

    Vector3 newScale(spinValueX, spinValueY, spinValueZ);

    currentObject->transform.setScales(newScale);

    refreshScene();
}

void MainWindow::onSceneObjectScaleChangedSlider()
{
    if (!currentObject) return;

    double sliderValueX = sceneObjectSliderScaleX->value();
    double sliderValueY = sceneObjectSliderScaleY->value();
    double sliderValueZ = sceneObjectSliderScaleZ->value();

    double spinValueX = sceneObjectSpinScaleX->value();
    double spinValueY = sceneObjectSpinScaleY->value();
    double spinValueZ = sceneObjectSpinScaleZ->value();

    if (static_cast<int>(sliderValueX) != spinValueX) {
        sceneObjectSpinScaleX->setValue(sliderValueX/100);
        spinValueX = sliderValueX/100;
    }

    if (static_cast<int>(sliderValueY) != spinValueY) {
        sceneObjectSpinScaleY->setValue(sliderValueY/100);
        spinValueY = sliderValueY/100;
    }

    if (static_cast<int>(sliderValueZ) != spinValueZ) {
        sceneObjectSpinScaleZ->setValue(sliderValueZ/100);
        spinValueZ = sliderValueZ/100;
    }

    currentObject->transform.setScales(Vector3(spinValueX, spinValueY, spinValueZ));

    refreshScene();
}

void MainWindow::onSCeneObjectRotChangedSpin()
{
    if (!currentObject) return;

    double sliderValueX = sceneObjectSliderRotX->value();
    double sliderValueY = sceneObjectSliderRotY->value();
    double sliderValueZ = sceneObjectSliderRotZ->value();

    double spinValueX = sceneObjectSpinRotX->value();
    double spinValueY = sceneObjectSpinRotY->value();
    double spinValueZ = sceneObjectSpinRotZ->value();

    if (static_cast<int>(sliderValueX) != spinValueX) {
        sceneObjectSliderRotX->setValue(sceneObjectSpinRotX->value());
        sliderValueX = sceneObjectSpinRotX->value();
    }
    if (static_cast<int>(sliderValueY) != spinValueY) {
        sceneObjectSliderRotY->setValue(sceneObjectSpinRotY->value());
        sliderValueY = sceneObjectSpinRotY->value();
    }
    if (static_cast<int>(sliderValueZ) != spinValueZ) {
        sceneObjectSliderRotZ->setValue(sceneObjectSpinRotZ->value());
        sliderValueZ = sceneObjectSpinRotZ->value();
    }

    currentObject->transform.setAngles(Vector3(spinValueX, spinValueY, spinValueZ), false);

    refreshScene();
}

void MainWindow::onSCeneObjectRotChangedSlider()
{
    if (!currentObject) return;

    double sliderValueX = sceneObjectSliderRotX->value();
    double sliderValueY = sceneObjectSliderRotY->value();
    double sliderValueZ = sceneObjectSliderRotZ->value();

    double spinValueX = sceneObjectSpinRotX->value();
    double spinValueY = sceneObjectSpinRotY->value();
    double spinValueZ = sceneObjectSpinRotZ->value();

    if (static_cast<int>(sliderValueX) != spinValueX) {
        sceneObjectSpinRotX->setValue(sliderValueX);
        spinValueX = sliderValueX;
    }

    if (static_cast<int>(sliderValueY) != spinValueY) {
        sceneObjectSpinRotY->setValue(sliderValueY);
        spinValueY = sliderValueY;
    }

    if (static_cast<int>(sliderValueZ) != spinValueZ) {
        sceneObjectSpinRotZ->setValue(sliderValueZ);
        spinValueZ = sliderValueZ;
    }

    currentObject->transform.setAngles(Vector3(spinValueX, spinValueY, spinValueZ), false);

    refreshScene();
}

void MainWindow::onDisplayModeCurIndexChanged(){
    /*
    if(RenderableObject3D* currentRenderableObject = dynamic_cast<RenderableObject3D*>(currentObject.get())){
    currentObject->viewportDisplay.displayMode = objectDisplayModeComboBox->itemData(objectDisplayModeComboBox->currentIndex()).value<ViewportDisplay::DisplayMode>();
        switch(currentObject->viewportDisplay.displayMode){
        case ViewportDisplay::WIREFRAME:
            currentRenderableObject->setRenderStrategy(std::make_unique<BasicRenderStrategy>());
            break;
        case ViewportDisplay::SOLID:
            currentRenderableObject->setRenderStrategy(std::make_unique<RasterRenderStrategy>());
            break;
        case ViewportDisplay::TEXTURED:
            currentRenderableObject->setRenderStrategy(std::make_unique<FrontOnlyRenderStrategy>()); //TODO change to an actuall textured render
            break;
        }
    }*/

    if(RenderableObject3D* currentRenderableObject = dynamic_cast<RenderableObject3D*>(currentObject.get())){
        currentRenderableObject->displaySettings->renderMode =
            static_cast<DisplaySettings::RenderMode>
            (objectDisplayModeComboBox->itemData(objectDisplayModeComboBox->currentIndex()).toInt());
        /*
        switch(currentObject->viewportDisplay.displayMode){
        case ViewportDisplay::WIREFRAME:
            currentRenderableObject->setRenderStrategy(std::make_unique<BasicRenderStrategy>());
            break;
        case ViewportDisplay::SOLID:
            currentRenderableObject->setRenderStrategy(std::make_unique<RasterRenderStrategy>());
            break;
        }
    */
    }

}

void MainWindow::onDisplayColorPickerValueChanged(const Color& color){
    std::cout<<"changed color Value to: "<<color<<std::endl;
    currentObject->viewportDisplay.color = color;
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

void MainWindow::onFileMenuImportObject(){
    QString fileName = QFileDialog::getOpenFileName(this, tr("Import .obj file"),"",tr("OBJ Files (*.obj);;Text Files (*.txt);;All Files (*)"));
    if (!fileName.isEmpty()) {
        qDebug() << "chosen file:" << fileName;


        std::shared_ptr<RenderableObject3D> loadedObject = objectLoader->loadObject(fileName.toStdString())[0];
        scene->addObject(loadedObject);
        QString itemText = QString("imported cube");
        objectsList->addItem(itemText);

        refreshScene();
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

    Renderer::IdBufferElement el = (*renderer->idBuffer)[y][x];

    std::cout<<"X: "<<x<<"  :  Y: "<<y;
    std::cout<<"    | vertexID: "<<el.vertexId<<" | edge: "<<el.edgeVertices.first<<" : "<<el.edgeVertices.second
              <<" | face: "<<el.faceId<<" | object: "<<el.objectId<<std::endl;

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
        }
    }
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
/*
 * W comboboxie odpowiednie tree itemy powinny sie wyswietlac w zaleznosci od wybranego obiektu
 * nullptr - nic , renderableObject - viewportDisplay i transform itp...
 */
