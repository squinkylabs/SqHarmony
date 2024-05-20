
#pragma once

#include <assert.h>

#include <iomanip>
#include <sstream>
#include <string>

class NumberFormatter {
public:
    // helper function.
    static std::string formatFloat(int decimalPlaces, const std::string s) {
        assert(decimalPlaces == 2);  // only case implemented

        // if tinny - negative exponential, then round to zero.
        auto pos = s.find("e-");
        if (pos != std::string::npos) {
            return "0.00";
        }

        pos = s.find('e');
        if (pos != std::string::npos) {
            return "xxx";
        }

        pos = s.find('.');
        if (pos == std::string::npos) {
            return s + ".00";
        }

        const int excess = int(s.length() - (pos + 3));
        if (excess > 0) {
            return s.substr(0, s.length() - excess);
        }
        if (excess < 0) {
            auto sret = s;
            for (int i = 0; i < -excess; ++i) {
                // sret.pop_back();
                sret.push_back('0');
            }
            return sret;
        }
        return s;
    }

    // full float to string function
    static std::string formatFloat2(int decimalPlaces, float x) {
        std::stringstream str;
        str << std::fixed;
        if (x >= 100) {
            str << std::setprecision(2) << x;
            const auto s = str.str();
            auto pos = s.find('e');
            return (pos == std::string::npos) ? s : "xxx";
        } else if (x > 99) {
            str << std::setprecision(4) << x;
            const auto s = str.str();
            // auto pos = s.find('e');
            // return (pos == std::string::npos) ? s : "xxx";
            return s;
        } else {
            str << std::setprecision(4) << x;
            const auto s = str.str();
            return formatFloat(2, s);
        }
    }
};