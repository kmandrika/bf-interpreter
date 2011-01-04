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

#ifndef _H_BF_CELLS
#define _H_BF_CELLS

#include <vector>

namespace detail {

inline unsigned int growth_factor()
{
        //! todo: more elaborate growth factor
        return 1000;
}

//! Cells
template<typename T = unsigned char, typename S = std::vector<T> > struct cells_t {
        typedef S storage_t;

public:
        explicit cells_t(unsigned int init_size = 65536) : cells_(init_size, 0) {
	}

        T& operator [](unsigned int index);

private:
        storage_t cells_;
};

template<typename T, typename S> inline T& cells_t<T, S>::operator [](unsigned int index)
{
        typename storage_t::size_type size = cells_.size();

        if (index >= size)
	        cells_.resize(size + growth_factor(), 0);

	return cells_[index];
}

} // namespace detail

#endif /* _H_BF_CELLS */
