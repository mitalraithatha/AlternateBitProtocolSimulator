/*! \file receiver_cadmium.hpp
    \brief Receiver_cadmium.hpp header file defines classes for Receiver.
	Receiver works in 2 phases which is Active Phase and Passive phase.
	In passive phase, receiver accepts packets from input port.
	and also sends acknowledgment using output port after pre-fixed interval in active phase. 
	By default receiver always in Passive phase and once it sent acknowledgment it goes to 
	Passive phase again.
*/
/** Original Author: Cristina Ruiz Martin
* Organization : ARSLab - Carleton University
* Modify by: Mital Raithatha
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

/** 
* Structure definition for input and output message.
*/

struct receiver_defs{
    struct output : public out_port<message_t> {
    };
    struct input : public in_port<message_t> {
    };
};

/** 
* Receiver class definition.
*/
template<typename TIME>
class Receiver {
	using defs = receiver_defs; 
	public:
		//Parameters to be overwritten when instantiating the atomic model
		TIME   PREPARATION_TIME;  /**< TIME constant for delay between input and output */ //!<constant for Time delay .
		
		/** 
		* default constructor for Receiver class
		* Initialize TIME constant for Delay,
		* Initialize Acknowledgment number to zero at beginning,
		* and sending state sets to False - Passive phase.
		*/
		Receiver() noexcept {
			PREPARATION_TIME = TIME("00:00:10");
			state.acknowledgement_number = 0;
			state.sending = false;
		}
		/**
		* Structure definition for Receiver state and Acknowledgment number
		*/
		struct state_type {
			int acknowledgement_number; /**< Number which send for acknowledgment. */ //!< Acknowledge Number.
			bool sending; /**< Sending values: True for Active phase and False for Passive Phase. */ //!< State of the receiver.
		};
        state_type state;
        // ports definition
        using input_ports = std::tuple<typename defs::input>;
        using output_ports = std::tuple<typename defs::output>;
        
		/** 
		* Receiver class member function for internal transition which change sending state 
		* from Active (True) to Passive (False) 
		*/
        void internal_transition() {
        	state.sending = false;
        }
		/**
		* Receiver class member function for external transition
		* This function make sure that only one message is being sent per time unit, if more than one messages in input.
		* after that it sets sending state to TRUE(Active phase) and acknowledgment number.
		* @param e TIME type variable
		* @param mbs make_message_bags type variable
		* 
		*/
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
        /**
		* confluence transition calls internal_transition and external_transition respectively.
		* @param e TIME type variable
		* @param mbs make_message_bags type variable
		*/
        void confluence_transition(TIME e,
        		typename make_message_bags<input_ports>::type mbs) {
        			internal_transition();
        			external_transition(TIME(), std::move(mbs));
        }		
		/**
		* This function generate message bags using modulus(%) operation on acknowledgment number
		* and send (push_back) to output port.
		* @return Message bags 
		*/
        typename make_message_bags<output_ports>::type output() const {
        	typename make_message_bags<output_ports>::type bags;
            message_t out;
            out.value = state.acknowledgement_number % 10;
            get_messages<typename defs::output>(bags).push_back(out);
            return bags;
        }
		/**
		* This function generate time for next internal transition.
		* this function checks sending state first and set next_internal.
		* if sending state is Active (non zero or TRUE) next internal sets to PREPARATION_TIME
		* or it sets to infinity. 
		* @return time for next internal 
		*/
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
		/**
		* Function that create acknowledgment number in form of string stream
		* @param ostream stream
		* @param i  receiver state type
		* @return output string stream 
		*/
        friend std::ostringstream& operator<<(
        		std::ostringstream& output_stream,
				const typename Receiver<TIME>::state_type& i) {
        			output_stream << "acknowledgement_number: " <<
					i.acknowledgement_number;
        			return output_stream;
        }
};
  

#endif // __RECEIVER_CADMIUM_HPP__
