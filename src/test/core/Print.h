#ifndef test_core_Print_h
#define test_core_Print_h

#include "core/Space.h"
#include "core/Table.h"

// Functions used for printing out table data.
void PrintTableStats(const Core::Table& table);
void PrintTableOwners(const Core::Table& table);

// Functions used for printing out space data.
void PrintSpace(const Core::Space& space);
void PrintSpaceTables(const Core::Space& space);
void PrintSpaceTableLookup(const Core::Space& space);
void PrintSpaceTablesOwners(const Core::Space& space);
void PrintSpaceMembers(const Core::Space& space);
void PrintSpaceAddressBin(const Core::Space& space);
void PrintSpaceUnusedMemRefs(const Core::Space& space);

#endif
