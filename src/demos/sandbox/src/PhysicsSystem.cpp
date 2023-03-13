
#include "PhysicsSystem.h"
#include "Components.h"

PhysicsSystem::PhysicsSystem(Coordinator &coordinator) : coordinator{coordinator} {}

void PhysicsSystem::update(const float dt) {
    for (Entity const &entity : entities) {
        auto& rigidBody = coordinator.getComponent<RigidBody>(entity);
        auto& gravity = coordinator.getComponent<Gravity>(entity);
        auto& transform = coordinator.getComponent<Transform>(entity);

        transform.position += rigidBody.velocity * dt;
        rigidBody.velocity += gravity.force * dt;
    }
}
