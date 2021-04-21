
#pragma once

#include <map>
#include <string>
#include <memory>

class Scene;
class Object;

class Asset {
public:
    virtual ~Asset();
    
    virtual Scene* getOrCreateScene(const std::string &name) = 0;
    
    virtual Object* getOrCreateObject(const std::string &name) = 0;
    
};
