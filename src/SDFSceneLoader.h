#ifndef __SDFSCENELOADER
#define __SDFSCENELOADER

#include <unordered_map>

#include "sdf_scenes/SDFMandelbulb.h"
#include "sdf_scenes/SDFJulia.h"
#include "sdf_scenes/SDFDoohickey.h"
#include "sdf_scenes/SDFPeanut.h"
#include "sdf_scenes/SDFHelix.h"
#include "sdf_scenes/SDFMorph.h"
#include "sdf_scenes/SDFSharkEgg.h"
#include "sdf_scenes/SDFDonut.h"
#include "sdf_scenes/SDFCoolS.h"
#include "PrettyLogger.h"


using namespace std;

unordered_map<const char *, Scene(*)()>
options = {
    {"julia", loadSDFJuliaScene},
    {"mandel", loadSDFMandelbulbScene},
    {"doohickey", loadSDFDoohickeyScene},
    {"helix", loadSDFHelixScene},
    {"morph", loadSDFMorphScene},
    {"sharkegg", loadSDFSharkEggScene},
    {"donut", loadSDFDonutScene},
    {"cools", loadSDFCoolSScene}
};

Scene loadWithSDFLoader(const char *scene) {
    if (options.find(scene) != options.end())
        return options[scene]();
    printf("SDF option not found available sdf options:\n");
    for (const auto &option: options) {
        printf("\t%s\n", option.first);
        if (strequal(option.first, scene)) {
            return option.second();
        }
    }
    LOG_FATAL("No sdf scene with name \"%s\" has been implemented", scene)
}

#endif
