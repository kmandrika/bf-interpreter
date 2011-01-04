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
template<
        typename T = unsigned int                 // pc counter
      , typename S = std::vector<unsigned char>   // cell storage
>
struct state_t {
        state_t() : pc_(0) {
	}

        void increment_current_cell();
        void decrement_current_cell();

        void increment_pc();
        void decrement_pc();

        typename S::value_type  get() const;
        typename S::value_type& set();         // this should probably be something like set(C value)

private:
        cells_t<S> cells_;
        T pc_;
};

template<typename T, typename S> inline void state_t<T, S>::increment_current_cell()
{
        if (cells_[pc_] + 1 > std::numeric_limits<typename S::value_type>::max())
	        error("cell overflow");

        ++cells_[pc_];
}

template<typename T, typename S> inline void state_t<T, S>::decrement_current_cell()
{
        if (cells_[pc_] - 1 < 0)
	        error("cell underflow");

        --cells_[pc_];
}

template<typename T, typename S> inline void state_t<T, S>::increment_pc()
{
        if (pc_ + 1 > std::numeric_limits<T>::max())
	        error("pc overflow");

	++pc_;
}

template<typename T, typename S> inline void state_t<T, S>::decrement_pc()
{
        if (pc_ - 1 < 0)
	        error("pc underflow");

	--pc_;
}

template<typename T, typename S> inline typename S::value_type state_t<T, S>::get() const
{
        return cells_[pc_];
}

template<typename T, typename S> inline typename S::value_type& state_t<T, S>::set()
{
        return cells_[pc_];
}

} // namespace detail

#endif /* _H_BF_STATE */
