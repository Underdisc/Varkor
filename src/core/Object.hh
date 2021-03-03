namespace Core {
namespace World {

template<typename T>
void Object::AddComponent() const
{
  Space& space = GetSpace(mSpace);
  space.AddComponent<T>(mMember);
}

template<typename T>
void Object::RemComponent() const
{
  Space& space = GetSpace(mSpace);
  space.RemComponent<T>(mMember);
}

template<typename T>
T* Object::GetComponent() const
{
  Space& space = GetSpace(mSpace);
  return space.GetComponent<T>(mMember);
}

template<typename T>
bool Object::HasComponent() const
{
  Space& space = GetSpace(mSpace);
  return space.HasComponent<T>(mMember);
}

} // namespace World
} // namespace Core
