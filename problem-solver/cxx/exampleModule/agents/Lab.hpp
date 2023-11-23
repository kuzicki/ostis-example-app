#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "Lab.generated.hpp"

namespace exampleModule
{

class Lab : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::question_lab, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
};

} // namespace exampleModule
