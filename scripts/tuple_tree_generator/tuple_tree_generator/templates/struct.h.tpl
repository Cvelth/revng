#pragma once

/*#-
This template file is distributed under the MIT License. See LICENSE.md for details.
The notice below applies to the generated files.
#*/
//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

// This file is autogenerated! Do not edit it directly

#include <compare>

#include "revng/ADT/TrackingContainer.h"
#include "revng/TupleTree/TupleTreeDiff.h"
#include "revng/TupleTree/TupleTreeReference.h"
#include "revng/Support/Assert.h"
/**- if emit_tracking **/
#include "revng/Support/AccessTracker.h"
/**- endif **/

void fieldAccessed(llvm::StringRef FieldName, llvm::StringRef StructName);

/** for header in includes **/
#include "/*= generator.user_include_path =*//*= header =*/"
/**- endfor **/

/** if emit_tracking **/
namespace revng {
struct Tracking;
}

struct ReadFields;
/**- endif **/

/*= struct.doc | docstring -=*/
struct /*= struct | fullname =*/
  /**- if struct.inherits **/ : public /*= struct.inherits | user_fullname =*/ /** endif -**/
{
  /** if emit_tracking -**/
  friend struct revng::Tracking;
  inline static constexpr bool HasTracking = true;
  /**- endif **/

  /** if struct.inherits **/
  static constexpr const /*= struct.inherits.name =*/Kind::Values AssociatedKind = /*= struct.inherits.name =*/Kind::/*= struct.name =*/;
  using BaseClass = /*= struct.inherits | user_fullname =*/;
  /**- else **//** if struct.abstract **/
  using BaseClass = /*= struct | user_fullname =*/;
  /**- else **/
  using BaseClass = void;
  /**- endif **//** endif **/

  /*#- --- Member list --- #*/
  /**- for field in struct.fields **/
private:
  /*= field | field_type =*/ The/*= field.name =*/ = /*= field | field_type =*/{};
  static_assert(Yamlizable</*= field | field_type =*/>);

  /**- if emit_tracking **/
  mutable revng::AccessTracker /*= field.name =*/Tracker = revng::AccessTracker(false);
  /** endif -**/

public:
  using /*= field.name =*/Type = /*= field | field_type =*/;

  /*= field.doc | docstring =*/
  const /*= field | field_type =*/ & /*= field.name =*/() const {
    /**- if emit_tracking **/
    /**- if not field in struct.key_fields **/
    /*= field.name =*/Tracker.access();

    /**- if emit_tracking_debug **/
    if (/*= field.name =*/Tracker.peak())
      fieldAccessed("/*= field.name =*/" , "/*= struct | fullname =*/");
    /** endif -**/

    /** endif -**/
    /** endif -**/

    return The/*= field.name =*/;
  }

  /*= field.doc | docstring =*/
  /*= field | field_type =*/ & /*= field.name =*/() {
  /**- if emit_tracking **/
    /*= field.name =*/Tracker.access();
  /** endif -**/
    return The/*= field.name =*/;
  }
  /**- endfor **/

  /*# --- Default constructor --- #*/
  /// Default constructor
  /*= struct.name =*/() :
    /**- if struct.inherits **//*= struct.inherits.name =*/()/** endif **/
    /**- for field in struct.fields **/
    /**- if not loop.first or struct.inherits **/, /** endif **/The/*= field.name =*/()
    /**- endfor **/ {
      /**- if struct.inherits -**/
      Kind() = AssociatedKind;
      /**- endif -**/
    }

  /*# --- Key constructor --- #*/
  /**- if struct.key_fields **/
  /// Key constructor
  /*= struct.name =*/(
    /**- for field in struct.key_fields **/
    /*=- field | field_type =*/ /*= field.name =*//** if not loop.last **/, /** endif **/
    /**- endfor **/
  ) :
    /**- if struct.inherits **/
    /*=- struct.inherits.name =*/(
      /**- for field in struct.key_fields **/
      /*=- field.name =*//** if not loop.last **/, /** endif **/
      /**- endfor **/
    )
    /**- else **/
    /**- for field in struct.key_fields **/
    The/*=- field.name =*/(/*= field.name =*/)/** if not loop.last **/, /** endif **/
    /**- endfor **/
    /**- endif **/ {
      /**- if struct.inherits and not 'Kind' in struct.key_fields | map(attribute='name') -**/
      Kind() = AssociatedKind;
      /**- endif -**/
    }
  /** endif **/

  /*# --- Full constructor --- #*/
  /** if struct.emit_full_constructor **/
  /// Full constructor
  /*= struct.name =*/(
    /*#- Inherited fields #*/
    /**- for field in struct.inherits.fields **/
    /*=- field | field_type =*/ /*= field.name =*/
    /** if (struct.fields | length > 0) or (not loop.last) **/, /** endif **/
    /**- endfor **/

    /*#- Own fields #*/
    /**- for field in struct.fields **/
    /*=- field | field_type =*/ /*= field.name =*//** if not loop.last **/, /** endif **/
    /**- endfor **/
  ) :
    /*#- Invoke base class constructor #*/
    /**- if struct.inherits **/
    /*= struct.inherits.name =*/(
      /**- for field in struct.inherits.fields **/
      /*= field.name =*//** if not loop.last **/, /** endif **/
      /**- endfor **/
    )
    /** if struct.fields | length > 0 **/, /** endif **/
    /** endif **/

    /*#- Initialize own fields #*/
    /**- for field in struct.fields **/
    The/*=- field.name =*/(/*= field.name =*/)/** if not loop.last **/, /** endif **/
    /**- endfor **/ {}
  /** endif **/

  /*# --- Key definition for KeyedObjectTraits --- #*/
  /** if struct._key **/
  using KeyTuple = std::tuple<
    /**- for key_field in struct.key_fields -**/
    /*= key_field | field_type =*//** if not loop.last **/, /** endif **/
    /**- endfor -**/
  >;
  struct Key : public KeyTuple {
    using KeyTuple::KeyTuple;
  };
  /** endif **/

  /*# --- Comparison operator --- #*/
  /** if struct.key_fields **/
  Key key() const {
    return Key {
      /**- for key_field in struct.key_fields -**/
      /*= key_field.name =*/()/** if not loop.last **/, /** endif **/
      /**- endfor -**/
    };
  }
  bool operator==(const /*= struct.name =*/ &Other) const { return key() == Other.key(); }
  bool operator<(const /*= struct.name =*/ &Other) const { return key() < Other.key(); }
  bool operator>(const /*= struct.name =*/ &Other) const { return key() > Other.key(); }

  /** else **/
  bool operator==(const /*= struct.name =*/ &Other) const {
    /**- for field in struct.fields **/
    if (/*= field.name =*/() != Other./*= field.name =*/())
      return false;
    /**- endfor **/
    return true;
  }
  /** endif **/

  /**- if emit_tracking **/
private:
  template<size_t I>
  revng::AccessTracker& getTracker() const {
    /**- for field in struct.all_fields **/
    if constexpr (I == /*= loop.index0 =*/)
        return /*= field.name =*/Tracker;
    /**- endfor -**/
  }
  /** if upcastable **/
  /**- for child_type in upcastable|sort(attribute="user_fullname") **/
  friend /*= child_type | fullname =*/;
  /**- endfor **/
  /** endif **/

  template<size_t I>
  const auto& untrackedGet() const {
    if constexpr (false)
      return 0;
    /**- for field in struct.all_fields **/
    else if constexpr (I == /*= loop.index0 =*/)
      return The/*= field.name =*/;
    /**- endfor -**/
  }

  /** if struct._key **/
  Key untrackedKey() const {
    return {
      /** for key_field in struct.key_fields -**/
      The/*= key_field.name =*/
      /**- if not loop.last **/,
      /** endif **/
      /**- endfor **/
    };
  }
  /** endif **/

  /** endif -**/
public:
  bool localCompare(const /*= struct | user_fullname =*/ &Other) const;
};

/** if struct._key **/
template<>
struct std::tuple_size</*= struct | fullname =*/::Key>
  : public std::tuple_size</*= struct | fullname =*/::KeyTuple> {};

template<size_t I>
struct std::tuple_element<I, /*= struct | fullname =*/::Key>
  : public std::tuple_element<I, /*= struct | fullname =*/::KeyTuple> {};
/** endif **/

/*# --- UpcastablePointer stuff --- #*/
/** if upcastable **/
/*# Emit both const and non-const specialization of concrete_types_traits #*/
/** for const_qualifier in ["", "const"] **/
template<>
struct concrete_types_traits</*= const_qualifier =*/ /*= struct | user_fullname =*/> {
  using type = std::tuple<
    /**- for child_type in upcastable|sort(attribute="user_fullname") **/
    /*=- const_qualifier =*/ /*= child_type | user_fullname =*//** if not loop.last **/, /** endif **/
    /**- endfor **/
    /**- if not struct.abstract **/, /*=- const_qualifier =*/ /*= struct | user_fullname =*//** endif **/>;
};
/** endfor **/
/** endif **//*# End UpcastablePointer stuff #*/

