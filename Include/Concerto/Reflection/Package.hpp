//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_PACKAGE_HPP
#define CONCERTO_REFLECTION_PACKAGE_HPP

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
    class Class;
    class Object;
    class Namespace;

    class CONCERTO_REFLECTION_API Package
    {
    public:
        virtual ~Package() = default;

        virtual std::string_view GetName() const = 0;
        virtual std::size_t GetClassCount() const = 0;

        virtual const Class& GetClass(std::size_t index) const = 0;
        virtual const Class& GetClass(std::string_view name) const = 0;

        virtual bool HasClass(std::string_view name) const = 0;
}

#endif //CONCERTO_REFLECTION_PACKAGE_HPP