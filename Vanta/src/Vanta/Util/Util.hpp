#pragma once

namespace Vanta {
    namespace Util {

        /// ///////////// STRING //////////////////////////

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


        /// ///////////// ITERATOR ////////////////////////

        /// <summary>
        /// Return either the distance between the two iterators,
        /// or the maximum value, depending on which is smaller.
        /// </summary>
        /// <param name="beg">Iterator start.</param>
        /// <param name="end">Iterator end.</param>
        /// <param name="min">Value to compare to.</param>
        template<typename Iter>
        usize DistanceMin(Iter beg, Iter end, usize max) {
            usize count = 0;
            for (; beg != end; ++beg) {
                if (++count >= max)
                    return max;
            }
            return count;
        }
    }

    namespace StaticUtil {
        /// <summary>
        /// Data owning string suitable for constexpr function return types.
        /// </summary>
        template<usize N>
        struct ResultString {
            char Data[N];
        };

        /// <summary>
        /// Removes certain parts of an input string at compile time.
        /// </summary>
        /// <param name="string">Input string</param>
        /// <param name="remove">Removable text</param>
        /// <returns></returns>
        template<usize N, usize K>
        constexpr ResultString<N> StringRemove(const char(&string)[N], const char(&remove)[K]) {
            ResultString<N> result = {};

            usize srcIndex = 0;
            usize dstIndex = 0;
            while (srcIndex < N) {
                // Check ahead for complete match
                usize matchIndex = 0;
                while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && string[srcIndex + matchIndex] == remove[matchIndex])
                    matchIndex++;

                // Skip text if matched
                if (matchIndex == K - 1)
                    srcIndex += matchIndex;

                // Copy relevant text to result
                result.Data[dstIndex++] = (string[srcIndex] == '"') ? '\'' : string[srcIndex];
                srcIndex++;
            }
            return result;
        }

        /// <summary>
        /// Cleans up a function signature, removing unnecessary information.
        /// </summary>
        template<usize N>
        constexpr ResultString<N> SignatureClean(const char(&signature)[N]) {
            return StringRemove(signature, "__cdecl ");
        }

        /// <summary>
        /// Get the name from a function signature.
        /// Removes parameters, template arguments, and other extra symbols.
        /// </summary>
        template<usize N>
        constexpr ResultString<N> SignatureName(const char(&signature)[N]) {
            auto clean_sig = SignatureClean(signature);
            auto clean = clean_sig.Data;

            ResultString<N> result = {};

            // Find beginning of parameter list
            usize endIndex = N - 1;
            for (usize parenLvl = 0; endIndex > 0; endIndex--) {
                if (clean[endIndex] == ')') {
                    parenLvl++;
                }
                else if (clean[endIndex] == '(') {
                    parenLvl--;

                    if (parenLvl == 0)
                        break;
                }
            }

            // Find start of function name
            usize startIndex = endIndex - 1;
            for (usize chevLvl = 0; startIndex > 0; startIndex--) {
                if (clean[startIndex] == '>') {
                    chevLvl++;
                }
                else if (clean[startIndex] == '<') {
                    chevLvl--;
                }
                // Break if char isn't an identifier or scope (::)
                else if (!(clean[startIndex] >= 'a' && clean[startIndex] <= 'z') &&
                         !(clean[startIndex] >= 'A' && clean[startIndex] <= 'Z') &&
                         !(clean[startIndex] >= '0' && clean[startIndex] <= '9') &&
                         clean[startIndex] != '_' && clean[startIndex] != ':')
                {
                    // Don't break if inside < >
                    if (chevLvl == 0) {
                        startIndex++;
                        break;
                    }
                }
            }

            // Copy relevant text to result
            for (usize dstIndex = 0; startIndex < endIndex; dstIndex++, startIndex++) {
                result.Data[dstIndex] = clean[startIndex];
            }

            return result;
        }

        /// <summary>
        /// Parses a filepath and returns only the filename.
        /// Mainly used in the `VANTA_FILENAME` macro.
        /// </summary>
        constexpr const char* FileName(const char* path) {
            const char* file = path;

            while (char c = *path++)
                if (c == '/' || c == '\\')
                    file = path;

            return file;
        }
    }
}
