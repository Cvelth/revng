#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Pipeline/Kind.h"
#include "revng/Pipeline/RegisterContainerFactory.h"
#include "revng/Pipes/Kinds.h"
#include "revng/Pipes/Ranks.h"
#include "revng/Pipes/StringBufferContainer.h"
#include "revng/Pipes/StringMap.h"

namespace revng::kinds {

inline pipeline::SingleElementKind
  NewModelHeader("model-header",
                 Binary,
                 revng::ranks::Binary,
                 fat(revng::ranks::TypeDefinition,
                     revng::ranks::StructField,
                     revng::ranks::UnionField,
                     revng::ranks::EnumEntry,
                     revng::ranks::DynamicFunction,
                     revng::ranks::Segment,
                     revng::ranks::ArtificialStruct),
                 { &Decompiled });

inline pipeline::SingleElementKind
  NewHelperHeader("helper-header",
                  Binary,
                  revng::ranks::Binary,
                  fat(/* TODO: add location ranks */),
                  {});

} // namespace revng::kinds

namespace detail {

inline constexpr char ModelHeaderName[] = "model-header";
inline constexpr char HelperHeaderName[] = "helper-header";

inline constexpr char HeaderMIMEType[] = "text/x.h+ptml";
inline constexpr char HeaderSuffix[] = ".h";

template<auto... Values>
using SBF = revng::pipes::StringBufferContainer<Values...>;

// The real class is used here because aliasing an alias is not allowed.
namespace RPD = revng::pipes::detail;
template<auto... Values>
using TSM = RPD::GenericStringMap<&revng::ranks::TypeDefinition, Values...>;

template<typename T>
using RegisterDCC = pipeline::RegisterDefaultConstructibleContainer<T>;

} // namespace detail

using ModelHeaderContainer = detail::SBF<&revng::kinds::NewModelHeader,
                                         detail::ModelHeaderName,
                                         detail::HeaderMIMEType,
                                         detail::HeaderSuffix>;
inline detail::RegisterDCC<ModelHeaderContainer> RegisteredMHC;

using HelperHeaderContainer = detail::SBF<&revng::kinds::NewHelperHeader,
                                          detail::HelperHeaderName,
                                          detail::HeaderMIMEType,
                                          detail::HeaderSuffix>;
inline detail::RegisterDCC<HelperHeaderContainer> RegisteredHHC;
