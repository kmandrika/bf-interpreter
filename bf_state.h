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

#ifndef _H_BF_STATE
#define _H_BF_STATE

#include "bf_cells.h"
#include "bf_error.h"

#include <limits>

namespace detail {

//! Program state - responsible for incrementing and decrementing the pc and
//  cell values.
template<typename C = unsigned char, typename T = unsigned int> struct state_t {
        state_t() : pc_(0) {
	}

        void increment_current_cell();
        void decrement_current_cell();

        void operator ++();
        void operator --();

        C& get();

private:
        cells_t<C> cells_;
        T pc_;
};

template<typename C, typename T> inline void state_t<C, T>::increment_current_cell()
{
        if (cells_[pc_] + 1 > std::numeric_limits<C>::max())
	        error("cell overflow");

        ++cells_[pc_];
}

template<typename C, typename T> inline void state_t<C, T>::decrement_current_cell()
{
        if (cells_[pc_] - 1 < 0)
	        error("cell underflow");

        --cells_[pc_];
}

template<typename C, typename T> inline void state_t<C, T>::operator ++()
{
        if (pc_ + 1 > std::numeric_limits<T>::max())
	        error("pc overflow");

	++pc_;
}

template<typename C, typename T> inline void state_t<C, T>::operator --()
{
        if (pc_ - 1 < 0)
	        error("pc underflow");

	--pc_;
}

template<typename C, typename T> inline C& state_t<C, T>::get()
{
        return cells_[pc_];
}

} // namespace detail

#endif /* _H_BF_STATE */
