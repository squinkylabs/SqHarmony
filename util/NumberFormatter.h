
#pragma once

#include <string>
class NumberFormatter {
public:
    static std::string formatFloat(int decimalPlaces, const std::string s) {
        auto pos = s.find('.');
        assert(pos != std::string::npos);
        const int excess = s.length() - (pos + 3);
        SQINFO("excess =%d", excess);
        if (excess > 0) {
            return s.substr(0, s.length() - excess);
        }
        assert(false);
        return "";
    }
};