
#pragma once

#include <string>
class NumberFormatter {
public:
    static std::string formatFloat(int decimalPlaces, const std::string s) {
        auto pos = s.find('.');
        if (pos == std::string::npos) {
            return s + ".00";
        }

        const int excess = s.length() - (pos + 3);
        if (excess > 0) {
            return s.substr(0, s.length() - excess);
        }
        if (excess < 0) {
            auto sret = s;
            for (int i = 0; i < -excess; ++i) {
                //sret.pop_back();
                sret.push_back('0');
            }
            return sret;
        }
        return s;
    }
};