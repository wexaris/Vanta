#pragma once
#include <Vanta/Vanta.hpp>

namespace Vanta {
    namespace Editor {

        class EditorCamera : public Camera {
        public:
            EditorCamera(float, float, float, float) {

            }

            void SetViewportSize(uint /*width*/, uint /*height*/) {
                
            }

            virtual void Resize(uint /*width*/, uint /*height*/) override {

            }

        private:

        };
    }
}
