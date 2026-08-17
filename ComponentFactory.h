#ifndef COMPONENTFACTORY_H
#define COMPONENTFACTORY_H

#include <memory>
#include <string>
#include "Component.h"
#include "Types.h"

class ComponentFactory {
public:
    static std::unique_ptr<Component> createComponent(ComponentType type, const std::string& id, Point2D pos);
};

#endif // COMPONENTFACTORY_H