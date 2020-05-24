#include "JobInfo.hpp"
#include "json.hpp"
#include <fstream>
#include <iomanip>
#include <string>
#include <type_traits>
#include <vector>

namespace util
{
    template <typename T>
    std::string to_string(const T& t)
    {
        std::string str { std::to_string(t) };
        int offset { 1 };
        if(str.find_last_not_of('0') == str.find('.'))
        {
            offset = 0;
        }
        str.erase(str.find_last_not_of('0') + offset, std::string::npos);
        return str;
    }
} // namespace util

std::ostream& operator<<(std::ostream& out, std::optional<JobInfo> ji)
{
    constexpr int width = 10;
    if(! ji)
        return out;
    auto& ji_ = ji.value();
    out << std::setiosflags(std::ios_base::left) << std::setw(width) << "Type: ";
    out << std::setiosflags(std::ios_base::right) << std::setw(width) << jobType[ji_.job_type];
    if(! ji_.invert)
        out << '\n';
    else
        out << " Inverted\n";
    out << std::setiosflags(std::ios_base::left) << std::setw(width) << "Blade Width: ";
    out << std::setiosflags(std::ios_base::right) << std::setw(width) << ji_.blade_width << '\n';
    out << std::setiosflags(std::ios_base::left) << std::setw(width) << "Board Width: ";
    out << std::setiosflags(std::ios_base::right) << std::setw(width) << ji_.board_width << '\n';
    out << std::setiosflags(std::ios_base::left) << std::setw(width) << "Overlap: ";
    out << std::setiosflags(std::ios_base::right) << std::setw(width) << ji_.cut_overlap << '\n';
    switch(ji_.job_type)
    {
    case JobType::GapSize:
        out << std::setiosflags(std::ios_base::left) << std::setw(width) << "Cut Size: ";
        out << std::setw(width) << std::setiosflags(std::ios_base::right) << ji_.cut_width << '\n';
        out << std::setiosflags(std::ios_base::left) << std::setw(width) << "Finger Size: ";
        out << std::setw(width) << std::setiosflags(std::ios_base::right) << ji_.finger_width << '\n';
        break;

    case JobType::nFinger:
        out << std::setiosflags(std::ios_base::left) << std::setw(width) << "Fingers: " << std::setiosflags(std::ios_base::right) << ji_.num_fingers << '\n';
        break;

    case END:
        out << "jobtype error\n";
    }
    return out;
}

[[nodiscard]] JobInfo getJobInfo(std::filesystem::path path)
{
    std::ifstream file { path };
    if(! file)
        if(! std::filesystem::exists(path))
        {
            std::cerr << "Specified path " << path << " does not exist\n";
            abort();
        }
    nlohmann::json data;
    JobInfo job_info;
    file >> data;
    for(int jt { 0 }; jt < END; jt++)
        if(data.at("type").get<std::string>() == jobType[jt])
            job_info.job_type = static_cast<JobType>(jt);
    if(job_info.job_type == END)
    {
    }
    data.at("invert").get_to(job_info.invert);
    data.at("blade_width").get_to(job_info.blade_width);
    data.at("board_width").get_to(job_info.board_width);
    data.at("overlap").get_to(job_info.cut_overlap);

    switch(job_info.job_type)
    {
    case JobType::GapSize:
        data.at("cut_width").get_to(job_info.cut_width);
        data.at("finger_width").get_to(job_info.finger_width);
        break;

    case JobType::nFinger:
        data.at("fingers").get_to(job_info.num_fingers);
        job_info.getGapWidths();
        break;

    case JobType::END:
        std::cerr << "Invalid JobType: " << data["type"] << '\n';
        abort();
    }
    return job_info;
}

[[nodiscard]] std::optional<JobInfo> getJobInfo_old(std::filesystem::path path)
{
    std::ifstream file { path };
    if(! file)
    {
        if(! std::filesystem::exists(path))
            std::cerr << "Job Info directory " << path << " does not exist\n";
    }
    JobInfo job_info;

    std::string garbage;
    std::string job_type;
    file >> job_type;
    file >> garbage;
    file >> job_info.invert;
    file >> garbage;
    file >> job_info.blade_width;
    file >> garbage;
    file >> job_info.board_width;
    file >> garbage;
    file >> job_info.cut_overlap;

    if(job_type == jobType[JobType::GapSize])
    {
        job_info.job_type = JobType::GapSize;
        file >> garbage;
        file >> job_info.cut_width;
        file >> garbage;
        file >> job_info.finger_width;
    }
    else if(job_type == jobType[JobType::nFinger])
    {
        job_info.job_type = JobType::nFinger;
        file >> garbage;
        file >> job_info.num_fingers;
    }
    else
    {
        std::cerr << "Invalid JobType: " << job_type << '\n';
        return {};
    }
    return job_info;
}

void cut(std::vector<double>& instr, JobInfo& job_info)
{
    double end = job_info.current_position + job_info.cut_width;
    if(end > job_info.board_width)
        end = job_info.board_width;

    job_info.current_position += job_info.blade_width;

    for(; job_info.current_position <= end; job_info.current_position += job_info.blade_width * job_info.cut_overlap)
        instr.emplace_back(job_info.current_position);
    if(job_info.current_position - job_info.blade_width * job_info.cut_overlap < end - .001)
        instr.emplace_back(end);
    job_info.current_position = end;
}

[[nodiscard]] std::vector<double> getPositions(JobInfo& job_info)
{
    std::vector<double> positions;
    if(job_info.invert)
        cut(positions, job_info);
    while(job_info.current_position < job_info.board_width)
    {
        job_info.current_position += job_info.finger_width;
        cut(positions, job_info);
    }
    positions.emplace_back(0);
    return positions;
}

[[nodiscard]] Instruction sendTo_Instr(double pos)
{
    using namespace std::literals::string_literals;
    return Instruction { "G1 Z"s + util::to_string(pos) + "\n\r" };
}

std::vector<Instruction> getInstructions(JobInfo job_info)
{
    std::vector<Instruction> instructions;
    for(auto const& pos : getPositions(job_info))
        instructions.emplace_back(sendTo_Instr(pos));
    return instructions;
}