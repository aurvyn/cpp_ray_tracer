#ifndef __SDFSCENELOADER
#define __SDFSCENELOADER

#include "sdf_scenes/SDFMandelbulb.h"
#include "sdf_scenes/SDFJulia.h"
#include "sdf_scenes/SDFDoohickey.h"
#include "sdf_scenes/SDFPeanut.h"
#include "sdf_scenes/SDFHelix.h"
#include "sdf_scenes/SDFMorph.h"
#include "sdf_scenes/SDFSharkEgg.h"
#include "sdf_scenes/SDFDonut.h"
#include "PrettyLogger.h"
#include <string>

Scene loadWithSDFLoader(const char* scene) {
    std::string name(scene);
    if (name == "julia") return loadSDFJuliaScene();
    if (name == "mandelbulb") return loadSDFMandelbulbScene();
    if (name == "doohickey") return loadSDFDoohickeyScene();
    if (name == "peanut") return loadSDFPeanutScene();
    if (name == "helix") return loadSDFHelixScene();
    if (name == "morph") return loadSDFMorphScene();
    if (name == "sharkegg") return loadSDFSharkEggScene();
    if (name == "donut") return loadSDFDonutScene();
    LOG_FATAL("No sdf scene with name \"%s\" has been implemented", scene);
    Scene deadscene;
    return deadscene;
}

#endif