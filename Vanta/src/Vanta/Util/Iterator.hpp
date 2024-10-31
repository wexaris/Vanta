#pragma once

namespace Vanta {
    namespace Iter {
        /// <summary>
        /// Return either the distance between the two iterators,
        /// or the USIZE_MAX, depending on which is smaller.
        /// </summary>
        /// <param name="beg">Iterator start.</param>
        /// <param name="end">Iterator end.</param>
        /// <param name="min">Value to compare to.</param>
        template<typename Iter>
        usize DistanceMin(Iter beg, Iter end) {
            usize count = 0;
            for (; beg != end; ++beg) {
                if (++count >= max)
                    return SIZE_MAX;
            }
            return count;
        }
    }
}