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
    timer->start(30);
}

MainWindow::~MainWindow()
{

}

void MainWindow::setupUI()
{
    centralWidgetContainer = new QWidget(this);
    setCentralWidget(centralWidgetContainer);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidgetContainer);

    QWidget* leftPanel = new QWidget(this);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);

    objectsList = new QListWidget(leftPanel);
    addCubeButton = new QPushButton("Add Cube", leftPanel);

    leftLayout->addWidget(objectsList);
    leftLayout->addWidget(addCubeButton);

    leftPanel->setLayout(leftLayout);
    mainLayout->addWidget(leftPanel, 1);

    sceneDisplay = new QLabel(this);
    sceneDisplay->setFixedSize(800,600);
    sceneDisplay->setScaledContents(false);

    /*
    sceneDisplay = new QLabel(this);
    sceneDisplay->setMinimumSize(400, 300);
    sceneDisplay->setStyleSheet("background-color: black;");
    sceneDisplay->setAlignment(Qt::AlignCenter);
    */

    QWidget* centerPanel = new QWidget(this);
    QVBoxLayout* centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->addWidget(sceneDisplay);
    centerPanel->setLayout(centerLayout);

    mainLayout->addWidget(centerPanel, 3);

    QWidget* rightPanel = new QWidget(this);
    QFormLayout* rightLayout = new QFormLayout(rightPanel);

    sceneObjectSliderPosX = new QSlider(Qt::Horizontal, rightPanel);
    sceneObjectSliderPosY = new QSlider(Qt::Horizontal, rightPanel);
    sceneObjectSliderPosZ = new QSlider(Qt::Horizontal, rightPanel);
    sceneObjectSliderPosX->setRange(-200, 200);
    sceneObjectSliderPosY->setRange(-200, 200);
    sceneObjectSliderPosZ->setRange(-200, 200);

    sceneObjectSpinPosX =  new QSpinBox(rightPanel);
    sceneObjectSpinPosY =  new QSpinBox(rightPanel);
    sceneObjectSpinPosZ =  new QSpinBox(rightPanel);
    sceneObjectSpinPosX->setRange(-200, 200);
    sceneObjectSpinPosY->setRange(-200, 200);
    sceneObjectSpinPosZ->setRange(-200, 200);

    sceneObjectSliderScaleX = new QSlider(Qt::Horizontal, rightPanel);
    sceneObjectSliderScaleY = new QSlider(Qt::Horizontal, rightPanel);
    sceneObjectSliderScaleZ = new QSlider(Qt::Horizontal, rightPanel);
    sceneObjectSliderScaleX->setRange(0, 300);
    sceneObjectSliderScaleY->setRange(0, 300);
    sceneObjectSliderScaleZ->setRange(0, 300);
    sceneObjectSliderScaleX->setValue(100);
    sceneObjectSliderScaleY->setValue(100);
    sceneObjectSliderScaleZ->setValue(100);

    sceneObjectSpinScaleX =  new QDoubleSpinBox(rightPanel);
    sceneObjectSpinScaleY =  new QDoubleSpinBox(rightPanel);
    sceneObjectSpinScaleZ =  new QDoubleSpinBox(rightPanel);
    sceneObjectSpinScaleX->setRange(0.0, 3.0);
    sceneObjectSpinScaleY->setRange(0.0, 3.0);
    sceneObjectSpinScaleZ->setRange(0.0, 3.0);
    sceneObjectSpinScaleX->setValue(1.0);
    sceneObjectSpinScaleY->setValue(1.0);
    sceneObjectSpinScaleZ->setValue(1.0);

    sceneObjectSliderRotX = new QSlider(Qt::Horizontal, rightPanel);
    sceneObjectSliderRotY = new QSlider(Qt::Horizontal, rightPanel);
    sceneObjectSliderRotZ = new QSlider(Qt::Horizontal, rightPanel);
    sceneObjectSliderRotX->setRange(0, 360);
    sceneObjectSliderRotY->setRange(0, 360);
    sceneObjectSliderRotZ->setRange(0, 360);

    sceneObjectSpinRotX =  new QSpinBox(rightPanel);
    sceneObjectSpinRotY =  new QSpinBox(rightPanel);
    sceneObjectSpinRotZ =  new QSpinBox(rightPanel);
    sceneObjectSpinRotX->setRange(0, 360);
    sceneObjectSpinRotY->setRange(0, 360);
    sceneObjectSpinRotZ->setRange(0, 360);

    rightLayout->addRow("Pos X:", sceneObjectSliderPosX);
    rightLayout->addRow("", sceneObjectSpinPosX);
    rightLayout->addRow("Pos Y:", sceneObjectSliderPosY);
    rightLayout->addRow("", sceneObjectSpinPosY);
    rightLayout->addRow("Pos Z:", sceneObjectSliderPosZ);
    rightLayout->addRow("", sceneObjectSpinPosZ);


    rightLayout->addRow("Scale X:", sceneObjectSliderScaleX);
    rightLayout->addRow("", sceneObjectSpinScaleX);
    rightLayout->addRow("Scale Y:", sceneObjectSliderScaleY);
    rightLayout->addRow("", sceneObjectSpinScaleY);
    rightLayout->addRow("Scale Z:", sceneObjectSliderScaleZ);
    rightLayout->addRow("", sceneObjectSpinScaleZ);

    rightLayout->addRow("Rot X:", sceneObjectSliderRotX);
    rightLayout->addRow("", sceneObjectSpinRotX);
    rightLayout->addRow("Rot Y:", sceneObjectSliderRotY);
    rightLayout->addRow("", sceneObjectSpinRotY);
    rightLayout->addRow("Rot Z:", sceneObjectSliderRotZ);
    rightLayout->addRow("", sceneObjectSpinRotZ);

    rightPanel->setLayout(rightLayout);
    mainLayout->addWidget(rightPanel, 2);

    connect(addCubeButton, &QPushButton::clicked, this, &MainWindow::onAddCubeClicked);
    connect(objectsList, &QListWidget::itemSelectionChanged, this, &MainWindow::onObjectSelected);

    connect(sceneObjectSliderPosX, &QSlider::valueChanged, this, &MainWindow::onSceneObjectPosChangedSlider);
    connect(sceneObjectSliderPosY, &QSlider::valueChanged, this, &MainWindow::onSceneObjectPosChangedSlider);
    connect(sceneObjectSliderPosZ, &QSlider::valueChanged, this, &MainWindow::onSceneObjectPosChangedSlider);

    connect(sceneObjectSpinPosX,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSceneObjectPosChangedSpin);
    connect(sceneObjectSpinPosY,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSceneObjectPosChangedSpin);
    connect(sceneObjectSpinPosZ,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSceneObjectPosChangedSpin);

    connect(sceneObjectSliderScaleX, &QSlider::valueChanged, this, &MainWindow::onSceneObjectScaleChangedSlider);
    connect(sceneObjectSliderScaleY, &QSlider::valueChanged, this, &MainWindow::onSceneObjectScaleChangedSlider);
    connect(sceneObjectSliderScaleZ, &QSlider::valueChanged, this, &MainWindow::onSceneObjectScaleChangedSlider);

    connect(sceneObjectSpinScaleX,  QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onSceneObjectScaleChangedSpin);
    connect(sceneObjectSpinScaleY,  QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onSceneObjectScaleChangedSpin);
    connect(sceneObjectSpinScaleZ,  QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onSceneObjectScaleChangedSpin);

    connect(sceneObjectSliderRotX, &QSlider::valueChanged, this, &MainWindow::onSCeneObjectRotChangedSlider);
    connect(sceneObjectSliderRotY, &QSlider::valueChanged, this, &MainWindow::onSCeneObjectRotChangedSlider);
    connect(sceneObjectSliderRotZ, &QSlider::valueChanged, this, &MainWindow::onSCeneObjectRotChangedSlider);

    connect(sceneObjectSpinRotX,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSCeneObjectRotChangedSpin);
    connect(sceneObjectSpinRotY,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSCeneObjectRotChangedSpin);
    connect(sceneObjectSpinRotZ,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSCeneObjectRotChangedSpin);
}

void MainWindow::setupScene()
{
    scene = std::make_shared<Scene>();

    camera = std::make_shared<Camera>();

    auto img = std::make_shared<QImage>(800, 600, QImage::Format_RGBA8888);
    //img->fill(Qt::black);
    PixelPainter pixelPainter = PixelPainter(img);
    pixelPainter.fillImage(Colors::Black);

    renderingSurface = std::make_shared<RenderingSurface>(img);
    renderer = std::make_shared<Renderer>(renderingSurface, scene, camera);

    camera->transform.setPosition({ 0.0, 0.0, -200.0 });
    camera->setFov(200.0);

    auto cube = std::make_shared<Cube>(50);
    scene->addObject(cube);

    QListWidgetItem* item = new QListWidgetItem("Cube");
    objectsList->addItem(item);

    refreshScene();
}

void MainWindow::refreshScene()
{
    renderingSurface->clearImg();
    renderer->renderScene();

    auto imgPtr = renderingSurface->getImg();
    if (!imgPtr) return;

    QPixmap pix = QPixmap::fromImage(*imgPtr);
    sceneDisplay->setPixmap(pix);
}

void MainWindow::onAddCubeClicked()
{

    static int cubeCount = 2;
    auto newCube = std::make_shared<Cube>(30);
    scene->addObject(newCube);

    QString itemText = QString("Cube #%1 (side=30)").arg(cubeCount++);
    objectsList->addItem(itemText);

    refreshScene();
}

void MainWindow::onObjectSelected()
{

    int objectId = objectsList->currentRow();
    if (objectId < 0 || objectId >= scene->objectsAmount()) {
        currentObject = nullptr;
        return;
    }

    currentObject = scene->getObject(objectId);
    if (!currentObject) return;

    Vector3 pos = currentObject->transform.getPosition();

    sceneObjectSliderPosX->setValue(pos.x);
    sceneObjectSliderPosY->setValue(pos.y);
    sceneObjectSliderPosZ->setValue(pos.z);

    sceneObjectSpinPosX->setValue(pos.x);
    sceneObjectSpinPosY->setValue(pos.y);
    sceneObjectSpinPosZ->setValue(pos.z);

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
