/// \file SVG.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/FormatVariadic.h"

#include "revng/Model/Binary.h"
#include "revng/Yield/CallGraph.h"
#include "revng/Yield/ControlFlow/Configuration.h"
#include "revng/Yield/ControlFlow/Extraction.h"
#include "revng/Yield/ControlFlow/NodeSizeCalculation.h"
#include "revng/Yield/Graph.h"
#include "revng/Yield/HTML.h"
#include "revng/Yield/SVG.h"
#include "revng/Yield/Support/SugiyamaStyleGraphLayout.h"

namespace tags {

static constexpr auto UnconditionalEdge = "unconditional";
static constexpr auto CallEdge = "call";
static constexpr auto TakenEdge = "taken";
static constexpr auto RefusedEdge = "refused";

static constexpr auto NodeBody = "node-body";
static constexpr auto NodeContents = "node-contents";

static constexpr auto UnconditionalArrowHead = "unconditional-arrow-head";
static constexpr auto CallArrowHead = "call-arrow-head";
static constexpr auto TakenArrowHead = "taken-arrow-head";
static constexpr auto RefusedArrowHead = "refused-arrow-head";

} // namespace tags

namespace templates {

constexpr auto *Point = "M {0} {1} ";
constexpr auto *Line = "L {0} {1} ";
constexpr auto *BezierCubic = "C {0} {1} {2} {3} {4} {5} ";

constexpr auto *Edge = R"Edge(<path
  class='{0}-edge'
  {1}
  marker-end="url(#{0}-arrow-head)"
  fill="none" />
)Edge";

constexpr auto *Rect = R"(<rect
  class="{0}"
  x="{1}"
  y="{2}"
  rx="{3}"
  ry="{3}"
  width="{4}"
  height="{5}" />)";

constexpr auto *ForeignObject = R"(<foreignObject
  class="{0}"
  x="{1}"
  y="{2}"
  width="{3}"
  height="{4}"><body
  xmlns="http://www.w3.org/1999/xhtml">{5}</body></foreignObject>
)";

static constexpr auto ArrowHead = R"(<marker
  id="{0}"
  markerWidth="{1}"
  markerHeight="{1}"
  refX="{5}"
  refY="{2}"
  orient="{6}"><polygon
  points="{4}, {1} {3}, {2} {4}, {4} {1}, {2}" /></marker>
)";

static constexpr auto Graph = R"(<svg
  xmlns="http://www.w3.org/2000/svg"
  viewbox="{0} {1} {2} {3}"
  width="{2}"
  height="{3}"><defs>{4}</defs>{5}</svg>
)";

} // namespace templates

static std::string_view edgeTypeAsString(yield::Graph::EdgeType Type) {
  switch (Type) {
  case yield::Graph::EdgeType::Unconditional:
    return tags::UnconditionalEdge;
  case yield::Graph::EdgeType::Call:
    return tags::CallEdge;
  case yield::Graph::EdgeType::Taken:
    return tags::TakenEdge;
  case yield::Graph::EdgeType::Refused:
    return tags::RefusedEdge;
  default:
    revng_abort("Unknown edge type");
  }
}

static std::string
cubicBend(const yield::Graph::Point &From, const yield::Graph::Point &To) {
  static constexpr yield::Graph::Coordinate BendFactor = 0.8;

  yield::Graph::Coordinate XDistance = To.X - From.X;
  return llvm::formatv(templates::BezierCubic,
                       From.X + XDistance * BendFactor,
                       -From.Y,
                       To.X - XDistance * BendFactor,
                       -To.Y,
                       To.X,
                       -To.Y);
}

static std::string edge(const std::vector<yield::Graph::Point> &Path,
                        const yield::Graph::EdgeType &Type,
                        bool UseOrthogonalBends) {
  std::string Points = "d=\"";

  revng_assert(!Path.empty());
  const auto &First = Path.front();
  Points += llvm::formatv(templates::Point, First.X, -First.Y);

  if (UseOrthogonalBends) {
    for (size_t Index = 1; Index < Path.size(); ++Index)
      Points += llvm::formatv(templates::Line, Path[Index].X, -Path[Index].Y);
  } else {
    revng_assert(Path.size() >= 2);
    for (auto Iter = Path.begin(); Iter != std::prev(Path.end()); ++Iter)
      Points += cubicBend(*Iter, *std::next(Iter));
  }

  revng_assert(!Points.empty());
  revng_assert(Points.back() == ' ');
  Points.pop_back(); // Remove an extra space at the end.
  Points += '"';

  return llvm::formatv(templates::Edge, edgeTypeAsString(Type), Points);
}

static std::string content(const yield::Node *Node,
                           const yield::Function &Function,
                           const model::Binary &Binary) {
  revng_assert(Node != nullptr);

  if (Node->Address.isValid()) {
    // A normal node.
    return yield::html::controlFlowNode(Node->Address, Function, Binary);
  } else {
    // The entry/exit/error node.
    return "";
  }
}

static std::string node(const yield::Node *Node,
                        std::string &&Content,
                        const yield::cfg::Configuration &Configuration) {
  yield::Graph::Size HalfSize{ Node->Size.W / 2, Node->Size.H / 2 };
  yield::Graph::Point TopLeft{ Node->Center.X - HalfSize.W,
                               -Node->Center.Y - HalfSize.H };

  auto Text = llvm::formatv(templates::ForeignObject,
                            tags::NodeContents,
                            TopLeft.X,
                            TopLeft.Y,
                            Node->Size.W,
                            Node->Size.H,
                            std::move(Content));

  auto Body = llvm::formatv(templates::Rect,
                            tags::NodeBody,
                            TopLeft.X,
                            TopLeft.Y,
                            Configuration.NodeCornerRoundingFactor,
                            Node->Size.W,
                            Node->Size.H);

  return llvm::formatv("{0}", std::move(Text) + std::move(Body));
}

struct Viewbox {
  yield::Graph::Point TopLeft = { -1, -1 };
  yield::Graph::Point BottomRight = { +1, +1 };
};

static Viewbox makeViewbox(const yield::Node *Node) {
  yield::Graph::Size HalfSize{ Node->Size.W / 2, Node->Size.H / 2 };
  yield::Graph::Point TopLeft{ Node->Center.X - HalfSize.W,
                               -Node->Center.Y - HalfSize.H };
  yield::Graph::Point BottomRight{ Node->Center.X + HalfSize.W,
                                   -Node->Center.Y + HalfSize.H };
  return Viewbox{ .TopLeft = std::move(TopLeft),
                  .BottomRight = std::move(BottomRight) };
}

static void expandViewbox(Viewbox &LHS, const Viewbox &RHS) {
  if (RHS.TopLeft.X < LHS.TopLeft.X)
    LHS.TopLeft.X = RHS.TopLeft.X;
  if (RHS.TopLeft.Y < LHS.TopLeft.Y)
    LHS.TopLeft.Y = RHS.TopLeft.Y;
  if (RHS.BottomRight.X > LHS.BottomRight.X)
    LHS.BottomRight.X = RHS.BottomRight.X;
  if (RHS.BottomRight.Y > LHS.BottomRight.Y)
    LHS.BottomRight.Y = RHS.BottomRight.Y;
}

static void expandViewbox(Viewbox &Box, const yield::Graph::Point &Point) {
  if (Box.TopLeft.X > Point.X)
    Box.TopLeft.X = Point.X;
  if (Box.TopLeft.Y > -Point.Y)
    Box.TopLeft.Y = -Point.Y;
  if (Box.BottomRight.X < Point.X)
    Box.BottomRight.X = Point.X;
  if (Box.BottomRight.Y < -Point.Y)
    Box.BottomRight.Y = -Point.Y;
}

static Viewbox calculateViewbox(const yield::Graph &Graph) {
  revng_assert(Graph.size() != 0);
  revng_assert(Graph.getEntryNode() != nullptr);

  // Ensure every node fits.
  Viewbox Result = makeViewbox(Graph.getEntryNode());
  for (const auto *Node : Graph.nodes())
    expandViewbox(Result, makeViewbox(Node));

  // Ensure every edge point fits.
  for (const auto *From : Graph.nodes())
    for (const auto [To, Label] : From->successor_edges())
      for (const auto &Point : Label->Path)
        expandViewbox(Result, Point);

  // Add some extra padding for a good measure.
  Result.TopLeft.X -= 50;
  Result.TopLeft.Y -= 50;
  Result.BottomRight.X += 50;
  Result.BottomRight.Y += 50;

  return Result;
}

static std::string
arrowHead(llvm::StringRef Name, float Size, float Dip, float Shift = 0) {
  return llvm::formatv(templates::ArrowHead,
                       Name,
                       std::to_string(Size),
                       std::to_string(Size / 2),
                       std::to_string(Dip),
                       "0",
                       std::to_string(Size - Shift),
                       "auto");
}

static std::string
duplicateArrowHeadsImpl(float Size, float Dip, float Shift = 0) {
  return arrowHead(tags::UnconditionalArrowHead, Size, Dip, Shift)
         + arrowHead(tags::CallArrowHead, Size, Dip, Shift)
         + arrowHead(tags::TakenArrowHead, Size, Dip, Shift)
         + arrowHead(tags::RefusedArrowHead, Size, Dip, Shift);
}

static std::string
defaultArrowHeads(const yield::cfg::Configuration &Configuration) {
  if (Configuration.UseOrthogonalBends == true)
    return duplicateArrowHeadsImpl(8, 3, 0);
  else
    return duplicateArrowHeadsImpl(8, 3, 2);
}

static std::string exportCFG(const yield::Graph &Graph,
                             const yield::Function &Function,
                             const model::Binary &Binary,
                             const yield::cfg::Configuration &Configuration) {
  std::string Result;

  // Export all the edges.
  for (const auto *From : Graph.nodes()) {
    for (const auto [To, Edge] : From->successor_edges()) {
      revng_assert(Edge != nullptr);
      revng_assert(Edge->Status != yield::Graph::EdgeStatus::Unrouted);
      Result += edge(Edge->Path, Edge->Type, Configuration.UseOrthogonalBends);
    }
  }

  // Export all the nodes.
  for (const auto *Node : Graph.nodes())
    Result += node(Node, content(Node, Function, Binary), Configuration);

  Viewbox Box = calculateViewbox(Graph);
  return llvm::formatv(templates::Graph,
                       Box.TopLeft.X,
                       Box.TopLeft.Y,
                       Box.BottomRight.X - Box.TopLeft.X,
                       Box.BottomRight.Y - Box.TopLeft.Y,
                       defaultArrowHeads(Configuration),
                       std::move(Result));
}

std::string yield::svg::controlFlow(const yield::Function &InternalFunction,
                                    const model::Binary &Binary) {
  constexpr auto Configuration = cfg::Configuration::getDefault();

  yield::Graph ControlFlowGraph = cfg::extractFromInternal(InternalFunction,
                                                           Binary,
                                                           Configuration);

  cfg::calculateNodeSizes(ControlFlowGraph,
                          InternalFunction,
                          Binary,
                          Configuration);

  sugiyama::layout(ControlFlowGraph, Configuration);

  return exportCFG(ControlFlowGraph, InternalFunction, Binary, Configuration);
}

static std::string
exportCallGraph(const yield::Graph &Graph,
                const model::Binary &Binary,
                const yield::cfg::Configuration &Configuration) {
  std::string Result;

  // Export all the edges.
  for (const auto *From : Graph.nodes()) {
    if (From->Address.isValid()) {
      for (const auto [To, Edge] : From->successor_edges()) {
        if (To->Address.isValid()) {
          revng_assert(Edge != nullptr);
          revng_assert(Edge->Status != yield::Graph::EdgeStatus::Unrouted);
          Result += edge(Edge->Path,
                         Edge->Type,
                         Configuration.UseOrthogonalBends);
        }
      }
    }
  }

  // Export all the nodes.
  for (const auto *Node : Graph.nodes())
    if (Node->Address.isValid())
      Result += node(Node,
                     yield::html::functionLink(Node->Address, Binary),
                     Configuration);

  Viewbox Box = calculateViewbox(Graph);
  return llvm::formatv(templates::Graph,
                       Box.TopLeft.X,
                       Box.TopLeft.Y,
                       Box.BottomRight.X - Box.TopLeft.X,
                       Box.BottomRight.Y - Box.TopLeft.Y,
                       defaultArrowHeads(Configuration),
                       std::move(Result));
}

static void
callGraphNodeSizeHelper(yield::Graph &InternalGraph,
                        const model::Binary &Binary,
                        const yield::cfg::Configuration &Configuration) {
  for (auto *Node : InternalGraph.nodes()) {
    if (Node->Address.isValid()) {
      // A normal node
      auto FunctionIterator = Binary.Functions.find(Node->Address);
      revng_assert(FunctionIterator != Binary.Functions.end());

      size_t NameLength = FunctionIterator->name().size();
      revng_assert(NameLength != 0);

      Node->Size = yield::Graph::Size{ 1 * Configuration.LabelFontSize
                                         * Configuration.VerticalFontFactor,
                                       NameLength * Configuration.LabelFontSize
                                         * Configuration.HorizontalFontFactor };
    } else {
      // An entry node.
      Node->Size = yield::Graph::Size{ 30, 30 };
    }

    Node->Size.W += Configuration.InternalNodeMarginSize * 2;
    Node->Size.H += Configuration.InternalNodeMarginSize * 2;
  }

  // TODO: we might want to consider equalizing node widths.
}

static void rotateTheGraph(yield::Graph &InternalGraph) {
  for (auto *Node : InternalGraph.nodes()) {
    std::swap(Node->Size.W, Node->Size.H);
    std::swap(Node->Center.X, Node->Center.Y = -Node->Center.Y);

    for (auto [_, Edge] : Node->successor_edges())
      for (auto &[X, Y] : Edge->Path)
        std::swap(X, Y = -Y);
  }
}

constexpr yield::cfg::Configuration defaultCallGraphConfiguration() {
  auto Result = yield::cfg::Configuration::getDefault();

  Result.UseOrthogonalBends = true;
  Result.ExternalNodeMarginSize = 20.f;
  Result.EdgeMarginSize = 15.f;
  Result.PreserveLinearSegments = true;
  // Result.VirtualNodeWeight = 1.f;

  return Result;
}

std::string yield::svg::calls(const yield::CallGraph &CallGraph,
                              const model::Binary &Binary) {
  constexpr auto Configuration = defaultCallGraphConfiguration();

  yield::Graph InternalGraph = CallGraph.toYieldGraph();
  callGraphNodeSizeHelper(InternalGraph, Binary, Configuration);

  using RankingStrategy = sugiyama::RankingStrategy;
  sugiyama::layout(InternalGraph,
                   Configuration,
                   RankingStrategy::BreadthFirstSearch);

  rotateTheGraph(InternalGraph);
  return exportCallGraph(InternalGraph, Binary, Configuration);
}

static auto flipPoint(yield::Graph::Point const &Point) {
  return yield::Graph::Point{ Point.X, -Point.Y };
};
static auto
calculateDelta(yield::Graph::Point const &LHS, yield::Graph::Point const &RHS) {
  return yield::Graph::Point{ RHS.X - LHS.X, RHS.Y - LHS.Y };
}
static auto translatePoint(yield::Graph::Point const &Point,
                           yield::Graph::Point const &Delta) {
  return yield::Graph::Point{ Point.X + Delta.X, Point.Y + Delta.Y };
}
static auto convertPoint(yield::Graph::Point const &Point,
                         yield::Graph::Point const &Delta) {
  return translatePoint(flipPoint(Point), Delta);
}

static yield::Graph combineHalvesHelper(const MetaAddress &SlicePoint,
                                        yield::Graph &&ForwardsSlice,
                                        yield::Graph &&BackwardsSlice) {
  revng_assert(ForwardsSlice.size() != 0 && BackwardsSlice.size() != 0);

  auto IsSlicePoint = [&SlicePoint](const auto *Node) {
    return Node->Address == SlicePoint;
  };

  auto ForwardsIterator = llvm::find_if(ForwardsSlice.nodes(), IsSlicePoint);
  revng_assert(ForwardsIterator != ForwardsSlice.nodes().end());
  auto *ForwardsSlicePoint = *ForwardsIterator;
  revng_assert(ForwardsSlicePoint != nullptr);

  auto BackwardsIterator = llvm::find_if(BackwardsSlice.nodes(), IsSlicePoint);
  revng_assert(BackwardsIterator != BackwardsSlice.nodes().end());
  auto *BackwardsSlicePoint = *BackwardsIterator;
  revng_assert(BackwardsSlicePoint != nullptr);

  // Find the distance all the nodes of one of the graphs need to be shifted so
  // that the `SlicePoint`s overlap.
  auto Delta = calculateDelta(flipPoint((*BackwardsIterator)->Center),
                              (*ForwardsIterator)->Center);

  // Ready the backwards part of the graph
  for (auto *From : BackwardsSlice.nodes()) {
    From->Center = convertPoint(From->Center, Delta);
    for (auto [Neighbor, Label] : From->successor_edges())
      for (auto &Point : Label->Path)
        Point = convertPoint(Point, Delta);
  }

  // Define a map for faster node lookup.
  llvm::DenseMap<yield::Graph::Node *, yield::Graph::Node *> Lookup;
  auto AccessLookup = [&Lookup](yield::Graph::Node *Key) {
    auto Iterator = Lookup.find(Key);
    revng_assert(Iterator != Lookup.end() && Iterator->second != nullptr);
    return Iterator->second;
  };

  // Move the nodes from the backwards slice into the forwards one.
  for (auto *Node : BackwardsSlice.nodes()) {
    revng_assert(Node != nullptr);
    if (Node != BackwardsSlicePoint) {
      auto NewNode = ForwardsSlice.addNode(Node->moveData());
      auto [Iterator, Success] = Lookup.try_emplace(Node, NewNode);
      revng_assert(Success == true);
    } else {
      auto [Iterator, Success] = Lookup.try_emplace(BackwardsSlicePoint,
                                                    ForwardsSlicePoint);
      revng_assert(Success == true);
    }
  }

  // Move all the edges while also inverting their direction.
  for (auto *From : BackwardsSlice.nodes()) {
    for (auto [To, Label] : From->successor_edges()) {
      std::reverse(Label->Path.begin(), Label->Path.end());
      AccessLookup(To)->addSuccessor(AccessLookup(From), std::move(*Label));
    }
  }

  return std::move(ForwardsSlice);
}

std::string yield::svg::callsSlice(const MetaAddress &SlicePoint,
                                   const yield::CallGraph &Graph,
                                   const model::Binary &Binary) {
  constexpr auto Configuration = defaultCallGraphConfiguration();

  // Ready the forwards facing part of the slice.
  auto ForwardsSlice = Graph.forwardsSlice(SlicePoint).toYieldGraph();
  for (auto *From : ForwardsSlice.nodes())
    for (auto [To, Label] : From->successor_edges())
      Label->Type = yield::Graph::EdgeType::Taken;
  callGraphNodeSizeHelper(ForwardsSlice, Binary, Configuration);
  using RankingStrategy = sugiyama::RankingStrategy;
  
  rotateTheGraph(ForwardsSlice);
  sugiyama::layout(ForwardsSlice,
                   Configuration,
                   RankingStrategy::BreadthFirstSearch);

  // Ready the backwards facing part of the slice.
  auto BackwardsSlice = Graph.backwardsSlice(SlicePoint).toYieldGraph();
  for (auto *From : BackwardsSlice.nodes())
    for (auto [To, Label] : From->successor_edges())
      Label->Type = yield::Graph::EdgeType::Refused;
  callGraphNodeSizeHelper(BackwardsSlice, Binary, Configuration);
  using RankingStrategy = sugiyama::RankingStrategy;
  
  rotateTheGraph(BackwardsSlice);
  sugiyama::layout(BackwardsSlice,
                   Configuration,
                   RankingStrategy::BreadthFirstSearch);

  // Consume the halves to produce a combined graph and export it.
  auto Combined = combineHalvesHelper(SlicePoint,
                                      std::move(ForwardsSlice),
                                      std::move(BackwardsSlice));
  // rotateTheGraph(Combined);
  return exportCallGraph(Combined, Binary, Configuration);
}
