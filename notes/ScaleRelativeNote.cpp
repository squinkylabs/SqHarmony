#include "ScaleRelativeNote.h"


bool ScaleRelativeNote::isTonal() const {
    bool fRet = false;

    switch (pitch) {
        case 1:
        case 4:
        case 5:
            fRet = true;
    }
    return fRet;
}
