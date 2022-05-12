/// \file SugiyamaStyleGraphLayout.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <compare>
#include <map>
#include <optional>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "llvm/ADT/BreadthFirstIterator.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/SmallVector.h"

#include "revng/ADT/SmallMap.h"
#include "revng/Yield/Support/SugiyamaStyleGraphLayout.h"

//
// Aliases for important existing types as well as new types introduced for
// better communication between the parts of the layouter.
//

using RankingStrategy = yield::sugiyama::RankingStrategy;
using Configuration = yield::sugiyama::Configuration;

using ExternalGraph = yield::Graph;
using ExternalNode = ExternalGraph::Node;
using ExternalLabel = ExternalNode::Edge;

using Point = yield::Graph::Point;
using Size = yield::Graph::Size;

using Index = size_t;
using Rank = size_t;
using RankDelta = int64_t;

namespace detail {
/// A simple class for keeping a count.
class SimpleIndexCounter {
public:
  Index next() { return Count++; }

private:
  Index Count = 0;
};
} // namespace detail

/// An internal node representation. Contains a pointer to an external node,
/// an index and center/size helper members.
struct InternalNodeBase {
  using Indexer = detail::SimpleIndexCounter;

  explicit InternalNodeBase(ExternalNode *Node, Indexer &IRef) :
    Pointer(Node), Index(IRef.next()), LocalCenter{ 0, 0 }, LocalSize{ 0, 0 } {}

  auto operator<=>(const InternalNodeBase &Another) const {
    return Index <=> Another.Index;
  }

  Point &center() { return Pointer ? Pointer->Center : LocalCenter; }
  Point const &center() const {
    return Pointer ? Pointer->Center : LocalCenter;
  }
  Size &size() { return Pointer ? Pointer->Size : LocalSize; }
  Size const &size() const { return Pointer ? Pointer->Size : LocalSize; }

  bool isVirtual() const { return Pointer == nullptr; }

public:
  ExternalNode *Pointer;
  const Index Index;

private:
  Point LocalCenter; // Used to temporarily store positions of "fake" nodes.
  Size LocalSize; // Used to temporarily store size of "fake" nodes.
};

/// An internal edge label representation. Contains two pointers to external
/// labels: a forward-facing one and a backward-facing one.
struct InternalLabelBase {
public:
  InternalLabelBase(ExternalLabel *Label, bool IsBackwards = false) :
    Pointer(Label), IsBackwards(IsBackwards) {}

public:
  ExternalLabel *Pointer = nullptr;
  bool IsBackwards = false;
};

using InternalNode = MutableEdgeNode<InternalNodeBase, InternalLabelBase>;
using InternalLabel = InternalNode::Edge;
class InternalGraph : public GenericGraph<InternalNode, 16, true> {
  using Base = GenericGraph<InternalNode, 16, true>;

public:
  template<class... Args>
  InternalNode *addNode(Args &&...A) {
    return Base::addNode(A..., Indexer);
  }

private:
  typename InternalNode::Indexer Indexer;
};

/// A wrapper around an `InternalNode` pointer used for comparison overloading.
class NodeView {
public:
  NodeView(InternalNode *Pointer = nullptr) : Pointer(Pointer) {}

  operator InternalNode *() const {
    revng_assert(Pointer != nullptr);
    return Pointer;
  }
  InternalNode &operator*() const {
    revng_assert(Pointer != nullptr);
    return *Pointer;
  }
  InternalNode *operator->() const {
    revng_assert(Pointer != nullptr);
    return Pointer;
  }

  auto operator<=>(const NodeView &Another) const {
    revng_assert(Pointer != nullptr);
    return Pointer->Index <=> Another.Pointer->Index;
  }

private:
  InternalNode *Pointer;
};

namespace std {
template<>
struct hash<::NodeView> {
  auto operator()(const ::NodeView &View) const {
    return std::hash<InternalNode *>()(&*View);
  }
};
} // namespace std

namespace detail {

/// A generic view onto an edge. It stores views onto the `From` and `To`
/// nodes as well as free information about the edge label.
template<typename LabelType>
struct GenericEdgeView {
protected:
  static constexpr bool OwnsLabel = !std::is_pointer_v<std::decay_t<LabelType>>;
  using ParamType = std::conditional_t<OwnsLabel,
                                       std::decay_t<LabelType> &&,
                                       std::decay_t<LabelType>>;

public:
  NodeView From, To;
  LabelType Label;

  GenericEdgeView(NodeView From, NodeView To, ParamType Label) :
    From(From), To(To), Label(std::move(Label)) {}
  auto operator<=>(const GenericEdgeView &) const = default;

  std::remove_pointer_t<std::decay_t<LabelType>> &label() {
    if constexpr (OwnsLabel)
      return Label;
    else
      return *Label;
  }
  const std::remove_pointer_t<std::decay_t<LabelType>> &label() const {
    if constexpr (OwnsLabel)
      return Label;
    else
      return *Label;
  }
};

} // namespace detail

/// A view onto an edge. It stores `From` and `To` node views as well as a
/// label pointer.
using EdgeView = detail::GenericEdgeView<InternalLabel *>;

/// Returns a list of backwards edges of the graph and the order
/// they were visited in.
/// \note: this function could use a rework.
static std::pair<std::set<EdgeView>, std::vector<NodeView>>
pickBackwardsEdges(InternalGraph &Graph) {
  std::set<EdgeView> Result;

  std::unordered_set<NodeView> Visited, OnStack;
  std::vector<NodeView> VisitOrder;

  for (auto *Node : Graph.nodes()) {
    if (!Visited.contains(Node)) {
      std::vector<std::pair<NodeView, Rank>> Stack{ { Node, 0 } };
      while (!Stack.empty()) {
        auto [Current, Rank] = Stack.back();
        Stack.pop_back();

        if (Rank == 0) {
          VisitOrder.push_back(Current);
          Visited.emplace(Current);
          OnStack.emplace(Current);
        }

        bool Finished = true;
        for (auto It = std::next(Current->successor_edges().begin(), Rank);
             It != Current->successor_edges().end();
             ++It) {
          auto [Neighbor, Label] = *It;
          if (!Visited.contains(Neighbor)) {
            Stack.emplace_back(Current, Rank + 1);
            Stack.emplace_back(Neighbor, 0);
            Visited.emplace(Neighbor);
            Finished = false;
            break;
          } else if (OnStack.contains(Neighbor))
            Result.emplace(Current, Neighbor, Label);
        }

        if (Finished) {
          revng_assert(OnStack.contains(Current));
          OnStack.erase(Current);
        }
      }
    }
  }

  revng_assert(VisitOrder.size() == Graph.size());
  return { Result, VisitOrder };
}

/// Reverses the edge (its `From` node becomes its `To` node and vice versa).
static void invertEdge(EdgeView Edge) {
  revng_assert(Edge.Label != nullptr);

  Edge.Label->IsBackwards = !Edge.Label->IsBackwards;
  Edge.To->addSuccessor(Edge.From, std::move(*Edge.Label));

  auto Comparator = [&Edge](auto SuccessorEdge) {
    return SuccessorEdge.Neighbor == Edge.To
           && SuccessorEdge.Label == Edge.Label;
  };
  auto Iterator = llvm::find_if(Edge.From->successor_edges(), Comparator);
  revng_assert(Iterator != Edge.From->successor_edges().end(),
               "Unable to reverse an edge that doesn't exist");
  Edge.From->removeSuccessor(Iterator);
}

// A simple container that's used to indicate a self-loop.
struct SelfLoop {
  InternalNode *Node;
  ExternalLabel *Label;

  SelfLoop(InternalNode *Node, ExternalLabel *Label) :
    Node(Node), Label(Label) {}
};
using SelfLoopContainer = std::vector<SelfLoop>;

// Removes self-loops from the graph and returns their labels.
static SelfLoopContainer removeSelfLoops(InternalGraph &Graph) {
  SelfLoopContainer Result;

  for (auto *Node : Graph.nodes()) {
    for (auto Iterator = Node->successor_edges().begin();
         Iterator != Node->successor_edges().end();) {
      if (Iterator->Neighbor->Index == Node->Index) {
        Result.emplace_back(Node, Iterator->Label->Pointer);
        Iterator = Node->removeSuccessor(Iterator);
      } else {
        ++Iterator;
      }
    }
  }

  return Result;
}

/// Ensures an "internal" graph to be a DAG by "flipping" edges to prevent
/// loops.
static void convertToDAG(InternalGraph &Graph) {
  if (auto Initial = pickBackwardsEdges(Graph).first; !Initial.empty()) {
    // Iteratively reverse all the initial backedges.
    for (auto &&Edge : Initial)
      invertEdge(std::move(Edge));

    // There is a rare corner case where flipping an edge causes a change in
    // the DFS order that results in new backedges. In that case we enforce
    // acyclic orientation. But, it may be suboptimal: some of the forward
    // edges are reversed as well.
    if (auto [Edges, Order] = pickBackwardsEdges(Graph); !Edges.empty()) {
      std::unordered_map<NodeView, Index> Lookup;
      for (Index I = 0; I < Order.size(); ++I)
        Lookup.emplace(Order.at(I), I);

      std::vector<EdgeView> ToReverse;
      for (auto *From : Graph.nodes())
        for (auto [To, Label] : From->successor_edges())
          if (From->hasPredecessors() && Lookup.at(From) > Lookup.at(To))
            ToReverse.emplace_back(From, To, Label);

      for (auto &&Edge : ToReverse)
        invertEdge(std::move(Edge));

      if (!pickBackwardsEdges(Graph).first.empty()) {
        for (auto *From : Graph.nodes())
          for (auto Iterator = From->successors().begin();
               Iterator != From->successors().end();)
            if (From->Index == (*Iterator)->Index)
              Iterator = From->removeSuccessor(Iterator);
            else
              ++Iterator;
      }

      revng_assert(pickBackwardsEdges(Graph).first.empty());
    }
  }
}

/// An internal data structure used to pass node ranks around.
using RankContainer = std::unordered_map<NodeView, Rank>;

/// Ranks the nodes using specified ranking strategy.
template<RankingStrategy Strategy>
RankContainer rankNodes(InternalGraph &Graph);

/// Ranks the nodes using specified ranking strategy.
template<RankingStrategy Strategy>
RankContainer rankNodes(InternalGraph &Graph, int64_t DiamondBound);

/// `RankingStrategy::BreadthFirstSearch` template specialization.
/// Assigns ranks based on the BFS visit order.
/// This is the general ranking induced by the graph.
constexpr auto BFS = RankingStrategy::BreadthFirstSearch;
template<>
RankContainer rankNodes<BFS>(InternalGraph &Graph) {
  RankContainer Ranks;

  for (auto *Node : llvm::breadth_first(Graph.getEntryNode())) {
    auto CurrentRank = Ranks.try_emplace(Node, Rank(-1)).first->second;
    for (auto *Successor : Node->successors())
      if (auto SuccessorIt = Ranks.find(Successor); SuccessorIt == Ranks.end())
        Ranks.emplace(Successor, CurrentRank + 1);
      else if (SuccessorIt->second < CurrentRank + 1)
        SuccessorIt->second = CurrentRank + 1;
  }

  revng_assert(Ranks.size() == Graph.size());
  return Ranks;
}

/// `RankingStrategy::DepthFirstSearch` template specialization.
/// Assigns ranks based on the DFS time.
constexpr auto DFS = RankingStrategy::DepthFirstSearch;
template<>
RankContainer rankNodes<DFS>(InternalGraph &Graph) {
  RankContainer Ranks;

  auto Counter = Graph.getEntryNode()->isVirtual() ? Rank(-1) : Rank(0);
  for (auto *Node : llvm::depth_first(Graph.getEntryNode()))
    Ranks.try_emplace(Node, Counter++);

  revng_assert(Ranks.size() == Graph.size());
  return Ranks;
}

/// `RankingStrategy::Topological` template specialization.
/// Assigns ranks based on the DFS topological order.
///
/// This ranking can be used to get a layout where each node is on
/// its own layer. The idea is that it produces a layout with a single
/// real node per layer while going along a path before vising the brances,
/// in opposition to the conventional BFS-based topological order, where
/// all the brances are visited at the same time
///
/// The goal of this ranking is to produce something similar to what ghidra
/// does without using a decompiler.
constexpr auto TRS = RankingStrategy::Topological;
template<>
RankContainer rankNodes<TRS>(InternalGraph &Graph) {
  RankContainer Ranks;

  auto Counter = Graph.getEntryNode()->isVirtual() ? Rank(-1) : Rank(0);
  for (auto *Node : llvm::ReversePostOrderTraversal(Graph.getEntryNode()))
    Ranks.try_emplace(Node, Counter++);

  revng_assert(Ranks.size() == Graph.size());
  return Ranks;
}

/// Checks whether two nodes are disjoint provided pointers to the nodes
/// and the lists of successors for each and every node.
///
/// It also detects small diamonds with depth around 3.
///
/// The expected number of branches is around 2 so the total number of visited
/// nodes should be less than 16 on average
static bool areDisjoint(NodeView A, NodeView B, size_t MaxDepth = -1) {
  std::deque<NodeView> Queue;

  std::vector<NodeView> Worklist;
  Worklist.push_back(A);
  Worklist.push_back(B);

  // Perform a joint BFS and check that there are no shared nodes
  SmallMap<NodeView, size_t, 16> VisitedPrevious;
  for (const auto &Current : Worklist) {
    SmallMap<NodeView, size_t, 16> VisitedNow;
    VisitedNow[Current] = 0;
    if (VisitedPrevious.find(Current) != VisitedPrevious.end())
      return false;
    Queue.emplace_back(Current);

    while (!Queue.empty()) {
      auto TopNode = Queue.front();
      Queue.pop_front();

      auto VisitedTop = VisitedNow[Current];
      if (VisitedTop > MaxDepth)
        continue;

      for (auto *Next : TopNode->successors()) {
        if (VisitedNow.find(Next) == VisitedNow.end()) {
          Queue.emplace_back(Next);
          VisitedNow[Next] = VisitedTop + 1;
        }
      }
    }
    std::swap(VisitedPrevious, VisitedNow);
  }

  return true;
}

/// `RankingStrategy::DisjointDepthFirstSearch` template specialization.
/// Assigns ranks based on the DFS time but allows multiple nodes at the same
/// rank when two paths diverge if they either rejoin shortly or never
/// rejoin again.
constexpr auto DDFS = RankingStrategy::DisjointDepthFirstSearch;
template<>
RankContainer rankNodes<DDFS>(InternalGraph &Graph, int64_t DiamondBound) {
  RankContainer Ranks;

  std::vector<NodeView> Stack;
  for (auto *Node : Graph.nodes()) {
    if (!Node->hasPredecessors() && !Ranks.contains(Node)) {
      Stack.emplace_back(Node);

      size_t DFSTime = 0;
      while (!Stack.empty()) {
        auto Current = Stack.back();
        Stack.pop_back();

        auto &CurrentRank = Ranks[Current];
        if (CurrentRank == 0)
          CurrentRank = DFSTime++;

        bool IsDisjoint = true;
        bool IsDiamond = true;
        if (Current->successorCount() > 1) {
          auto *First = *Current->successors().begin();
          for (auto NextIt = std::next(Current->successors().begin());
               NextIt != Current->successors().end();
               ++NextIt) {
            auto &DB = DiamondBound;
            IsDisjoint = IsDisjoint && areDisjoint(First, *NextIt);
            IsDiamond = IsDiamond && !areDisjoint(First, *NextIt, DB);
          }
        } else {
          IsDisjoint = IsDiamond = false;
        }

        for (auto *Next : Current->successors()) {
          if (!Ranks.contains(Next)) {
            Stack.push_back(Next);
            if (IsDisjoint || IsDiamond)
              Ranks[Next] = CurrentRank + 1;
            else
              Ranks[Next] = 0;
          }
        }
      }
    }
  }

  revng_assert(Ranks.size() == Graph.size());
  return Ranks;
}

template<>
RankContainer rankNodes<DDFS>(InternalGraph &Graph) {
  return rankNodes<DDFS>(Graph, 4u);
}

// Calculates the absolute difference in rank between two nodes.
// In other words, the result represents the number of layers the edge between
// the specified two nodes needs to go through.
static RankDelta delta(NodeView LHS, NodeView RHS, const RankContainer &Ranks) {
  return std::abs(RankDelta(Ranks.at(LHS)) - RankDelta(Ranks.at(RHS)));
}

// Returns a list of edges that span accross more than a single layer.
static std::vector<EdgeView>
pickLongEdges(InternalGraph &Graph, const RankContainer &Ranks) {
  std::vector<EdgeView> Result;

  for (auto *From : Graph.nodes())
    for (auto [To, Label] : From->successor_edges())
      if (delta(From, To, Ranks) > RankDelta(1))
        Result.emplace_back(From, To, Label);

  return Result;
}

namespace detail {
// Contains helper classes for `NodeClassifier`.

class PartialClassifierStorage {
protected:
  std::unordered_set<NodeView> BackwardsEdgeNodes;
};

class FullNodeClassifierStorage : public PartialClassifierStorage {
protected:
  std::unordered_set<NodeView> LongEdgeNodes;
};

namespace detail {
template<RankingStrategy RS>
constexpr bool Condition = RS == RankingStrategy::DepthFirstSearch
                           || RS == RankingStrategy::Topological;
}

template<RankingStrategy Strategy>
using NodeClassifierStorage = std::conditional_t<detail::Condition<Strategy>,
                                                 FullNodeClassifierStorage,
                                                 PartialClassifierStorage>;

} // namespace detail

/// It is used to classify the nodes by selecting the right cluster depending
/// on its neighbors. This helps to make routes with edges routed accross
/// multiple "virtual" nodes that require less bends.
template<RankingStrategy RS>
class NodeClassifier : public detail::NodeClassifierStorage<RS> {
  using Storage = detail::NodeClassifierStorage<RS>;

  enum Cluster { Left = 0, Middle = 1, Right = 2 };

public:
  void addBackwardsEdgePartition(NodeView LHS, NodeView RHS) {
    Storage::BackwardsEdgeNodes.emplace(LHS);
    Storage::BackwardsEdgeNodes.emplace(RHS);
  }

  void addLongEdgePartition(NodeView LHS, NodeView RHS) {
    if constexpr (RS == RankingStrategy::DepthFirstSearch
                  || RS == RankingStrategy::Topological) {
      Storage::LongEdgeNodes.emplace(LHS);
      Storage::LongEdgeNodes.emplace(RHS);
    }
  }

  size_t operator()(NodeView Node) const {
    if constexpr (RS == RankingStrategy::DepthFirstSearch
                  || RS == RankingStrategy::Topological)
      if (!Storage::LongEdgeNodes.contains(Node))
        return !Storage::BackwardsEdgeNodes.contains(Node) ? Middle : Right;
      else
        return Left;
    else
      return !Storage::BackwardsEdgeNodes.contains(Node) ? Left : Right;
  }
};

template<typename EdgeType, RankingStrategy Strategy>
void partition(const std::vector<EdgeType> &Edges,
               InternalGraph &Graph,
               const RankContainer &Ranks,
               NodeClassifier<Strategy> &Classifier) {
  for (auto &Edge : Edges) {
    size_t PartitionCount = delta(Edge.From, Edge.To, Ranks);

    auto Current = Edge.From;
    if (PartitionCount != 0) {
      for (size_t Partition = 0; Partition < PartitionCount - 1; ++Partition) {
        auto NewNode = Graph.addNode(nullptr);

        const InternalLabel &LabelCopy = Edge.label();
        Current->addSuccessor(NewNode, LabelCopy);
        Classifier.addLongEdgePartition(Current, NewNode);

        Current = NewNode;
      }
    }

    const InternalLabel &LabelCopy = Edge.label();
    Current->addSuccessor(Edge.To, LabelCopy);
    Classifier.addLongEdgePartition(Current, Edge.To);
  }
}

/// Updates node ranking after the graph was modified.
///
/// Garanties ranking consistency i.e. that each node has
/// a rank greater than its predecessors.
///
/// Be careful, rank order is NOT preserved.
/// \note: There used to be an additional parameter for that, but it was never
/// actually implemented, so I removed it.
static RankContainer &updateRanks(InternalGraph &Graph, RankContainer &Ranks) {
  Ranks.try_emplace(Graph.getEntryNode(),
                    Graph.getEntryNode()->isVirtual() ? Rank(-1) : Rank(0));

  for (auto *Current : llvm::ReversePostOrderTraversal(Graph.getEntryNode())) {
    auto &CurrentRank = Ranks[Current];
    for (auto *Predecessor : Current->predecessors())
      if (auto Rank = Ranks[Predecessor]; Rank + 1 > CurrentRank)
        CurrentRank = Rank + 1;
  }

  revng_assert(Ranks.size() == Graph.size());
  return Ranks;
}

/// Breaks long edges into partitions by introducing new internal nodes.
template<RankingStrategy Strategy>
RankContainer
partitionLongEdges(InternalGraph &Graph, NodeClassifier<Strategy> &Classifier) {
  // To simplify the ranking algorithms, if there's more than one entry point,
  // an artifitial entry node is added. This new node has a single edge per
  // real entry point.
  llvm::SmallVector<NodeView, 4> EntryNodes;
  for (auto *Node : Graph.nodes())
    if (!Node->hasPredecessors())
      EntryNodes.emplace_back(Node);
  revng_assert(!EntryNodes.empty());

  revng_assert(Graph.getEntryNode() == nullptr);
  if (EntryNodes.size() > 1) {
    auto EntryPoint = Graph.addNode(nullptr);
    for (auto &Node : EntryNodes)
      EntryPoint->addSuccessor(Node, nullptr);
    Graph.setEntryNode(EntryPoint);
  } else {
    Graph.setEntryNode(EntryNodes.front());
  }

  // Because a long edge can also be a backwards edge, edges that are certainly
  // long need to be removed first, so that DFS-based rankind algorithms don't
  // mistakenly take any undesired shortcuts. Simple BFS ranking used
  // internally to differencite such "certainly long" edges.

  // Rank nodes based on a BreadthFirstSearch pass-through.
  auto Ranks = rankNodes<RankingStrategy::BreadthFirstSearch>(Graph);

  /// A copy of an edge label.
  using EdgeCopy = detail::GenericEdgeView<InternalLabel>;

  // Temporary save them outside of the graph.
  std::vector<EdgeCopy> SavedLongEdges;
  for (auto *From : Graph.nodes()) {
    for (auto Iterator = From->successor_edges_rbegin();
         Iterator != From->successor_edges_rend();) {
      if (auto [To, Label] = *Iterator; delta(From, To, Ranks) > RankDelta(1)) {
        revng_assert(Label != nullptr);
        SavedLongEdges.emplace_back(From, To, std::move(*Label));
        Iterator = From->removeSuccessor(Iterator);
      } else {
        ++Iterator;
      }
    }
  }

  // Calculate real ranks for the remainder of the graph.
  Ranks = rankNodes<Strategy>(Graph);

  // Pick new long edges based on the real ranks.
  auto NewLongEdges = pickLongEdges(Graph, Ranks);

  // Add partitions based on removed earlier edges.
  partition(SavedLongEdges, Graph, Ranks, Classifier);

  // Add partitions based on the new long edges.
  partition(NewLongEdges, Graph, Ranks, Classifier);
  for (auto &Edge : NewLongEdges)
    Edge.From->removeSuccessors(Edge.To);

  // Now, make the DFS ranking consistent by checking that the rank of a node
  // is greater than the rank of its predecessors.
  //
  // Eventually, this ranking score becomes a proper hierarchy.
  updateRanks(Graph, Ranks);

  // Make sure that new long edges are properly broken up.
  NewLongEdges = pickLongEdges(Graph, Ranks);
  partition(NewLongEdges, Graph, Ranks, Classifier);
  for (auto &Edge : NewLongEdges)
    Edge.From->removeSuccessors(Edge.To);

  updateRanks(Graph, Ranks);

  // Remove an artificial entry node if it was added.
  if (Graph.hasEntryNode && Graph.getEntryNode() != nullptr) {
    if (Graph.getEntryNode()->isVirtual()) {
      Ranks.erase(Graph.getEntryNode());
      Graph.removeNode(Graph.getEntryNode());
    }
  }

  revng_assert(pickLongEdges(Graph, Ranks).empty());
  return Ranks;
}

/// A view onto one of the edge labels. It stores `From` and `To` node views
/// as well as a pointer to an external label.
using DirectionlessEdgeView = detail::GenericEdgeView<ExternalLabel *>;

template<RankingStrategy Strategy>
void partitionArtificialBackwardsEdges(InternalGraph &Graph,
                                       RankContainer &Ranks,
                                       NodeClassifier<Strategy> &Classifier) {
  for (size_t NodeIndex = 0; NodeIndex < Graph.size(); ++NodeIndex) {
    auto *From = *std::next(Graph.nodes().begin(), NodeIndex);
    for (auto EdgeIterator = From->successor_edges_rbegin();
         EdgeIterator != From->successor_edges_rend();) {
      auto [To, Label] = *EdgeIterator;
      if (From->isVirtual() != To->isVirtual() && Label->IsBackwards == true) {
        auto *LabelPointer = Label->Pointer;
        EdgeIterator = From->removeSuccessor(EdgeIterator);

        auto *NewNode1 = Graph.addNode(nullptr);
        auto *NewNode2 = Graph.addNode(nullptr);

        if (From->isVirtual() && !To->isVirtual()) {
          // Fix the low point of a backwards edge
          From->addSuccessor(NewNode2, InternalLabel(LabelPointer, true));
          NewNode2->addSuccessor(NewNode1, InternalLabel(LabelPointer, true));
          To->addSuccessor(NewNode1, InternalLabel(LabelPointer, false));

          Classifier.addBackwardsEdgePartition(From, NewNode2);
          Classifier.addBackwardsEdgePartition(NewNode2, NewNode1);
          Classifier.addBackwardsEdgePartition(To, NewNode1);

          Ranks[NewNode1] = Ranks.at(To) + 1;
          Ranks[NewNode2] = Ranks.at(To);
        } else {
          // Fix the high point of a backwards edge
          NewNode1->addSuccessor(From, InternalLabel(LabelPointer, false));
          NewNode1->addSuccessor(NewNode2, InternalLabel(LabelPointer, true));
          NewNode2->addSuccessor(To, InternalLabel(LabelPointer, true));

          Classifier.addBackwardsEdgePartition(NewNode1, From);
          Classifier.addBackwardsEdgePartition(NewNode1, NewNode2);
          Classifier.addBackwardsEdgePartition(NewNode2, To);

          Ranks[NewNode1] = Ranks.at(From) - 1;
          Ranks[NewNode2] = Ranks.at(From);
        }
      } else {
        ++EdgeIterator;
      }
    }
  }
}

template<RankingStrategy Strategy>
void partitionOriginalBackwardsEdges(InternalGraph &Graph,
                                     RankContainer &Ranks,
                                     NodeClassifier<Strategy> &Classifier) {
  for (size_t NodeIndex = 0; NodeIndex < Graph.size(); ++NodeIndex) {
    auto *From = *std::next(Graph.nodes().begin(), NodeIndex);
    for (auto EdgeIterator = From->successor_edges_rbegin();
         EdgeIterator != From->successor_edges_rend();) {
      auto [To, Label] = *EdgeIterator;
      if (!From->isVirtual() && !To->isVirtual()
          && Label->IsBackwards == true) {
        auto *LabelPointer = Label->Pointer;
        EdgeIterator = From->removeSuccessor(EdgeIterator);

        auto *NewNode1 = Graph.addNode(nullptr);
        auto *NewNode2 = Graph.addNode(nullptr);
        auto *NewNode3 = Graph.addNode(nullptr);
        auto *NewNode4 = Graph.addNode(nullptr);

        NewNode1->addSuccessor(From, InternalLabel(LabelPointer, false));
        NewNode1->addSuccessor(NewNode2, InternalLabel(LabelPointer, true));
        NewNode2->addSuccessor(NewNode3, InternalLabel(LabelPointer, true));
        NewNode3->addSuccessor(NewNode4, InternalLabel(LabelPointer, true));
        To->addSuccessor(NewNode4, InternalLabel(LabelPointer, false));

        Classifier.addBackwardsEdgePartition(NewNode1, From);
        Classifier.addBackwardsEdgePartition(NewNode1, NewNode2);
        Classifier.addBackwardsEdgePartition(NewNode2, NewNode3);
        Classifier.addBackwardsEdgePartition(NewNode3, NewNode4);
        Classifier.addBackwardsEdgePartition(To, NewNode4);

        Ranks[NewNode1] = Ranks.at(From) - 1;
        Ranks[NewNode2] = Ranks.at(From);
        Ranks[NewNode3] = Ranks.at(To);
        Ranks[NewNode4] = Ranks.at(To) + 1;
      } else {
        ++EdgeIterator;
      }
    }
  }
}

template<RankingStrategy Strategy>
void partitionSelfLoops(InternalGraph &Graph,
                        RankContainer &Ranks,
                        SelfLoopContainer &SelfLoops,
                        NodeClassifier<Strategy> &Classifier) {
  for (auto &Edge : SelfLoops) {
    auto *NewNode1 = Graph.addNode(nullptr);
    auto *NewNode2 = Graph.addNode(nullptr);
    auto *NewNode3 = Graph.addNode(nullptr);

    NewNode1->addSuccessor(Edge.Node, InternalLabel(Edge.Label, false));
    NewNode1->addSuccessor(NewNode2, InternalLabel(Edge.Label, true));
    NewNode2->addSuccessor(NewNode3, InternalLabel(Edge.Label, true));
    Edge.Node->addSuccessor(NewNode3, InternalLabel(Edge.Label, false));

    Classifier.addBackwardsEdgePartition(NewNode1, Edge.Node);
    Classifier.addBackwardsEdgePartition(NewNode1, NewNode2);
    Classifier.addBackwardsEdgePartition(NewNode2, NewNode3);
    Classifier.addBackwardsEdgePartition(Edge.Node, NewNode3);

    Ranks[NewNode1] = Ranks.at(Edge.Node) - 1;
    Ranks[NewNode2] = Ranks.at(Edge.Node);
    Ranks[NewNode3] = Ranks.at(Edge.Node) + 1;
  }
}

//
// Parts of the layouter presented in order of their usage.
//

template<RankingStrategy Strategy>
std::tuple<InternalGraph, RankContainer, const NodeClassifier<Strategy>>
readyTheGraph(ExternalGraph &Graph) {
  InternalGraph Result; // Internal representation of the graph.

  // Add all the "external" nodes to the "internal" graph.
  std::unordered_map<ExternalNode *, InternalNode *> LookupTable;
  for (auto *Node : Graph.nodes())
    LookupTable.emplace(Node, Result.addNode(Node));

  // Add all the "external" edges to the "internal" graph.
  for (auto *From : Graph.nodes())
    for (auto [To, Label] : From->successor_edges())
      LookupTable.at(From)->addSuccessor(LookupTable.at(To),
                                         InternalLabel{ Label });

  NodeClassifier<Strategy> Classifier; // Is used for permutation selection.

  // Temporarily remove self-loops from the graph
  auto SelfLoops = removeSelfLoops(Result);

  convertToDAG(Result);

  // Split long edges into 1 rank wide partitions.
  auto Ranks = partitionLongEdges(Result, Classifier);

  // Split backwards facing edges created when partitioning the long edges up.
  partitionArtificialBackwardsEdges(Result, Ranks, Classifier);

  // Split the backwards facing edges from the "external" graph into partitions.
  partitionOriginalBackwardsEdges(Result, Ranks, Classifier);

  // Restore (and partition) self-loops.
  partitionSelfLoops(Result, Ranks, SelfLoops, Classifier);

  return { std::move(Result), std::move(Ranks), std::move(Classifier) };
}

/// An internal data structure used to pass around information about
/// the layers specific nodes belong to.
using LayerContainer = std::vector<std::vector<NodeView>>;

/// Converts given rankings to a layer container and updates ranks to remove
/// the layers that are not required for correct routing.
static LayerContainer
optimizeLayers(InternalGraph &Graph, RankContainer &Ranks) {
  LayerContainer Layers;
  for (auto &[Node, Rank] : Ranks) {
    if (Rank >= Layers.size())
      Layers.resize(Rank + 1);
    Layers[Rank].emplace_back(Node);
  }

  for (auto Iterator = Layers.begin(); Iterator != Layers.end();) {
    bool IsLayerRequired = false;
    for (auto Node : *Iterator) {
      // In order for a node to be easily removable, it shouldn't have
      // an external counterpart (meaning it was added when some of the edges
      // were partitioned) and shouldn't be necessary for backwards edge
      // routing (not a part of a V-shaped structure).
      //
      // Such nodes always have a single predecessor and a single successor.
      // Additionally, the ranks of those two neighbors have to be different.
      if (!Node->isVirtual()) {
        IsLayerRequired = true;
        break;
      }
      if ((Node->successorCount() != 1) || (Node->predecessorCount() != 1)) {
        IsLayerRequired = true;
        break;
      }

      auto SuccessorRank = Ranks.at(*Node->successors().begin());
      auto PredecessorRank = Ranks.at(*Node->predecessors().begin());
      if (SuccessorRank == PredecessorRank) {
        IsLayerRequired = true;
        break;
      }
    }

    if (!IsLayerRequired) {
      for (auto Node : *Iterator) {
        auto *Predecessor = *Node->predecessors().begin();
        auto *Successor = *Node->successors().begin();
        auto Label = std::move(*Node->predecessor_edges().begin()->Label);
        Predecessor->addSuccessor(Successor, std::move(Label));
        Ranks.erase(Node);
        Graph.removeNode(Node);
      }

      Iterator = Layers.erase(Iterator);
    } else {
      ++Iterator;
    }
  }

  // Update ranks
  for (size_t Index = 0; Index < Layers.size(); ++Index)
    for (auto Node : Layers[Index])
      Ranks.at(Node) = Index;

  revng_assert(Ranks.size() == Graph.size());
  return Layers;
}

/// Counts the total number of nodes within a `Layers` container.
static size_t countNodes(const LayerContainer &Layers) {
  size_t Counter = 0;
  for (auto &Layer : Layers)
    Counter += Layer.size();
  return Counter;
}

struct CrossingCalculator {
public:
  const LayerContainer &Layers; // A view onto the layered nodes.
  const RankContainer &Ranks; // A view onto node rankings.
  RankContainer &Permutation; // A view onto a current permutation.

private:
  /// Counts the number of edge crossings given two nodes and
  /// a map that represents which nodes in an adjacent layer
  /// are connected to one of the given nodes.
  static Rank countImpl(NodeView KNode,
                        NodeView LNode,
                        const std::map<Rank, bool> &SortedLayer,
                        const RankContainer &Permutation) {
    Rank CrossingCount = 0;
    if (SortedLayer.size() > 0) {
      bool KSide = SortedLayer.begin()->second;
      bool KLeft = Permutation.at(KNode) < Permutation.at(LNode);
      int64_t PreviousSegmentSize = 0;
      int64_t CurrentSegmentSize = 0;

      for (auto &[Position, Side] : SortedLayer) {
        if (Side == KSide) {
          CurrentSegmentSize += 1;
        } else {
          if (KSide && KLeft)
            CrossingCount += PreviousSegmentSize * CurrentSegmentSize;

          PreviousSegmentSize = CurrentSegmentSize;
          CurrentSegmentSize = 1;
          KSide = Side;
        }
      }

      if (KSide && KLeft)
        CrossingCount += PreviousSegmentSize * CurrentSegmentSize;
    }

    return CrossingCount;
  }

public:
  // Counts the crossings.
  Rank countCrossings(Rank CurrentRank, NodeView KNode, NodeView LNode) {
    revng_assert(CurrentRank < Layers.size());

    Rank CrossingCount = 0;

    if (CurrentRank != 0) {
      std::map<Rank, bool> SortedLayer;
      for (auto *Predecessor : KNode->predecessors())
        if (Ranks.at(Predecessor) == CurrentRank - 1)
          SortedLayer[Permutation.at(Predecessor)] = true;

      for (auto *Predecessor : LNode->predecessors())
        if (Ranks.at(Predecessor) == CurrentRank - 1)
          SortedLayer[Permutation.at(Predecessor)] = false;

      CrossingCount += countImpl(KNode, LNode, SortedLayer, Permutation);
    }

    if (CurrentRank != Layers.size() - 1) {
      std::map<Rank, bool> SortedLayer;
      for (auto *Successor : KNode->successors())
        if (Ranks.at(Successor) == CurrentRank + 1)
          SortedLayer[Permutation.at(Successor)] = true;

      for (auto *Successor : LNode->successors())
        if (Ranks.at(Successor) == CurrentRank + 1)
          SortedLayer[Permutation.at(Successor)] = false;

      CrossingCount += countImpl(KNode, LNode, SortedLayer, Permutation);
    }

    return CrossingCount;
  }

  /// Computes the difference in the crossing count
  /// based on the node positions (e.g. how much better/worse the crossing
  /// count becomes if a permutation were to be applied).
  RankDelta computeDelta(Rank CurrentRank, NodeView KNode, NodeView LNode) {
    auto KIterator = Permutation.find(KNode);
    auto LIterator = Permutation.find(LNode);
    revng_assert(KIterator != Permutation.end());
    revng_assert(LIterator != Permutation.end());

    auto OriginalCrossingCount = countCrossings(CurrentRank, KNode, LNode);
    std::swap(KIterator->second, LIterator->second);
    auto NewCrossingCount = countCrossings(CurrentRank, KNode, LNode);
    std::swap(KIterator->second, LIterator->second);

    return RankDelta(NewCrossingCount) - RankDelta(OriginalCrossingCount);
  }
};

/// Minimizes crossing count using a simple hill climbing algorithm.
/// The function can be sped up by providing an initial permutation found
/// using other techniques.
/// A function used for horizontal node segmentation can also be specified.
template<typename ClusterType>
LayerContainer minimizeCrossingCount(const RankContainer &Ranks,
                                     const ClusterType &Cluster,
                                     LayerContainer &&Layers) {
  revng_assert(countNodes(Layers) == Ranks.size());

  // In principle we'd like to compute the iteration count from some features in
  // the graph (number of nodes, number of edges, number of layers, number of
  // nodes in each layer, and so on...) so that, no matter how complex the
  // graph is, it's will always possible to terminate in an amount of time
  // that is guaranteed not to exceed an arbitrary hard limit.
  //
  // The problem is that we do not know how does the complexity of the graph
  // translate to minimization time, hence we run some experiments and select
  // a `ReferenceComplexity`. This number is an `IterationComplexity` of
  // a hand-picked graph that's moderately fast to lay out.
  constexpr size_t ReferenceComplexity = 70000;

  // To compare the current graph to the reference, we need to calculate
  // the `IterationComplexity` for the current graph:
  //
  // IterationComplexity = \sum_{i=0}^{Layers.size()}Layers.at(i).size()^2
  size_t IterationComplexity = 0;

  RankContainer Permutation;
  for (auto &Layer : Layers) {
    size_t LayerSize = Layer.size();
    for (size_t I = 0; I < LayerSize; I++)
      Permutation[Layer[I]] = I;
    IterationComplexity += LayerSize * LayerSize;
  }

  // The idea is that we cannot afford to do more than a single iteration
  // on any graph that's more complex than the reference one. On the other hand,
  // it's practical to allow graphs with complexity beneath that of the
  // reference to do multiple crossing minimization iterations.
  // The exact number depends on the ratio between the graph iteration
  // complexity to the reference iteration complexity.
  //
  // The reasoning is more or less like this: if the `IterationComplexity` is
  // small, each iteration is cheap and we can afford to do many iterations.
  // On the other hand, if the graph is large, the ratio rapidly goes to zero
  // and the iteration count drop to just a single one.
  //
  // TODO: we likely want to put a hardcoded cap on the max number of iterations
  size_t IterationCount = 1 + ReferenceComplexity / IterationComplexity;

  auto Comparator = [&Cluster, &Permutation](NodeView A, NodeView B) {
    if (Cluster(A) == Cluster(B))
      return Permutation.at(A) < Permutation.at(B);
    else
      return Cluster(A) < Cluster(B);
  };

  CrossingCalculator Calculator{ Layers, Ranks, Permutation };
  for (size_t Iteration = 0; Iteration < IterationCount; ++Iteration) {
    for (size_t Index = 0; Index < Layers.size(); ++Index) {
      if (size_t CurrentLayerSize = Layers[Index].size(); CurrentLayerSize) {

        // Minimize WRT of the previous layer
        // This can be expensive so we limit the number of times we repeat it.
        std::sort(Layers[Index].begin(), Layers[Index].end(), Comparator);
        for (size_t NodeIndex = 0; NodeIndex < CurrentLayerSize; ++NodeIndex)
          Permutation[Layers[Index][NodeIndex]] = NodeIndex;

        for (size_t NodeIndex = 0; NodeIndex < CurrentLayerSize; ++NodeIndex) {
          RankDelta ChoosenDelta = 0;
          std::pair<Rank, Rank> ChoosenNodes;

          for (size_t K = 0; K < CurrentLayerSize; ++K) {
            for (size_t L = K + 1; L < CurrentLayerSize; ++L) {
              auto KNode = Layers[Index][K];
              auto LNode = Layers[Index][L];
              auto Delta = Calculator.computeDelta(Index, KNode, LNode);
              if (Delta < ChoosenDelta) {
                ChoosenDelta = Delta;
                ChoosenNodes = { K, L };
              }
            }
          }

          if (ChoosenDelta == 0)
            break;

          auto KNode = Layers[Index][ChoosenNodes.first];
          auto LNode = Layers[Index][ChoosenNodes.second];
          std::swap(Permutation.at(KNode), Permutation.at(LNode));
        }

        std::sort(Layers[Index].begin(), Layers[Index].end(), Comparator);
        for (size_t NodeIndex = 0; NodeIndex < CurrentLayerSize; ++NodeIndex)
          Permutation[Layers[Index][NodeIndex]] = NodeIndex;
      }
    }
  }

  LayerContainer Result;
  for (auto &[Node, Rank] : Ranks) {
    if (Rank >= Result.size())
      Result.resize(Rank + 1);
    Result[Rank].emplace_back(Node);
  }
  for (auto &Layer : Result)
    std::sort(Layer.begin(),
              Layer.end(),
              [&Permutation](const auto &LHS, const auto &RHS) {
                return Permutation.at(LHS) < Permutation.at(RHS);
              });
  return Result;
}

template<bool PreOrPost, typename ClusterType>
class BarycentricComparator {
public:
  BarycentricComparator(const RankContainer &Ranks,
                        const RankContainer &Positions,
                        const LayerContainer &Layers,
                        const ClusterType &Cluster) :
    Ranks(Ranks), Positions(Positions), Layers(Layers), Cluster(Cluster) {}

  bool operator()(NodeView LHS, NodeView RHS) {
    if (int A = Cluster(LHS), B = Cluster(RHS); A == B) {
      auto BarycenterA = get(LHS), BarycenterB = get(RHS);
      if (std::isnan(BarycenterA) || std::isnan(BarycenterB))
        return LHS->Index < RHS->Index;
      else
        return BarycenterA < BarycenterB;
    } else {
      return A < B;
    }
  }

protected:
  double get(NodeView Node) {
    auto Iterator = Barycenters.lower_bound(Node);
    if (Iterator == Barycenters.end() || Node < Iterator->first)
      Iterator = Barycenters.insert(Iterator, { Node, compute(Node) });
    return Iterator->second;
  }

  double computeImpl(NodeView Node, auto NeighborContainerView) {
    if (NeighborContainerView.empty())
      return std::numeric_limits<double>::quiet_NaN();

    double Accumulator = 0;
    size_t Counter = 0;

    double CurrentLayerSize = Layers.at(Ranks.at(Node)).size();
    for (auto *Neighbor : NeighborContainerView) {
      if (auto It = Positions.find(Neighbor); It != Positions.end()) {
        auto NeighborLayerSize = Layers.at(Ranks.at(Neighbor)).size();
        Accumulator += (CurrentLayerSize / NeighborLayerSize) * It->second;
        ++Counter;
      }
    }

    if (Counter == 0)
      return std::numeric_limits<double>::quiet_NaN();
    return Accumulator / Counter;
  }

  double compute(NodeView Node) {
    if constexpr (PreOrPost)
      return computeImpl(Node, Node->predecessors());
    else
      return computeImpl(Node, Node->successors());
  }

private:
  std::map<NodeView, double> Barycenters;
  const RankContainer &Ranks;
  const RankContainer &Positions;
  const LayerContainer &Layers;
  const ClusterType &Cluster;
};

/// Sorts the permutations using the barycentric sorting described in
/// "A fast heuristic for hierarchical Manhattan layout" by G. Sander (2005)
template<typename ClusterType>
LayerContainer sortNodes(const RankContainer &Ranks,
                         const size_t IterationCount,
                         const ClusterType &Cluster,
                         LayerContainer &&Layers) {
  revng_assert(countNodes(Layers) == Ranks.size());

  RankContainer Positions;
  for (size_t Iteration = 0; Iteration < IterationCount; Iteration++) {
    for (size_t Index = 0; Index < Layers.size(); ++Index) {
      for (size_t Counter = 0; auto Node : Layers[Index])
        Positions[Node] = Counter++;

      using PBC = BarycentricComparator<true, decltype(Cluster)>;
      PBC Comparator(Ranks, Positions, Layers, Cluster);
      std::sort(Layers[Index].begin(), Layers[Index].end(), Comparator);

      for (size_t Counter = 0; auto Node : Layers[Index])
        Positions[Node] = Counter++;
    }
    for (size_t Index = Layers.size() - 1; Index != size_t(-1); --Index) {
      for (size_t Counter = 0; auto Node : Layers[Index])
        Positions[Node] = Counter++;

      using PBC = BarycentricComparator<false, decltype(Cluster)>;
      PBC Comparator(Ranks, Positions, Layers, Cluster);
      std::sort(Layers[Index].begin(), Layers[Index].end(), Comparator);

      for (size_t Counter = 0; auto Node : Layers[Index])
        Positions[Node] = Counter++;
    }
  }
  return std::move(Layers);
}

template<RankingStrategy Strategy>
LayerContainer selectPermutation(InternalGraph &Graph,
                                 RankContainer &Ranks,
                                 const NodeClassifier<Strategy> &Classifier) {
  // Build a layer container based on a given ranking, then remove layers
  // without any original nodes or nodes that are required for correct
  // backwards edge routing. Update ranks accordingly.
  auto InitialLayers = optimizeLayers(Graph, Ranks);

  auto MinimalCrossingLayers = minimizeCrossingCount(Ranks,
                                                     Classifier,
                                                     std::move(InitialLayers));

  // Iteration counts are chosen arbitrarily. If the computation time was not
  // an issue, we could keep iterating until convergence, but since it's not
  // the case, we have to choose a stoping point.
  //
  // The iteration count logarithmically depends on the layer number.
  size_t Iterations = std::log2(InitialLayers.size());

  if constexpr (Strategy == RankingStrategy::BreadthFirstSearch) {
    // \todo: Since layers are wide, the inner loops inside both crossing
    // minimization and node sorting are more costly on average.
    // Maybe inner loop orders should be randomized.
  }

  auto SortedLayers = sortNodes(Ranks,
                                Iterations * 3 + 10,
                                Classifier,
                                std::move(MinimalCrossingLayers));

  return SortedLayers;
}

/// Topologically orders nodes of an augmented graph generated based on a
/// layered version of the graph.
static std::vector<NodeView>
toAugmentedTopologicalOrder(InternalGraph &Graph,
                            const LayerContainer &Layers) {
  using AugmentedNode = BidirectionalNode<NodeView>;
  using AugmentedGraph = GenericGraph<AugmentedNode>;

  // Define internal graph.
  AugmentedGraph Augmented;

  // Add original nodes in.
  std::unordered_map<size_t, AugmentedNode *> LookupTable;
  for (auto *Node : Graph.nodes()) {
    auto NewNode = Augmented.addNode(Node);
    LookupTable.emplace(Node->Index, NewNode);
  }

  // Add original edges in.
  for (auto *From : Graph.nodes())
    for (auto *To : From->successors())
      LookupTable.at(From->Index)->addSuccessor(LookupTable.at(To->Index));

  // Add extra edges.
  for (size_t Layer = 0; Layer < Layers.size(); ++Layer) {
    for (size_t From = 0; From < Layers[Layer].size(); ++From) {
      for (size_t To = From + 1; To < Layers[Layer].size(); ++To) {
        auto *FromNode = LookupTable.at(Layers[Layer][From]->Index);
        auto *ToNode = LookupTable.at(Layers[Layer][To]->Index);
        FromNode->addSuccessor(ToNode);
      }
    }
  }

  // Ensure there's a single entry point.
  // \note: `EntryNode` is not added to the augmented graph and is never
  // `disconnect`ed, so be careful when using the graph from this point on.
  // The "broken" state is used as a minor optimization to avoid unnecessary
  // cleanup since the graph is stack-allocated and only exists until this
  // function is over.
  AugmentedNode EntryNode;
  for (auto *Node : Augmented.nodes())
    if (!Node->hasPredecessors())
      EntryNode.addSuccessor(Node);
  revng_assert(EntryNode.hasSuccessors());

  // Store the topological order for the augmented graph
  std::vector<NodeView> Order;
  for (auto *Node : llvm::ReversePostOrderTraversal(&EntryNode))
    if (Node != &EntryNode)
      Order.emplace_back(*Node);

  revng_assert(Order.size() == Graph.size());
  return Order;
}

/// An internal data structure used to pass around information about the way
/// graph is split onto segments.
using SegmentContainer = std::unordered_map<NodeView, NodeView>;

/// A helper used to simplify the parent lookup when deciding on the linear
/// segments of the layout.
class ParentLookupHelper {
protected:
  NodeView lookupImpl(NodeView Node) {
    if (auto Iterator = HeadsRef.find(Node); Iterator == HeadsRef.end())
      return (HeadsRef[Node] = Node);
    else if (Iterator->second == Node)
      return Node;
    else
      return lookupImpl(Iterator->second);
  }

public:
  ParentLookupHelper(SegmentContainer &HeadsRef) : HeadsRef(HeadsRef) {}
  NodeView operator()(NodeView Node) { return lookupImpl(Node); }

private:
  SegmentContainer &HeadsRef;
};

/// Looks for the linear segments and ensure an optimal combination of them
/// is selected. It uses an algorithm from the Sander's paper.
/// The worst case complexity is O(N^2) for cases where jump table is huge,
/// but the common case is very far from that because normally both
/// entry and exit edge count is slim (intuitively, our
/// layouts are tall rather than wide).
/// \note: it's probably a good idea to think about loosening the dependence
/// on tall graph layouts since we will want to also lay more generic graphs
/// out.
/// \note: it's probably a good idea to split this function up into a couple
/// of smaller ones, it's too long.
static SegmentContainer
selectLinearSegments(InternalGraph &Graph,
                     const RankContainer &Ranks,
                     const LayerContainer &Layers,
                     const std::vector<NodeView> &Order) {
  SegmentContainer Heads, Tails;

  std::vector<NodeView> Modified;
  for (auto *Node : Graph.nodes()) {
    Heads[Node] = Tails[Node] = Node;
    Modified.emplace_back(Node);
  }

  while (!Modified.empty()) {
    decltype(Modified) NewlyModified;
    for (auto Node : Modified) {
      if (Tails.at(Node)->successorCount() == 1) {
        auto Current = Tails.at(Node);
        auto *Successor = *Current->successors().begin();
        if (Ranks.at(Current) < Ranks.at(Successor)) {
          if (Successor->predecessorCount() == 1) {
            // Merge segments
            Tails[Node] = Tails.at(Successor);
            NewlyModified.push_back(Node);
          }
        }
      }
    }

    std::swap(Modified, NewlyModified);
  }

  ParentLookupHelper ParentLookup(Heads);
  for (auto &[Head, Tail] : Tails) {
    auto ParentView = ParentLookup(Head);

    auto Current = Head;
    while (Current != Tail) {
      revng_assert(Current->hasSuccessors());
      Current = *Current->successors().begin();
      Heads[ParentLookup(Current)] = ParentView;
    }
  }

  auto &ParentMap = Heads;

  std::unordered_map<NodeView, size_t> OrderLookupTable;
  for (size_t Index = 0; Index < Order.size(); ++Index)
    OrderLookupTable[Order[Index]] = Index;

  std::deque<NodeView> Queue;
  for (auto &[Node, Parent] : ParentMap)
    if (Node != ParentLookup(Parent))
      Queue.emplace_back(Node);

  while (!Queue.empty()) {
    auto Current = Queue.front();
    Queue.pop_front();

    auto Parent = ParentMap[Current];
    if (Current == Parent)
      continue;

    auto CurrentIndex = OrderLookupTable[Current];
    auto ParentIndex = OrderLookupTable[Parent];
    for (auto Cousin : Layers.at(Ranks.at(Current))) {
      if (Cousin == Current)
        continue;

      auto CousinIndex = OrderLookupTable[Cousin];
      if (ParentMap.contains(Cousin)) {
        if (auto Ommer = ParentMap[Cousin]; Ommer != Cousin) {
          auto OmmerIndex = OrderLookupTable[Ommer];
          auto PD = RankDelta(ParentIndex) - RankDelta(OmmerIndex);
          auto CD = RankDelta(CurrentIndex) - RankDelta(CousinIndex);
          // Compares the relative directions between parents and children.
          // If `ParentIndex < OmmerIndex` (the parent is placed before the
          // ommer), the `PD` is negative. It's the same for children (`CD`).
          // Multiplication acts as a `XOR` operation on them - if both
          // relations are the same - the product is positive. It's negative
          // otherwise. The straightforward segmentation is impossible for
          // the case with opposite orientation, that's why the segments are
          // broken (both `Current` and `Cousin` are set as the heads of their
          // respective segments).
          if (PD * CD < 0) {
            ParentMap[Current] = Current;
            ParentMap[Cousin] = Cousin;

            for (auto *Successor : Current->successors())
              Queue.emplace_back(Successor);
            for (auto *Successor : Cousin->successors())
              Queue.emplace_back(Successor);
          }
        }
      }
    }
  }

  SegmentContainer Result;
  for (auto &[Node, _] : ParentMap)
    Result[Node] = ParentLookup(Node);

  if (Result.size() != Graph.size())
    for (auto *Node : Graph.nodes())
      Result.try_emplace(Node, Node);
  revng_assert(Result.size() == Graph.size());

  return Result;
}

/// It's used to describe the position of a single node within the layered grid.
struct LogicalPosition {
  Rank Layer; /// Rank of the layer the node is in.
  Rank Index; /// An index within the layer
};

/// It's used to describe the complete layout by storing the exact
/// position of each node relative to all the others.
using LayoutContainer = std::unordered_map<NodeView, LogicalPosition>;

/// "Levels up" a `LayerContainer` to a `LayoutContainer`.
static LayoutContainer toLayout(const LayerContainer &Layers) {
  LayoutContainer Result;
  for (size_t Index = 0; Index < Layers.size(); ++Index)
    for (size_t NodeIndex = 0; NodeIndex < Layers[Index].size(); ++NodeIndex)
      Result[Layers[Index][NodeIndex]] = { Index, NodeIndex };
  return Result;
}

/// To determine the coordinates, we need to solve this equation system:
/// X_n : X_n > X_p(n) + W_n / 2 + W_p(n) / 2 + margin X_n : X_n = X_s(n)
///
/// It can be done iteratively until convergence, see the paper for the proof.
/// \note: this function is too long. It should probably be split up.
static void setHorizontalCoordinates(const LayerContainer &Layers,
                                     const std::vector<NodeView> &Order,
                                     const SegmentContainer &LinearSegments,
                                     const LayoutContainer &Layout,
                                     float MarginSize) {
  // `IterationCount` can be tuned depending on the types of nodes and edges
  // \note: BFS layouts have wider layers, so it might make sense to make
  // `IterationCount` larger for them.
  size_t IterationCount = 10 + 5 * std::log2(Layers.size());

  // Clear the existing coordinates so they don't interfere
  for (auto Node : Order)
    Node->center() = { 0, 0 };

  // First, the minimal horizontal width for each layer is computed.
  //
  // Starting each layer at a proportional negative offset helps
  // with centering the layout.
  std::vector<double> MinimalLayerWidths;
  MinimalLayerWidths.resize(Layers.size());
  for (size_t Index = 0; Index < Layers.size(); ++Index)
    for (auto Node : Layers[Index])
      MinimalLayerWidths[Index] += Node->size().W + MarginSize;

  for (size_t Iteration = 0; Iteration < IterationCount; ++Iteration) {
    for (auto [Child, Parent] : LinearSegments)
      if (Child->center().X > Parent->center().X)
        Parent->center().X = Child->center().X;
      else
        Child->center().X = Parent->center().X;

    // \note: I feel like this loop can be A LOT simpler.
    std::map<Rank, double> MinimumX;
    for (auto Node : Order) {
      auto LayerIndex = Layout.at(Node).Layer;
      auto Iterator = MinimumX.lower_bound(LayerIndex);
      if (Iterator == MinimumX.end() || Iterator->first > LayerIndex) {
        auto Offset = -MinimalLayerWidths[LayerIndex] / 2;
        Iterator = MinimumX.insert(Iterator, { LayerIndex, Offset });
      }

      auto LeftX = Iterator->second + Node->size().W / 2 + MarginSize;
      if (LeftX > Node->center().X)
        Node->center().X = LeftX;
      Iterator->second = Node->center().X + Node->size().W / 2 + MarginSize;
    }
  }

  // At this point, the layout is very left heavy.
  // To conteract this, nodes are pushed to the right if their neighbours are
  // very far away and the successors are also on the right.
  // The positions are weighted based on the successor positions.
  for (size_t Iteration = 0; Iteration < IterationCount; ++Iteration) {
    // First of all, a safe right margin is computed for each segment.
    // A given coordinate is safe to use if no node in the segment would
    // intersect a neighbour

    std::map<NodeView, double> RightSegments;
    {
      // Check for segments that are on the rightmost edge of the
      // graph, these are the segments that can be moved freely.
      std::map<NodeView, bool> FreeSegments;

      for (auto [Child, Parent] : LinearSegments) {
        auto const &ChildPosition = Layout.at(Child);
        auto ChildLayerSize = Layers.at(ChildPosition.Layer).size();
        if (ChildLayerSize == ChildPosition.Index + 1) {
          auto Iterator = FreeSegments.lower_bound(Parent);
          if (Iterator == FreeSegments.end()
              || Iterator->first->Index > Parent->Index)
            FreeSegments.insert(Iterator, { Parent, true });
        } else {
          FreeSegments[Parent] = false;
        }
      }

      std::vector<double> Barycenters(Layers.size());
      for (size_t Index = 0; Index < Layers.size(); ++Index) {
        if (Layers[Index].size() != 0) {
          for (auto Node : Layers[Index])
            Barycenters[Index] += Node->center().X;
          Barycenters[Index] /= Layers[Index].size();
        }
      }

      for (size_t Index = 0; Index < Layers.size(); ++Index) {
        if (Layers[Index].size() != 0) {
          for (size_t J = 0; J < Layers[Index].size() - 1; ++J) {
            auto Current = Layers[Index][J];
            auto Next = Layers[Index][J + 1];
            double RightMargin = Next->center().X - Next->size().W / 2
                                 - MarginSize * 2 - Current->size().W / 2;
            auto Parent = LinearSegments.at(Current);
            auto Iterator = RightSegments.lower_bound(Parent);
            if (Iterator == RightSegments.end()
                || Iterator->first->Index > Parent->Index)
              RightSegments.insert(Iterator, { Parent, RightMargin });
            else if (RightMargin < Iterator->second)
              Iterator->second = RightMargin;
          }

          // Set the bounds for the rightmost node.
          //
          // Any coordinate is safe for the rightmost node, but the barycenters
          // of both layers should be taken into the account, so that
          // the layout does not become skewed
          float RightMargin = Layers[Index].back()->center().X;
          if (Index + 1 < Layers.size())
            if (auto Dlt = Barycenters[Index + 1] - Barycenters[Index]; Dlt > 0)
              RightMargin += Dlt;

          auto Parent = LinearSegments.at(Layers[Index].back());
          auto Iterator = RightSegments.lower_bound(Parent);
          if (Iterator == RightSegments.end()
              || Iterator->first->Index > Parent->Index)
            RightSegments.insert(Iterator, { Parent, RightMargin });
          else if (FreeSegments.at(Parent)) {
            if (RightMargin > Iterator->second)
              Iterator->second = RightMargin;
          } else if (RightMargin < Iterator->second) {
            Iterator->second = RightMargin;
          }
        }
      }
    }

    // Balance the graph to the right while keeping the safe right margin
    for (size_t Index = Layers.size() - 1; Index != size_t(-1); --Index) {
      if (Layers[Index].empty())
        continue;

      // First move the rightmost node.
      if (auto Node = Layers[Index].back(); !Node->isVirtual()) {
        if (Node->successorCount() > 0) {
          double Barycenter = 0, TotalWeight = 0;
          for (auto *Next : Node->successors()) {
            float Weight = Next->isVirtual() ? 1 : 10;
            // This weight as an arbitrary number used to help the algorithm
            // prioritize putting a node closer to its non-virtual successors.

            Barycenter += Weight * Next->center().X;
            TotalWeight += Weight;
          }
          Barycenter /= TotalWeight;

          if (Barycenter > Node->center().X)
            Node->center().X = Barycenter;
        }
      }

      // Then move the rest.
      for (size_t J = Layers[Index].size() - 1; J != size_t(-1); --J) {
        auto Node = Layers[Index][J];

        float LayerMargin = RightSegments.at(LinearSegments.at(Node));
        if (Node->successorCount() > 0) {
          double Barycenter = 0;
          double TotalWeight = 0;
          for (auto *Next : Node->successors()) {
            float Weight = Next->isVirtual() ? 1 : 10;
            // This weight as an arbitrary number used to help the algorithm
            // prioritize putting a node closer to its non-virtual successors.

            Barycenter += Weight * Next->center().X;
            TotalWeight += Weight;
          }
          Barycenter /= TotalWeight;

          if (Barycenter < LayerMargin)
            LayerMargin = Barycenter;
        }

        if (LayerMargin > Node->center().X)
          Node->center().X = LayerMargin;
      }
    }
  }
}

/// A view onto an edge. It stores `From` and `To` node views, a pointer to
/// an external label and a flag declaring the direction of the edge.
struct DirectedEdgeView : public DirectionlessEdgeView {
  bool IsBackwards = false;

  DirectedEdgeView(NodeView From, NodeView To, ParamType L, bool IsBackwards) :
    DirectionlessEdgeView(From, To, L), IsBackwards(IsBackwards) {}
};

/// Detects whether Edge faces left, right or neither.
/// \note: Legacy function, could possibly be merged into its only user.
static auto getFacingDirection(const DirectedEdgeView &Edge) {
  auto &LHS = Edge.To->center().X;
  auto &RHS = Edge.From->center().X;
  return (LHS == RHS ? 0 : (Edge.IsBackwards ? 1 : -1) * (LHS > RHS ? 1 : -1));
}

/// Returns `true` if `Edge` is facing left-to-right, `false` otherwise.
static bool facesRight(const DirectedEdgeView &Edge) {
  return getFacingDirection(Edge) > 0;
}

/// Compares two edges. This is used as a comparator for horizontal edge lane
/// sorting. The direction of an edge is the most most important characteristic
/// since we want to split "left-to-right" edges from "right-to-left" ones -
/// that helps to minimize the number of crossings.
/// When directions are the same, edges are sorted based on the horizontal
/// coordinates of their ends (the edge that needs to go further is placed
/// closer to the outside of the laning section).
static bool compareHorizontalLanes(const DirectedEdgeView &LHS,
                                   const DirectedEdgeView &RHS) {
  bool LHSFacesRight = facesRight(LHS);
  bool RHSFacesRight = facesRight(RHS);
  if (LHSFacesRight == RHSFacesRight) {
    auto LHSFromX = LHS.From->center().X;
    auto RHSFromX = RHS.From->center().X;
    auto LHSToX = LHS.To->center().X;
    auto RHSToX = RHS.To->center().X;
    auto [LHSMin, LHSMax] = std::minmax(LHSFromX, LHSToX);
    auto [RHSMin, RHSMax] = std::minmax(RHSFromX, RHSToX);

    if (LHSFacesRight)
      return LHSMax == RHSMax ? LHSMin < RHSMin : LHSMax < RHSMax;
    else
      return LHSMax == RHSMax ? LHSMin > RHSMin : LHSMax > RHSMax;
  } else {
    return LHSFacesRight < RHSFacesRight;
  }
}

/// Returns the entry node of an edge taking into account whether the edge was
/// reversed or not.
static NodeView getEntry(const DirectedEdgeView &Edge) {
  return Edge.IsBackwards ? Edge.To : Edge.From;
}

/// Returns the exit node of an edge taking into account whether the edge was
/// reversed or not.
static NodeView getExit(const DirectedEdgeView &Edge) {
  return Edge.IsBackwards ? Edge.From : Edge.To;
}

/// An internal data structure used to pass around information about the lanes
/// used to route edges.
struct LaneContainer {
  /// Stores edges that require a horizontal section grouped by layer.
  std::vector<std::map<DirectedEdgeView, Rank>> Horizontal;

  /// Stores edges entering a node groped by the node they enter.
  std::unordered_map<NodeView, std::map<DirectedEdgeView, Rank>> Entries;

  /// Stores edges leaving a node grouped by the node they leave.
  std::unordered_map<NodeView, std::map<DirectedEdgeView, Rank>> Exits;
};

/// Distributes "touching" edges accross lanes to minimize the crossing count.
static LaneContainer assignLanes(InternalGraph &Graph,
                                 const SegmentContainer &LinearSegments,
                                 const LayoutContainer &Layout) {
  // Stores edges that require a horizontal section grouped by the layer rank.
  std::vector<llvm::SmallVector<DirectedEdgeView, 2>> Horizontal;

  // Stores edges entering a node grouped by the node they enter.
  std::unordered_map<NodeView, llvm::SmallVector<DirectedEdgeView, 2>> Entries;

  // Stores edges leaving a node grouped by the node they leave.
  std::unordered_map<NodeView, llvm::SmallVector<DirectedEdgeView, 2>> Exits;

  // Calculate the number of lanes needed for each layer
  for (auto *From : Graph.nodes()) {
    for (auto [To, Label] : From->successor_edges()) {
      // If the ends of an edge are not a part of the same linear segment or
      // their horizontal coordinates are not aligned, a bend is necessary.
      if (LinearSegments.at(From) != LinearSegments.at(To)
          || From->center().X != To->center().X) {
        auto LayerIndex = std::min(Layout.at(From).Layer, Layout.at(To).Layer);
        if (LayerIndex >= Horizontal.size())
          Horizontal.resize(LayerIndex + 1);

        if (Label->IsBackwards) {
          Horizontal[LayerIndex].emplace_back(To, From, Label->Pointer, true);
          Entries[From].emplace_back(To, From, Label->Pointer, true);
          Exits[To].emplace_back(To, From, Label->Pointer, true);
        } else {
          Horizontal[LayerIndex].emplace_back(From, To, Label->Pointer, false);
          Entries[To].emplace_back(From, To, Label->Pointer, false);
          Exits[From].emplace_back(From, To, Label->Pointer, false);
        }
      }
    }
  }

  LaneContainer Result;

  // Sort edges when they leave nodes
  auto ExitComparator = [&Layout](const DirectedEdgeView &LHS,
                                  const DirectedEdgeView &RHS) {
    return Layout.at(getExit(LHS)).Index < Layout.at(getExit(RHS)).Index;
  };
  for (auto &[Node, Edges] : Exits) {
    std::sort(Edges.begin(), Edges.end(), ExitComparator);
    auto &NodeExits = Result.Exits[Node];
    for (size_t ExitRank = 0; ExitRank < Edges.size(); ExitRank++)
      NodeExits.try_emplace(Edges[ExitRank], ExitRank);
  }

  // Sort edges where they enter nodes
  auto EntryComparator = [&Layout](const DirectedEdgeView &LHS,
                                   const DirectedEdgeView &RHS) {
    return Layout.at(getEntry(LHS)).Index < Layout.at(getEntry(RHS)).Index;
  };
  for (auto &[Node, Edges] : Entries) {
    std::sort(Edges.begin(), Edges.end(), EntryComparator);
    auto &NodeEntries = Result.Entries[Node];
    for (size_t EntryRank = 0; EntryRank < Edges.size(); EntryRank++)
      NodeEntries.try_emplace(Edges[EntryRank], EntryRank);
  }

  // Sort horizontal lanes
  Result.Horizontal.resize(Horizontal.size());
  for (size_t Index = 0; Index < Horizontal.size(); ++Index) {
    auto &CurrentLane = Horizontal[Index];
    // The edges going to the left and the edges going to the right are
    // distinguished in order to minimize the number of crossings.
    //
    // "left-to-right" edges need to be layered from the closest to the most
    // distant one, while "right-to-left" edges - in the opposite order.
    std::sort(CurrentLane.begin(), CurrentLane.end(), compareHorizontalLanes);
    for (size_t I = 0; I < CurrentLane.size(); I++)
      Result.Horizontal[Index][CurrentLane[I]] = CurrentLane.size() - I;
  }

  return Result;
}

/// Sets final vertical coordinates for each of the nodes.
static void setVerticalCoordinates(const LayerContainer &Layers,
                                   const LaneContainer &Lanes,
                                   float MarginSize,
                                   float EdgeDistance) {
  float LastY = 0;
  for (size_t Index = 0; Index < Layers.size(); ++Index) {
    float MaxHeight = 0;
    for (auto Node : Layers[Index]) {
      auto NodeHeight = Node->size().H;
      if (MaxHeight < NodeHeight)
        MaxHeight = NodeHeight;
      Node->center().Y = LastY - Node->size().H / 2;
    }

    auto LaneCount = Index < Lanes.Horizontal.size() ?
                       Lanes.Horizontal.at(Index).size() :
                       0;
    LastY -= MaxHeight + EdgeDistance * LaneCount + MarginSize * 2;
  }
}

/// An internal data structure used to represent a corner. It stores three
/// points: center of the corner and two ends.
struct Corner {
  Point Start, Center, End;
};

/// An internal data structure used to represent a pair of nodes.
struct NodePair {
  NodeView From, To;

  auto operator<=>(const NodePair &) const = default;
};

/// An internal data structure used to pass around information about the routed
/// corners.
using CornerContainer = std::map<NodePair, Corner>;

/// Routes edges that form backwards facing corners. For their indication,
/// V-shaped structures were added to the graph when the backwards edges
/// were partitioned.
static CornerContainer routeBackwardsCorners(InternalGraph &Graph,
                                             const RankContainer &Ranks,
                                             const LaneContainer &Lanes,
                                             float MarginSize,
                                             float EdgeDistance) {
  std::vector<DirectedEdgeView> CornerEdges;

  // To keep the hierarchy consistent, V-shapes were added using forward
  // direction. So that's what we're going to use to detect them.
  for (auto *From : Graph.nodes())
    for (auto [To, Label] : From->successor_edges())
      if (!From->isVirtual() != !To->isVirtual() && !Label->IsBackwards)
        CornerEdges.emplace_back(From, To, Label->Pointer, false);

  CornerContainer Corners;
  for (auto &Edge : CornerEdges) {
    auto LaneIndex = 0;
    auto Rank = std::min(Ranks.at(Edge.From), Ranks.at(Edge.To));
    if (Rank < Lanes.Horizontal.size()) {
      auto &CurrentLayerLanes = Lanes.Horizontal.at(Rank);
      auto Iterator = CurrentLayerLanes.find(Edge);
      if (Iterator != CurrentLayerLanes.end())
        LaneIndex = Iterator->second;
    }

    if (Edge.From->isVirtual() && !Edge.To->isVirtual()) {
      if (Edge.From->successorCount() != 2 || Edge.From->hasPredecessors())
        continue;

      // One side of the corner.
      auto *First = *Edge.From->successors().begin();

      // The other side.
      auto *Second = *std::next(Edge.From->successors().begin());

      // Make sure there are no self-loops, otherwise it's not a corner.
      if (First->Index == Edge.From->Index || Second->Index == Edge.From->Index)
        continue;

      auto ToUpperEdge = First->center().Y + First->size().H / 2;
      auto FromUpperEdge = Second->center().Y + Second->size().H / 2;
      Edge.From->center().X = (First->center().X + Second->center().X) / 2;
      Edge.From->center().Y = std::min(ToUpperEdge, FromUpperEdge) + MarginSize
                              + LaneIndex * EdgeDistance;

      auto &From = Edge.From;
      for (auto [To, Label] : From->successor_edges()) {
        auto FromTop = From->center().Y + From->size().H / 2;
        auto ToTop = To->center().Y + To->size().H / 2;

        if (Label->IsBackwards) {
          revng_assert(!Corners.contains({ To, From }));

          auto FromPoint = Point{ To->center().X, ToTop };
          auto CenterPoint = Point{ To->center().X, From->center().Y };
          auto ToPoint = Point{ From->center().X, FromTop };

          Corners.emplace(NodePair{ To, From },
                          Corner{ FromPoint, CenterPoint, ToPoint });
        } else {
          revng_assert(!Corners.contains({ From, To }));

          auto ToLane = To->center().X;
          if (auto It = Lanes.Entries.find(To); It != Lanes.Entries.end()) {
            auto View = DirectedEdgeView{ From, To, Label->Pointer, false };
            revng_assert(It->second.contains(View));

            auto EntryIndex = float(It->second.at(View));
            auto CenteredIndex = EntryIndex - float(It->second.size() - 1) / 2;

            auto ToLaneGap = EdgeDistance / 2;
            if (It->second.size() != 0) {
              auto AlternativeGap = To->size().W / 2 / It->second.size();
              if (AlternativeGap < ToLaneGap)
                ToLaneGap = AlternativeGap;
            }

            ToLane += ToLaneGap * CenteredIndex;
          }

          auto FromPoint = Point{ From->center().X, FromTop };
          auto CenterPoint = Point{ ToLane, From->center().Y };
          auto ToPoint = Point{ ToLane, ToTop };

          Corners.emplace(NodePair{ From, To },
                          Corner{ FromPoint, CenterPoint, ToPoint });
        }
      }
    } else {
      if (Edge.To->predecessorCount() != 2 || Edge.To->hasSuccessors())
        continue;

      // One side of the corner.
      auto *First = *Edge.To->predecessors().begin();

      // The other side.
      auto *Second = *std::next(Edge.To->predecessors().begin());

      // Make sure there are no self-loops, otherwise it's not a corner.
      if (First->Index == Edge.To->Index || Second->Index == Edge.To->Index)
        continue;

      Edge.To->center().X = (First->center().X + Second->center().X) / 2;
      Edge.To->center().Y += MarginSize + LaneIndex * EdgeDistance;

      auto &To = Edge.To;
      for (auto [From, Label] : To->predecessor_edges()) {
        auto FromBottom = From->center().Y - From->size().H / 2;
        auto ToBottom = To->center().Y - To->size().H / 2;

        if (Label->IsBackwards) {
          revng_assert(!Corners.contains({ To, From }));

          auto FromPoint = Point{ To->center().X, ToBottom };
          auto CenterPoint = Point{ From->center().X, To->center().Y };
          auto ToPoint = Point{ From->center().X, FromBottom };

          Corners.emplace(NodePair{ To, From },
                          Corner{ FromPoint, CenterPoint, ToPoint });
        } else {
          revng_assert(!Corners.contains({ From, To }));

          auto FromLane = From->center().X;
          if (auto It = Lanes.Exits.find(From); It != Lanes.Exits.end()) {
            auto View = DirectedEdgeView{ From, To, Label->Pointer, false };
            revng_assert(It->second.contains(View));

            auto ExitIndex = float(It->second.at(View));
            auto CenteredIndex = ExitIndex - float(It->second.size() - 1) / 2;

            auto FromLaneGap = EdgeDistance / 2;
            if (It->second.size() != 0) {
              auto AlternativeGap = From->size().W / 2 / It->second.size();
              if (AlternativeGap < FromLaneGap)
                FromLaneGap = AlternativeGap;
            }

            FromLane += FromLaneGap * CenteredIndex;
          }

          auto FromPoint = Point{ FromLane, FromBottom };
          auto CenterPoint = Point{ FromLane, To->center().Y };
          auto ToPoint = Point{ To->center().X, ToBottom };

          Corners.emplace(NodePair{ From, To },
                          Corner{ FromPoint, CenterPoint, ToPoint });
        }
      }
    }
  }

  return Corners;
}

/// An internal data structure used to represent all the necessary information
/// for an edge to be routed. This data is usable even after the internal graph
/// was destroyed.
struct RoutableEdge {
  ExternalLabel *Label;
  Point FromCenter, ToCenter;
  Size FromSize, ToSize;
  Rank LaneIndex, ExitCount, EntryCount;
  float CenteredExitIndex, CenteredEntryIndex;
  std::optional<Corner> Prerouted;
};

/// An internal data structure used to pass around an ordered list of routed
/// edges.
using OrderedEdgeContainer = std::vector<RoutableEdge>;

/// A helper class used for construction of `RoutableEdge`s.
class RoutableEdgeMaker {
public:
  RoutableEdgeMaker(const RankContainer &Ranks,
                    const LaneContainer &Lanes,
                    CornerContainer &&Prerouted) :
    Ranks(Ranks), Lanes(Lanes), Prerouted(std::move(Prerouted)) {}

  RoutableEdge make(NodeView From, NodeView To, ExternalLabel *Label) {
    auto View = DirectedEdgeView{ From, To, Label, false };

    Rank ExitIndex = 0;
    Rank ExitCount = 1;
    if (auto It = Lanes.Exits.find(From); It != Lanes.Exits.end()) {
      if (It->second.size() != 0) {
        revng_assert(It->second.contains(View));
        ExitIndex = It->second.at(View);
        ExitCount = It->second.size();
      }
    }

    Rank EntryIndex = 0;
    Rank EntryCount = 1;
    if (auto It = Lanes.Entries.find(To); It != Lanes.Entries.end()) {
      if (It->second.size() != 0) {
        revng_assert(It->second.contains(View));
        EntryIndex = It->second.at(View);
        EntryCount = It->second.size();
      }
    }

    Rank LaneIndex = 0;
    if (auto LayerIndex = std::min(Ranks.at(From), Ranks.at(To));
        LayerIndex < Lanes.Horizontal.size()
        && Lanes.Horizontal[LayerIndex].size()) {
      if (auto Iterator = Lanes.Horizontal[LayerIndex].find(View);
          Iterator != Lanes.Horizontal[LayerIndex].end())
        LaneIndex = Iterator->second;
    }

    decltype(RoutableEdge::Prerouted) CurrentRoute = std::nullopt;
    if (auto Iterator = Prerouted.find({ From, To });
        Iterator != Prerouted.end())
      CurrentRoute = std::move(Iterator->second);

    return RoutableEdge{
      .Label = Label,
      .FromCenter = From->center(),
      .ToCenter = To->center(),
      .FromSize = From->size(),
      .ToSize = To->size(),
      .LaneIndex = LaneIndex,
      .ExitCount = ExitCount,
      .EntryCount = EntryCount,
      .CenteredExitIndex = float(ExitIndex) - float(ExitCount - 1) / 2,
      .CenteredEntryIndex = float(EntryIndex) - float(EntryCount - 1) / 2,
      .Prerouted = CurrentRoute
    };
  }

private:
  const RankContainer &Ranks;
  const LaneContainer &Lanes;
  CornerContainer &&Prerouted;
};

/// Consumes a DAG to produce the optimal routing order.
static OrderedEdgeContainer orderEdges(InternalGraph &&Graph,
                                       CornerContainer &&Prerouted,
                                       const RankContainer &Ranks,
                                       const LaneContainer &Lanes) {
  for (auto *From : Graph.nodes()) {
    for (auto Iterator = From->successor_edges().begin();
         Iterator != From->successor_edges().end();) {
      if (auto [To, Label] = *Iterator; Label->IsBackwards) {
        Label->IsBackwards = !Label->IsBackwards;
        To->addSuccessor(From, std::move(*Label));
        Iterator = From->removeSuccessor(Iterator);
      } else {
        ++Iterator;
      }
    }
  }

  OrderedEdgeContainer Result;
  RoutableEdgeMaker Maker(Ranks, Lanes, std::move(Prerouted));
  for (auto *From : Graph.nodes()) {
    if (!From->isVirtual()) {
      for (auto [To, Label] : From->successor_edges()) {
        revng_assert(Label->IsBackwards == false);
        Result.emplace_back(Maker.make(From, To, Label->Pointer));
        if (To->isVirtual()) {
          for (auto *Current : llvm::depth_first(To)) {
            if (!Current->isVirtual())
              break;

            revng_assert(Current->successorCount() == 1);
            revng_assert(Current->predecessorCount() == 1
                         || (Current->predecessorCount() == 2
                             && Graph.hasEntryNode && Graph.getEntryNode()
                             && Graph.getEntryNode()->isVirtual()));

            auto [Next, NextLabel] = *Current->successor_edges().begin();
            revng_assert(NextLabel->IsBackwards == false);
            Result.emplace_back(Maker.make(Current, Next, NextLabel->Pointer));
          }
        }
      }
    }
  }

  // Move the graph out of an input parameter so that it gets deleted at
  // the end of the scope of this function.
  auto GraphOnLocalStack = std::move(Graph);

  return Result;
}

template<yield::sugiyama::RankingStrategy RS>
static bool calculateSugiyamaLayout(ExternalGraph &Graph,
                                    const Configuration &Configuration) {
  static_assert(IsMutableEdgeNode<InternalNode>,
                "LayouterSugiyama requires mutable edge nodes.");

  // Ready the graph for the layouter: this converts `Graph` into
  // an internal graph and guaranties that it's has no loops (some of the
  // edges might have to be temporarily inverted to ensure this), a single
  // entry point (an extra node might have to be added) and that both
  // long edges and backwards facing edges are split up into into chunks
  // that span at most one layer at a time.
  auto [DAG, Ranks, Classified] = readyTheGraph<RS>(Graph);

  // Select an optimal node permutation per layer.
  // \note: since this is the part with the highest complexity, it needs extra
  // care for the layouter to perform well.
  // \suggestion: Maybe we should consider something more optimal instead of
  // a simple hill climbing algorithm.
  auto Layers = selectPermutation<RS>(DAG, Ranks, Classified);

  // Compute an augmented topological ordering of the nodes of the graph.
  auto Order = toAugmentedTopologicalOrder(DAG, Layers);

  // Decide on which segments of the graph can be made linear, e.g. each edge
  // within the same linear segment is a straight line.
  auto LinearSegments = selectLinearSegments(DAG, Ranks, Layers, Order);

  // Finalize the logical positions for each of the nodes.
  const auto FinalLayout = toLayout(Layers);

  // Set half the node coordinates.
  const auto &Margin = Configuration.NodeMarginSize;
  setHorizontalCoordinates(Layers, Order, LinearSegments, FinalLayout, Margin);

  // Decide the lanes used for edge routing in a way to prevent the crossings.
  auto Lanes = assignLanes(DAG, LinearSegments, FinalLayout);

  // Set the rest of the coordinates. Node layouting is complete after this.
  const auto &EdgeGap = Configuration.EdgeMarginSize;
  setVerticalCoordinates(Layers, Lanes, Margin, EdgeGap);

  // Route edges forming backwards facing corners.
  auto Prerouted = routeBackwardsCorners(DAG, Ranks, Lanes, Margin, EdgeGap);

  // Now that the corners are routed, the DAG representation is not needed
  // anymore, both the graph and the routed corners get consumed to construct
  // an ordered list of edges with all the information necessary for them
  // to get routed (see `OrderedEdgeContainer`).
  auto Edges = orderEdges(std::move(DAG), std::move(Prerouted), Ranks, Lanes);

  return true;
}

bool yield::sugiyama::layout(Graph &Graph, const Configuration &Configuration) {
  using RS = yield::sugiyama::RankingStrategy;

  switch (Configuration.Ranking) {
  case RS::BreadthFirstSearch:
    return calculateSugiyamaLayout<RS::BreadthFirstSearch>(Graph,
                                                           Configuration);
  case RS::DepthFirstSearch:
    return calculateSugiyamaLayout<RS::DepthFirstSearch>(Graph, Configuration);
  case RS::Topological:
    return calculateSugiyamaLayout<RS::Topological>(Graph, Configuration);
  case RS::DisjointDepthFirstSearch:
    return calculateSugiyamaLayout<RS::DisjointDepthFirstSearch>(Graph,
                                                                 Configuration);
  default:
    revng_abort("Unknown ranking strategy");
  }
}
