#pragma once

#include <chrono>

namespace parallel_db::utils
{
class Timer
{
public:
    Timer() : time_( clock_::now() ) { }

    void reset() { time_ = clock_::now(); }

    [[nodiscard]] long getTime() const
    {
        return std::chrono::duration_cast< std::chrono::milliseconds >( clock_::now() - time_ )
            .count();
    }

private:
    typedef std::chrono::high_resolution_clock clock_;
    std::chrono::time_point< clock_ > time_;
};
} // namespace parallel_db::utils