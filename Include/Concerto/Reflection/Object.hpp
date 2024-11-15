//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_OBJECT_HPP
#define CONCERTO_REFLECTION_OBJECT_HPP

#include "Concerto/Reflection/Defines.hpp"

#define CCT_OBJECT(className)   \
        friend class Internal##className##Class; \

namespace cct::refl
{
    class Class;
	class CONCERTO_REFLECTION_API Object
    {
    public:
        virtual ~Object() = default;
        const Class& GetStaticClass() const;
    private:
        Class* _staticClass;
    };
}

#endif //CONCERTO_REFLECTION_OBJECT_HPP