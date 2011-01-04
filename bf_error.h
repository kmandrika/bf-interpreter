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

#ifndef _H_BF_ERROR
#define _H_BF_ERROR

#include <iostream>
#include <iomanip>
#include <cstdlib>

namespace detail {

inline void error(const char* message, const char* program = NULL, unsigned int command_index = 0)
{
        std::cout<<message<<std::endl;

	if (program != NULL)
	        std::cout<<program<<std::endl<<std::setfill(' ')<<std::setw(command_index + 1)<<'^'<<std::endl;

	exit(-1);
}

} // namespace detail

#endif /* _H_BF_ERROR */
