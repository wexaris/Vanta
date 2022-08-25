#include "Editor/Window/Console.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

namespace Vanta {
    namespace Editor {

        Console Console::s_Instance;

        void Console::Log(const std::string& message) {
            usize idx = (m_ItemCount + m_OldestItemIndex) % MAX_ITEM_COUNT;
            m_Items[idx] = message;

            if (m_ItemCount < MAX_ITEM_COUNT) { m_ItemCount++; }
            else { m_OldestItemIndex = (m_OldestItemIndex + 1) % MAX_ITEM_COUNT; }
        }

        void Console::Execute(const std::string& input) {
            Log("# " + input + "\n");
            m_ScrollToBottom = true; // On input, scroll to bottom even if auto scroll is off
        }

        void Console::Clear() {
            m_ItemCount = 0;
            m_OldestItemIndex = 0;
        }

        void Console::OnGUIRender() {
            if (!m_Open)
                return;

            ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);

            if (!ImGui::Begin("Console", &m_Open)) {
                ImGui::End();
                return;
            }

            // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
            // So e.g. IsItemHovered() will return true when hovering the title bar.
            // Here we create a context menu only available from the title bar.
            /*if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Close"))
                    m_Open = false;
                ImGui::EndPopup();
            }*/

            // Reserve enough left-over height for 1 separator + 1 input text
            const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
            ImGui::BeginChild("ConsoleTextBox", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
            if (ImGui::BeginPopupContextWindow()) {
                if (ImGui::Selectable("Clear"))
                    Clear();
                ImGui::EndPopup();
            }

            // Disable auto scroll if user has scrolled away from bottom
            m_AutoScroll = !(ImGui::GetScrollY() < ImGui::GetScrollMaxY());

            // Display every line as a separate entry so we can change their color or add custom widgets.
            // If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
            // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
            // to only process visible items. The clipper will automatically measure the height of your first item and then
            // "seek" to display only items in the visible area.
            // To use the clipper we can replace your standard loop:
            //      for (int i = 0; i < Items.Size; i++)
            //   With:
            //      ImGuiListClipper clipper;
            //      clipper.Begin(Items.Size);
            //      while (clipper.Step())
            //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            // - That your items are evenly spaced (same height)
            // - That you have cheap random access to your elements (you can access them given their index,
            //   without processing all the ones before)
            // You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
            // We would need random-access on the post-filtered list.
            // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
            // or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
            // and appending newly elements as they are inserted. This is left as a task to the user until we can manage
            // to improve this example code!
            // If your items are of variable height:
            // - Split them into same height items would be simpler and facilitate random-seeking into your list.
            // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

            for (usize i = 0; i < m_ItemCount; i++) {
                usize idx = (i + m_OldestItemIndex) % MAX_ITEM_COUNT;
                ImGui::TextUnformatted(m_Items[idx].c_str());
            }

            // Auto scroll
            if (m_ScrollToBottom || (m_AutoScroll && ImGui::GetScrollY() <= ImGui::GetScrollMaxY()))
                ImGui::SetScrollHereY(1.0f);
            m_ScrollToBottom = false;

            ImGui::PopStyleVar();
            ImGui::EndChild();
            ImGui::Separator();

            // Command-line
            bool reclaim_focus = false;
            ImGuiInputTextFlags textbox_flags = ImGuiInputTextFlags_EnterReturnsTrue;

            static std::string buffer;
            if (ImGui::InputText("Input", &buffer, textbox_flags, nullptr, nullptr)) {
                if (!buffer.empty())
                    Execute(buffer);
                reclaim_focus = true;
            }

            // Auto-focus on window apparition
            ImGui::SetItemDefaultFocus();
            if (reclaim_focus)
                ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

            ImGui::End();
        }
    }
}
