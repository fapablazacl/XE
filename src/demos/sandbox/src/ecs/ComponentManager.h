//
// Created by Felipe Apablaza on 27-03-23.
//

#ifndef XE_COMPONENTMANAGER_H
#define XE_COMPONENTMANAGER_H

#include "ComponentArray.h"
#include <memory>


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


#endif // XE_COMPONENTMANAGER_H
