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

    void loadOnlyDistantLight(Scene& scene, QListWidget& objectsList){
        Vector3 lightDirection(0.0 , -1.0 , -1.0);
        std::shared_ptr<DistantLight> light = std::make_shared<DistantLight>( lightDirection.normalize());
        light->color = Colors::White;
        light->intensity = 2.0;
        light->castsShadow = true;
        scene.addObject(light);
        QString itemTextLight = QString("light");
        objectsList.addItem(itemTextLight);
    }

    void loadPointLightTestScene4(Scene& scene, QListWidget& objectsList){

        auto cube3 = std::make_shared<Cube>();
        scene.addObject(cube3);
        QString itemTextCube3 = QString("Cub3");
        objectsList.addItem(itemTextCube3);
        cube3->transform.setScales(Vector3(0.1,0.1,0.1));
        cube3->transform.setPositionX(100);
        cube3->transform.setScales(Vector3(0.1, 3.0, 3.0));
        cube3->viewportDisplay.color = Colors::Red;

        auto cube4 = std::make_shared<Cube>();
        scene.addObject(cube4);
        QString itemTextCube4 = QString("Cube4");
        objectsList.addItem(itemTextCube4);
        cube4->transform.setScales(Vector3(0.1,0.1,0.1));
        cube4->transform.setPositionY(100);
        cube4->transform.setScales(Vector3(3.0, 0.1, 3.0));
        cube4->viewportDisplay.color = Colors::Purple;


        std::shared_ptr<PointLight> light = std::make_shared<PointLight>();
        light->color = Colors::White;
        light->intensity = 2.0;
        light->castsShadow = true;
        scene.addObject(light);
        QString itemTextLight = QString("light");
        objectsList.addItem(itemTextLight);
    }

    void loadSpotLightOnEmptyWall(Scene& scene, QListWidget& objectsList){
        Vector3 lightDirection(0.0 , 0.0 , -1.0);
        std::shared_ptr<SpotLight> light = std::make_shared<SpotLight>();
        light->color = Colors::White;
        light->intensity = 2.0;
        light->castsShadow = true;
        light->direction = lightDirection;
        light->transform.setPositionZ(200);
        scene.addObject(light);
        QString itemTextLight = QString("light");
        objectsList.addItem(itemTextLight);

        auto floor = std::make_shared<Cube>();
        scene.addObject(floor);
        QString itemTextFloor = QString("floor");
        objectsList.addItem(itemTextFloor);
        floor->transform.setScales(Vector3(0.5,0.5,0.5));
        floor->viewportDisplay.color = Colors::Purple;
    }

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

    void loadSpotLightTestScene1(Scene& scene, QListWidget& objectsList){
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


        std::shared_ptr<SpotLight> light = std::make_shared<SpotLight>();
        light->direction = {-1.0,0.0,0.0};
        light->color = Colors::White;
        light->intensity = 1.0;
        light->castsShadow = true;
        light->transform.setPositionX(300);
        scene.addObject(light);
        QString itemTextLight = QString("light");
        objectsList.addItem(itemTextLight);
    }


    void loadSpotLightTestScene2(Scene& scene, QListWidget& objectsList){
        auto floor = std::make_shared<Cube>();
        scene.addObject(floor);
        QString itemTextFloor = QString("floor");
        objectsList.addItem(itemTextFloor);
        floor->transform.setScales(Vector3(2.0,2.0,0.1));
        floor->transform.setPositionZ(-50);
        floor->viewportDisplay.color = Colors::Green;


        auto cube = std::make_shared<Cube>();
        scene.addObject(cube);
        QString itemTextCube = QString("Cube");
        objectsList.addItem(itemTextCube);
        cube->transform.setScales(Vector3(0.4,0.4,0.4));
        cube->transform.setAngleX(0.7);
        cube->viewportDisplay.color = Colors::Blue;


        std::shared_ptr<SpotLight> light = std::make_shared<SpotLight>();
        light->biasType = Light::BiasType::SLOPE_SCALED;
        light->direction = {-1.0,0.0,0.0};
        light->color = Colors::White;
        light->intensity = 8.0;
        light->castsShadow = true;
        light->transform.setPositionX(300);
        scene.addObject(light);
        QString itemTextLight = QString("light");
        objectsList.addItem(itemTextLight);
    }

    void loadPointLightTestScene1(Scene& scene, QListWidget& objectsList){

        auto cube1 = std::make_shared<Cube>();
        scene.addObject(cube1);
        QString itemTextCube1 = QString("Cube1");
        objectsList.addItem(itemTextCube1);
        cube1->transform.setScales(Vector3(0.1,0.1,0.1));
        cube1->transform.setPositionX(-50);
        cube1->viewportDisplay.color = Colors::Blue;

        auto cube2 = std::make_shared<Cube>();
        scene.addObject(cube2);
        QString itemTextCube2 = QString("Cube2");
        objectsList.addItem(itemTextCube2);
        cube2->transform.setScales(Vector3(0.1,0.1,0.1));
        cube2->transform.setPositionZ(-50);
        cube2->transform.setPositionY(-50);
        cube2->viewportDisplay.color = Colors::Green;

        auto cube3 = std::make_shared<Cube>();
        scene.addObject(cube3);
        QString itemTextCube3 = QString("Cub3");
        objectsList.addItem(itemTextCube3);
        cube3->transform.setScales(Vector3(0.1,0.1,0.1));
        cube3->transform.setPositionX(50);
        cube3->viewportDisplay.color = Colors::Red;

        auto cube4 = std::make_shared<Cube>();
        scene.addObject(cube4);
        QString itemTextCube4 = QString("Cube4");
        objectsList.addItem(itemTextCube4);
        cube4->transform.setScales(Vector3(0.1,0.1,0.1));
        cube4->transform.setPositionZ(50);
        cube4->transform.setPositionY(50);
        cube4->viewportDisplay.color = Colors::Purple;

        std::shared_ptr<PointLight> light = std::make_shared<PointLight>();
        light->color = Colors::White;
        light->intensity = 2.0;
        light->castsShadow = true;
        scene.addObject(light);
        QString itemTextLight = QString("light");
        objectsList.addItem(itemTextLight);
    }

    void loadPointLightTestScene2(Scene& scene, QListWidget& objectsList){

        auto cube1 = std::make_shared<Cube>();
        scene.addObject(cube1);
        QString itemTextCube1 = QString("Cube1");
        objectsList.addItem(itemTextCube1);
        cube1->transform.setScales(Vector3(0.1,0.1,0.1));
        cube1->transform.setPositionX(-100);
        cube1->transform.setScales(Vector3(0.1, 3.0, 3.0));
        cube1->viewportDisplay.color = Colors::Blue;

        auto cube2 = std::make_shared<Cube>();
        scene.addObject(cube2);
        QString itemTextCube2 = QString("Cube2");
        objectsList.addItem(itemTextCube2);
        cube2->transform.setScales(Vector3(0.1,0.1,0.1));
        cube2->transform.setPositionY(-100);
        cube2->transform.setScales(Vector3(3.0, 0.1, 3.0));
        cube2->viewportDisplay.color = Colors::Green;

        auto cube3 = std::make_shared<Cube>();
        scene.addObject(cube3);
        QString itemTextCube3 = QString("Cub3");
        objectsList.addItem(itemTextCube3);
        cube3->transform.setScales(Vector3(0.1,0.1,0.1));
        cube3->transform.setPositionX(100);
        cube3->transform.setScales(Vector3(0.1, 3.0, 3.0));
        cube3->viewportDisplay.color = Colors::Red;

        auto cube4 = std::make_shared<Cube>();
        scene.addObject(cube4);
        QString itemTextCube4 = QString("Cube4");
        objectsList.addItem(itemTextCube4);
        cube4->transform.setScales(Vector3(0.1,0.1,0.1));
        cube4->transform.setPositionY(100);
        cube4->transform.setScales(Vector3(3.0, 0.1, 3.0));
        cube4->viewportDisplay.color = Colors::Purple;

        auto backwall = std::make_shared<Cube>();
        scene.addObject(backwall);
        QString itemTextBackwall = QString("backwall");
        objectsList.addItem(itemTextBackwall);
        backwall->transform.setScales(Vector3(0.1,0.1,0.1));
        backwall->transform.setPositionZ(-100);
        backwall->transform.setScales(Vector3(3.0, 3.0, 0.1));
        backwall->viewportDisplay.color = Colors::Orange;

        std::shared_ptr<PointLight> light = std::make_shared<PointLight>();
        light->color = Colors::White;
        light->intensity = 2.0;
        light->castsShadow = true;
        scene.addObject(light);
        QString itemTextLight = QString("light");
        objectsList.addItem(itemTextLight);
    }

    void loadPointLightTestScene3(Scene& scene, QListWidget& objectsList){

        auto cube1 = std::make_shared<Cube>();
        scene.addObject(cube1);
        QString itemTextCube1 = QString("Cube1");
        objectsList.addItem(itemTextCube1);
        cube1->transform.setPositionX(-150);
        cube1->transform.setScales(Vector3(0.1, 3.0, 3.0));
        cube1->viewportDisplay.color = Colors::Blue;

        auto cube2 = std::make_shared<Cube>();
        scene.addObject(cube2);
        QString itemTextCube2 = QString("Cube2");
        objectsList.addItem(itemTextCube2);
        cube2->transform.setPositionX(-50);
        cube2->transform.setScales(Vector3(0.2, 0.2, 0.2));
        cube2->viewportDisplay.color = Colors::Green;

        auto cube3 = std::make_shared<Cube>();
        scene.addObject(cube3);
        QString itemTextCube3 = QString("Cub3");
        objectsList.addItem(itemTextCube3);
        cube3->transform.setPositionX(150);
        cube3->transform.setPositionY(150);
        cube3->transform.setAngleZ(0.7);
        cube3->transform.setAngleY(0.7);
        cube3->transform.setScales(Vector3(0.1, 3.0, 3.0));
        cube3->viewportDisplay.color = Colors::Red;

        auto cube4 = std::make_shared<Cube>();
        scene.addObject(cube4);
        QString itemTextCube4 = QString("Cube4");
        objectsList.addItem(itemTextCube4);
        cube4->transform.setPositionX(50);
        cube4->transform.setPositionY(50);
        cube4->transform.setScales(Vector3(0.2, 0.2, 0.2));
        cube4->viewportDisplay.color = Colors::Purple;

        std::shared_ptr<PointLight> light = std::make_shared<PointLight>();
        light->color = Colors::White;
        light->intensity = 2.0;
        light->castsShadow = true;
        scene.addObject(light);
        QString itemTextLight = QString("light");
        objectsList.addItem(itemTextLight);

    }




}

#endif // TESTSCENES_H
