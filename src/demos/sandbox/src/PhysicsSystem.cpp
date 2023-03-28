
#include "PhysicsSystem.h"
#include "Components.h"

PhysicsSystem::PhysicsSystem(Coordinator &coordinator) : coordinator{coordinator} {}

void PhysicsSystem::handleMessage(const Message &message) {
    if (message.type == Message::SYSTEM_UPDATE) {
        update(message.dt);
    }
}

void PhysicsSystem::update(const float dt) {
    for (Entity const &entity : entities) {
        auto& rigidBody = coordinator.getComponent<RigidBody>(entity);
        auto& gravity = coordinator.getComponent<Gravity>(entity);
        auto& transform = coordinator.getComponent<Transform>(entity);

        transform.position += rigidBody.velocity * dt;
        rigidBody.velocity += gravity.force * dt;
    }
}
