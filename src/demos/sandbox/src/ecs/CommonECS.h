//
// Created by Felipe Apablaza on 27-03-23.
//

#ifndef XE_COMMONECS_H
#define XE_COMMONECS_H

#include <bitset>
#include <cstdint>

using Entity = uint32_t;                    //! The Entity Identifier
const Entity MAX_ENTITIES = Entity{5000};   //! Max amount of entities supported

using ComponentType = std::uint32_t;
const ComponentType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;

#endif // XE_COMMONECS_H
