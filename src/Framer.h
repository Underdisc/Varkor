#ifndef Framer_h
#define Framer_h

namespace Framer {

extern float nAverageUpdatePeriod;

void Init();
void Purge();

void Start();
void End();

void SetVSync(bool enable);
void SetFramerate(int framerate);
void SetDefaultFramerate();
void SetFrameAverageCount(int count);

bool VSyncEnabled();
bool FramerateUncapped();

float FrameTime();
float GetAverageFrameUsage();
float GetAverageFps();
int GetFramerate();

} // namespace Framer

#endif
