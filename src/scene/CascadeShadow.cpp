//
// Created by igor on 30/04/2020.
//

#include <src/objects/boid/Boid.h>
#include "objects/terrain/BaseTerrain.h"
#include "objects/forest/Forest.h"
#include "CascadeShadow.h"

CascadeShadow::CascadeShadow(vcl::light_source &light, int mapResolution)
        : nCascades(3) {
    lastUpdated = 0;
    maps = std::make_shared<vcl::depth_maps>(nCascades, mapResolution);
    for (int i = 0; i < nCascades; i++) {
        lastCamera.emplace_back(vcl::camera_scene());
        lastSunAngle.emplace_back(0.0f);
        lastTime.emplace_back(0.0f);
        nMovableObjects.emplace_back(0);
    }
    float zN;
    float zF = light.get_z_near();
    for (int i = 0; i < nCascades; i++) {
        zN = zF;
        zF = light.get_z_far() / std::max(1.0f, (4.0f * (float) (nCascades - i - 1)));
        zNearFarList.push_back({zN, zF});
        lights.emplace_back(std::make_shared<vcl::light_source>(light.get_pos(), light.get_dir(), zN, zF, i));
    }
    zNearFarList.push_back({zNearFarList[0].first, zNearFarList.back().second});
}

void CascadeShadow::update(std::vector<std::shared_ptr<Object> > &movableObjects,
                           std::vector<std::shared_ptr<Object> > &stillObjects,
                           std::shared_ptr<SceneGui> &gui, std::shared_ptr<Shaders> &shaders,
                           float time) {
    shaders->override("depth", true);

    vcl::camera_scene &camera = gui->getCamera();

    float sunAngle = gui->getSunAngle();
    vcl::vec3 lightPos(1000.0f * std::cos((float) (0.5 * M_PI) - sunAngle), 0.0f,
                       1000.0f * std::sin((float) (0.5 * M_PI) - sunAngle));
    vcl::vec3 lightDir = -lightPos.normalized();
    lights[lastUpdated]->update(camera, lightPos, lightDir);
    lastCamera[lastUpdated] = camera;
    lastSunAngle[lastUpdated] = sunAngle;
    lastTime[lastUpdated] = time;

    render(stillObjects, camera);
    render(movableObjects, camera);
    lastUpdated = (lastUpdated + 1) % nCascades;

    shaders->override("depth", false);
    maps->unbind_all(gui->getWindowWidth(), gui->getWindowHeight());
}

std::vector<std::pair<float, float> > &CascadeShadow::getZNearFarList() {
    return zNearFarList;
}

void CascadeShadow::render(std::vector<std::shared_ptr<Object> > &objects, vcl::camera_scene &camera) {
    maps->bind(lastUpdated);
    nMovableObjects[lastUpdated] = 0;

    for (auto &obj : objects) {
        // If it is the terrain, it will be in all cascades
        auto *t = dynamic_cast<BaseTerrain *> (obj.get());
        auto *f = dynamic_cast<Forest *>(obj.get());
        auto *b = dynamic_cast<Boid *>(obj.get());
        if (t != nullptr) {
            t->setLight(lights, lastUpdated);
            t->draw(camera);
        } else if (f != nullptr) {
            // A forest will be decomposed in its objects
            for (auto &o : f->getObjects())
                renderObject(o, camera);
        } else if (b != nullptr) {
            // A boid will be decomposed in its birds
            for (auto &o : b->getObjects())
                renderObject(o, camera);
        } else renderObject(obj, camera);
    }
}

void CascadeShadow::renderObject(std::shared_ptr<Object> &obj, vcl::camera_scene &camera) {
    if (!obj->hasShadow())
        obj->setLight(lights[lastUpdated]);

    // A normal object must be in the correct frustum to be rendered
    if (obj->hasShadow() && (obj->getLight()->get_shadow_map_id() == lastUpdated ||
                             obj->getBoundingSphere().isInLightRange(camera, *lights[lastUpdated]))) {
        obj->setLight(lights[lastUpdated]);
        nMovableObjects[lastUpdated]++;
        obj->draw(camera);
    }
}
