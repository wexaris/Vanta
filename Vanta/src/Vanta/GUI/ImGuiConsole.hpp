#pragma once
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink.h>
#include <mutex>

namespace Vanta {

    class ImGuiConsole {
    public:
        void Log(const std::string& message);
        void Execute(const std::string& input);

        void Clear();

        void OnGUIRender();

        static ImGuiConsole& Get() { return s_Instance; }

    private:
        constexpr static usize MAX_ITEM_COUNT = 1024;

        static ImGuiConsole s_Instance;

        std::array<std::string, MAX_ITEM_COUNT> m_Items;
        usize m_ItemCount = 0;
        usize m_OldestItemIndex = 0;
        bool m_AutoScroll = true;
        bool m_ScrollToBottom = false;
        bool m_Open = true;

        ImGuiConsole() = default;
        ~ImGuiConsole() = default;
    };

    template<typename Mutex>
    class ConsoleSink : public spdlog::sinks::base_sink<Mutex> {
    public:
        ConsoleSink() = default;
        ~ConsoleSink() = default;

    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override {
            spdlog::memory_buf_t formatted;
            spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
            std::string message = fmt::to_string(formatted);
            ImGuiConsole::Get().Log(message);
        }

        void flush_() override {}
    };

    using ConsoleSink_mt = ConsoleSink<std::mutex>;
    using ConsoleSink_st = ConsoleSink<spdlog::details::null_mutex>;
}
