#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <unordered_map>
#include <variant>

#include "revng/ADT/GenericGraph.h"
#include "revng/Yield/Support/GraphLayout/Traits.h"

namespace yield::layout::sugiyama {

class InternalGraph;

namespace detail {

struct InternalNode {
  layout::Point Center;
  layout::Size Size;

  bool IsVirtual;

private:
  std::size_t Index;

public:
  InternalNode(std::size_t Index, layout::Size &&Size) :
    Center({ 0, 0 }), Size(std::move(Size)), IsVirtual(false), Index(Index) {}
  InternalNode(std::size_t Index) :
    Center({ 0, 0 }), Size({ 0, 0 }), IsVirtual(true), Index(Index) {}

  ~InternalNode() = default;
  InternalNode(const InternalNode &) = delete;
  InternalNode(InternalNode &&) = default;
  InternalNode &operator=(const InternalNode &) = delete;
  InternalNode &operator=(InternalNode &&) = default;

  std::size_t index() const { return Index; }
};

/// Represents an edge in the internal graph.
///
/// Because the layouter heavily relies on splitting edges into pieces
/// (partitioning them, see `GraphPreparation.cpp` for more details), we need
/// a way to let multiple paths accumulate into a single path that can
/// eventually be exported for the original edge.
///
/// To achieve this, each edge holds EITHER a real path (`PathOwner`) OR just
/// a reference to a path held by some other edge (`PathReference`).
///
/// To limit the opportunities for breaking stuff by using this non-intuitive
/// system incorrectly, access to the path is heavily limited to just two
/// methods:
/// - `appendPoint` - adds a point to the current path without regard of whether
///                   the path is owned by this edge, or some other one.
/// - `getPath`     - returns the path reference (which allows it to be moved
///                   out), but is only defined for the edge owners. This
///                   guarantees that each path can only be returned at most
///                   once (the only accessor asserts it).
///
/// There's also a `isVirtual` helper for determining whether the path is owned
/// or not. The expected way to extract the edge paths is
/// ```cpp
/// for (auto &Edge : allTheEdgesOfMyGraph())
///   if (Edge.isVirtual())
///     acceptThePath(std::move(Edge.getPath()));
/// ```
struct InternalEdge {
  bool IsRouted;
  bool IsBackwards;

private:
  std::size_t Index;

  using PathOwner = std::unique_ptr<layout::Path>;
  using PathReference = layout::Path *;
  std::variant<PathOwner, PathReference> Path;

public:
  /// Make a real edge.
  explicit InternalEdge(std::size_t Index) :
    IsRouted(false),
    IsBackwards(false),
    Index(Index),
    Path(std::make_unique<layout::Path>()) {}

  /// Make a virtual edge pointing to a path in another edge.
  InternalEdge(std::size_t Index, InternalEdge &Another, bool IsBackwards) :
    IsRouted(Another.IsRouted),
    IsBackwards(IsBackwards),
    Index(Index),
    Path(&Another.getPathImpl()) {}

  ~InternalEdge() = default;
  InternalEdge(const InternalEdge &) = delete;
  InternalEdge(InternalEdge &&) = default;
  InternalEdge &operator=(const InternalEdge &) = delete;
  InternalEdge &operator=(InternalEdge &&) = default;

  std::size_t index() const { return Index; }
  bool isVirtual() const { return std::holds_alternative<PathOwner>(Path); }

  /// Adds a point to the edge path or modifies its last point if that's
  /// sufficient, based on their coordinates.
  void appendPoint(const layout::Point &Point) {
    appendPointImpl(getPathImpl(), Point);
  }
  void appendPoint(layout::Coordinate X, layout::Coordinate Y) {
    return appendPoint(layout::Point(X, Y));
  }

  layout::Path &getPath() {
    revng_assert(!isVirtual(), "`getPath` should only be used on real edges.");
    return getPathImpl();
  }

private:
  layout::Path &getPathImpl() {
    auto Visitor = []<typename Type>(Type &Value) -> PathReference {
      if constexpr (std::is_same<std::decay_t<Type>, PathOwner>::value)
        return Value.get();
      else if constexpr (std::is_same<std::decay_t<Type>, PathReference>::value)
        return Value;
      else
        static_assert(type_always_false<Type>::value, "Unknown variant type");
    };
    return *std::visit(Visitor, Path);
  }

  static void appendPointImpl(layout::Path &Path, const layout::Point &Point) {
    if (Path.size() > 1) {
      auto &First = *std::prev(std::prev(Path.end()));
      auto &Second = *std::prev(Path.end());

      auto LHS = (Point.Y - Second.Y) * (Second.X - First.X);
      auto RHS = (Second.Y - First.Y) * (Point.X - Second.X);
      if (LHS == RHS)
        Path.pop_back();
    }
    Path.push_back(Point);
  }
};

} // namespace detail

using InternalNode = MutableEdgeNode<detail::InternalNode,
                                     detail::InternalEdge,
                                     false>;

namespace detail {

/// The result type of the \ref make method containing the newly-built
/// graph and the lookup table necessary for the results of the layouter to be
/// merged back into the original graph.
template<typename NodeRef, typename EdgeRef>
struct InternalGraphWithLookupTable;

} // namespace detail

class InternalGraph : public GenericGraph<InternalNode, 16, true> {
public:
  using GenericGraph<InternalNode, 16, true>::GenericGraph;

private:
  std::size_t NodeIndexCounter = 0;
  std::size_t EdgeIndexCounter = 0;

public:
  template<typename NodeRef, typename EdgeRef>
  struct OutputLookups {
    std::vector<NodeRef> Nodes;
    std::vector<EdgeRef> Edges;
  };

public:
  /// Split the input graph into two pieces: an internal graph the layouter
  /// requires and a lookup table allowing the results to be exported back
  /// into the original graph (see \ref exportInto).
  /// @tparam GraphType The type of the graph to split. It has to provide
  ///         `yield::layout::LayoutableGraphTraits`.
  /// @param Graph An input graph.
  /// @return A pair of the output graph and the lookup table
  ///         (see \ref InternalGraphPair)
  template<layout::HasLayoutableGraphTraits GraphType>
  static auto make(const GraphType &Graph) {
    using LLVMTrait = llvm::GraphTraits<GraphType>;
    using LayoutTrait = layout::LayoutableGraphTraits<GraphType>;

    using NodeRef = typename LLVMTrait::NodeRef;
    using EdgeRef = typename LLVMTrait::EdgeRef;

    InternalGraph Result;
    OutputLookups<NodeRef, EdgeRef> Lookup;
    Lookup.Nodes.reserve(LLVMTrait::size(Graph));

    std::unordered_map<NodeRef, InternalGraph::Node *> InternalLookup;
    for (NodeRef Node : llvm::nodes(Graph)) {
      layout::Size Size = LayoutTrait::getNodeSize(Node);

      auto [It, Success] = InternalLookup.try_emplace(Node,
                                                      Result.makeNode(Size));
      revng_assert(Success);
      revng_assert(It->second->index() == Lookup.Nodes.size());
      Lookup.Nodes.emplace_back(Node);
    }

    for (NodeRef From : llvm::nodes(Graph)) {
      for (EdgeRef Edge : llvm::children_edges<GraphType>(From)) {
        const auto &NewEdge = Result.makeEdge(InternalLookup.at(From),
                                              InternalLookup.at(Edge.Neighbor));
        revng_assert(NewEdge->index() == Lookup.Edges.size());
        Lookup.Edges.emplace_back(Edge);
      }
    }

    using ResultType = detail::InternalGraphWithLookupTable<NodeRef, EdgeRef>;
    return ResultType{ .Graph = std::move(Result),
                       .Lookup = std::move(Lookup) };
  }

  /// Uses lookup table produced when \ref make 'ing the graph to export
  /// the results of the layouter back into it.
  ///
  /// @param Lookup The table produced by the \ref method when making this graph
  ///
  /// @tparam NodeR The node type of the original graph. It is automatically
  ///         deduced from the \ref Lookup.
  /// @tparam EdgeR The edge type of the original graph. It is automatically
  ///         deduced from the \ref Lookup.
  /// @tparam GraphType The output graph type.
  ///
  /// \note All the virtual nodes are ignored, so for a node/edge to be
  ///       exported, it has to have been present in the original graph passed
  ///       into \ref make.
  template<layout::HasLayoutableGraphTraits GraphType,
           typename NodeR = typename llvm::GraphTraits<GraphType>::NodeRef,
           typename EdgeR = typename llvm::GraphTraits<GraphType>::EdgeRef>
  void exportInto(const OutputLookups<NodeR, EdgeR> &Lookup) {
    using LLVMTrait = llvm::GraphTraits<GraphType>;
    using LayoutTrait = layout::LayoutableGraphTraits<GraphType>;

    using NodeRef = typename LLVMTrait::NodeRef;
    using EdgeRef = typename LLVMTrait::EdgeRef;
    static_assert(std::is_same<NodeRef, NodeR>::value);
    static_assert(std::is_same<EdgeRef, EdgeR>::value);

    for (InternalNode *Node : nodes())
      if (!Node->IsVirtual)
        LayoutTrait::setNodePosition(Lookup.Nodes[Node->index()],
                                     std::move(Node->Center));

    for (InternalNode *From : nodes()) {
      for (auto [To, Label] : From->successor_edges()) {
        revng_assert(Label->IsRouted == true);

        if (!Label->isVirtual())
          LayoutTrait::setEdgePath(Lookup.Edges[Label->index()],
                                   std::move(Label->getPath()));
      }
    }
  }

public:
  Node *makeNode(layout::Size Size, bool IsVirtual = false) {
    return addNode(detail::InternalNode{ NodeIndexCounter++, std::move(Size) });
  }
  Node *makeVirtualNode() {
    return addNode(detail::InternalNode{ NodeIndexCounter++ });
  }
  Node::Edge *makeEdge(Node *From, Node *To) {
    return From->addSuccessor(To, Node::Edge(EdgeIndexCounter++)).Label;
  }
  Node::Edge makeVirtualEdge(Node::Edge &Edge, bool IsBackwards) {
    return Node::Edge(EdgeIndexCounter++, Edge, IsBackwards);
  }
};

namespace detail {

template<typename NodeRef, typename EdgeRef>
struct InternalGraphWithLookupTable {
  InternalGraph Graph;
  const InternalGraph::OutputLookups<NodeRef, EdgeRef> Lookup;
};

} // namespace detail.

} // namespace yield::layout::sugiyama
