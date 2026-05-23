#include "contactcache.h"

#include <cmath>
#include <cstdint>
#include "Collision/physicsgeometry.h"
#include "physicssettings.h"

using namespace PhysicsSettings;

namespace {

std::uint64_t mixHash(std::uint64_t seed, std::uint64_t value) {
    seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
    return seed;
}

std::uint64_t pointerHash(const void* ptr) {
    return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(ptr) >> 4);
}

std::uint64_t quantizedCoordHash(float value) {
    constexpr float quantization = 0.25f;
    std::int64_t quantized = static_cast<std::int64_t>(std::floor(value / quantization));
    return static_cast<std::uint64_t>(quantized);
}

std::uint64_t makeContactKey(const PhysicsContact& contact) {
    std::uint64_t seed = 1469598103934665603ULL;
    seed = mixHash(seed, pointerHash(contact.colliderA));
    seed = mixHash(seed, pointerHash(contact.colliderB));
    seed = mixHash(seed, static_cast<std::uint64_t>(normalSlot(contact.normal)));
    seed = mixHash(seed, quantizedCoordHash(contact.point.x));
    seed = mixHash(seed, quantizedCoordHash(contact.point.y));
    seed = mixHash(seed, quantizedCoordHash(contact.point.z));
    return seed == 0 ? 1 : seed;
}

}

void ContactCache::prepareContacts(std::vector<PhysicsContact>& contacts) const {
    for (PhysicsContact& contact : contacts) {
        contact.key = makeContactKey(contact);
        auto cached = impulses.find(contact.key);
        if (cached == impulses.end())
            continue;

        contact.normalImpulse = cached->second.normalImpulse * kWarmStartScale;
        contact.tangentImpulse = cached->second.tangentImpulse * kWarmStartScale;
    }
}

void ContactCache::storeContactImpulses(const std::vector<PhysicsContact>& contacts) {
    ++solverFrame;

    for (const PhysicsContact& contact : contacts) {
        if (contact.key == 0)
            continue;

        CachedContactImpulse& cached = impulses[contact.key];
        cached.normalImpulse = contact.normalImpulse;
        cached.tangentImpulse = contact.tangentImpulse;
        cached.lastSeenFrame = solverFrame;
    }

    for (auto it = impulses.begin(); it != impulses.end();) {
        if (solverFrame - it->second.lastSeenFrame > kContactCacheKeepFrames)
            it = impulses.erase(it);
        else
            ++it;
    }
}
