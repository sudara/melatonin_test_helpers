#pragma once

namespace melatonin
{
    struct isBetween : Catch::Matchers::MatcherGenericBase
    {
        float min;
        float max;

        isBetween (float m, float x) : min (m), max (x) {}

        template <typename SampleType>
        [[nodiscard]] bool match (std::vector<SampleType> array) const
        {
            jassert (min < max);

            for (auto& item : array)
                if (item < min - std::numeric_limits<SampleType>::epsilon() * 100 || item > max + std::numeric_limits<SampleType>::epsilon() * 100)
                    return false;
            return true;
        }

        [[nodiscard]] std::string describe() const override
        {
            std::ostringstream ss;
            ss << "\n items are between "
               << min << " and " << max << "\n";
            return ss.str();
        }
    };
}
