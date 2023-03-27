//
// Created by Felipe Apablaza on 27-03-23.
//

#ifndef XE_COMPONENTARRAY_H
#define XE_COMPONENTARRAY_H

#include "CommonECS.h"
#include <unordered_map>
#include <array>


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

#endif // XE_COMPONENTARRAY_H
