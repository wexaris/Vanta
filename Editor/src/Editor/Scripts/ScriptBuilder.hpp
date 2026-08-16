#pragma once

#include <Vanta/Vanta.hpp>
#include <Vanta/Project/Project.hpp>

#include "ScriptBuildOptions.hpp"

namespace Vanta {
    namespace Editor {

        class ScriptBuilder {
        public:
            virtual ~ScriptBuilder() = default;
            virtual bool Build(const ScriptBuildOptions& options) const = 0;
        };

        class NativeScriptBuilder final : public ScriptBuilder {
        public:
            bool Build(const ScriptBuildOptions& options) const override;
        };

        class CSharpScriptBuilder final : public ScriptBuilder {
        public:
            bool Build(const ScriptBuildOptions& options) const override;
        };

    }
}