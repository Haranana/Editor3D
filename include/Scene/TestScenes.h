#ifndef TESTSCENES_H
#define TESTSCENES_H
#include "Scene.h"
#include <QListWidget>
#include "Scene/Cube.h"
#include "Scene/DistantLight.h"
#include "Scene/PointLight.h"
#include "Scene/SpotLight.h"
#include "Math/Utility.h"

namespace TestScenes{


    //small blue cube | large green cube | small blue cube
    void loadDistantLightTestScene1(Scene& scene, QListWidget& objectsList){
        auto floor = std::make_shared<Cube>();
        scene.addObject(floor);
        QString itemTextFloor = QString("floor");
        objectsList.addItem(itemTextFloor);
        floor->transform.setScales(Vector3(0.5,0.5,0.5));
        floor->transform.setPositionX(-50);
        floor->viewportDisplay.color = Colors::Green;

        auto cube2 = std::make_shared<Cube>();
        scene.addObject(cube2);
        QString itemTextCube2 = QString("Cube2");
        objectsList.addItem(itemTextCube2);
        cube2->transform.setScales(Vector3(0.1,0.1,0.1));
        cube2->transform.setPositionX(-150);
        cube2->viewportDisplay.color = Colors::Blue;

        auto cube = std::make_shared<Cube>();
        scene.addObject(cube);
        QString itemTextCube = QString("Cube");
        objectsList.addItem(itemTextCube);
        cube->transform.setScales(Vector3(0.1,0.1,0.1));
        cube->transform.setAngleX(0.7);
        cube->transform.setPositionX(50);
        cube->viewportDisplay.color = Colors::Blue;

        Vector3 lightDirection(-1.0 , 0.0 , 0.0);
        std::shared_ptr<DistantLight> light = std::make_shared<DistantLight>( lightDirection.normalize());
        light->color = Colors::White;
        light->intensity = 2.0;
        light->castsShadow = true;
        scene.addObject(light);
        QString itemTextLight = QString("light");
        objectsList.addItem(itemTextLight);
    }

    //cube with a wall behind it, illuminated by distant light
    void loadDistantLightTestScene2(Scene& scene, QListWidget& objectsList){
        auto floor = std::make_shared<Cube>();
        scene.addObject(floor);
        QString itemTextFloor = QString("wall");
        objectsList.addItem(itemTextFloor);
        floor->transform.setScales(Vector3(0.1,2.0,2.0));
        floor->transform.setPositionX(-100);
        floor->transform.setAngleY(140, false);
        floor->viewportDisplay.color = Colors::Green;

        auto cube = std::make_shared<Cube>();
        scene.addObject(cube);
        QString itemTextCube = QString("Cube");
        objectsList.addItem(itemTextCube);
        cube->transform.setScales(Vector3(0.1,0.1,0.1));
        cube->transform.setAngleX(0.7);
        cube->transform.setAngleY(140, false);
        cube->transform.setPositionX(50);
        cube->viewportDisplay.color = Colors::Blue;

        Vector3 lightDirection(-1.0 , 0.0 , 0.0);
        std::shared_ptr<DistantLight> light = std::make_shared<DistantLight>( lightDirection.normalize());
        light->color = Colors::White;
        light->intensity = 2.0;
        light->castsShadow = true;
        scene.addObject(light);
        QString itemTextLight = QString("light");
        objectsList.addItem(itemTextLight);
    }



}

#endif // TESTSCENES_H
