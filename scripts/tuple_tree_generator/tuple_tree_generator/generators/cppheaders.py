#
# This file is distributed under the MIT License. See LICENSE.md for details.
#

import graphlib
from collections import defaultdict
from typing import Dict, Optional

from jinja2 import Environment

from ..schema import Definition, EnumDefinition, ReferenceDefinition, ScalarDefinition, Schema
from ..schema import SequenceDefinition, SequenceStructField, StructDefinition, StructField
from ..schema import UpcastableDefinition
from .jinja_utils import loader


class CppHeadersGenerator:
    def __init__(
        self,
        schema: Schema,
        root_type: str,
        emit_tracking: bool,
        emit_tracking_debug: bool,
        user_include_path: Optional[str] = None,
    ):
        self.schema = schema
        self.root_type = root_type
        self.emit_tracking = emit_tracking
        self.emit_tracking_debug = emit_tracking_debug

        if not user_include_path:
            user_include_path = ""
        elif not user_include_path.endswith("/"):
            user_include_path = user_include_path + "/"

        self.environment = Environment(
            block_start_string="/**",
            block_end_string="**/",
            variable_start_string="/*=",
            variable_end_string="=*/",
            comment_start_string="/*#",
            comment_end_string="#*/",
            loader=loader,
        )

        # More convenient than escaping the double braces
        self.environment.globals["nodiscard"] = "[[ nodiscard ]]"
        self.environment.filters["docstring"] = self.render_docstring
        self.environment.filters["field_type"] = self.field_type
        self.environment.filters["fullname"] = self.fullname
        self.environment.filters["user_fullname"] = self.user_fullname
        self.environment.filters["is_struct_field"] = self.is_struct_field
        self.environment.filters["len"] = len
        self.enum_template = self.environment.get_template("enum.h.tpl")
        self.struct_template = self.environment.get_template("struct.h.tpl")
        self.struct_late_template = self.environment.get_template("struct_late.h.tpl")
        self.struct_impl_template = self.environment.get_template("struct_impl.cpp.tpl")
        self.struct_forward_decls_template = self.environment.get_template(
            "struct_forward_decls.h.tpl"
        )
        self.class_forward_decls_template = self.environment.get_template(
            "class_forward_decls.h.tpl"
        )

        # Path where user-provided headers are assumed to be located
        # Prepended to include statements (e.g. #include "<user_include_path>/Class.h")
        self.user_include_path = user_include_path

        self.depgraph = self._build_depgraph()
        self._inverse_depgraph = self._build_inverse_depgraph()

    def emit(self) -> Dict[str, str]:
        sources = {
            "ForwardDecls.h": self._emit_forward_decls(),
        }

        early_definitions = self._emit_early_type_definitions()
        late_definitions = self._emit_late_type_definitions()
        impl_definitions = self._emit_impl()
        sources.update(early_definitions)
        sources.update(late_definitions)
        sources.update(impl_definitions)
        return sources

    def _build_depgraph(self):
        depgraph = {}
        for definition in self.schema.definitions.values():
            depgraph[definition.name] = set()
            for dependency_name in definition.dependencies:
                dependency_type = self.schema.get_definition_for(dependency_name)
                if dependency_type:
                    depgraph[definition.name].add(dependency_type.name)
        return depgraph

    def _build_inverse_depgraph(self):
        inverse_depgraph = defaultdict(set)
        for definition in self.schema.definitions.values():
            for dependency in definition.dependencies:
                inverse_depgraph[dependency].add(definition.name)
        return inverse_depgraph

    def _compute_includes(self, type_to_emit):
        # Register which headers we need
        # We assume that the headers will always be in the 'Generated'
        # folder, relative to the user_include_path
        includes = set()
        includes.add("Generated/ForwardDecls.h")
        for dep in type_to_emit.dependencies:
            dep_definition = self.schema.get_definition_for(dep)
            if dep_definition and not isinstance(dep_definition, ScalarDefinition):
                if not dep_definition.autogenerated:
                    includes.add(f"{dep_definition.name}.h")
                else:
                    includes.add(f"Generated/Early/{dep_definition.name}.h")

        return includes

    def _emit_early_type_definitions(self):
        definitions = {}
        toposorter = graphlib.TopologicalSorter(self.depgraph)
        order = list(toposorter.static_order())
        for type_name_to_emit in order:
            type_to_emit = self.schema.get_definition_for(type_name_to_emit)
            if not type_to_emit:
                # Should we emit a warning here?
                continue

            filename = f"Early/{type_to_emit.name}.h"
            assert filename not in definitions
            if isinstance(type_to_emit, StructDefinition):
                upcastable_types = self.schema.get_upcastable_types(type_to_emit)
                includes = self._compute_includes(type_to_emit)
                definition = self.struct_template.render(
                    struct=type_to_emit,
                    upcastable=upcastable_types,
                    user_include_path=self.user_include_path,
                    includes=includes,
                    emit_tracking=self.emit_tracking,
                    emit_tracking_debug=self.emit_tracking_debug,
                )
            elif isinstance(type_to_emit, EnumDefinition):
                definition = self.enum_template.render(enum=type_to_emit)
            elif isinstance(type_to_emit, ScalarDefinition):
                definition = None
            else:
                raise ValueError()

            if definition is not None:
                definitions[filename] = definition

        return definitions

    def _emit_late_type_definitions(self):
        all_known_types = set()

        for struct in self.schema.struct_definitions():
            all_known_types.add(self.user_fullname(struct))
            for field in struct.fields:
                all_known_types.add(self._cpp_type(field.resolved_type))
                if isinstance(field, SequenceStructField):
                    all_known_types.add(self._cpp_type(field.resolved_element_type))

        definitions = {}
        for type_to_emit in self.schema.definitions.values():
            filename = f"Late/{type_to_emit.name}.h"
            assert filename not in definitions

            if isinstance(type_to_emit, StructDefinition):
                upcastable_types = self.schema.get_upcastable_types(type_to_emit)
                definition = self.struct_late_template.render(
                    struct=type_to_emit,
                    upcastable=upcastable_types,
                    schema=self.schema,
                    user_include_path=self.user_include_path,
                    root_type=self.root_type,
                    namespace=self.schema.generated_namespace,
                    all_types=all_known_types,
                    base_namespace=self.schema.base_namespace,
                    emit_tracking=self.emit_tracking,
                )
            elif isinstance(type_to_emit, EnumDefinition):
                definition = ""
            elif isinstance(type_to_emit, ScalarDefinition):
                definition = None
            else:
                raise ValueError()

            if definition is not None:
                definitions[filename] = definition
        return definitions

    def _emit_impl(self):
        definition = ""
        for type_to_emit in self.schema.definitions.values():
            if isinstance(type_to_emit, StructDefinition):
                upcastable_types = self.schema.get_upcastable_types(type_to_emit)
                definition += (
                    self.struct_impl_template.render(
                        struct=type_to_emit,
                        upcastable=upcastable_types,
                        user_include_path=self.user_include_path,
                        schema=self.schema,
                        root_type=self.root_type,
                        base_namespace=self.schema.base_namespace,
                    )
                    + "\n"
                )
            elif isinstance(type_to_emit, (EnumDefinition, ScalarDefinition)):
                pass
            else:
                raise ValueError()

        return {"Impl.cpp": definition}

    def _emit_forward_decls(self):
        generated_ns_to_names = defaultdict(set)
        user_ns_to_names = defaultdict(set)
        generated_ns_to_names = defaultdict(set)
        for definition in self.schema.struct_definitions():
            # Forward declaration for autogenerated class
            generated_ns_to_names[f"{definition.namespace}::generated"].add(definition.name)
            # Forward declaration for user-defined derived class
            user_ns_to_names[definition.namespace].add(definition.name)

        abstract_names = [
            definition.name
            for definition in self.schema.struct_definitions()
            if definition.abstract
        ]

        return "\n".join(
            [
                self.struct_forward_decls_template.render(ns_to_names=generated_ns_to_names),
                self.class_forward_decls_template.render(
                    ns_to_names=user_ns_to_names, abstract_names=abstract_names
                ),
            ]
        )

    def _cpp_type(self, definition: Definition):
        assert isinstance(definition, Definition)
        if isinstance(definition, StructDefinition):
            return f"{definition.namespace}::{definition.name}"
        elif isinstance(definition, SequenceDefinition):
            if definition.sequence_type == "SortedVector" and self.emit_tracking:
                return f"TrackingSortedVector<{self._cpp_type(definition.element_type)}>"
            if definition.sequence_type == "MutableSet" and self.emit_tracking:
                return f"TrackingMutableSet<{self._cpp_type(definition.element_type)}>"
            return f"{definition.sequence_type}<{self._cpp_type(definition.element_type)}>"
        elif isinstance(definition, EnumDefinition):
            return f"{definition.namespace}::{definition.name}::Values"
        elif isinstance(definition, ScalarDefinition):
            if definition.name == "string":
                return "std::string"
            return definition.name
        elif isinstance(definition, ReferenceDefinition):
            root_type = self._cpp_type(definition.root)
            return f"TupleTreeReference<{self._cpp_type(definition.pointee)}, {root_type}>"
        elif isinstance(definition, UpcastableDefinition):
            return f"UpcastablePointer<{self._cpp_type(definition.base)}>"
        else:
            assert False

    def field_type(self, field: StructField):
        assert field.resolved_type is not None
        return self._cpp_type(field.resolved_type)

    @staticmethod
    def fullname(resolved_type: Definition):
        if isinstance(resolved_type, StructDefinition):
            return f"{resolved_type.namespace}::generated::{resolved_type.name}"
        elif isinstance(resolved_type, EnumDefinition):
            return f"{resolved_type.namespace}::{resolved_type.name}::Values"
        else:
            raise ValueError(resolved_type)

    @staticmethod
    def user_fullname(resolved_type: Definition):
        if isinstance(resolved_type, StructDefinition):
            return f"{resolved_type.namespace}::{resolved_type.name}"
        elif isinstance(resolved_type, EnumDefinition):
            return f"{resolved_type.namespace}::{resolved_type.name}::Values"
        else:
            raise ValueError(resolved_type)

    @staticmethod
    def is_struct_field(field: StructField):
        return isinstance(field, StructField)

    @staticmethod
    def render_docstring(docstr: str):
        if not docstr:
            return ""
        rendered_docstring = "\n".join(f"/// {line}" for line in docstr.splitlines())
        if not rendered_docstring.endswith("\n"):
            rendered_docstring = rendered_docstring + "\n"
        return rendered_docstring
