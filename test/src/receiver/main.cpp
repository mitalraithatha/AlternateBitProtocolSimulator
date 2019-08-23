/*! \file main.cpp
    \brief This main file implements the Receiver operation
    This file also sets up the application generator that
    takes input as full file location path where the output data will be stored
    and then generated formatted log file using the Cadmium and
    Destimes library. Log file have port number, model information, time stamp details
    this simulation timely controlled and runs until 04:00:00:000 time
*/
 
#define RECEIVER_TEST_INPUT  "../test/data/receiver/receiver_input_test.txt"
#define RECEIVER_TEST_OUTPUT  "../test/data/receiver/receiver_test_output.txt"
#define RECEIVER_FORMATTED_TEST_OUTPUT "../test/data/receiver/formatted_receiver_output.txt"

#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>
#include <cadmium/modeling/coupling.hpp>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/concept/coupled_model_assert.hpp>
#include <cadmium/modeling/dynamic_coupled.hpp>
#include <cadmium/modeling/dynamic_atomic.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/tuple_to_ostream.hpp>
#include <cadmium/logger/common_loggers.hpp>

#include <NDTime.hpp>
#include "../../../lib/vendor/iestream.hpp"
#include "../../../include/data_structures/message.hpp"
#include "../../../include/atomics/receiver_cadmium.hpp"
#include "../../../include/atomics/format_convert.hpp"

using namespace std;
using hclock = chrono::high_resolution_clock;
using TIME = NDTime;

/**
* Structure which holds input ports for messages
*/
struct input : public cadmium::in_port<message_t>{};

/**
* Structure which holds output ports for messages
*/
struct output : public cadmium::out_port<message_t>{};


/**
* Application generator class ask for file path and wait to get input
*/

template<typename T>
class ApplicationGen : public iestream_input<message_t,T> {
    public:
        /**
         * Default constructor for class application generator.
        */ 
        ApplicationGen() = default;
        /**
         * Parameterized constructor for class application generator.
         * This constructor ask for file path which contain inputs.
         * @param full file path
        */
        ApplicationGen(const char* file_path) : 
            iestream_input<message_t,T>(file_path) {
            }
};

int main() {
    const char *input_file = RECEIVER_TEST_OUTPUT; //defining input parameter for file format converter.
    const char *output_file = RECEIVER_FORMATTED_TEST_OUTPUT; //defining output parameter for file format converter.
    auto start = hclock::now(); /** To measure simulation execution time    */
    /**
     * Generates text log file for all the message application operations
     * and store out_data variable return value in the file.
    */
    static std::ofstream out_data(RECEIVER_TEST_OUTPUT);
    /**
     * This structure holds ostream- output stream for log file.
    */
    struct oss_sink_provider {
        /**
         * This function generate Ostream - output stream.
         *@return output stream.
        */
        static std::ostream& sink() {
            return out_data;
        }
    };
    /**
     * cadmium and Destimes library functions use to create formatted and detailed log file
     * so that it is easy to read and understand
    */
    using info = cadmium::logger::logger<cadmium::logger::logger_info,
                 cadmium::dynamic::logger::formatter<TIME>,
                 oss_sink_provider>;
    using debug = cadmium::logger::logger<cadmium::logger::logger_debug,
                  cadmium::dynamic::logger::formatter<TIME>,
                  oss_sink_provider>;
    using state = cadmium::logger::logger<cadmium::logger::logger_state,
                  cadmium::dynamic::logger::formatter<TIME>,
                  oss_sink_provider>;
    using log_messages = cadmium::logger::logger<cadmium::logger::logger_messages,
                         cadmium::dynamic::logger::formatter<TIME>,
                         oss_sink_provider>;
    using routing = cadmium::logger::logger<cadmium::logger::logger_message_routing,
                    cadmium::dynamic::logger::formatter<TIME>,
                    oss_sink_provider>;
    using global_time = cadmium::logger::logger<cadmium::logger::logger_global_time,
                        cadmium::dynamic::logger::formatter<TIME>,
                        oss_sink_provider>;
    using local_time = cadmium::logger::logger<cadmium::logger::logger_local_time,
                       cadmium::dynamic::logger::formatter<TIME>,
                       oss_sink_provider>;
    using log_all = cadmium::logger::multilogger<info,debug, state,log_messages,
                    routing, global_time, local_time>;
    using logger_top = cadmium::logger::multilogger<log_messages, global_time>;



    string input_data_control = RECEIVER_TEST_INPUT;
    const char * p_input_data_control = input_data_control.c_str();
    /**
     * Generator initialize with generator model information, time and input details.
    */
    std::shared_ptr<cadmium::dynamic::modeling::model> generator =
    cadmium::dynamic::translate::make_dynamic_atomic_model<ApplicationGen,
    TIME, const char* >("generator" , std::move(p_input_data_control));

    /**
     * Receiver1 initialize with Receiver1 information and receive time.
    */

    std::shared_ptr<cadmium::dynamic::modeling::model> receiver1 =
    cadmium::dynamic::translate::make_dynamic_atomic_model<Receiver,
    TIME>("receiver1");

    /**
     * store values for operations that have been performed during execution 
     * for single time frame and then store in output file
    */


    cadmium::dynamic::modeling::Ports iports_TOP = {};
    cadmium::dynamic::modeling::Ports oports_TOP = {typeid(output)};
    cadmium::dynamic::modeling::Models submodels_TOP = {generator, receiver1};
    cadmium::dynamic::modeling::EICs eics_TOP = {};
    cadmium::dynamic::modeling::EOCs eocs_TOP = {
        cadmium::dynamic::translate::make_EOC<receiver_defs::output,
        output>("receiver1")
    };
    cadmium::dynamic::modeling::ICs ics_TOP = {
        cadmium::dynamic::translate::make_IC<iestream_input_defs<message_t>::out,
        receiver_defs::input>("generator","receiver1")
    };
    std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP = 
    std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
        "TOP",
        submodels_TOP,
        iports_TOP,
        oports_TOP,
        eics_TOP,
        eocs_TOP,
        ics_TOP
    );

    /**
     * Runner creates to measure time and after that simulation starts and run until
     * 04:00:00:000 time.
    */

    auto elapsed1 = std::chrono::duration_cast<std::chrono::duration<double,
                    std::ratio<1>>>(hclock::now() - start).count();
    cout << "Model Created. Elapsed time: " << elapsed1 << "sec" << endl;
    
    cadmium::dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    elapsed1 = std::chrono::duration_cast<std::chrono::duration<double,
               std::ratio<1>>>(hclock::now() - start).count();
    cout << "Runner Created. Elapsed time: " << elapsed1 << "sec" << endl;
    
    cout << "Simulation starts" << endl;
    
    r.run_until(NDTime("04:00:00:000"));
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double,
                   std::ratio<1>>>(hclock::now() - start).count();
    cout << "Simulation took:" << elapsed << "sec" << endl;
    converter(input_file, output_file);
    return 0;
}

