//
// Created by Felipe Apablaza on 27-03-23.
//

#ifndef XE_SYSTEMMANAGER_H
#define XE_SYSTEMMANAGER_H

#include "CommonECS.h"
#include "System.h"
#include <memory>
#include <unordered_map>

class Coordinator;
class SystemManager {
public:
    template <typename T> std::shared_ptr<T> registerSystem(Coordinator &coordinator) {
        const char *name = typeid(T).name();

        auto system = std::make_shared<T>(coordinator);

        systems.insert({name, system});

        return system;
    }

    template <typename T> void setSignature(const Signature signature) {
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

            if ((entitySignature & systemSignature) == systemSignature) {
                system->entities.insert(entity);
            } else {
                system->entities.erase(entity);
            }
        }
    }

private:
    std::unordered_map<const char *, Signature> signatures;
    std::unordered_map<const char *, std::shared_ptr<System>> systems;
};

#endif // XE_SYSTEMMANAGER_H
