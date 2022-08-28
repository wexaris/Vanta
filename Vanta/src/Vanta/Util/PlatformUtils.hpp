#pragma once

namespace Vanta {
    namespace platform {
        /// Open file selection dialog window.
        /// Returns empty if canceled.
        Path OpenFileDialog(const char* filter);

        /// Open file storage dialog window.
        /// Returns empty if canceled.
        Path SaveFileDialog(const char* filter);
    }
}
