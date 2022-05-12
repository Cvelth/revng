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
