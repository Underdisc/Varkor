#include <random>

#include "ext/Tracy.h"
#include "test/perf/Helper.h"
#include "test/world/Print.h"
#include "world/Space.h"

struct A {
  Vec3 m1;
  Vec3 m2;
  A() {
    Write(0.0f);
  }
  void Write(float value) {
    m1 = {value, value, value};
    m2 = {value, value, value};
  }
};

struct B {
  float m1;
  Vec3 m2;
  B() {
    Write(0.0f);
  }
  void Write(float value) {
    m1 = value;
    m2 = {value, value, value};
  }
};

struct C {
  Vec2 m1;
  Vec2 m2;
  C() {
    Write(0.0f);
  }
  void Write(float value) {
    m1 = {value, value};
    m2 = {value, value};
  }
};

struct D {
  int m1;
  float m2;
  Vec2 m3;
  D() {
    Write(0.0f);
  }
  void Write(float value) {
    m1 = (int)value;
    m2 = value;
    m3 = {value, value};
  }
};

void Create() {
  // Create a large number of entities.
  ZoneScopedC(0xFF0000);
  World::Space space;
  for (int i = 0; i < 1'000'000; ++i) {
    space.CreateMember();
  }
}

void Add() {
  // Create a large number of entities and add the same components to each.
  ZoneScopedC(0x00FF00);
  World::Space space;
  for (int i = 0; i < 500'000; ++i) {
    World::MemberId id = space.CreateMember();
    space.Add<A>(id);
    space.Add<B>(id);
    space.Add<C>(id);
    space.Add<D>(id);
  }
}

void AddWriteDelete() {
  ZoneScopedC(0x0000FF);
  World::Space space;
  const int cycles = 300;
  for (int i = 0; i < cycles; ++i) {
    // Create entities and add components to them.
    const size_t idBufferSize = 2'000;
    World::MemberId idBuffer[idBufferSize];
    for (int j = 0; j < idBufferSize; ++j) {
      idBuffer[j] = space.CreateMember();
      space.Add<A>(idBuffer[j]);
      space.Add<B>(idBuffer[j]);
      space.Add<C>(idBuffer[j]);
      space.Add<D>(idBuffer[j]);
    }

    // Write to all components.
    for (int j = 0; j < idBufferSize; ++j) {
      space.Get<A>(idBuffer[j]).Write(1.0f);
      space.Get<B>(idBuffer[j]).Write(1.0f);
      space.Get<C>(idBuffer[j]).Write(1.0f);
      space.Get<D>(idBuffer[j]).Write(1.0f);
    }

    // Delete all entities.
    for (int j = 0; j < idBufferSize; ++j) {
      space.DeleteMember(idBuffer[j]);
    }
  }
}

void DistributedAddWriteRemoveAddDelete() {
  ZoneScopedC(0xFFFF00);
  World::Space space;
  const int cycles = 300;
  for (int i = 0; i < cycles; ++i) {
    const size_t idBufferSize = 2'000;
    World::MemberId idBuffer[idBufferSize];

    // Create entities with distributed component sets.
    for (int j = 0; j < idBufferSize; ++j) {
      World::MemberId& id = idBuffer[j];
      id = space.CreateMember();
      // clang-format off
      switch (j % 4) {
      case 0: space.Add<A>(id); space.Add<D>(id); break;
      case 1: space.Add<B>(id); space.Add<A>(id); break;
      case 2: space.Add<C>(id); space.Add<B>(id); break;
      case 3: space.Add<D>(id); space.Add<C>(id); break;
      }
      // clang-format on
    }

    // Write to all components.
    for (int j = 0; j < idBufferSize; j++) {
      World::MemberId id = idBuffer[j];
      // clang-format off
      switch (j % 4) {
      case 0: space.Get<A>(id).Write(1.0f); space.Get<D>(id).Write(1.0f); break;
      case 1: space.Get<B>(id).Write(1.0f); space.Get<A>(id).Write(1.0f); break;
      case 2: space.Get<C>(id).Write(1.0f); space.Get<B>(id).Write(1.0f); break;
      case 3: space.Get<D>(id).Write(1.0f); space.Get<C>(id).Write(1.0f); break;
      }
      // clang-format on
    }

    // Remove all of the components but keep the entites.
    for (int j = 0; j < idBufferSize; j++) {
      World::MemberId id = idBuffer[j];
      // clang-format off
      switch (j % 4) {
      case 0: space.Rem<A>(id); space.Rem<D>(id); break;
      case 1: space.Rem<B>(id); space.Rem<A>(id); break;
      case 2: space.Rem<C>(id); space.Rem<B>(id); break;
      case 3: space.Rem<D>(id); space.Rem<C>(id); break;
      }
      // clang-format on
    }

    // Add components in a different configuration.
    for (int j = idBufferSize - 1; j >= 0; j--) {
      World::MemberId id = idBuffer[j];
      // clang-format off
      switch ((j + 1) % 4) {
      case 0: space.Add<A>(id); space.Add<D>(id); break;
      case 1: space.Add<B>(id); space.Add<A>(id); break;
      case 2: space.Add<C>(id); space.Add<B>(id); break;
      case 3: space.Add<D>(id); space.Add<C>(id); break;
      }
      // clang-format on
    }

    // Delete all entities.
    for (int j = 0; j < idBufferSize; ++j) {
      space.DeleteMember(idBuffer[j]);
    }
  }
}

void RelationshipAddWriteDelete() {
  ZoneScopedC(0xFF00FF);
  World::Space space;
  const int cycles = 500;
  for (int i = 0; i < cycles; ++i) {
    // Create all entities with a relationship heirarchy.
    const size_t rootBranchCount = 10;
    World::MemberId rootIds[rootBranchCount];
    for (int j = 0; j < rootBranchCount; ++j) {
      rootIds[j] = space.CreateMember();
      space.Add<A>(rootIds[j]);
      space.Add<B>(rootIds[j]);
      for (int k = 0; k < 10; ++k) {
        World::MemberId childId = space.CreateChildMember(rootIds[j]);
        space.Add<C>(childId);
        for (int l = 0; l < 20; ++l) {
          World::MemberId grandchildId = space.CreateChildMember(childId);
          space.Add<D>(grandchildId);
        }
      }
    }

    // Write new values to all components.
    for (int j = 0; j < rootBranchCount; ++j) {
      space.Get<A>(rootIds[j]).Write(1.0f);
      space.Get<B>(rootIds[j]).Write(1.0f);
      const auto& rootRelationship = space.Get<Comp::Relationship>(rootIds[j]);
      for (World::MemberId childId: rootRelationship.mChildren) {
        space.Get<C>(childId).Write(1.0f);
        const auto& childRelationship = space.Get<Comp::Relationship>(childId);
        for (World::MemberId grandchildId: childRelationship.mChildren) {
          space.Get<D>(grandchildId).Write(1.0f);
        }
      }
    }

    // Delete all entities by deleting only root entities.
    for (int j = 0; j < rootBranchCount; ++j) {
      space.DeleteMember(rootIds[j]);
    }
  }
}

void Random() {
  ZoneScopedC(0x00FFFF);
  World::Space space;
  Ds::Vector<World::MemberId> idBuffer;
  std::mt19937 generator;
  std::uniform_int_distribution<unsigned long long> distribution(0);

  auto acquireRandomEntity = [&]() -> World::MemberId {
    if (idBuffer.Size() == 0) {
      return World::nInvalidMemberId;
    }
    return idBuffer[distribution(generator) % idBuffer.Size()];
  };

  auto randomCreate = [&]() {
    World::MemberId id = space.CreateMember();
    idBuffer.Push(id);
    for (int j = 0; j < 4; ++j) {
      int result = distribution(generator) % 2;
      if (result == 1) {
        switch (j) {
        case 0: space.Add<A>(id); break;
        case 1: space.Add<B>(id); break;
        case 2: space.Add<C>(id); break;
        case 3: space.Add<D>(id); break;
        }
      }
    }
  };

  auto randomWrite = [&]() {
    World::MemberId id = acquireRandomEntity();
    if (id == World::nInvalidMemberId) {
      return;
    }
    A* a = space.TryGet<A>(id);
    B* b = space.TryGet<B>(id);
    C* c = space.TryGet<C>(id);
    D* d = space.TryGet<D>(id);
    if (a != nullptr) a->Write(a->m1[0] + 1.0f);
    if (b != nullptr) b->Write(b->m1 + 1.0f);
    if (c != nullptr) c->Write(c->m1[0] + 1.0f);
    if (d != nullptr) d->Write(d->m1 + 1);
  };

  auto randomEnsure = [&]() {
    World::MemberId id = acquireRandomEntity();
    if (id == World::nInvalidMemberId) {
      return;
    }
    int type = distribution(generator) % 4;
    switch (type) {
    case 0: space.Ensure<A>(id); break;
    case 1: space.Ensure<B>(id); break;
    case 2: space.Ensure<C>(id); break;
    case 3: space.Ensure<D>(id); break;
    }
  };

  auto randomTryRemove = [&]() {
    World::MemberId id = acquireRandomEntity();
    if (id == World::nInvalidMemberId) {
      return;
    }
    int type = distribution(generator) % 4;
    switch (type) {
    case 0: space.TryRem<A>(id); break;
    case 1: space.TryRem<B>(id); break;
    case 2: space.TryRem<C>(id); break;
    case 3: space.TryRem<D>(id); break;
    }
  };

  auto randomDelete = [&]() {
    if (idBuffer.Size() == 0) {
      return;
    }
    size_t idx = distribution(generator) % idBuffer.Size();
    World::MemberId id = idBuffer[idx];
    space.DeleteMember(id);
    idBuffer.LazyRemove(idx);
  };

  const int cycles = 10;
  for (int i = 0; i < cycles; ++i) {
    const int creationAttempts = 50'000;
    for (int j = 0; j < creationAttempts; ++j) {
      // Attempt to create an entity or perform a random write
      float createProbability =
        (float)(creationAttempts - j) / (float)creationAttempts;
      float result = (float)(distribution(generator) % 1'000) / 1'000.0f;
      if (result <= createProbability) {
        randomCreate();
      }
      else {
        randomWrite();
      }
    }

    // Perform random operations
    const int inbetweenOperations = 100'000;
    for (int j = 0; j < inbetweenOperations; ++j) {
      int randResult = distribution(generator) % 3;
      switch (randResult) {
      case 0: randomWrite(); break;
      case 1: randomEnsure(); break;
      case 2: randomTryRemove(); break;
      }
    }

    const int deletionAttempts = 50'000;
    for (int j = 0; j < deletionAttempts; ++j) {
      // Attempt to delete or write to a random entity.
      float deleteProbability = (float)j / (float)deletionAttempts;
      float result = (float)(distribution(generator) % 1'000) / 1'000.0f;
      if (result <= deleteProbability) {
        randomDelete();
      }
      else {
        randomWrite();
      }
    }
  }
}

int main(void) {
  ProfileThread("Main");

  Error::Init();

  RegisterComponentTypes();
  RegisterComponent(A);
  RegisterComponent(B);
  RegisterComponent(C);
  RegisterComponent(D);

  Profile(Create, 50);
  Profile(Add, 50);
  Profile(AddWriteDelete, 50);
  Profile(DistributedAddWriteRemoveAddDelete, 50);
  Profile(RelationshipAddWriteDelete, 50);
  Profile(Random, 50);
}
