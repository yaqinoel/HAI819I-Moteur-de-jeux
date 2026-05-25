#include "charactercontroller.h"
#include <Scenes/DynamicVoxel.h>
#include <Scenes/PhysicsCube.h>
#include <Scenes/Cube.h>
#include <common/3dEntities/Meshes/Terrain/proceduralvoxelterrain.h>

CharacterController::CharacterController() {
    canSleep = false;
    paused = false;
    for(int i = 0; i < inventory_size; i ++){
        inventory.push_back(glm::vec2(0, 0));
    }
}
void drawCube(Scene *scene, glm::vec3 center, glm::quat rotation)
{
    float sizemult = 1.001f;

    auto rotatePoint = [&](glm::vec3 p)
    {
        return center + (rotation * (p * sizemult));
    };

    glm::vec3 p000 = rotatePoint(glm::vec3(-0.5f, -0.5f, -0.5f));
    glm::vec3 p001 = rotatePoint(glm::vec3(-0.5f, -0.5f,  0.5f));
    glm::vec3 p010 = rotatePoint(glm::vec3(-0.5f,  0.5f, -0.5f));
    glm::vec3 p011 = rotatePoint(glm::vec3(-0.5f,  0.5f,  0.5f));

    glm::vec3 p100 = rotatePoint(glm::vec3( 0.5f, -0.5f, -0.5f));
    glm::vec3 p101 = rotatePoint(glm::vec3( 0.5f, -0.5f,  0.5f));
    glm::vec3 p110 = rotatePoint(glm::vec3( 0.5f,  0.5f, -0.5f));
    glm::vec3 p111 = rotatePoint(glm::vec3( 0.5f,  0.5f,  0.5f));

    // Top face
    scene->drawLine(p111, p110);
    scene->drawLine(p110, p010);
    scene->drawLine(p010, p011);
    scene->drawLine(p011, p111);

    // Bottom face
    scene->drawLine(p101, p100);
    scene->drawLine(p100, p000);
    scene->drawLine(p000, p001);
    scene->drawLine(p001, p101);

    // Vertical edges
    scene->drawLine(p111, p101);
    scene->drawLine(p110, p100);
    scene->drawLine(p010, p000);
    scene->drawLine(p011, p001);
}

bool CharacterController::add_in_inventory(glm::ivec2 obj, int start){
    for(int i = start; i < inventory_size; i ++){
        if(inventory[i][1] == 0){
            inventory[i] = obj;
            return true;
        }
        if(inventory[i][0] == obj[0]){
            int tot = inventory[i][1]+obj[1];
            if(tot <= 64){
                inventory[i][1] = tot;
                return true;
            }
            glm::ivec2 remaining = glm::ivec2(obj[0], 0);
            remaining[1] = (tot)%64;
            inventory[i][1] += obj[1] - remaining[1];
            return add_in_inventory(remaining, start+1);
        }
    }
    return false;
}

int CharacterController::remove_one_in_inventory(){
    int v = inventory[current_inventory_case_selected][0];
    inventory[current_inventory_case_selected][1]--;
    if(inventory[current_inventory_case_selected][1] <= 0) inventory[current_inventory_case_selected] = glm::ivec2(0, 0);
    return v;
}

void CharacterController::process(float deltaTime){
    RigidBody3D::process(deltaTime);
    GLFWwindow* window = glfwGetCurrentContext();
    if(!paused)glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (scene->inputPressed("escape")){
        paused = !paused;
        cam->paused = paused;
    }

    if(paused) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return;
    }

    //rotation
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    axialInputs = glm::vec2(0);

    glm::vec3 cameraForward = cam->forward();
    glm::vec3 planarCameraForward = glm::vec3(cameraForward.x, 0.0f, cameraForward.z);
    if (glm::dot(planarCameraForward, planarCameraForward) > 0.0001f) {
        cameraForwardxz = glm::normalize(planarCameraForward);
    }

    if(scene->inputPressed("wheelUp")){
        current_inventory_case_selected = (current_inventory_case_selected+1)%10;
    }
    if(scene->inputPressed("wheelDown")){
        current_inventory_case_selected = (current_inventory_case_selected+9)%10;
    }

    if (scene->inputHeld("forward")){
        axialInputs.x += 1;
    }

    if (scene->inputHeld("backwards")){
        axialInputs.x -= 1;
    }
    if (scene->inputHeld("right")){
        axialInputs.y += 1;
    }
    if (scene->inputHeld("left")){
        axialInputs.y -= 1;
    }
    if (scene->inputPressed("jump")){
        if(velocity.y <= 0.01 && isOnGround()){
            jumpPressed = true;
        }
    }
    if (scene->inputPressed("action3") && inventory[current_inventory_case_selected][1] > 0){
        const float projectileSpawnDistance = 2.0f;
        const float projectileHalfExtent = 0.5f;
        const float projectileSpawnPadding = 0.15f;
        const float projectileMinSpawnDistance = projectileHalfExtent + projectileSpawnPadding;

        float spawnDistance = projectileSpawnDistance;
        RayIntersection spawnRaycast = scene->raycast(
            cam->getGlobalPosition(),
            cameraForward,
            projectileSpawnDistance + projectileHalfExtent + projectileSpawnPadding,
            1ULL
            );
        if (spawnRaycast.intersectionExists) {
            spawnDistance = spawnRaycast.t - projectileHalfExtent - projectileSpawnPadding;
        }

        if (spawnDistance < projectileMinSpawnDistance) {
            return;
        }

        if (spawnDistance >= projectileMinSpawnDistance) {
            RigidBody3D* voxel = makeDynamicVoxel(inventory[current_inventory_case_selected][0], scene->worldMaterial);
            voxel->setGlobalPosition(cam->getGlobalPosition() + cameraForward * spawnDistance);
            voxel->setForward(cameraForwardxz);
            voxel->velocity = cameraForward * 8.0f + UP * 3.0f;
            instantiate(voxel);
            remove_one_in_inventory();
        }
    }

    if (scene->inputPressed("testBigVoxel")){
        const float projectileSpawnDistance = 3.0f;
        const float projectileHalfExtent = 1.1f;
        const float projectileSpawnPadding = 0.15f;
        const float projectileMinSpawnDistance = projectileHalfExtent + projectileSpawnPadding;

        float spawnDistance = projectileSpawnDistance;
        RayIntersection spawnRaycast = scene->raycast(
            cam->getGlobalPosition(),
            cameraForward,
            projectileSpawnDistance + projectileHalfExtent + projectileSpawnPadding,
            1ULL
            );
        if (spawnRaycast.intersectionExists) {
            spawnDistance = spawnRaycast.t - projectileHalfExtent - projectileSpawnPadding;
        }

        if (spawnDistance >= projectileMinSpawnDistance) {
            RigidBody3D* voxel = makeDynamicVoxelIsland(scene->worldMaterial);
            voxel->setGlobalPosition(cam->getGlobalPosition() + cameraForward * spawnDistance);
            voxel->setForward(cameraForwardxz);
            voxel->velocity = cameraForward * 18.0f + UP * 6.0f;
            instantiate(voxel);
        }
    }

    if (scene->inputPressed("testPBRCube")){
        const float projectileSpawnDistance = 3.0f;
        const float projectileHalfExtent = 1.1f;
        const float projectileSpawnPadding = 0.15f;
        const float projectileMinSpawnDistance = projectileHalfExtent + projectileSpawnPadding;

        float spawnDistance = projectileSpawnDistance;
        RayIntersection spawnRaycast = scene->raycast(
            cam->getGlobalPosition(),
            cameraForward,
            projectileSpawnDistance + projectileHalfExtent + projectileSpawnPadding,
            1ULL
            );
        if (spawnRaycast.intersectionExists) {
            spawnDistance = spawnRaycast.t - projectileHalfExtent - projectileSpawnPadding;
        }

        if (spawnDistance >= projectileMinSpawnDistance) {
            RigidBody3D* voxel = makePBRPhysicsCube(scene->pbrCubeTestMaterial);
            voxel->setGlobalPosition(cam->getGlobalPosition() + cameraForward * spawnDistance);
            voxel->setForward(cameraForwardxz);
            voxel->velocity = cameraForward * 18.0f + UP * 6.0f;
            instantiate(voxel);
        }
    }

    glm::vec3 ray_start = cam->getGlobalPosition();
    glm::vec3 camera_forward = cam->forward();
    float ray_length = 4.0;
    RayIntersection forward_raycast = scene->raycast(ray_start, camera_forward, ray_length, 1ULL);
    selectedCollider = nullptr;
    if(forward_raycast.intersectionExists && forward_raycast.collider != nullptr){
        Collider3D* collider = forward_raycast.collider;
        glm::vec3 worldHit = forward_raycast.point;
        glm::vec3 colliderPos = collider->getGlobalPosition();
        glm::quat colliderRot = collider->getGlobalRotation();
        glm::vec3 localHit = glm::inverse(colliderRot) * (worldHit - colliderPos);
        localHit += glm::inverse(colliderRot) * (camera_forward * 0.01f);
        selectedLocalCenter = glm::round(localHit);
        selectedCollider = collider;
        ShapeType shape_type = collider->getShape()->type;
        if(shape_type == TERRAIN_VOXEL){
            if (scene->inputPressed("action1")){
                ProceduralVoxelTerrain* voxel = dynamic_cast<ProceduralVoxelTerrain*>(collider->getParent());
                if (voxel)
                {
                    glm::vec3 worldVoxelCenter = colliderPos + (colliderRot * selectedLocalCenter);
                    add_in_inventory(glm::vec2(voxel->removeTile(worldVoxelCenter), 1));
                }
            }
            if (scene->inputPressed("action2") && inventory[current_inventory_case_selected][1] > 0){
                ProceduralVoxelTerrain* voxel = dynamic_cast<ProceduralVoxelTerrain*>(collider->getParent());
                if (voxel)
                {
                    glm::vec3 localHit_out = localHit - glm::inverse(colliderRot) * (camera_forward * 0.01f)*2.0f;
                    glm::vec3 selectedLocalCenter_out = glm::round(localHit_out);
                    glm::vec3 worldVoxelCenter = colliderPos + (colliderRot * selectedLocalCenter_out);
                    auto hits = scene->cubeOverlapTest(worldVoxelCenter,colliderRot,glm::vec3(0.99f)); // with all
                    if (hits.empty()) {
                        voxel->addTile(worldVoxelCenter, remove_one_in_inventory());
                    }
                }
            }
        }
        if(shape_type == VOXEL){
            if (scene->inputPressed("action1")){
                DynamicVoxel* voxel = dynamic_cast<DynamicVoxel*>(collider->getParent());
                if (voxel)
                {
                    glm::vec3 worldVoxelCenter = colliderPos + (colliderRot * selectedLocalCenter);
                    add_in_inventory(glm::vec2(voxel->removeTile(worldVoxelCenter), 1));
                    selectedCollider = nullptr;
                }
            }
            if (scene->inputPressed("action2") && inventory[current_inventory_case_selected][1] > 0){
                DynamicVoxel* voxel = dynamic_cast<DynamicVoxel*>(collider->getParent());
                if (voxel)
                {
                    glm::vec3 localHit_out = localHit - glm::inverse(colliderRot) * (camera_forward * 0.01f)*2.0f;
                    glm::vec3 selectedLocalCenter_out = glm::round(localHit_out);
                    glm::vec3 worldVoxelCenter = colliderPos + (colliderRot * selectedLocalCenter_out);
                    auto hits = scene->cubeOverlapTest(worldVoxelCenter,colliderRot,glm::vec3(0.95f));
                    if (hits.empty()) {
                        voxel->addTile(worldVoxelCenter, remove_one_in_inventory());
                    }
                }
            }
        }
    }
}

void CharacterController::lateProcess(float deltaTime){
    (void)deltaTime;
    if (selectedCollider == nullptr || !selectedCollider->getVisible() || paused) {
        return;
    }

    glm::quat colliderRot = selectedCollider->getGlobalRotation();
    glm::vec3 worldVoxelCenter = selectedCollider->getGlobalPosition() + (colliderRot * selectedLocalCenter);
        drawCube(scene, worldVoxelCenter, colliderRot);
}


void CharacterController::physicsProcess(){
    RigidBody3D::physicsProcess();
    if(paused)return;
    glm::vec3 planarVelocity = (axialInputs.x * cameraForwardxz + axialInputs.y * cam->right());
    if(planarVelocity != glm::vec3(0)) planarVelocity = glm::normalize(planarVelocity)*speed;

    velocity = planarVelocity+glm::vec3(0, velocity.y, 0);

    if(jumpPressed){
        velocity.y = jumpStrength;
        jumpPressed = false;
    }
}
