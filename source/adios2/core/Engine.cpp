/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * Engine.cpp
 *
 *  Created on: Dec 19, 2016
 *      Author: William F Godoy godoywf@ornl.gov
 */

#include "Engine.h"
#include "Engine.tcc"

#include <stdexcept>

namespace adios2
{
namespace core
{

Engine::Engine(const std::string engineType, IO &io, const std::string &name,
               const Mode openMode, MPI_Comm mpiComm)
: m_EngineType(engineType), m_IO(io), m_Name(name), m_OpenMode(openMode),
  m_MPIComm(mpiComm), m_DebugMode(io.m_DebugMode)
{
}

Engine::~Engine(){};

Engine::operator bool() const noexcept { return !m_IsClosed; }

IO &Engine::GetIO() noexcept { return m_IO; }

Mode Engine::OpenMode() const noexcept { return m_OpenMode; }

StepStatus Engine::BeginStep()
{
    if (m_OpenMode == Mode::Read)
    {
        return BeginStep(StepMode::NextAvailable, 0.0f);
    }
    else
    {
        return BeginStep(StepMode::Append, 0.0f);
    }
}

StepStatus Engine::BeginStep(StepMode mode, const float timeoutSeconds)
{
    ThrowUp("BeginStep");
    return StepStatus::OtherError;
}

size_t Engine::CurrentStep() const
{
    ThrowUp("CurrentStep");
    return 0;
}

void Engine::EndStep() { ThrowUp("EndStep"); }
void Engine::PerformPuts() { ThrowUp("PerformPuts"); }
void Engine::PerformGets() { ThrowUp("PerformGets"); }

void Engine::Close(const int transportIndex)
{
    DoClose(transportIndex);

    if (transportIndex == -1)
    {
        MPI_Comm_free(&m_MPIComm);
        m_IsClosed = true;
    }
}

void Engine::Flush(const int /*transportIndex*/) { ThrowUp("Flush"); }

void Engine::FixedSchedule() noexcept { m_FixedLocalSchedule = true; };

// PROTECTED
void Engine::Init() {}
void Engine::InitParameters() {}
void Engine::InitTransports() {}

// DoPut*
#define declare_type(T)                                                        \
    void Engine::DoPutSync(Variable<T> &, const T *) { ThrowUp("DoPutSync"); } \
    void Engine::DoPutDeferred(Variable<T> &, const T *)                       \
    {                                                                          \
        ThrowUp("DoPutDeferred");                                              \
    }
ADIOS2_FOREACH_TYPE_1ARG(declare_type)
#undef declare_type

// DoGet*
#define declare_type(T)                                                        \
    void Engine::DoGetSync(Variable<T> &, T *) { ThrowUp("DoGetSync"); }       \
    void Engine::DoGetDeferred(Variable<T> &, T *) { ThrowUp("DoGetDeferred"); }

ADIOS2_FOREACH_TYPE_1ARG(declare_type)
#undef declare_type

// PRIVATE
void Engine::ThrowUp(const std::string function) const
{
    throw std::invalid_argument("ERROR: Engine derived class " + m_EngineType +
                                " doesn't implement function " + function +
                                "\n");
}

void Engine::CheckOpenModes(const std::set<Mode> &modes,
                            const std::string hint) const
{
    if (modes.count(m_OpenMode) == 0)
    {
        throw std::invalid_argument(
            "ERROR: Engine Open Mode not valid for function, " + hint);
    }
}

// PUBLIC TEMPLATE FUNCTIONS EXPANSION WITH SCOPED TYPES
#define declare_template_instantiation(T)                                      \
                                                                               \
    template void Engine::Put<T>(Variable<T> &, const T *, const Mode);        \
    template void Engine::Put<T>(const std::string &, const T *, const Mode);  \
                                                                               \
    template void Engine::Put<T>(Variable<T> &, const T &);                    \
    template void Engine::Put<T>(const std::string &, const T &);              \
                                                                               \
    template void Engine::Get<T>(Variable<T> &, T *, const Mode);              \
    template void Engine::Get<T>(const std::string &, T *, const Mode);        \
                                                                               \
    template void Engine::Get<T>(Variable<T> &, T &, const Mode);              \
    template void Engine::Get<T>(const std::string &, T &, const Mode);        \
                                                                               \
    template void Engine::Get<T>(Variable<T> &, std::vector<T> &, const Mode); \
    template void Engine::Get<T>(const std::string &, std::vector<T> &,        \
                                 const Mode);                                  \
                                                                               \
    template Variable<T> &Engine::FindVariable(                                \
        const std::string &variableName, const std::string hint);

ADIOS2_FOREACH_TYPE_1ARG(declare_template_instantiation)
#undef declare_template_instantiation

} // end namespace core
} // end namespace adios2
