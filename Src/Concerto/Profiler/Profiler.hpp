//
// Created by arthur on 24/08/2023.
//
#ifndef CONCERTO_REFLECTION_PROFILER_PROFILER_HPP
#define CONCERTO_REFLECTION_PROFILER_PROFILER_HPP


#ifdef CCT_REFL_PROFILING
#include <tracy/Tracy.hpp>
#include <source_location>
#define CCT_REFL_PROFILER_SCOPE(name) ZoneScopedN(name)
#define CCT_REFL_AUTO_PROFILER_SCOPE ZoneScoped
#define CCT_REFL_FRAME_MARK FrameMark
#else
#define CCT_REFL_PROFILER_SCOPE(name)
#define CCT_REFL_AUTO_PROFILER_SCOPE
#define CCT_REFL_FRAME_MARK
#endif


#endif //CONCERTO_REFLECTION_PROFILER_PROFILER_HPP