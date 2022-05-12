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
