namespace World {

int CreateId();

template<typename T>
ComponentId ComponentType<T>::smId = nInvalidComponentId;

template<typename T>
void ComponentType<T>::Register()
{
  if (smId == nInvalidComponentId)
  {
    smId = CreateId();
    ComponentData data;
    data.mSize = sizeof(T);
    nComponentTypeData.Insert(ComponentType<T>::smId, data);
  }
}

} // namespace World
