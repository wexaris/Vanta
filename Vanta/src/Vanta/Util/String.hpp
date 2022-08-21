#pragma once

namespace Vanta {
    namespace Util {
        /// Trim whitespace on left
        static inline void TrimLeft(std::string& s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
        }

        /// Trim whitespace on right
        static inline void TrimRight(std::string& s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(), s.end());
        }

        /// Trim whitespace
        static inline void Trim(std::string& s) {
            TrimLeft(s);
            TrimRight(s);
        }
    }
}