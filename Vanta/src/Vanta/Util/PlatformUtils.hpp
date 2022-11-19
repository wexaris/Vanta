#pragma once

namespace Vanta {

    struct Platform {
        /// Open file selection dialog window.
        /// Returns empty if canceled.
        static Path OpenFileDialog(const char* filter);

        /// Open file storage dialog window.
        /// Returns empty if canceled.
        static Path SaveFileDialog(const char* filter);
    };
}
