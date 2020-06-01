//
// Created by Aloysio Galvão Lopes on 2020-05-30.
//

#include "Bird.h"

Bird::Bird(Shaders &shaders, vcl::vec3 pos, float scale, vcl::vec3 speed) : Object(true), p(pos), dp(speed) {
    mesh = vcl::mesh_primitive_cone();
    mesh.shader = shaders["mesh"];
    mesh.uniform.transform.scaling = scale;
}

void Bird::drawMesh(vcl::camera_scene &camera) {
    // Adjusting mesh rotation and angle
    mesh.uniform.light = light;
    mesh.uniform.transform.rotation = vcl::rotation_between_vector_mat3({0, 0, 1}, dp);
    mesh.uniform.transform.translation = p;
    ndp = dp;

    vcl::draw(mesh, camera);
}

vcl::vec3 Bird::getSpeed() {
    return dp;
}

void Bird::setFutureSpeed(vcl::vec3 speed) {
    ndp = speed;
}

void Bird::addFutureSpeed(vcl::vec3 speed) {
    ndp += speed;
}

vcl::vec3 Bird::getFutureSpeed() {
    return ndp;
}

vcl::vec3 Bird::getPosition() {
    return p;
}

void Bird::setPosition(vcl::vec3 pos) {
    p = pos;
}

void Bird::stepSpeed() {
    dp = ndp;
}

void Bird::stepPosition() {
    p += dp;
}
