#pragma once

#include <common/Controls/hoveringcontroller.h>
#include "Cube.h"

HoveringController* makeHoveringController(){
    HoveringController* character = new HoveringController();
    Node3d* child = makeCube();
    character->addChild(child);
    character->name = "rigidbody cube controller";
    character->addCollisionShape(((CollisionShape3D*)(child->getChildren()[0])));

    return character;
}
