

#include "Scale.h"
#include "SharpsFlatsPref.h"

#include <assert.h>



bool resolveSharpPref(UIPrefSharpsFlats pref, const class Scale& scale ) {
    const auto scalePref = scale.getSharpsFlatsPref();
    bool ret = false;
    switch (pref) {
        case UIPrefSharpsFlats::DefaultPlusSharps:
            // only set to flats if scale is a flats scals
            ret = scalePref != SharpsFlatsPref::Flats;
            break;
        case UIPrefSharpsFlats::DefaultPlusFlats:
            // only set to sharps is scale is a sharp scales
            ret = scalePref == SharpsFlatsPref::Sharps;
            // SQINFO("in convert, ui pref = flats+def so set sharps to %d", thePrefs.sharpsOrFlats);
            break;
        case UIPrefSharpsFlats::Sharps:
            ret = true;
            break;
        case UIPrefSharpsFlats::Flats:
            ret = false;
            break;
        default:
            assert(false);
    }
    return ret;
}
