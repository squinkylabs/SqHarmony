#pragma once

#include <algorithm>

#include "NoteBuffer.h"

// we don't need this any more
class NoteBufferSorter {
public:
    static void sortAscending(NoteBuffer&);
};

//inline void NoteBufferSorter::sortAscending(NoteBuffer& nb) {
//    std::sort(nb.begin(), nb.end());//
//}