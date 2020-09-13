#ifndef texture_h
#define texture_h

class Texture
{
public:
  Texture(const char* textureFile);
  ~Texture();
  void Purge();
  int Width() const;
  int Height() const;
  int Channels() const;
  unsigned int Id() const;

private:
  int mWidth;
  int mHeight;
  int mChannels;
  unsigned int mId;
};

#endif