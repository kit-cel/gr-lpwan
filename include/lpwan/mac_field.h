/* -*- c++ -*- */
/* 
 * Copyright 2017 Kristian Maier <kristian.maier@gmx.de>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef INCLUDED_LPWAN_MAC_FIELD_H
#define INCLUDED_LPWAN_MAC_FIELD_H

#include <lpwan/api.h>

namespace gr {
  namespace lpwan {

    /*!
     * \brief base class for fragmentation bitfields
     *
     */
    class lpwan_API mac_field
    {
    public:
        virtual std::vector<uint8_t> get_data() = 0;
        virtual void set_data(const std::vector<uint8_t> &v) = 0;

        void append_data(std::vector<uint8_t> &d);
    protected:

    };

  } // namespace lpwan
} // namespace gr

#endif /* INCLUDED_LPWAN_MAC_FIELD_H */

