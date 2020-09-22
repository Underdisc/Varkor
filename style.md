# Style Guide
This only covers the style that is used for naming identifiers. All other formatting should be handled by clang-format and the specifications for the formatting can be found in .clang-format.

Besides this, there are no other style rules. The style rules are purely for making all code look consistent. I refuse to use any style rules that encourage the use of certain design patterns over speed. Speed is always the most important factor of any code.

## Variable Naming Scheme
Variable names are always camel case and begin with a lowercase letter.

- Local Variables `variableName`
- Member Variables `mVariableName`
- Static Member Variables `smVariableName`
- Namespace Variables `nVariableName`

## Function Naming Scheme
Functions are always camel case and start with an uppercase letter.

- Literally Any Function `FunctionName`

## Class and Struct Naming Scheme
Classes and Structs are always camel case and start with an uppercase letter.

- Literally Any Class or Struct `ClassOrStructName`

## Namespace Naming Scheme
Namespaces are always camel case and start with an uppercase letter.

- Literally Any Namespace `NamespaceName`

## Enum Naming Scheme
Enums are always camel case and start with an uppercase letter. Members of an enum follow the same scheme.

- Literally Any Enum `EnumName`
- Literally Any Enum Member `EnumMemberName`

## Everything Else
.clang-format specifies all other style rules.
