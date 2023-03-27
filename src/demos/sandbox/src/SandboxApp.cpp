

#include "SandboxApp.h"
#include "Components.h"

#include <random>

namespace Sandbox {
    SandboxApp::SandboxApp(int , char **) {}

    void SandboxApp::initialize() {
        coordinator.registerComponent<Gravity>();
        coordinator.registerComponent<RigidBody>();
        coordinator.registerComponent<Transform>();

        physicsSystem = coordinator.registerSystem<PhysicsSystem>();

        Signature signature;
        signature.set(coordinator.getComponentType<Gravity>(), true);
        signature.set(coordinator.getComponentType<RigidBody>(), true);
        signature.set(coordinator.getComponentType<Transform>(), true);

        std::vector<Entity> entities(MAX_ENTITIES);

        std::default_random_engine randomEngine;
        std::uniform_real_distribution<float> randPosition(-100.0f, 100.0f);
        std::uniform_real_distribution<float> randRotation(0.0f, 3.0);
        std::uniform_real_distribution<float> randScale(3.0f, 5.0f);
        std::uniform_real_distribution<float> randGravity(-10.0f, -1.0f);

        const float scale = randScale(randomEngine);

        for (Entity &entity : entities) {
            entity = coordinator.createEntity();

            coordinator.addComponent(entity, Gravity{XE::Vector3f{0.0f, randGravity(randomEngine), 0.0f}});
            coordinator.addComponent(entity, RigidBody{XE::Vector3f{0.0f, 0.0f, 0.0f}, XE::Vector3f{0.0f, 0.0f, 0.0f}});
            coordinator.addComponent(entity, Transform{
                XE::Vector3f{randPosition(randomEngine), randPosition(randomEngine), randPosition(randomEngine)},
                XE::Vector3f{randRotation(randomEngine), randRotation(randomEngine), randRotation(randomEngine)},
                XE::Vector3f{scale, scale, scale}
            });
        }
    }

    void SandboxApp::update(const float seconds) {
        physicsSystem->update(seconds);
    }

    void SandboxApp::render() {
        ++frames;
    }

    bool SandboxApp::shouldClose() const { return !(running && frames < maxFrames); }

} // namespace Sandbox
