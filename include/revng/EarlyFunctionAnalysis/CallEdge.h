#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/ADT/MutableSet.h"
#include "revng/EarlyFunctionAnalysis/FunctionEdgeBase.h"
#include "revng/Model/Binary.h"
#include "revng/Model/FunctionAttribute.h"
#include "revng/Model/VerifyHelper.h"

/* TUPLE-TREE-YAML
name: CallEdge
doc: A CFG edge to represent function calls (direct, indirect and tail calls)
type: struct
inherits: FunctionEdgeBase
fields:
  - name: DynamicFunction
    doc: |
      Name of the dynamic function being called, or empty if not a dynamic call
    type: "std::string"
    optional: true
  - name: Attributes
    doc: |
      Attributes for this function

      Note: To have the effective list of attributes for this call site, you
      have to add attributes on the called function.
      TODO: switch to std::set
    sequence:
      type: MutableSet
      elementType: model::FunctionAttribute::Values
    optional: true
TUPLE-TREE-YAML */

#include "revng/EarlyFunctionAnalysis/Generated/Early/CallEdge.h"

class efa::CallEdge : public efa::generated::CallEdge {
private:
  static constexpr const FunctionEdgeType::Values
    AssociatedType = FunctionEdgeType::FunctionCall;

public:
  CallEdge() : efa::generated::CallEdge() { Type = AssociatedType; }

  CallEdge(MetaAddress Destination, FunctionEdgeType::Values Type) :
    efa::generated::CallEdge(Destination, Type) {}

public:
  static bool classof(const FunctionEdgeBase *A) { return classof(A->key()); }
  static bool classof(const Key &K) {
    return FunctionEdgeType::isCall(std::get<1>(K));
  }

public:
  bool verify() const debug_function;
  bool verify(bool Assert) const debug_function;
  bool verify(model::VerifyHelper &VH) const;
  void dump() const debug_function;
};

inline model::TypePath getPrototype(const model::Binary &Binary,
                                    MetaAddress FunctionAddress,
                                    MetaAddress CallerBlockAddress,
                                    const efa::CallEdge &Edge) {
  auto It = Binary.Functions.at(FunctionAddress)
              .CallSitePrototypes.find(CallerBlockAddress);
  if (It != Binary.Functions.at(FunctionAddress).CallSitePrototypes.end())
    return It->Prototype;

  if (Edge.Type == efa::FunctionEdgeType::FunctionCall) {
    if (not Edge.DynamicFunction.empty()) {
      // Get the dynamic function prototype
      return Binary.ImportedDynamicFunctions.at(Edge.DynamicFunction).Prototype;
    } else if (Edge.Destination.isValid()) {
      // Get the function prototype
      return Binary.Functions.at(Edge.Destination).Prototype;
    } else {
      revng_abort();
    }
  } else {
    return Binary.DefaultPrototype;
  }
}

inline bool hasAttribute(const model::Binary &Binary,
                         const efa::CallEdge &Edge,
                         model::FunctionAttribute::Values Attribute) {
  if (Edge.Attributes.count(Attribute) != 0)
    return true;

  if (Edge.Type == efa::FunctionEdgeType::FunctionCall) {
    const MutableSet<model::FunctionAttribute::Values>
      *CalleeAttributes = nullptr;
    if (not Edge.DynamicFunction.empty()) {
      const auto &F = Binary.ImportedDynamicFunctions.at(Edge.DynamicFunction);
      CalleeAttributes = &F.Attributes;
    } else if (Edge.Destination.isValid()) {
      CalleeAttributes = &Binary.Functions.at(Edge.Destination).Attributes;
    } else {
      revng_abort();
    }

    return CalleeAttributes->count(Attribute) != 0;
  }

  return false;
}

#include "revng/EarlyFunctionAnalysis/Generated/Late/CallEdge.h"
