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
    currentPosition  = globPos;
    markDirty();
}


void RigidBody3D::setGlobalRotation(const glm::quat &globRot){
    currentRotation  = globRot;
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

            // float percent = 0.8f; // 20% leeway
            // float slop = 0.01f;   // small allowance
            // glm::vec3 correction = std::max(collision.t - slop, 0.0f) / (invMassA + invMassB) * percent * collision.axis;
            // currentPosition -= invMassA * correction;
            // B->currentPosition += invMassB * correction;
        }
        return;
    }
    else{
        // std::cout << "\n\n " << std::endl;
        // std::cout << "position " << glm::to_string(currentPosition) << std::endl;
        // std::cout << "t " << collision.t << std::endl;
        // std::cout << "posA " << glm::to_string(currentPosition) << std::endl;
        // std::cout << "posB " << glm::to_string(collision.colliderB->getGlobalPosition()) << std::endl;


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

            // std::cout << "\nmass " << mass << std::endl;
            // std::cout << "velocity " << glm::to_string(velocity) << std::endl;
            // std::cout << "relativeVelocity " << glm::to_string(relativeVelocity) << std::endl;
            // std::cout << "velAlongNormal " << velAlongNormal << std::endl;
            // std::cout << "collision.axis " << glm::to_string(collision.axis) << std::endl;
            // std::cout << "rA " << glm::to_string(rA) << std::endl;
            // std::cout << "inverseInertia " << glm::to_string(inverseInertia) << std::endl;
            // std::cout << "raCrossN " << glm::to_string(rnA) << std::endl;
            // std::cout << "denom " << denom << std::endl;
            // std::cout << "velAlongNormal " << velAlongNormal << std::endl;
            // std::cout << "j " << j << std::endl;
            // std::cout << "impulse " << glm::to_string(impulse) << std::endl;
            // std::cout << "vel currentl " << glm::to_string(addedVelocity)<< std::endl;
        }

        velocity += addedVelocity;
        angularVelocity += addedAngularVelocity;
        //std::cout << " vel final " << glm::to_string(velocity) <<"\n"<< std::endl;
    }
}

void RigidBody3D::applyImpulse(glm::vec3 impulse, glm::vec3 worldPoint){
    if(mass != 0 && impulse != glm::vec3(0)){
        velocity += impulse / mass;

        glm::vec3 r = worldPoint - currentPosition;
        glm::vec3 angularImpulse = glm::cross(r, impulse);

        angularVelocity += inverseInertia * angularImpulse;
    }
}
