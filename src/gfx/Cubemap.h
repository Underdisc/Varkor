#ifndef gfx_Cubemap_h
#define gfx_Cubemap_h

#include <glad/glad.h>

#include "vlk/Valkor.h"

namespace Gfx {

struct Cubemap
{
public:
  enum class FilterType
  {
    Nearest,
    Linear,
    Count,
    Invalid
  };
  constexpr static const char* smFilterTypeStrings[(int)FilterType::Count] {
    "Nearest", "Linear"};
  constexpr static GLint smFilterTypeGlValues[(int)FilterType::Count] {
    GL_NEAREST, GL_LINEAR};
  static FilterType GetFilterType(const std::string& filterTypeString);

  // Determines whether one or six image files are used in the cubemap.
  enum class Specification
  {
    Split,
    Single,
    Count,
    Invalid,
  };
  constexpr static const char*
    smSpecificationStrings[(int)Specification::Count] = {"Split", "Single"};
  static Specification GetSpecification(const std::string& specificationString);

  // The key strings for image files in a split specification.
  constexpr static int smFileDescriptorCount = 6;
  constexpr static const char* smFileDescriptorStrings[smFileDescriptorCount] {
    "Right", "Left", "Top", "Bottom", "Back", "Front"};

  struct Config
  {
    std::string mFaceFiles[smFileDescriptorCount];
    GLint mGlFilter;
    Specification mSpecification;
  };

  Cubemap();
  Cubemap(Cubemap&& other);
  Cubemap& operator=(Cubemap&& other);
  ~Cubemap();

  Result Init(const Vlk::Explorer& configEx);
  Result Init(const Config& config);
  static void EditConfig(Vlk::Value* configValP);

  GLuint Id() const;

private:
  struct Face
  {
  public:
    static VResult<Face> Init(const std::string& file);
    unsigned char* mData;
    int mWidth;
    int mHeight;
    int mChannels;

  private:
    Face();
    Face(Face&& other);
    ~Face();
    friend VResult<Face>;
  };

  GLuint mId;
};

} // namespace Gfx

#endif