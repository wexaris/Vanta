#pragma once

namespace Vanta {
    namespace Util {

        /// <summary>
        /// Return either the distance between the two iterators,
        /// or the maximum value, depending on which is smaller.
        /// </summary>
        /// <param name="beg">Iterator start.</param>
        /// <param name="end">Iterator end.</param>
        /// <param name="min">Value to compare to.</param>
        template<typename Iter>
        usize DistanceMin(Iter beg, Iter end, usize max) {
            usize count = 0;
            for (; beg != end; ++beg) {
                if (++count >= max)
                    return max;
            }
            return count;
        }
    }
}
