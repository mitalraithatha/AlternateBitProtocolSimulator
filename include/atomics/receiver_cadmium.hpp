/**
* Cristina Ruiz Martin
* ARSLab - Carleton University
*
*/

#ifndef __RECEIVER_CADMIUM_HPP__
#define __RECEIVER_CADMIUM_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>
#include <math.h> 
#include <assert.h>
#include <memory>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <limits>
#include <random>
#include "../data_structures/message.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct receiver_defs{
    struct output : public out_port<Message_t> {
    };
    struct input : public in_port<Message_t> {
    };
};

template<typename TIME>
class Receiver {
	using defs = receiver_defs; // putting definitions in context
	public:
		//Parameters to be overwritten when instantiating the atomic model
		TIME   PREPARATION_TIME;
		// default constructor
		Receiver() noexcept {
			PREPARATION_TIME = TIME("00:00:10");
			state.acknowledgement_number = 0;
			state.sending = false;
		}
		// state definition
		struct state_type {
			int acknowledgement_number;
			bool sending;
		};
        state_type state;
        // ports definition
        using input_ports = std::tuple<typename defs::input>;
        using output_ports = std::tuple<typename defs::output>;
        // internal transition
        void internal_transition() {
        	state.sending = false;
        }
        // external transition
        void external_transition(TIME e,
        		typename make_message_bags<input_ports>::type mbs) {
        			if(get_messages<typename defs::input>(mbs).size()>1) {
        				assert(false && "one message per time uniti");
        			}
        			for(const auto &x : get_messages<typename defs::input>(mbs)) {
        				state.acknowledgement_number = static_cast < int > (x.value);
        				state.sending = true;
        			}
                           
        }
        // confluence transition
        void confluence_transition(TIME e,
        		typename make_message_bags<input_ports>::type mbs) {
        			internal_transition();
        			external_transition(TIME(), std::move(mbs));
        }
        // output function
        typename make_message_bags<output_ports>::type output() const {
        	typename make_message_bags<output_ports>::type bags;
            Message_t out;
            out.value = state.acknowledgement_number % 10;
            get_messages<typename defs::output>(bags).push_back(out);
            return bags;
        }
        // time_advance function
        TIME time_advance() const {
        	TIME next_internal;
            if (state.sending) {
                next_internal = PREPARATION_TIME;
            }
            else {
                next_internal = std::numeric_limits<TIME>::infinity();
            }
            return next_internal;
        }
        friend std::ostringstream& operator<<(
        		std::ostringstream& output_stream,
				const typename Receiver<TIME>::state_type& i) {
        			output_stream << "acknowledgement_number: " <<
					i.acknowledgement_number;
        			return output_stream;
        }
};
  

#endif // __RECEIVER_CADMIUM_HPP__
