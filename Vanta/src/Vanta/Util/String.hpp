#pragma once

namespace Vanta {
    namespace String {

        /// Trim whitespace on left of string
        inline void TrimLeft(std::string& s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch);
                }));
        }

        /// Trim whitespace on right of string
        inline void TrimRight(std::string& s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
                }).base(), s.end());
        }

        /// Trim string whitespace
        inline void Trim(std::string& s) {
            TrimLeft(s);
            TrimRight(s);
        }

        /// Trim whitespace on left of string
        inline std::string TrimLeft(const std::string& s) {
            std::string new_str = s;
            TrimLeft(new_str);
            return new_str;
        }

        /// Trim whitespace on right of string
        inline std::string TrimRight(const std::string& s) {
            std::string new_str = s;
            TrimRight(new_str);
            return new_str;
        }

        /// Trim string whitespace
        inline std::string Trim(const std::string& s) {
            std::string new_str = s;
            Trim(new_str);
            return new_str;
        }
    }
}
