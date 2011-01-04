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

void display_error_and_throw(const char* message, const char* program, unsigned int command_index)
{
        using namespace std;

	cout<<program<<endl<<setfill(' ')<<setw(command_index + 1)<<'^'<<endl;

	throw runtime_error(message);
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

	display_error_and_throw("Can't find a match.", program, command_index);
	return 0;
}

} // namespace detail

int evaluate(const char* program, size_t program_size, bool ignore_unknowns = false)
{
        detail::state_t<> state;
	unsigned int command_index = 0;

	try {
	        while (command_index != program_size) {
		        switch (program[command_index]) {
			case ' ':
			case '\n':
			        break;
			case '>':
			        state.increment_pc();
				break;
			case '<':
			        state.decrement_pc();
				break;
			case '+':
			        state.increment_current_cell();
				break;
			case '-':
			        state.decrement_current_cell();
				break;
			case '.':
			        std::cout<<state.get()<<std::flush;
				break;
			case ',':
			        std::cin>>state.set();
				break;
			case '[':
			        if (state.get() == 0)
				        command_index = detail::track(program, command_index, detail::forward(program_size));
				break;
			case ']':
			        if (state.get() != 0)
				        command_index = detail::track(program, command_index, detail::backward(0));
				break;
			default:
			        if (!ignore_unknowns)
				        detail::display_error_and_throw("Invalid command here:", program, command_index);
			}

			++command_index;
		}
	}
	catch (std::runtime_error& e) {
	        std::cout<<e.what()<<std::endl;

	        return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

namespace detail {

struct reader_t {
        explicit reader_t(const char* filename) : length_(0) {
		std::ifstream file(filename, std::fstream::in);

		if (!file)
		        throw std::runtime_error("Couldn't read file");

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

	        return EXIT_FAILURE;
	}
}

#endif /* _H_BF_EVAL */
