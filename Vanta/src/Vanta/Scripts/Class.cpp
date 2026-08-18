#include "vantapch.hpp"
#include "Vanta/Scripts/Class.hpp"
#include "Vanta/Scripts/Field.hpp"

namespace Vanta {
    namespace Scripts {

        ScriptClass::ScriptClass(std::vector<Ref<ScriptField>> fields) {
            VANTA_PROFILE_FUNCTION();
            std::transform(
                std::make_move_iterator(fields.begin()),
                std::make_move_iterator(fields.end()),
                std::inserter(m_Fields, m_Fields.end()),
                [](Ref<ScriptField>&& field) {
                    return std::make_pair(field->Name, std::move(field));
                }
            );
        }
    }
}
