//! A simple Brainfuck interpreter.
/*!
    Copyright (C) 2011  Konstantin Mandrika

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _H_BF_EVAL
#define _H_BF_EVAL

#include "bf_state.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <fstream>

namespace detail {

template<int D> struct direction {
        explicit direction(unsigned int bound) : bound_(bound) {
	}

        static int R() { return -D; }
        static int L() { return  D; }

        int operator ()() const {
	        return D;
	}

        unsigned int bound() const {
	        return bound_;
	}

private:
        unsigned int bound_;
};

typedef direction< 1> forward;
typedef direction<-1> backward;

inline int display_error_cause(const char* message, const char* program, unsigned int command_index)
{
        //! Display error and what caused it.
	std::cout<<"Error: "<<message<<", cause:"<<std::endl
		 <<program<<std::endl<<std::setfill(' ')<<std::setw(command_index + 1)<<'^'<<std::endl;

	return EXIT_FAILURE;
}

//! todo: get rid of this paradigm, it's slow
template<typename Direction> unsigned int track(const char* program, unsigned int command_index, Direction direction)
{
        unsigned int i = command_index;
	unsigned int op_count = 0;

	while (i != direction.bound()) {
	        switch (program[i]) {
		case ']': op_count += direction.R(); break;
		case '[': op_count += direction.L(); break;
		}

		if (op_count == 0)
		        return i;

		i += direction();
	}

	throw std::runtime_error("can't find corresponding command");
	return 0;
}

inline unsigned int repetition_length(const char* program, size_t program_size, unsigned int command_index, char command)
{
        unsigned int length = 0;

	while (program[command_index++] == command && ++length) {
	}

	return length;
}

const char O_Clear[] = "[-]";
const char O_Add[] = "[->+<]";

} // namespace detail

#define OPTIMIZE_REPETITIONS(program, program_size, command_index, command, state, function)     \
        repetitions = detail::repetition_length(program, program_size, command_index, command);  \
        state.function(repetitions);                                                             \
        command_index += repetitions

#define OPTIMIZE_PATTERN(program, program_size, command_index, pattern)	                         \
        (command_index + sizeof pattern - 2 < program_size && !memcmp(                           \
                static_cast<const void *>(program + command_index)                               \
	      , static_cast<const void *>(pattern)		                                 \
	      , sizeof pattern - 1			                                         \
	))

#define OPTIMIZE_CLEAR(program, program_size, command_index, state)                              \
        if (OPTIMIZE_PATTERN(program, program_size, command_index, detail::O_Clear)) {           \
	        state.set() = 0;                                                                 \
		command_index += sizeof detail::O_Clear - 2;                                     \
		break;                                                                           \
        }

#define OPTIMIZE_ADD(program, program_size, command_index, state)	                         \
        if (OPTIMIZE_PATTERN(program, program_size, command_index, detail::O_Add)) {             \
	        unsigned int this_cell = state.get();                                            \
		unsigned int next_cell;                                                          \
		state.set() = 0;                                                                 \
		state.increment_pc();                                                            \
		next_cell = state.get();                                                         \
		state.set() = this_cell + next_cell;                                             \
		state.decrement_pc();                                                            \
		command_index += sizeof detail::O_Add - 2;                                       \
		break;                                                                           \
	}

int evaluate(const char* program, size_t program_size, bool ignore_unknowns = false)
{
        detail::state_t<unsigned int, std::vector<unsigned int> > state;
	detail::cells_t<std::vector<unsigned int> > jump;

	unsigned int command_index = 0;
	unsigned int repetitions = 0;

	try {
	        while (command_index != program_size) {
		        switch (program[command_index]) {
			case ' ':
			case '\n':
			        break;
			case '>':
			        OPTIMIZE_REPETITIONS(program, program_size, command_index, '>', state, increment_pc);
				continue;
			case '<':
			        OPTIMIZE_REPETITIONS(program, program_size, command_index, '<', state, decrement_pc);
				continue;
			case '+':
			        OPTIMIZE_REPETITIONS(program, program_size, command_index, '+', state, increment_current_cell);
				continue;
			case '-':
			        OPTIMIZE_REPETITIONS(program, program_size, command_index, '-', state, decrement_current_cell);
				continue;
			case '.':
			        std::cout<<static_cast<unsigned char>(state.get())<<std::flush;
				break;
			case ',':
			        std::cin>>state.set();
				break;
			case '[':
			        OPTIMIZE_CLEAR(program, program_size, command_index, state);
				OPTIMIZE_ADD(program, program_size, command_index, state);

			        if (state.get() == 0)
				        command_index = jump[command_index] == 0 ?
					        jump[command_index] = detail::track(program, command_index, detail::forward(program_size)) :
					        jump[command_index];
				break;
			case ']':
			        if (state.get() != 0)
				        command_index = jump[command_index] == 0 ?
					        jump[command_index] = detail::track(program, command_index, detail::backward(0)) :
					        jump[command_index];
				break;
			default:
			        if (!ignore_unknowns)
				        throw std::runtime_error("found an invalid command");
			}

			++command_index;
		}
	}
	catch (std::runtime_error& e) {
	        return detail::display_error_cause(e.what(), program, command_index);
	}

	return EXIT_SUCCESS;
}

namespace detail {

struct reader_t {
        explicit reader_t(const char* filename) : length_(0) {
		std::ifstream file(filename, std::fstream::in);

		if (!file)
		        throw std::runtime_error("Couldn't read file.");

		std::string line;

		while (std::getline(file, line)) {
		        raw_.insert(raw_.end(), line.begin(), line.end());
			length_ += line.size();
		}
	}

        operator const char* () const { return raw_.c_str(); }
        operator size_t () const { return length_; }

private:
        std::string raw_;
        size_t length_;
};

}

int evaluate(const char* filename, bool ignore_unknowns = false)
{
        try {
	        detail::reader_t reader(filename);

		return evaluate(reader, reader, ignore_unknowns);
	}
	catch (std::runtime_error& e) {
	        std::cout<<e.what()<<std::endl;
	}

	return EXIT_FAILURE;
}

#endif /* _H_BF_EVAL */
