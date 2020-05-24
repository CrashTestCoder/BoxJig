#include "Button.hpp"
#include "JobInfo.hpp"
#include <filesystem>

#if __has_include(<wiringPi.h>)
#    include <wiringPi.hpp>
#else
#    include "wiringPiStandin.hpp"
#endif

std::filesystem::path serial_dir { "/dev/ttyACM0" };

int main()
{
    wiringPiSetup();
    Button slide_switch { 1 };
    auto job_info = getJobInfo("job_info.json");
    std::cout << job_info;
    for(auto instr : getInstructions(job_info))
    {
        instr.send();
        slide_switch.waitForNext(0);
    }
    return 0;
}