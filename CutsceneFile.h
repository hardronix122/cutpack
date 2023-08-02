#ifndef CUTPACK_CUTSCENEFILE_H
#define CUTPACK_CUTSCENEFILE_H

#include <cstdint>

struct CutsceneFile {
    uint32_t offset;
    uint32_t size;
    uint32_t name;
    uint32_t extension;
};

#endif //CUTPACK_CUTSCENEFILE_H
