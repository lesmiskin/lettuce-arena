#ifndef SCENE_H
#define SCENE_H

#include "common.h"
#include "renderer.h"

#include <stdbool.h>

extern void sceneRenderFrame(void);
extern void sceneAnimateFrame(void);
extern void sceneGameFrame();
extern void initScene();

#endif
