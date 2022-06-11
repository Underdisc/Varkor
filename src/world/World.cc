#include <sstream>
#include <utility>

#include "vlk/Valkor.h"
#include "world/Space.h"
#include "world/World.h"

namespace World {

bool nPause = true;
Ds::List<Space> nSpaces;

// Function pointers for calling into project code.
void (*nCentralUpdate)() = nullptr;
void (*nSpaceUpdate)(SpaceIt spaceIt) = nullptr;

void Purge()
{
  nSpaces.Clear();
}

void Update()
{
  if (nPause) {
    return;
  }
  if (nCentralUpdate != nullptr) {
    nCentralUpdate();
  }
  SpaceIt it = nSpaces.begin();
  SpaceIt itE = nSpaces.end();
  while (it != itE) {
    it->Update();
    if (nSpaceUpdate != nullptr) {
      nSpaceUpdate(it);
    }
    ++it;
  }
}

SpaceIt CreateTopSpace()
{
  std::stringstream defaultName;
  defaultName << "Space" << nSpaces.Size();
  SpaceIt newSpaceIt = nSpaces.Emplace(nSpaces.end(), defaultName.str());
  return newSpaceIt;
}

void DeleteSpace(SpaceIt it)
{
  nSpaces.Erase(it);
}

ValueResult<SpaceIt> LoadSpace(const char* filename)
{
  Vlk::Value rootVal;
  Result result = rootVal.Read(filename);
  if (!result.Success()) {
    return ValueResult<SpaceIt>(std::move(result), nSpaces.end());
  }
  SpaceIt newSpaceIt = CreateTopSpace();
  Vlk::Explorer rootEx(rootVal);
  newSpaceIt->Deserialize(rootEx);
  return ValueResult<SpaceIt>(newSpaceIt);
}

} // namespace World
