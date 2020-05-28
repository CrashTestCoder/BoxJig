#include "Button.hpp"
#include "JobInfo.hpp"
#include <filesystem>
#include <fstream>
#include <mutex>
#include <regex>
#include <thread>
#include "Serial.hpp"

#if __has_include(<wiringPi.h>)
#    include <wiringPi.hpp>
#else
#    include "wiringPiStandin.hpp"
#endif

std::filesystem::path Serial::directory { "/dev/ttyACM0" };

namespace buttons
{
    Button start        = 2;
    Button slide_switch = 1;
    Button zero         = 3;
} // namespace buttons

int main()
{
    wiringPiSetup();

    while(1)
    {
        buttons::start.waitForNext(0);

        auto job_info = getJobInfo("job_info.json");
        Instruction { "G28 Z\n" }.send();

        for(auto instr : getInstructions(job_info))
        {
            instr.send();

            bool waiting = true;
            std::thread zero_listener { [&waiting] {
                buttons::slide_switch.waitForNext(0);
                waiting = false;
            } };
            bool zero_already_pressed = false;
            while(waiting)
            {
                if((! zero_already_pressed) && (zero_already_pressed = ! buttons::zero.get()))
                {
                    Instruction { "G28 Z\n" }.send();
                    std::this_thread::sleep_for(std::chrono::milliseconds(2));
                    instr.send();
                }
                else
                    zero_already_pressed = false;
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
            zero_listener.join();
        }
    }
    // backround_print.join();
    return 0;
} 