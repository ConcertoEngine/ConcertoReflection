//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_DEFINE_HPP
#define CONCERTO_REFLECTION_DEFINE_HPP

#include <Concerto/Core/Types.hpp>

#ifdef CCT_REFLECTION_BUILD
#define CCT_REFLECTION_API CCT_EXPORT
#else
#define CCT_REFLECTION_API CCT_IMPORT
#endif // CCT_REFLECTION_BUILD

#define CCT_PACKAGE(...) [[clang::annotate("cct::Package", __VA_ARGS__)]]
#define CCT_CLASS(...) [[clang::annotate("cct::Class", __VA_ARGS__)]]
#define CCT_MEMBER(...) [[clang::annotate("cct::Member", __VA_ARGS__)]]
#define CCT_METHOD(...) [[clang::annotate("cct::Method", __VA_ARGS__)]]
#define CCT_ENUM(...) [[clang::annotate("cct::Enum", __VA_ARGS__)]]
#define CCT_ENUM_VALUE(...) [[clang::annotate("cct::EnumValue", __VA_ARGS__)]]


#endif //CONCERTO_REFLECTION_DEFINE_HPP