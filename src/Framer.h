#ifndef Framer_h
#define Framer_h

namespace Framer {

void SetFramerate(int framerate);
void VSyncFrameLimiting();
void Start();
void End();
float DeltaTime();

} // namespace Framer

#endif