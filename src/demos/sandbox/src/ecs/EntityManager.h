//
// Created by Felipe Apablaza on 27-03-23.
//

#ifndef XE_ENTITYMANAGER_H
#define XE_ENTITYMANAGER_H

#include "CommonECS.h"
#include <queue>
#include <array>

class EntityManager {
public:
    EntityManager() {
        for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
            availableEntities.push(entity);
        }
    }

    Entity createEntity() {
        const Entity id = availableEntities.front();
        availableEntities.pop();

        ++livingEntities;

        return id;
    }

    void destroyEntity(const Entity entity) {
        signatures[entity].reset();
        availableEntities.push(entity);
        --livingEntities;
    }

    void setSignature(const Entity entity, Signature signature) {
        signatures[entity] = signature;
    }

    Signature getSignature(const Entity entity) const {
        return signatures[entity];
    }

private:
    std::queue<Entity> availableEntities;
    std::array<Signature, MAX_ENTITIES> signatures;
    uint32_t livingEntities = 0;
};


#endif // XE_ENTITYMANAGER_H
