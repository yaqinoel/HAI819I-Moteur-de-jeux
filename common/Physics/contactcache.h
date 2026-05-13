#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include "physicstypes.h"

struct CachedContactImpulse {
    float normalImpulse = 0.0f;
    glm::vec3 tangentImpulse = glm::vec3(0.0f);
    int lastSeenFrame = 0;
};

class ContactCache {
public:
    void prepareContacts(std::vector<PhysicsContact>& contacts) const;
    void storeContactImpulses(const std::vector<PhysicsContact>& contacts);

private:
    std::unordered_map<std::uint64_t, CachedContactImpulse> impulses;
    int solverFrame = 0;
};
