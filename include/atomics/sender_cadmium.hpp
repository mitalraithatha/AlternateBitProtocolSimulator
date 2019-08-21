/**
 * Cristina Ruiz Martin
 * ARSLab - Carleton University
 */

#ifndef __SENDER_CADMIUM_HPP__
#define __SENDER_CADMIUM_HPP__

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
struct sender_defs {
    struct packet_sent_output: public out_port<message_t> {};
	struct ack_received_output: public out_port<message_t> {};
	struct data_output: public out_port<message_t> {};
	struct control_input: public in_port<message_t> {};
	struct ack_input: public in_port<message_t> {};
};

template<typename TIME>
class Sender {
	using defs=sender_defs; // putting definitions in context
    public:
	//Parameters to be overwriten when instantiating the atomic model
	TIME preparation_time;
	TIME time_out;
	    // default constructor
	Sender() noexcept {
		preparation_time = TIME("00:00:10");
		time_out = TIME("00:00:20");
		state.alternating_bit = 0;
		state.next_internal = std::numeric_limits<TIME>::infinity();
		state.model_active = false;
	}

	// state definition
	struct state_type {
		bool acknowledgment;
		int packet_number;
		int total_packet_number;
		int alternating_bit;
		bool sending;
		bool model_active;
		TIME next_internal;
    };
	state_type state;
	// ports definition
	using input_ports=std::tuple<typename defs::control_input, typename defs::ack_input>;
	using output_ports=std::tuple<typename defs::packet_sent_output,
	    typename defs::ack_received_output, typename defs::data_output>;

	// internal transition
	void internal_transition() {
		if (state.acknowledgment) {
			if (state.packet_number < state.total_packet_number) {
				state.packet_number++;
				state.acknowledgment = false;
				state.alternating_bit = (state.alternating_bit + 1) % 2;
				state.sending = true;
				state.model_active = true;
				state.next_internal = preparation_time;
			} else {
				state.model_active = false;
				state.next_internal = std::numeric_limits<TIME>::infinity();
			}
		} else {
			if (state.sending) {
				state.sending = false;
				state.model_active = true;
				state.next_internal = time_out;
			} else {
				state.sending = true;
				state.model_active = true;
				state.next_internal = preparation_time;
			}
		}
	}

	// external transition
	void external_transition(
	    TIME e,typename make_message_bags<input_ports>::type mbs) {
		    if ((get_messages<typename defs::control_input>(mbs).size()
			        + get_messages<typename defs::ack_input>(mbs).size()) > 1){
			    assert(false && "one message per time uniti");
		    }
		for (const auto &x : get_messages<typename defs::control_input>(mbs)) {
		    if (state.model_active == false) {
				state.total_packet_number = static_cast<int>(x.value);
				if (state.total_packet_number > 0) {
					state.packet_number = 1;
					state.acknowledgment = false;
					state.sending = true;
					//set initial alt_bit
					state.alternating_bit = state.packet_number % 2;
					state.model_active = true;
					state.next_internal = preparation_time;
				} else {
				      if (state.next_internal
						      != std::numeric_limits<TIME>::infinity()) {
					      state.next_internal = state.next_internal - e;
					  }
				}
			}
		}
		for (const auto &x : get_messages<typename defs::ack_input>(mbs)) {
			if (state.model_active == true) {
				if (state.alternating_bit == static_cast<int>(x.value)) {
					state.acknowledgment = true;
					state.sending = false;
					state.next_internal = TIME("00:00:00");
				} else {
					if (state.next_internal
							!= std::numeric_limits<TIME>::infinity()) {
						state.next_internal = state.next_internal - e;
					}
				}
			}
		}

	}

	// confluence transition
	void confluence_transition(
	                   TIME e,typename make_message_bags<input_ports>::type mbs) {
	    internal_transition();
		external_transition(TIME(), std::move(mbs));
	}

	// output function
	typename make_message_bags<output_ports>::type output() const {
		typename make_message_bags<output_ports>::type bags;
		message_t out;
		if (state.sending) {
			out.value = state.packet_number * 10 + state.alternating_bit;
			get_messages<typename defs::data_output>(bags).push_back(out);
			out.value = state.packet_number;
			get_messages<typename defs::packet_sent_output>(bags).push_back(out);
		} else {
		      if (state.acknowledgment) {
			      out.value = state.alternating_bit;
				  get_messages<typename defs::ack_received_output>(bags)
						  .push_back(out);
			  }
		}
		return bags;

	}

	// time_advance function
	TIME time_advance() const {
		return state.next_internal;
	}

	friend std::ostringstream& operator<<(std::ostringstream &os,
			const typename Sender<TIME>::state_type &i) {
	    os << "packetNum: " << i.packet_number << " & totalPacketNum: "
				<< i.total_packet_number;
		return os;
	}
};

#endif // __SENDER_CADMIUM_HPP__
