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

//! Cells - a wrapper that facilitates automatic growth.
//  the cell storage type requires operator [], value_type, ::resize(), and ::size()
template<typename S = std::vector<unsigned char> > struct cells_t {
        explicit cells_t(unsigned int init_size = 65536, typename S::value_type initial = 0) : cells_(init_size, initial), initial_(initial) {
        }

        typename S::value_type&       operator [](unsigned int index);
        typename S::value_type const& operator [](unsigned int index) const;

        std::size_t size() const;

private:
        mutable S cells_;
        typename S::value_type initial_;
};

template<typename S> inline typename S::value_type& cells_t<S>::operator [](unsigned int index)
{
        typename S::size_type size = cells_.size();

        if (index >= size)
                cells_.resize(size + (index - size) + growth_factor(), initial_);

        return cells_[index];
}

template<typename S> inline typename S::value_type const& cells_t<S>::operator [](unsigned int index) const
{
        typename S::size_type size = cells_.size();

        if (index >= size)
                cells_.resize(size + (index - size) + growth_factor(), initial_);

        return cells_[index];
}

template<typename S> inline std::size_t cells_t<S>::size() const
{
        return cells_.size();
}

} // namespace detail

#endif /* _H_BF_CELLS */
