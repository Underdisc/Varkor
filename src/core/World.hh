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

} // namespace World
