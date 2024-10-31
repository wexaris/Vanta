#pragma once

namespace Vanta {
    namespace StaticString {
        /// <summary>
        /// Data owning string suitable for constexpr function return types.
        /// </summary>
        template<size_t N>
        struct StaticString {
            char Data[N];
        };

        /// <summary>
        /// Removes certain parts of an input string at compile time.
        /// </summary>
        /// <param name="string">Input string</param>
        /// <param name="remove">Removable text</param>
        /// <returns></returns>
        template<size_t N, size_t K>
        constexpr StaticString<N> ConstStringDelete(const char(&string)[N], const char(&remove)[K]) {
            StaticString<N> result = {};

            size_t srcIndex = 0;
            size_t dstIndex = 0;
            while (srcIndex < N) {
                // Check ahead for complete match
                size_t matchIndex = 0;
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
        template<size_t N>
        constexpr StaticString<N> SignatureClean(const char(&signature)[N]) {
            return ConstStringDelete(signature, "__cdecl ");
        }

        /// <summary>
        /// Get the name from a function signature.
        /// Removes parameters, template arguments, and other extra symbols.
        /// </summary>
        template<size_t N>
        constexpr StaticString<N> SignatureName(const char(&signature)[N]) {
            auto clean_sig = SignatureClean(signature);
            auto clean = clean_sig.Data;

            StaticString<N> result = {};

            // Find beginning of parameter list
            size_t endIndex = N - 1;
            for (size_t parenLvl = 0; endIndex > 0; endIndex--) {
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
            size_t startIndex = endIndex - 1;
            for (size_t chevLvl = 0; startIndex > 0; startIndex--) {
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
            for (size_t dstIndex = 0; startIndex < endIndex; dstIndex++, startIndex++) {
                result.Data[dstIndex] = clean[startIndex];
            }

            return result;
        }

        /// <summary>
        /// Parses a filepath and returns only the filename.
        /// Mainly used in the `VANTA_FILENAME` macro.
        /// </summary>
        constexpr const char* PathFileName(const char* path) {
            const char* file = path;

            while (char c = *path++)
                if (c == '/' || c == '\\')
                    file = path;

            return file;
        }
    }
}
