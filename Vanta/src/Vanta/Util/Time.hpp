#pragma once
#include <chrono>

namespace Vanta {
    /// <summary>
    /// A single moment in time.
    /// Easy-to-use wrapper around `chrono::time_point`.
    /// </summary>
    struct Time {
        std::chrono::time_point<std::chrono::steady_clock> Point;

        Time() : Point(std::chrono::steady_clock::now()) {}

        long long AsSeconds() const {
            return std::chrono::time_point_cast<std::chrono::seconds>(Point).time_since_epoch().count();
        }

        long long AsMilliseconds() const {
            return std::chrono::time_point_cast<std::chrono::milliseconds>(Point).time_since_epoch().count();
        }

        long long AsMicroseconds() const {
            return std::chrono::time_point_cast<std::chrono::microseconds>(Point).time_since_epoch().count();
        }

        long long AsNanoseconds() const {
            return std::chrono::time_point_cast<std::chrono::nanoseconds>(Point).time_since_epoch().count();
        }

        double AsSecondsf() const      { return AsNanoseconds() * 1E-9; }
        double AsMillisecondsf() const { return AsNanoseconds() * 1E-6; }
        double AsMicrosecondsf() const { return AsNanoseconds() * 1E-3; }
    };


    /// <summary>
    /// A span of time.
    /// Easy-to-use wrapper around `chrono::nanoseconds`.
    /// </summary>
    struct Duration {
        std::chrono::nanoseconds Span;

        Duration(const std::chrono::nanoseconds& amount) : Span(amount) {}
        Duration(const Time& from, const Time& to) : Span(to.Point - from.Point) {}

        static Duration SinceLaunch();

        long long AsSeconds() const {
            return std::chrono::duration_cast<std::chrono::seconds>(Span).count();
        }

        long long AsMilliseconds() const {
            return std::chrono::duration_cast<std::chrono::milliseconds>(Span).count();
        }

        long long AsMicroseconds() const {
            return std::chrono::duration_cast<std::chrono::microseconds>(Span).count();
        }

        long long AsNanoseconds() const {
            return std::chrono::duration_cast<std::chrono::nanoseconds>(Span).count();
        }

        double AsSecondsf() const      { return AsNanoseconds() * 1E-9; }
        double AsMillisecondsf() const { return AsNanoseconds() * 1E-6; }
        double AsMicrosecondsf() const { return AsNanoseconds() * 1E-3; }

        Duration operator-(const Duration& other) { return Duration(Span - other.Span); }
        Duration operator+(const Duration& other) { return Duration(Span + other.Span); }
    };


    inline Duration operator-(const Time& first, const Time& second) {
        return Duration(first.Point - second.Point);
    }
}
