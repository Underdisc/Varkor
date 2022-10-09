# Style Guide
This covers the style of identifiers and conventions commonly used within the codebase.

All formatting is handled by clang-format, for which there is a config file in the root of the repo. Sometimes clang-format is awkward. If something can look a bit nicer without it, turning clang-format off for a block is acceptable.

## Naming
### Variables
Variable names are always camel case and begin with a lowercase letter.

- Local Variables `variableName`
- Member Variables `mVariableName`
- Static Member Variables `smVariableName`
- Namespace Variables `nVariableName`

### Other
All other identifier names are camel case and begin with an uppercase letter. This is subect to change.

- Functions 'FunctionName'
- Structs `StructName`
- Namespaces `NamespaceName`
- Enum `EnumName`
- Enum Members `EnumMemberName`
- Macros `Macro`

## Conventions
### Modified Args Passed As Pointer
Functions shouldn't take non-const references. If a function modifies an argument, that function must take it as a pointer. Knowing whether an argument is potentially modified is valuable information.