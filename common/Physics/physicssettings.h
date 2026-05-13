#pragma once

namespace PhysicsSettings {

constexpr float kAxisEpsilon = 1e-6f;
constexpr float kContactEpsilon = 1e-3f;
constexpr float kAngularImpulseScale = 0.55f;
constexpr float kAirAngularDamping = 0.985f;
constexpr float kGroundAngularDamping = 0.82f;
constexpr float kMaxAngularSpeed = 10.0f;
constexpr float kRestingVerticalSpeed = 0.08f;
constexpr float kRestingAngularSpeed = 0.08f;
constexpr float kSleepLinearSpeed = 0.25f;
constexpr float kSleepAngularSpeed = 0.25f;
constexpr float kSleepDelay = 0.1f;
constexpr float kWarmStartScale = 0.85f;
constexpr float kWakeImpactSpeed = 0.35f;
constexpr float kWakePenetration = 0.03f;
constexpr float kSupportNormalY = 0.45f;
constexpr float kMovingSupportSpeed = 0.02f;
constexpr int kContactCacheKeepFrames = 3;

constexpr float kPositionSlop = 0.004f;
constexpr float kPositionCorrectionPercent = 0.25f;

}
