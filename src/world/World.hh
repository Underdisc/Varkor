namespace World {

template<typename T>
void UpdateComponentType(const Space& space, SpaceId spaceId)
{
  Object currentObject;
  currentObject.mSpace = spaceId;
  Table::Visitor<T> visitor = space.CreateTableVisitor<T>();
  while (!visitor.End())
  {
    currentObject.mMember = visitor.CurrentOwner();
    visitor.CurrentComponent().Update(currentObject);
    visitor.Next();
  }
}

} // namespace World
