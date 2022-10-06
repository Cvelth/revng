/// \file ModelAlignment.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/ABI/Definition.h"
#include "revng/ABI/ModelAlignment.h"
#include "revng/ABI/Predefined.h"
#include "revng/Model/Binary.h"

namespace abi {

static RecursiveCoroutine<std::optional<uint64_t>>
naturalAlignmentImpl(model::VerifyHelper &VH,
                     const model::Type &Type,
                     const abi::Definition &ABI);
static RecursiveCoroutine<std::optional<uint64_t>>
naturalAlignmentImpl(model::VerifyHelper &VH,
                     const model::QualifiedType &Type,
                     const abi::Definition &ABI);

template<typename FieldType>
static RecursiveCoroutine<std::optional<uint64_t>>
aggregateAlignmentImpl(model::VerifyHelper &VH,
                       const SortedVector<FieldType> Fields,
                       const abi::Definition &ABI) {
  uint64_t Alignment = 0;

  for (const auto &Field : Fields) {
    if (auto A = rc_recur naturalAlignmentImpl(VH, Field.Type, ABI))
      Alignment = std::max(Alignment, *A);
    else
      rc_return std::nullopt;
  }

  rc_return Alignment;
}

static RecursiveCoroutine<std::optional<uint64_t>>
naturalAlignmentImpl(model::VerifyHelper &VH,
                     const model::Type &Type,
                     const abi::Definition &ABI) {
  auto MaybeAlignment = VH.alignment(&Type);
  if (MaybeAlignment)
    rc_return MaybeAlignment;

  // This code assumes that the type `Type` is well formed.
  uint64_t Alignment;

  switch (Type.Kind) {
  case model::TypeKind::RawFunctionType:
  case model::TypeKind::CABIFunctionType:
    // Function prototypes have no size - hence no alignment.
    Alignment = 0;
    break;

  case model::TypeKind::PrimitiveType:
    // The alignment of primitives is simple to figure out based on the abi
    if (const auto *P = llvm::cast<model::PrimitiveType>(&Type);
        P->PrimitiveKind == model::PrimitiveTypeKind::Void) {
      // Make sure void types have no size.
      if (P->Size != 0)
        rc_return std::nullopt;

      Alignment = 0;
    } else {
      Alignment = ABI.TypeSpecific.at(P->Size).AlignAt;
    }
    break;

  case model::TypeKind::EnumType: {
    // The alignment of an enum is the same as the alignment of its underlying
    // type
    const auto &Underlying = llvm::cast<model::EnumType>(&Type)->UnderlyingType;
    if (auto A = rc_recur naturalAlignmentImpl(VH, Underlying, ABI))
      Alignment = *A;
    else
      rc_return std::nullopt;
  } break;

  case model::TypeKind::TypedefType: {
    // The alignment of an enum is the same as the alignment of its underlying
    // type
    const auto &Typedef = llvm::cast<model::TypedefType>(&Type)->UnderlyingType;
    if (auto A = rc_recur naturalAlignmentImpl(VH, Typedef, ABI))
      Alignment = *A;
    else
      rc_return std::nullopt;
  } break;

  case model::TypeKind::StructType: {
    // The alignment of a struct is the same as the alignment of its most
    // strictly aligned member.
    const auto &Fields = llvm::cast<model::StructType>(&Type)->Fields;
    if (auto A = rc_recur aggregateAlignmentImpl(VH, Fields, ABI))
      Alignment = *A;
    else
      rc_return std::nullopt;
  } break;

  case model::TypeKind::UnionType: {
    // The alignment of a union is the same as the alignment of its most
    // strictly aligned member.
    const auto &Fields = llvm::cast<model::UnionType>(&Type)->Fields;
    if (auto A = rc_recur aggregateAlignmentImpl(VH, Fields, ABI))
      Alignment = *A;
    else
      rc_return std::nullopt;
  } break;

  case model::TypeKind::Invalid:
  case model::TypeKind::Count:
  default:
    rc_return std::nullopt;
  }

  VH.setSize(&Type, Alignment);

  rc_return Alignment;
}

static RecursiveCoroutine<std::optional<uint64_t>>
naturalAlignmentImpl(model::VerifyHelper &VH,
                     const model::QualifiedType &Type,
                     const abi::Definition &ABI) {
  // This code assumes that the QualifiedType is well formed.
  for (auto It = Type.Qualifiers.begin(); It != Type.Qualifiers.end(); ++It) {
    switch (It->Kind) {
    case model::QualifierKind::Pointer:
      // Doesn't matter what the type is, use alignment of the pointer.
      rc_return ABI.TypeSpecific.at(It->Size).AlignAt;

    case model::QualifierKind::Array: {
      // The alignment of an array is the same as the alignment of its type.
      const model::QualifiedType Element{
        Type.UnqualifiedType, { std::next(It), Type.Qualifiers.end() }
      };
      if (auto MaybeAlignment = rc_recur naturalAlignmentImpl(VH, Element, ABI))
        rc_return *MaybeAlignment;
      else
        rc_return std::nullopt;
    }

    case model::QualifierKind::Const:
      // Const has no impact on alignment.
      break;

    case model::QualifierKind::Invalid:
    case model::QualifierKind::Count:
    default:
      rc_return std::nullopt;
    }
  }

  rc_return rc_recur naturalAlignmentImpl(VH, *Type.UnqualifiedType.get(), ABI);
}

RecursiveCoroutine<std::optional<uint64_t>>
naturalAlignment(model::VerifyHelper &VH,
                 const model::QualifiedType &Type,
                 model::ABI::Values ABI) {
  const abi::Definition &Def = abi::predefined::get(ABI);
  std::optional<uint64_t> Result = rc_recur naturalAlignmentImpl(VH, Type, Def);
  revng_check(Result);
  if (*Result == 0)
    rc_return std::nullopt;
  else
    rc_return Result;
}

} // namespace abi
