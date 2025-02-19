#ifndef __SDFSCENELOADER
#define __SDFSCENELOADER

#include <unordered_map>
#include <string>

#include "sdf_scenes/SDFMandelbulb.h"
#include "sdf_scenes/SDFJulia.h"
#include "sdf_scenes/SDFDoohickey.h"
#include "sdf_scenes/SDFPeanut.h"
#include "sdf_scenes/SDFHelix.h"
#include "sdf_scenes/SDFMorph.h"
#include "sdf_scenes/SDFSharkEgg.h"
#include "sdf_scenes/SDFDonut.h"
#include "sdf_scenes/SDFCoolS.h"
#include "sdf_scenes/SDFRevolvedBox.h"
#include "sdf_scenes/SDFRevolvedCross.h"
#include "sdf_scenes/SDFPeanut.h"
#include "sdf_scenes/SDFIntersect.h"
#include "sdf_scenes/SDFUnion.h"
#include "PrettyLogger.h"

using namespace std;

unordered_map<string, Scene(*)()>
options = {
    {"julia", loadSDFJuliaScene},
    {"mandel", loadSDFMandelbulbScene},
    {"doohickey", loadSDFDoohickeyScene},
    {"helix", loadSDFHelixScene},
    {"morph", loadSDFMorphScene},
    {"sharkegg", loadSDFSharkEggScene},
    {"donut", loadSDFDonutScene},
    {"cools", loadSDFCoolSScene},
    {"rbox", loadSDFRevolvedBoxScene},
    {"rcross", loadSDFRevolvedCrossScene},
    {"peanut", loadSDFPeanutScene},
    {"intersect", loadSDFIntersectScene},
    {"union", loadSDFUnionScene}
};

Scene loadWithSDFLoader(string scene) {
    if (options.find(scene) != options.end())
        return options[scene]();
    printf("SDF option not found available sdf options:\n");
    for (const auto &option: options) {
        printf("\t%s\n", option.first.c_str());
    }
    LOG_FATAL("No sdf scene with name \"%s\" has been implemented", scene.c_str())
}

#endif
