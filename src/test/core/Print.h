#ifndef test_core_Print_h
#define test_core_Print_h

#include "core/Space.h"
#include "core/Table.h"

// Functions used for printing out table data.
void PrintTableStats(const World::Table& table);
void PrintTableOwners(const World::Table& table);

// Functions used for printing out space data.
void PrintSpace(const World::Space& space);
void PrintSpaceTables(const World::Space& space);
void PrintSpaceTableLookup(const World::Space& space);
void PrintSpaceTablesOwners(const World::Space& space);
void PrintSpaceMembers(const World::Space& space);
void PrintSpaceAddressBin(const World::Space& space);
void PrintSpaceUnusedMemRefs(const World::Space& space);

#endif
