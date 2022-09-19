#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Yield/ControlFlow/Configuration.h"
#include "revng/Yield/Graph.h"

namespace yield::sugiyama {

/// Lists possible ranking strategies the layouter implements.
enum class RankingStrategy {
  BreadthFirstSearch,
  DepthFirstSearch,
  Topological,
  DisjointDepthFirstSearch
};

/// List graph orientation options the layouter implements.
enum class LayoutOrientation {
  LeftToRight,
  RightToLeft,
  TopToBottom,
  BottomToTop
};

struct Configuration {
public:
  /// Specifies the way the layers of the graph are decided
  RankingStrategy Ranking;

  /// Specifies the orientation of the layout, (e.g. left-to-right):
  /// The layers with lower ranks are closer to the first direction (e.g. left)
  /// while those with higher ranks - to the second one (e.g. right).
  LayoutOrientation Orientation;

  /// Specifies whether orthogonal bending rules should be used, if not each
  /// edge is just two points: its start and end.
  bool UseOrthogonalBends;

  /// Specifies whether linear segment preservation should be done.
  ///
  /// This causes nodes any pair of nodes (A and B) such that A has only one
  /// successor - B and B has only one predecessor - A to be always be routed
  /// with a straight line (they both have the same horizontal position).
  bool PreserveLinearSegments;

  /// Specifies the "weight" of virtual nodes as opposed to the real ones.
  ///
  /// Virtual nodes represent long edges (both forwards and backwards facing).
  ///
  /// If weight is equal to 1, the placing of virtual nodes is identical to that
  /// of the real ones.
  ///
  /// If weight is bigger than 1 (e.g. 10), the virtual nodes are more likely
  /// to be placed to the right of the layout (the bigger the weight, the more
  /// noticeable its effect is).
  ///
  /// If weight is smaller than 1 (e.g. 0.1), the virtual nodes are more likely
  /// to be placed to the left of the layout (the smaller the weight, the more
  /// noticeable its effect is).
  ///
  /// \note: the layouts will be unnaturally skewed if this value is negative.
  float VirtualNodeWeight;

  /// Specifies the minimum possible distance between two nodes.
  Graph::Dimension NodeMarginSize;

  /// Specifies the minimum possible distance between two edges.
  Graph::Dimension EdgeMarginSize;
};

/// A custom graph layering algorithm designed for pre-calculating majority of
/// the expensive stuff needed for graph rendering.
bool layout(Graph &Graph, const Configuration &Configuration);

inline bool
layout(Graph &Graph,
       const cfg::Configuration &CFG,
       RankingStrategy Ranking = RankingStrategy::DisjointDepthFirstSearch,
       LayoutOrientation Orientation = LayoutOrientation::TopToBottom) {
  return layout(Graph,
                Configuration{
                  .Ranking = Ranking,
                  .Orientation = Orientation,
                  .UseOrthogonalBends = CFG.UseOrthogonalBends,
                  .PreserveLinearSegments = CFG.PreserveLinearSegments,
                  .VirtualNodeWeight = CFG.VirtualNodeWeight,
                  .NodeMarginSize = CFG.ExternalNodeMarginSize,
                  .EdgeMarginSize = CFG.EdgeMarginSize });
}

} // namespace yield::sugiyama
