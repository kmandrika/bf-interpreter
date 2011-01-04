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
#include "bf_error.h"

namespace detail {

struct forward {
        explicit forward(unsigned int bound) : bound_(bound) {
	}

        static int R() { return -1; }
        static int L() { return 1; }

        int operator ()() const { return 1; }

        unsigned int bound() const { return bound_; }

private:
        unsigned int bound_;
};

struct backward {
        explicit backward(unsigned int bound) : bound_(bound) {
	}

        static int R() { return 1; }
        static int L() { return -1; }

        int operator ()() const { return -1; }

        unsigned int bound() const { return bound_; }

private:
        unsigned int bound_;
};

//! todo: get rid of this paradigm, it's slow
template<typename Direction> unsigned int peek(const char* program, unsigned int command_index, Direction direction)
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

	detail::error("Can't find a match for:", program, command_index);
	return 0;
}

} // namespace detail

void evaluate(const char* program, size_t program_size)
{
        detail::state_t<> state;
	unsigned int command_index = 0;

	while (command_index != program_size) {
	        switch (program[command_index]) {
		case ' ':
		        break;
		case '>':
		        ++state;
			break;
		case '<':
		        --state;
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
		        std::cin>>state.get();
			break;
		case '[':
		        if (state.get() == 0)
			        command_index = detail::peek(program, command_index, detail::forward(program_size));
			break;
		case ']':
		        if (state.get() != 0)
			        command_index = detail::peek(program, command_index, detail::backward(0));
			break;
		default:
		        detail::error("Invalid command here:", program, command_index);
		}

		++command_index;
	}
}

#endif /* _H_BF_EVAL */
