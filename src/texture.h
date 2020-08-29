#ifndef texture_h
#define texture_h

class Texture
{
public:
  Texture(const char* texture_file);
  ~Texture();
  void Purge();
  int Width() const;
  int Height() const;
  int Channels() const;
  unsigned int Id() const;

private:
  int _width;
  int _height;
  int _channels;
  unsigned int _id;
};

#endif