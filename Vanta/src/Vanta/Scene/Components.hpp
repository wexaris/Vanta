#pragma once
#include "Vanta/Scene/Components/IDComponent.hpp"
#include "Vanta/Scene/Components/TransformComponent.hpp"
#include "Vanta/Scene/Components/CameraComponent.hpp"
#include "Vanta/Scene/Components/Rigidbody2DComponent.hpp"
#include "Vanta/Scene/Components/BoxCollider2DComponent.hpp"
#include "Vanta/Scene/Components/CircleCollider2DComponent.hpp"
#include "Vanta/Scene/Components/SpriteComponent.hpp"
#include "Vanta/Scene/Components/CircleRendererComponent.hpp"
#include "Vanta/Scene/Components/ScriptComponent.hpp"
#include "Vanta/Scene/Components/NativeScriptComponent.hpp"
#include <entt/entt.hpp>

namespace Vanta {

    /// <summary>
    /// Helper type for grouping and shuttling components around.
    /// </summary>
    template<typename... Components>
    using ComponentList = entt::type_list<Components...>;

    //namespace Template {
    //    template<usize N, typename... Types>
    //    struct Get<N, ComponentList<Types...>> : Get<N, Types...> {};
    //
    //    template <typename T, typename... Types>
    //    struct Contains<T, ComponentList<Types...>> : Contains<Types...> {};
    //}

    using AllComponents = ComponentList<TransformComponent, CameraComponent,
        Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent,
        SpriteComponent, CircleRendererComponent, ScriptComponent, NativeScriptComponent>;
}
