
#if __has_include(<wiringPi.h>)
#    include <wiringPi.h>
#else
#    include "wiringPiStandin.hpp"
#endif

struct Startup__ { Startup__(){wiringPiSetupGpio();} } start__;

#include "Button.hpp"
#include "JobInfo.hpp"
#include <filesystem>
#include <fstream>
#include <mutex>
#include <regex>
#include <thread>
#include "Serial.hpp"


std::filesystem::path Serial::directory { "/dev/ttyACM0" };

namespace buttons
{
    Button start        = 6;
    Button slide_switch = 20;
    Button zero         = 21;
    Button go_back      = 16;
} // namespace buttons

int main()
{
    while(1)
    {
        buttons::start.waitForNext(0);

        auto job_info = getJobInfo("job_info.json");
        Instruction { "G28 Z\n" }.send();

        auto instructions = getInstructions(job_info);
        for(auto instr = std::begin(instructions); instr != std::end(instructions); instr++)
        {
            instr->send();

            bool waiting = true;
            std::thread index_listener { [&waiting] {
                buttons::slide_switch.waitForNext(0);
                waiting = false;
            } };
            std::thread zero { [&waiting, &instr] {
        	while(waiting)
		{
		        buttons::zero.waitForNext(0, &waiting);
	                if(!waiting) return;
                	Instruction { "G28 Z\n" }.send();
        	        std::this_thread::sleep_for(std::chrono::milliseconds(2));
	                instr->send();
		}
            } };
            std::thread reverse { [&waiting, &instr, &instructions] {
                while(waiting && instr != std::begin(instructions))
                {
                    buttons::go_back.waitForNext(0, &waiting);
                    if(!waiting) return;
                    instr--;
                    instr->send();
                }
            } };
            index_listener.join();
            zero.join();
            reverse.join();
        }
    }
    // backround_print.join();
    return 0;
} 
