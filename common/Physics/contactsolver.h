#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "physicstypes.h"

class RigidBody3D;

class ContactSolver {
public:
    void warmStartContacts(std::vector<PhysicsContact>& contacts) const;
    void solvePositions(const std::vector<PhysicsContact>& contacts) const;
    void solveVelocities(std::vector<PhysicsContact>& contacts) const;
};

float inverseMass(RigidBody3D* body);
glm::vec3 pointVelocity(RigidBody3D* body, const glm::vec3& point);
