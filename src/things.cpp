#include "things.h"

void thingsAdd(ThingerESP32 &thing)
{
  thing["GPIO_LED_BUILTIN"] << digitalPin(LED_BUILTIN);
}