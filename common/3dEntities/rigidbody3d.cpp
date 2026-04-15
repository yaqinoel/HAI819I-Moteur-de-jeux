#include "rigidbody3d.h"
#include "collisionshape3d.h"

RigidBody3D::RigidBody3D()
{
    previousPosition = getGlobalPosition();
    currentPosition = getGlobalPosition();
    previousRotation = getGlobalRotation();
    currentRotation = getGlobalRotation();
}

void RigidBody3D::physicsProcess(){
    previousPosition = currentPosition;
    previousRotation = currentRotation;
    velocity += gravity*scene->fixedDeltaTime;
}

void RigidBody3D::postPhysicsProcess(){
    currentPosition += velocity*scene->fixedDeltaTime;
    if(angularVelocity != glm::vec3(0)){
        glm::vec3 axis = glm::normalize(angularVelocity);
        float angle = glm::length(angularVelocity) * scene->fixedDeltaTime;
        glm::quat deltaRot = glm::angleAxis(angle, axis);
        currentRotation = glm::normalize(deltaRot * currentRotation);
    }
}

void RigidBody3D::interpolate(float alpha){
    //std::cout << glm::length(position - glm::mix(previousPosition, currentPosition, alpha)) << std::endl;
    Node3d::setGlobalPosition(glm::mix(previousPosition, currentPosition, std::min(1.0f,alpha)));
    Node3d::setGlobalRotation(glm::mix(previousRotation, currentRotation, std::min(1.0f,alpha)));
}
void RigidBody3D::Translate(const glm::vec3 translation){
    currentPosition  += translation;
    markDirty();
}

void RigidBody3D::setGlobalPosition(const glm::vec3 &globPos){
    Node3d::setGlobalPosition(globPos);
    currentPosition  = globPos;
    previousPosition = globPos;
    markDirty();
}


void RigidBody3D::setGlobalRotation(const glm::quat &globRot){
    Node3d::setGlobalRotation(globRot);
    currentRotation  = globRot;
    previousRotation = globRot;
    markDirty();
}

void RigidBody3D::setLocalRotation(const glm::quat rotation){
    Node3d::setLocalRotation(rotation);
    currentRotation = getGlobalRotation();
    markDirty();
}

void RigidBody3D::setLocalPosition(const glm::vec3 pos){
    Node3d::setLocalPosition(pos);
    currentPosition = getGlobalPosition();
    markDirty();
}

void RigidBody3D::addCollisionShape(CollisionShape3D* c){
    collisions.push_back(c);
    c->rb = this;
    mass += c->mass;
    inertia += c->getInertia() + c->mass * glm::length2(c->getGlobalPosition()- getGlobalPosition());
    if(inertia != glm::mat3(0)) inverseInertia = glm::inverse(inertia);
}

void RigidBody3D::solveCollision(ColliderIntersection collision){
    RigidBody3D* B = collision.colliderB->rb;
    glm::vec3 velA = velocity != glm::vec3(0) ? velocity : collision.axis * collision.t/scene->fixedDeltaTime;
    if(B != nullptr){
        std::cout << "a" << std::endl;
        for(glm::vec3 p : collision.contactPoints){
            glm::vec3 rA = p - getGlobalPosition();
            glm::vec3 rB = p - B->getGlobalPosition();

            glm::vec3 relativeVelocity = (B->velocity + glm::cross(B->angularVelocity, rB)) - (velA + glm::cross(angularVelocity, rA));

            float velAlongNormal = glm::dot(relativeVelocity, collision.axis);
            if(velAlongNormal > 0) continue; // moving apart, ignore

            float e = 0.5f; // restitution coefficient (elasticity)
            float invMassA = (mass > 0) ? 1.0f / mass : 0.0f;
            float invMassB = (B->mass > 0) ? 1.0f / B->mass : 0.0f;

            glm::mat3 invInertiaA = inverseInertia;
            glm::mat3 invInertiaB = B->inverseInertia;

            glm::vec3 raCrossN = glm::cross(rA, collision.axis);
            glm::vec3 rbCrossN = glm::cross(rB, collision.axis);

            float denom = invMassA + invMassB + glm::dot(collision.axis, glm::cross(invInertiaA * raCrossN, rA) + glm::cross(invInertiaB * rbCrossN, rB));

            float j = -(1 + e) * velAlongNormal / denom;

            glm::vec3 impulse = j * collision.axis;
            applyImpulse(-impulse, p);
            B->applyImpulse(impulse, p);

            float percent = 0.9f;
            float slop = 0.02f;
            glm::vec3 correction = std::max(collision.t - slop, 0.0f) / (invMassA + invMassB) * percent * collision.axis;
            currentPosition -= invMassA * correction;
            B->currentPosition += invMassB * correction;
        }
        return;
    }
    else{
        glm::vec3 addedVelocity = glm::vec3(0);
        glm::vec3 addedAngularVelocity = glm::vec3(0);
        for(glm::vec3 p : collision.contactPoints){
            glm::vec3 rA = p - currentPosition;

            glm::vec3 relativeVelocity = (velA + glm::cross(angularVelocity, rA)); //relative to the velocity of B which is 0

            float velAlongNormal = glm::dot(relativeVelocity, collision.axis);


            float invMassA = (mass > 0) ? 1.0f / mass : 0.0f;

            glm::vec3 rnA = glm::cross(rA, collision.axis);

            float denom = invMassA + glm::dot(collision.axis, glm::cross(inverseInertia * rnA, rA));

            float j = (-velAlongNormal / denom);
            glm::vec3 impulse = j * collision.axis;
            //if(j != 0) applyImpulse(impulse, p);

            addedVelocity += impulse/mass;
            addedAngularVelocity += inverseInertia * glm::cross(rA, impulse);
        }

        velocity += addedVelocity;
        angularVelocity += addedAngularVelocity*unlockedRotation;
    }
}

void RigidBody3D::applyImpulse(glm::vec3 impulse, glm::vec3 worldPoint){
    if(mass != 0 && impulse != glm::vec3(0)){
        velocity += impulse / mass;

        glm::vec3 r = worldPoint - currentPosition;
        glm::vec3 angularImpulse = glm::cross(r, impulse);

        angularVelocity += inverseInertia * angularImpulse * unlockedRotation;
    }
}
