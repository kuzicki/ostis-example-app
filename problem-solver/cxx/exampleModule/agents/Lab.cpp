#include <iostream>
#include <iterator>
#include <unordered_map>
#include <vector>
#include <string>

#include <sc-memory/sc_memory.hpp>

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include "sc-memory/sc_iterator.hpp"
#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_iterator.hpp"

#include "Lab.hpp"

using namespace std;
using namespace utils;

struct Node
{
    string data;
    Node* next;

    Node(string data) : data(data), next(nullptr) {}
};

struct List
{
    Node* first;

    List() : first(nullptr) {}

    bool isEmpty()
    {
        return first == nullptr;
    }

    void add_back(string data)
    {
        Node* newNode = new Node(data);

        if (isEmpty())
        {
            first = newNode;
            return;
        }

        Node* lastNode = first;

        while (lastNode->next != nullptr)
        {
            lastNode = lastNode->next;
        }

        lastNode->next = newNode;
    }

    void delete_back()
    {
        if (isEmpty())
            return;

        if (first->next == nullptr)
        {
            delete first;
            return;
        }

        Node* secondLast = first;

        while (secondLast->next->next != nullptr)
        {
            secondLast = secondLast->next;
        }

        delete (secondLast->next);
        secondLast->next = nullptr;
    }

    bool findElement(string element)
    {
        Node* node = first;

        while (node)
        {
            if (node->data == element)
                return true;

            node = node->next;
        }

        return false;
    }

    void print()
    {
        if (isEmpty()) return;

        Node* node = first;
        while (node)
        {
            cout << node->data << "->";
            node = node->next;
        }
        cout << endl;
    }

    void deleteMem(Node* node) {
        if (node != nullptr) {
            deleteMem(node->next);
            delete node;
        }
    }

    ~List() {
        cout << "List: here!" << endl;
        if (first != nullptr) {
            deleteMem(first);
        }
        cout << "List: here?" << endl;
    }
};

class Graph
{
private:
    unordered_map<string,vector<string>> pathes;
    unordered_map<string, List> l;
    unordered_map<string, int> distance;
    unordered_map<string, bool> visited;
public:
    vector<string> curPath;

    ~Graph() {
        cout << "Graph: here?" << endl;

    }

    void addEdge(string u, string v)
    {
        if (u == v) {
            l[u].add_back(u);
            return;
        }
        if (!l[u].findElement(v))
            l[u].add_back(v);
        if (!l[v].findElement(u))
            l[v].add_back(u);
    }
    
    void clear(string firstV) {
        curPath.push_back(firstV);
        for (auto& kv : l) {
            distance[kv.first] = -1;
            visited[kv.first] = false;
        }
    }

    void getLongestPath(string node, int currSum = 0)
	{
		string nd = node;
		if (visited[nd]) {
			curPath.pop_back();
			return;
		}
		visited[nd] = true;
		if (l.size() == 0)
			return;
		if (distance[nd] < currSum)
			distance[nd] = currSum, pathes[nd] = curPath;
		Node* iterNode = l[nd].first;

		while (iterNode != nullptr)
		{
			if (visited[iterNode->data]) {
				iterNode = iterNode->next;
				continue;
			}
			curPath.push_back(iterNode->data);
			getLongestPath(iterNode->data, currSum + 1);
			iterNode = iterNode->next;
		}
		if (curPath.size())
			curPath.pop_back();
		visited[nd] = false;
	}

    void viewDistance()
    {
        for (auto kv : distance) {
            cout << kv.first << " " << kv.second << endl;
        }
    }

    void viewDistance(string v, ScAddr pathNode, const unique_ptr<ScMemoryContext>& context)
    {
        cout << "The distance: " << distance[v] << endl;
        if (distance[v] > 0) {
            cout << distance[v] << endl;
            vector<ScAddr> nodeAddresses;
            for (int i = 0; i < pathes[v].size(); i++)
            {
                cout << pathes[v][i] << "->";
                nodeAddresses.push_back(context->HelperResolveSystemIdtf(pathes[v][i], ScType::NodeConst));
                context->CreateEdge(ScType::EdgeAccessConstPosPerm, pathNode, nodeAddresses[i]);
            }
            ScAddr edge;
            for (int i = 0; i < nodeAddresses.size() - 1; i++) {
                edge = context->CreateEdge(ScType::EdgeDCommonConst, nodeAddresses[i], nodeAddresses[i + 1]);
                context->CreateEdge(ScType::EdgeAccessConstPosPerm, pathNode, edge);
            }
            cout << endl;
        }
        else
            cout << "No such path." << endl;
    }
    void print()
    {
        for (auto& kv : l) {
            if (!kv.second.isEmpty()) {
                cout << kv.first << ": ";
                kv.second.print();
            }
        }
    }
};

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

void addGraph(const std::unique_ptr<ScMemoryContext>& context, ScAddr element, Graph* gr) {
  ScType type;
  type = context->GetElementType(element);

  if (type.IsNode() == ScType::Node) {
  }
  else
  {
    ScAddr elem1, elem2;
    elem1 = context->GetEdgeSource(element);
    string data1 = context->HelperGetSystemIdtf(elem1);
    elem2 = context->GetEdgeTarget(element);
    string data2 = context->HelperGetSystemIdtf(elem2);
    gr->addEdge(data1, data2);
  }
}

void run_graph(const std::unique_ptr<ScMemoryContext>& context, ScAddr sourceVertex) {
  Graph* gr = new Graph;
  ScAddr beginVertex, endVertex;
  ScAddr begin, end;
  begin = context->HelperResolveSystemIdtf("beginV", ScType::NodeConstRole);
  end = context->HelperResolveSystemIdtf("endV", ScType::NodeConstRole);
  string node1, node2;
  ScIterator5Ptr beginIt = context->Iterator5(sourceVertex,
 			ScType::EdgeAccessConstPosPerm,
 			ScType::NodeConst,
                        ScType::EdgeAccessConstPosPerm,
                        begin);
  while (beginIt->Next()) {
    beginVertex = beginIt->Get(2);
    node1 = context->HelperGetSystemIdtf(beginVertex);
    printEl(context, beginVertex);
  }
  ScIterator5Ptr endIt = context->Iterator5(sourceVertex,
 			ScType::EdgeAccessConstPosPerm,
 			ScType::NodeConst,
                        ScType::EdgeAccessConstPosPerm,
                        end);
  while (endIt->Next()) {
    endVertex = endIt->Get(2);
    node2 = context->HelperGetSystemIdtf(endVertex);
    printEl(context, endVertex);
  }

  ScIterator3Ptr nodes_it = context->Iterator3(sourceVertex, ScType::EdgeAccessConstPosPerm, ScType::Node);
  while (nodes_it->Next()) {
    ScAddr t_node = nodes_it->Get(2);
    ScIterator5Ptr edges_it = context->Iterator5(t_node, ScType::EdgeUCommonConst, ScType::Node, ScType::EdgeAccessConstPosPerm, sourceVertex);
    while (edges_it->Next()) {
      ScAddr t_edge = edges_it->Get(1);
      string firstNodeName = context->HelperGetSystemIdtf(t_node);
      if (firstNodeName == "end") {
                continue;
      }
      printEl(context, t_edge);
      addGraph(context, t_edge , gr);
    }
  }


  cout << "first" << endl;
  cout << node1 << endl;
  gr->clear(node1);
  gr->getLongestPath(node1);
  cout << "second" << endl;
  cout << node2 << endl;
  gr->print();
  cout << "Here?" << endl;
  ScAddr pathRel = context->HelperResolveSystemIdtf("longest_path", ScType::NodeConstNoRole);
  ScAddr path = context->CreateNode(ScType::NodeConstStruct);
  string graphName = context->HelperGetSystemIdtf(sourceVertex);
  context->HelperSetSystemIdtf(graphName + " path", path);
  ScAddr edgeToPath = context->CreateEdge(ScType::EdgeDCommonConst, sourceVertex, path);
  gr->viewDistance(node2, path, context);
  context->CreateEdge(ScType::EdgeAccessConstPosPerm, pathRel, edgeToPath);
}

SC_AGENT_IMPLEMENTATION(Lab)
{
  SC_LOG_INFO("Hello World has started!");
  ScAddr actionNode = otherAddr;
  
  cout << "Hello" << endl;
  run_graph(ms_context, actionNode);
  cout << "Here out" << endl;
  
  utils::AgentUtils::finishAgentWork(ms_context.get(), actionNode, true);
  SC_LOG_INFO("Hello World has finished");
  
  return SC_RESULT_OK;
}

} 
