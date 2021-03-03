namespace Core {
namespace World {

template<typename T>
void UpdateComponentType(const Space& space, SpaceRef spaceRef)
{
  Object currentObject;
  currentObject.mSpace = spaceRef;
  Table::Visitor<T> visitor = space.CreateTableVisitor<T>();
  while (!visitor.End())
  {
    currentObject.mMember = visitor.CurrentOwner();
    visitor.CurrentComponent().Update(currentObject);
    visitor.Next();
  }
}

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
