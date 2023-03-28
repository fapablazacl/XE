//
// Created by Felipe Apablaza on 27-03-23.
//

#ifndef XE_COORDINATOR_H
#define XE_COORDINATOR_H

#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"
#include "MessageBus.h"

#include <memory>

class Coordinator {
public:
    Coordinator() {
        entityManager = std::make_shared<EntityManager>();
        componentManager = std::make_shared<ComponentManager>();
        systemManager = std::make_shared<SystemManager>();
        messageBus = std::make_shared<MessageBus>();
    }

    Entity createEntity() {
        return entityManager->createEntity();
    }

    void destroyEntity(Entity entity) {
        entityManager->destroyEntity(entity);
        componentManager->entityDestroyed(entity);
        systemManager->entityDestroyed(entity);
    }

    template<typename T>
    void registerComponent() {
        componentManager->registerComponent<T>();
    }

    template<typename T>
    void addComponent(Entity entity, T component) {
        componentManager->addComponent(entity, component);

        Signature signature = entityManager->getSignature(entity);
        signature.set(componentManager->getComponentType<T>(), true);

        entityManager->setSignature(entity, signature);

        systemManager->entitySignatureChanged(entity, signature);
    }

    template<typename T>
    T& getComponent(Entity entity) {
        return componentManager->getComponent<T>(entity);
    }

    template<typename T>
    ComponentType getComponentType() {
        return componentManager->getComponentType<T>();
    }

    template<typename T>
    std::shared_ptr<T> registerSystem() {
        return systemManager->registerSystem<T>(*this);
    }

    template<typename T>
    void setSystemSignature(const Signature signature) {
        systemManager->setSignature<T>(signature);
    }

private:
    std::shared_ptr<EntityManager> entityManager;
    std::shared_ptr<ComponentManager> componentManager;
    std::shared_ptr<SystemManager> systemManager;
    std::shared_ptr<MessageBus> messageBus;
};

#endif // XE_COORDINATOR_H
