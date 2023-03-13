
#pragma once 

#include <iostream>
#include <bitset>
#include <queue>
#include <array>
#include <unordered_map>
#include <memory>
#include <set>

using Entity = uint32_t;                    //! The Entity Identifier
const Entity MAX_ENTITIES = Entity{5000};   //! Max amount of entities supported

using ComponentType = std::uint32_t;
const ComponentType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;

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


class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    
    virtual void entityDestroyed(const Entity entity) = 0;
};


template<typename Component>
class TComponentArray : public IComponentArray {
public:
    virtual ~TComponentArray() {}
    
    void insertData(Entity entity, Component component) {
        assert(entityToIndex.find(entity) == entityToIndex.end() && "Entity already has the component");
        
        const size_t newIndex = size;
        
        entityToIndex[entity] = newIndex;
        indexToEntity[newIndex] = entity;
        components[newIndex] = component;
        
        ++size;
    }
    
    
    void removeData(const Entity entity) {
        assert(entityToIndex.find(entity) != entityToIndex.end() && "Entity doesn't have the component");
        
        const size_t entityIndex = entityToIndex[entity];
        const size_t lastEntityIndex = size - 1;
        
        // put the last entity in the position of the to-be removed entity
        components[entityIndex] = components[lastEntityIndex];
        
        // update maps, to point to moved spot
        const Entity lastEntity = indexToEntity[lastEntityIndex];
        entityToIndex[lastEntity] = entityIndex;
        indexToEntity[entityIndex] = lastEntity;
        
        entityToIndex.erase(entity);
        indexToEntity.erase(lastEntityIndex);
        
        --size;
    }
    
    Component& getData(const Entity entity) {
        return components[entity];
    }
    
    void entityDestroyed(const Entity entity) override {
        removeData(entity);
    }
    
private:
    std::array<Component, MAX_ENTITIES> components;
    std::unordered_map<Entity, size_t> entityToIndex;
    std::unordered_map<size_t, Entity> indexToEntity;
    size_t size = 0;
};


class ComponentManager {
public:
    template<typename T>
    void registerComponent() {
        const char *typeName = typeid(T).name();
        
        componentTypes.insert({typeName, nextComponentType});
        componentArrays.insert({typeName, std::make_shared<TComponentArray<T>>()});
        
        ++nextComponentType;
    }
    
    template<typename T>
    ComponentType getComponentType() const {
        const char *typeName = typeid(T).name();
        const auto pos = componentTypes.find(typeName);
        
        assert(pos != componentTypes.end());
        
        return pos->second;
    }
    
    template<typename T>
    void addComponent(const Entity entity, T component) {
        getComponentArray<T>()->insertData(entity, component);
    }
    
    template<typename T>
    void removeComponent(const Entity entity) {
        getComponentArray<T>()->removeData(entity);
    }
    
    template<typename T>
    T& getComponent(const Entity entity) {
        return getComponentArray<T>()->getData(entity);
    }
    
    void entityDestroyed(Entity entity) {
        for (auto const& pair : componentArrays) {
            auto const& component = pair.second;
            component->entityDestroyed(entity);
        }
    }
  
private:
    template<typename T>
    std::shared_ptr<TComponentArray<T>> getComponentArray() {
        const char* typeName = typeid(T).name();
        return std::static_pointer_cast<TComponentArray<T>>(componentArrays[typeName]);
    }
    
private:
    std::unordered_map<const char *, ComponentType> componentTypes;
    std::unordered_map<const char *, std::shared_ptr<IComponentArray>> componentArrays;
    ComponentType nextComponentType{};
};



class System {
public:
    std::set<Entity> entities;
};


class Coordinator;
class SystemManager {
public:
    template<typename T>
    std::shared_ptr<T> registerSystem(Coordinator &coordinator) {
        const char *name = typeid(T).name();
        
        auto system = std::make_shared<T>(coordinator);
        
        systems.insert({name, system});
        
        return system;
    }
    
    template<typename T>
    void setSignature(const Signature signature) {
        const char *name = typeid(T).name();
        
        signatures.insert({name, signature});
    }
    
    void entityDestroyed(Entity entity) const {
        for (auto const &pair : systems) {
            auto const &system = pair.second;
            
            system->entities.erase(entity);
        }
    }
    
    
    void entitySignatureChanged(Entity entity, Signature entitySignature) {
        for (auto const &pair : systems) {
            auto const &type = pair.first;
            auto const &system = pair.second;
            auto const &systemSignature = signatures[type];
            
            if ( (entitySignature & systemSignature) == systemSignature) {
                system->entities.insert(entity);
            }
            else {
                system->entities.erase(entity);
            }
        }
    }
    
    
private:
    std::unordered_map<const char *, Signature> signatures;
    std::unordered_map<const char*, std::shared_ptr<System>> systems;
};


class Coordinator {
public:
    Coordinator() {
        entityManager = std::make_shared<EntityManager>();
        componentManager = std::make_shared<ComponentManager>();
        systemManager = std::make_shared<SystemManager>();
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
};
