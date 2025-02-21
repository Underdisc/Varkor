#include "ext/Tracy.h"

void Profile(void (*function)(), int count) {
  ZoneScopedC(0xFFFFFF);
  for (int i = 0; i < count; ++i) {
    function();
  }
}