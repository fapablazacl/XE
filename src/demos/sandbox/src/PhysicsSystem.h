
#pragma once

#include "ecs/ECS.h"

class PhysicsSystem : public System {
public:
    explicit PhysicsSystem(Coordinator &coordinator);

    virtual ~PhysicsSystem() {}

    void update(const float dt);

    void handleMessage(const Message &message) override;

private:
    Coordinator &coordinator;
};
