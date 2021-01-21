namespace Core {
namespace World {

template<typename T>
void Object::AddComponent()
{
  Space& space = GetSpace(mSpace);
  space.AddComponent<T>(mMember);
}

template<typename T>
void Object::RemComponent()
{
  Space& space = GetSpace(mSpace);
  space.RemComponent<T>(mMember);
}

template<typename T>
T* Object::GetComponent()
{
  Space& space = GetSpace(mSpace);
  return space.GetComponent<T>(mMember);
}

template<typename T>
bool Object::HasComponent()
{
  Space& space = GetSpace(mSpace);
  return space.HasComponent<T>(mMember);
}

} // namespace World
} // namespace Core
