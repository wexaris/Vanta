#include "vantapch.hpp"
#include "Vanta/Util/Time.hpp"
#include "Vanta/Util/Util.hpp"

namespace Vanta {

    static Time s_LaunchTime;

    Duration Duration::SinceLaunch() {
        return Time() - s_LaunchTime;
    }
}
