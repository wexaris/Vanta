#pragma once

#include <Vanta/Vanta.hpp>

namespace Vanta {

    struct FileSystem {
        /// Open folder selection dialog window.
        /// Returns empty if canceled.
        static Path OpenDirectoryDialog();

        /// Open file selection dialog window.
        /// Returns empty if canceled.
        static Path OpenFileDialog(const char* filter);

        /// Open file storage dialog window.
        /// Returns empty if canceled.
        static Path SaveFileDialog(const char* filter);
    };
}