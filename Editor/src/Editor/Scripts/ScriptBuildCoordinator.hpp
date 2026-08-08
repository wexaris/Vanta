#pragma once

#include "ScriptBuilder.hpp"

namespace Vanta {
    namespace Editor {

        class ScriptBuildCoordinator {
        public:
            void SetBuildConfiguration(ScriptBuildConfiguration configuration);
            ScriptBuildConfiguration GetBuildConfiguration() const;

            bool RebuildNative(bool clean = false);
            bool RebuildCSharp(bool clean = false);

        private:
            NativeScriptBuilder m_NativeBuilder;
            CSharpScriptBuilder m_CSharpBuilder;
            ScriptBuildConfiguration m_BuildConfiguration = ScriptBuildConfiguration::Debug;

            bool RebuildWithBuilder(const ScriptBuilder& builder, const Path& projectDirectory, bool clean, const char* failurePrefix);
        };

    }
}