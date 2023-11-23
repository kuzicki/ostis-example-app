#include <iostream>

#include <sc-memory/sc_memory.hpp>

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include "sc-memory/sc_iterator.hpp"
#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_iterator.hpp"

#include "Lab.hpp"

using namespace std;
using namespace utils;

namespace exampleModule
{

void printEl(const std::unique_ptr<ScMemoryContext>& context, ScAddr element) {
  ScType type;
  type = context->GetElementType(element);

  if (type.IsNode() == ScType::Node) {
    std::string data;
    data = context->HelperGetSystemIdtf(element);
    std::cout << data << std::endl;
  }
  else
  {
    ScAddr elem1, elem2;
    elem1 = context->GetEdgeSource(element);
    elem2 = context->GetEdgeTarget(element);
    std::cout << "(";
    printEl(context, elem1);
    std::cout << " -> ";
    printEl(context, elem2);
    std::cout << ")" << std::endl;
    std::cout << "end element" << std::endl;
  }
}

void run_graph(const std::unique_ptr<ScMemoryContext>& context, ScAddr sourceVertex, std::string firstV, std::string secondV) {
  std::cout << "Run" << std::endl;
  ScAddr beginVertex, endVertex;
  std::unique_ptr<ScMemoryContext> graphContext = std::make_unique<ScMemoryContext>(context->HelperGetSystemIdtf(sourceVertex));
  beginVertex = graphContext->HelperResolveSystemIdtf(firstV);
  endVertex = graphContext->HelperResolveSystemIdtf(secondV);
  ScIterator3Ptr nodes_it = context->Iterator3(
 			graphContext,
 			ScType::EdgeAccessConstPosPerm,
 			ScType::Node
 			);
  while (nodes_it->Next()) {
    ScAddr t_node = nodes_it->Get(2);
    printEl(graphContext, t_node);
  }
  
  printEl(graphContext, beginVertex);
  printEl(graphContext, endVertex);
  std::cout << "End" << std::endl;
}

SC_AGENT_IMPLEMENTATION(Lab)
{
  SC_LOG_INFO("Hello World has started!");
  ScAddr actionNode = otherAddr;
  
  cout << "Hello" << endl;
  run_graph(ms_context, actionNode, "1", "2");
  
  utils::AgentUtils::finishAgentWork(ms_context.get(), actionNode, true);
  SC_LOG_INFO("Hello World has finished");
  
  return SC_RESULT_OK;
}

} 
