/**
 * @file main.cpp
 *
 * @brief This file is to simulate and test sender behavior
 *        of ABP simulator.
 *
 *  Sender component is activated upon receiving a control signal.
 *  It starts sending packets with alternating bit and waits for an ack
 *  signal for a specific time and resends the packet with an alternating
 *  bit, if ack is not received.
 *  The program does unit testing and takes control data and ack data
 *  from two input file paths and writes output log to a destination file.
 *  Simulator is configured to run until 04:00:00:000 time
 *  @author sreejith unnithan.
 */
#define SENDER_TEST_OUTPUT "../test/data/sender/sender_test_output.txt"
/*!< macro that defines the output log file path */

#define SENDER_INPUT_TEST_CONTROL "../test/data/sender/sender_input_test_control_In.txt"
/*!< macro that defines the input path containing the data to activate sender*/

#define SENDER_INPUT_TEST_ACK_IN "../test/data/sender/sender_input_test_ack_In.txt"
/*!< macro that defines the input file path which represents received ack signal */

#define SENDER_FORMATTED_TEST_OUTPUT "../test/data/sender/formatted_sender_output.txt"
/*!< macro that defines the out put file path which contains the formatted output */

#define SENDER_SIMULATION_RUN_TIME "04:00:00:000"
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
#include "..\..\..\include\atomics\sender_cadmium.hpp"
#include "..\..\..\include\atomics\format_convert.hpp"



using namespace std;

using hclock=chrono::high_resolution_clock;
using TIME = NDTime;

struct input_control: public cadmium::in_port<message_t> {};
/*!< setting input port for trigger data */

struct input_acknowledgment: public cadmium::in_port<message_t> {};
/*!< setting input port for acknowledgment reception */

/***** SETING OUTPUT PORTS FOR COUPLEDs *****/
struct output_ack: public cadmium::out_port<message_t> {};
/*!< setting output port for acknowledgment*/

struct output_data: public cadmium::out_port<message_t> {};
/*!< setting output port for sending data*/

struct output_pack: public cadmium::out_port<message_t> {};
/*!< setting output port for sending output data packets*/

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

    static std::ofstream sender_output_data(SENDER_TEST_OUTPUT);
    /*!< function to create an output file stream and
     *  write to the specified output path
     */

    /*
     * structure that has function to create output stream
     */
    struct oss_sink_provider {
        static std::ostream& sink() {
            return sender_output_data;
        }
    };

    /*
     *Using Cadmium files to generate formatted log files
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
    using log_all=cadmium::logger::multilogger<info, debug, state, log_messages,
            routing, global_time, local_time>;

    using logger_top=cadmium::logger::multilogger<log_messages, global_time>;

    /*
     * Application generator
     */

    string input_data_control =SENDER_INPUT_TEST_CONTROL;
    /*!< variable that holds the input file name which contains trigger data*/

    const char *p_input_data_control = input_data_control.c_str();
    /*!< pointer to file path */

    /*
     * code to initialize the Application generator class invoking its constructor
     * with the input control file path
     */

    std::shared_ptr<cadmium::dynamic::modeling::model> generator_con =
        cadmium::dynamic::translate::make_dynamic_atomic_model<
            ApplicationGen, TIME, const char*>("generator_con",
                std::move(p_input_data_control));

    string input_data_ack = SENDER_INPUT_TEST_ACK_IN;
    /*!< variable that holds the input file name which contains ack data*/

    const char *p_input_data_ack = input_data_ack.c_str();
    /*!< pointer to file path */

    /*
     * code to initialize the Application generator class invoking its constructor
     * with the input file path with ack information
     */
    std::shared_ptr<cadmium::dynamic::modeling::model> generator_ack =
        cadmium::dynamic::translate::make_dynamic_atomic_model<
            ApplicationGen, TIME, const char*>("generator_ack",
                    std::move(p_input_data_ack));

    /*
     * sender simulation starts here
     */

    std::shared_ptr<cadmium::dynamic::modeling::model> sender1 =
        cadmium::dynamic::translate::make_dynamic_atomic_model<Sender, TIME>(
            "sender1");

    cadmium::dynamic::modeling::Ports iports_TOP = { };
    cadmium::dynamic::modeling::Ports oports_TOP = { 
	                                                typeid(output_data),
													typeid(output_pack), 
													typeid(output_ack) 
													};
    cadmium::dynamic::modeling::Models submodels_TOP = { generator_con,
                                                         generator_ack, 
														 sender1 
													   };
    cadmium::dynamic::modeling::EICs eics_TOP = { };
    cadmium::dynamic::modeling::EOCs eocs_TOP = {
        cadmium::dynamic::translate::make_EOC<sender_defs::packet_sent_output,
            output_pack>("sender1"),
                cadmium::dynamic::translate::make_EOC<sender_defs::ack_received_output,
                    output_ack>("sender1"), cadmium::dynamic::translate::make_EOC<
                        sender_defs::data_output, output_data>("sender1") };
    cadmium::dynamic::modeling::ICs ics_TOP = {
        cadmium::dynamic::translate::make_IC<
            iestream_input_defs<message_t>::out,
                sender_defs::control_input>("generator_con", "sender1"),
                    cadmium::dynamic::translate::make_IC<
                        iestream_input_defs<message_t>::out,
                            sender_defs::ack_input>("generator_ack", "sender1") };
    std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP =
        std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>("TOP",
            submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP,ics_TOP);

    /*
     * simulation ends
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
    /*
     * running the model until 04:00:00:000
     */
    r.run_until(NDTime(SENDER_SIMULATION_RUN_TIME));
    auto elapsed_simulation_time =
             std::chrono::duration_cast<
                 std::chrono::duration<double, std::ratio<1>>>(
                     hclock::now() - start).count();
    /*!< elapsed time since the simulation has started */

    cout << "Simulation took:" << elapsed_simulation_time << "sec" << endl;
    const char *input_file = SENDER_TEST_OUTPUT;
    /*!< pointer to file path containing the sender output logs */
    const char *output_file = SENDER_FORMATTED_TEST_OUTPUT;
    /*!< pointer to file path containing the formatted sender output logs */
    converter(input_file, output_file);
    return 0;
}
