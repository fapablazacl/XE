
#pragma once

#include "ecs/ECS.h"

class PhysicsSystem : public System {
public:
    explicit PhysicsSystem(Coordinator &coordinator);

    void update(const float dt);

private:
    Coordinator &coordinator;
};
