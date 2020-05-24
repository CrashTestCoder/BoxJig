#pragma once

#include "Instruction.hpp"
#include <filesystem>
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>

enum JobType { GapSize,
               nFinger,
               END };

constexpr std::string_view jobType[] = {
    [JobType::GapSize] = "GapSize",
    [JobType::nFinger] = "nFinger"
};

struct JobInfo
{
    JobType job_type = END;
    bool invert;
    double blade_width;
    double board_width;
    double cut_overlap; // in blade_widths

    double cut_width;
    double finger_width;

    int num_fingers;

    double current_position = 0;

    constexpr void getGapWidths()
    {
        if(job_type == JobType::GapSize)
        {
            if(invert)
                std::swap(cut_width, finger_width);
            return;
        }
        cut_width    = board_width / (2 * num_fingers);
        finger_width = cut_width;
    }

    friend std::ostream& operator<<(std::ostream& out, std::optional<JobInfo> ji);
};

[[nodiscard]] extern JobInfo getJobInfo(std::filesystem::path path);

[[nodiscard]] extern std::vector<Instruction> getInstructions(JobInfo job_info);
