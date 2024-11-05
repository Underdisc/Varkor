#include "world/Space.h"
#include "ext/Tracy.h"
#include "test/world/TestTypes.h"

struct A
{
  Vec3 m1;
  Vec3 m2;
};

struct B
{
  float m1;
  Vec3 m2;
};

struct C
{
  Vec2 m1;
  Vec2 m2;
};

void CreateMembers()
{
  ZoneScopedC(0xFF0000);
  World::Space space;
  for (int i = 0; i < 1'000'000; ++i) {
    space.CreateMember();
  }
}

void AddComponents()
{
  ZoneScopedC(0x00FF00);
  World::Space space;
  for (int i = 0; i < 100'000; ++i) {
    World::MemberId id = space.CreateMember();
    space.Add<A>(id).m1 = {0.0f, 0.0f, 0.0f};
    space.Add<B>(id).m1 = 0.0f;
    space.Add<C>(id).m1 = {0.0f, 0.0f};
  }
}

void AddComponentsDeleteMembers()
{
  ZoneScopedC(0x0000FF);
  World::Space space;
  int idBufferUsage = 0;
  const size_t idBufferSize = 1000;
  World::MemberId idBuffer[idBufferSize];
  for (int i = 0; i < 100'000; ++i) {
    World::MemberId& id = idBuffer[idBufferUsage];
    id = space.CreateMember();
    space.Add<A>(id).m1 = {0.0f, 0.0f, 0.0f};
    space.Add<B>(id).m1 = 0.0f;
    space.Add<C>(id).m1 = {0.0f, 0.0f};
    if (++idBufferUsage == idBufferSize) {
      for (int i = 0; i < idBufferSize; ++i) {
        space.DeleteMember(idBuffer[i]);
      }
      idBufferUsage = 0;
    }
  }
}

void Profile(void (*function)(), int count)
{
  ZoneScopedC(0xFFFFFF);
  srand(42);
  for (int i = 0; i < count; ++i) {
    function();
  }
}

int main(void)
{
  ProfileThread("Main");

  Error::Init();

  RegisterComponentTypes();
  RegisterComponent(A);
  RegisterComponent(B);
  RegisterComponent(C);

  const int count = 100;
  Profile(CreateMembers, count);
  Profile(AddComponents, count);
  Profile(AddComponentsDeleteMembers, count);
}
