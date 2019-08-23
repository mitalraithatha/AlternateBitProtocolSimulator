/**
 * @file main.cpp
 *
 * @brief This file is to simulate and test subnet(network) behavior
 *        of ABP simulator.
 *
 *  This file simulates the network behavior which passes the packet
 *  with a time delay and with 95% probability
 *  The program takes input from a file path and writes output
 *  to a destination file.
 *  Simulator is configured to run until 04:00:00:000 time
 *
 *  @author sreejith unnithan.
 */


#define SUBNET_TEST_OUTPUT "../test/data/subnet/subnet_test_output.txt"
/*!< macro that defines the input file path */

#define SUBNET_TEST_INPUT "../test/data/subnet/subnet_input_test.txt"
/*!< macro that defines the output file path */

#define SUBNET_FORMATTED_TEST_OUTPUT "../test/data/subnet/formatted_subnet_output.txt"
/*!< macro that defines the formatted output file path */

#define SUBNET_SIMULATION_RUN_TIME "04:00:00:000"
/*!< macro that defines the simulation run time */

#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>
#include <cadmium\modeling\coupling.hpp>
#include <cadmium\modeling\ports.hpp>
#include <cadmium\modeling\dynamic_model_translator.hpp>
#include <cadmium\concept\coupled_model_assert.hpp>
#include <cadmium\modeling\dynamic_coupled.hpp>
#include <cadmium\modeling\dynamic_atomic.hpp>
#include <cadmium\engine\pdevs_dynamic_runner.hpp>
#include <cadmium\logger\tuple_to_ostream.hpp>
#include <cadmium\logger\common_loggers.hpp>
#include <NDTime.hpp>

#include "..\..\..\lib\vendor\iestream.hpp"
#include "..\..\..\include\data_structures\message.hpp"
#include "..\..\..\include\atomics\subnet_cadmium.hpp"
#include "..\..\..\include\atomics\format_convert.hpp"
#include "..\..\..\include\atomics\format_convert.hpp"


using namespace std;

using hclock=chrono::high_resolution_clock;
using TIME = NDTime;

/**
 * setting input port for messages
 */
struct input_in: public cadmium::in_port<message_t> {
};

/**
 *  setting output port for messages
 */
struct output_out: public cadmium::out_port<message_t> {
};

/*!\brief Application generator class.
 *
 * Class that inherits the properties of input_event_stream
 * PDEVS Model(iestream.hpp).This class is for the application generator
 * which reads the content from file path and transmits as message
 * defined in the structure message_t.
 */
template<typename T>
class ApplicationGen: public iestream_input<message_t, T> {
   public:
         ApplicationGen() = default;
         /*!< default constructor for the class */

         ApplicationGen(const char *file_path) :
         iestream_input<message_t, T>(file_path) {}
         /*!< parameterized constructor, taking input file path
          * as argument.
          * @param file_path
          */
};

/*! **Program Driver** */

int main() {
    
    auto start = hclock::now(); //!<start time.
    /*!< variable to hold the start time of simulation. */

    static std::ofstream output_data(SUBNET_TEST_OUTPUT);
    /*!< function to create an output file stream and
     *  write to the specified output path
     */


    /*
     * structure that has function to create output stream
     */
    struct oss_sink_provider {
        static std::ostream& sink() {
            return output_data;
        }
    };

    /*
     * Using Cadmium files to generate formatted log files
     */

    using info=cadmium::logger::logger<cadmium::logger::logger_info,
        cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
    using debug=cadmium::logger::logger<cadmium::logger::logger_debug,
        cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
    using state=cadmium::logger::logger<cadmium::logger::logger_state,
        cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
    using log_messages=cadmium::logger::logger<cadmium::logger::logger_messages,
        cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
    using routing=cadmium::logger::logger<cadmium::logger::logger_message_routing,
        cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
    using global_time=cadmium::logger::logger<cadmium::logger::logger_global_time,
        cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
    using local_time=cadmium::logger::logger<cadmium::logger::logger_local_time,
        cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
    using log_all=cadmium::logger::multilogger<info, debug, state,
        log_messages, routing, global_time, local_time>;

    using logger_top=cadmium::logger::multilogger<log_messages, global_time>;


    string input_data = SUBNET_TEST_INPUT;
    /*!< variable that holds the input file name */

    const char *p_input_data = input_data.c_str();
    /*!< pointer to file */

    /*
     * code to initialize the Application generator class invoking its constructor
     * with the input file path
     */
    std::shared_ptr<cadmium::dynamic::modeling::model> generator =
        cadmium::dynamic::translate::make_dynamic_atomic_model<
            ApplicationGen, TIME, const char*>("generator",
                std::move(p_input_data));

    /*
     * Here output data is being generated from the subnet.
     * Subnet simulation starts here
     */

    std::shared_ptr<cadmium::dynamic::modeling::model> subnet1 =
        cadmium::dynamic::translate::make_dynamic_atomic_model<Subnet, TIME>(
            "subnet1");


    cadmium::dynamic::modeling::Ports iports_TOP = { };
    cadmium::dynamic::modeling::Ports oports_TOP = { typeid(output_out) };
    cadmium::dynamic::modeling::Models submodels_TOP = { generator, subnet1 };
    cadmium::dynamic::modeling::EICs eics_TOP = { };
    cadmium::dynamic::modeling::EOCs eocs_TOP = {
        cadmium::dynamic::translate::make_EOC<subnet_defs::output, output_out>(
            "subnet1") };
    cadmium::dynamic::modeling::ICs ics_TOP = {
        cadmium::dynamic::translate::make_IC<
            iestream_input_defs<message_t>::out, subnet_defs::input>(
                "generator", "subnet1") };
    std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP =
        std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>("TOP",
            submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP,ics_TOP);

    /*
     * simulation end
     */

    auto elapsed_time =
             std::chrono::duration_cast<
                 std::chrono::duration<double, std::ratio<1>>>(
                     hclock::now() - start).count();
    /*!< elapsed time since the model has started */

    cout << "Model Created. Elapsed time: " << elapsed_time << "sec" << endl;

    cadmium::dynamic::engine::runner<NDTime, logger_top> r(TOP, { 0 });
    elapsed_time =
        std::chrono::duration_cast<
            std::chrono::duration<double, std::ratio<1>>>(
                hclock::now() - start).count();
    cout << "Runner Created. Elapsed time: " << elapsed_time << "sec" << endl;

    cout << "Simulation starts" << endl;

    r.run_until(NDTime(SUBNET_SIMULATION_RUN_TIME));
    auto elapsed_simulation_time =
             std::chrono::duration_cast<
                 std::chrono::duration<double, std::ratio<1>>>(
                     hclock::now() - start).count();
    /*!< elapsed time since the simulation has started */
    cout << "Simulation took:" << elapsed_simulation_time << "sec" << endl;
    const char *input_file = SUBNET_TEST_OUTPUT;
    const char *output_file = SUBNET_FORMATTED_TEST_OUTPUT;
    converter(input_file, output_file);
    return 0;
}
