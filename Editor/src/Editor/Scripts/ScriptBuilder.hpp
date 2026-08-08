#pragma once

#include <Vanta/Vanta.hpp>

#include "ScriptBuildTypes.hpp"

namespace Vanta {
    namespace Editor {

        class ScriptBuilder {
        public:
            virtual ~ScriptBuilder() = default;
            virtual bool Build(const Path& projectDirectory, const ScriptBuildOptions& options) const = 0;
        };

        class NativeScriptBuilder final : public ScriptBuilder {
        public:
            bool Build(const Path& projectDirectory, const ScriptBuildOptions& options) const override;
        };

        class CSharpScriptBuilder final : public ScriptBuilder {
        public:
            bool Build(const Path& projectDirectory, const ScriptBuildOptions& options) const override;
        };

    }
}