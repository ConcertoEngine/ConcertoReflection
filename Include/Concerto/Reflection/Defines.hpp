//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_DEFINE_HPP
#define CONCERTO_REFLECTION_DEFINE_HPP

#include <Concerto/Core/Types.hpp>

#ifdef CONCERTO_REFLECTION_BUILD
#define CONCERTO_REFLECTION_API CONCERTO_EXPORT
#else
#define CONCERTO_REFLECTION_API CONCERTO_IMPORT
#endif // CONCERTO_REFLECTION_BUILD

#define CONCERTO_OBJECT(className)              \
public:                                         \
	static const cct::refl::Class* GetClass();  \

#endif //CONCERTO_REFLECTION_DEFINE_HPP