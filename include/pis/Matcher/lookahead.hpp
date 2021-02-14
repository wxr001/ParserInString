// ParserInString -*- C++ -*-

// Copyright (C) 2020  Scott Brown

// This file is part of the UCL library.
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with this library.  If not, see <http://www.gnu.org/licenses/>.

#ifndef PARSER_IN_STRING_MATCHER_LOOKAHEAD
#define PARSER_IN_STRING_MATCHER_LOOKAHEAD

#include "pis/Common/Identity.hpp"
#include "pis/Common/TAVLHelper.hpp"
#include "pis/Common/Tfuncs.hpp"
#include "pis/Common/Tlist.hpp"
#include "pis/Common/Tstring.hpp"
#include "pis/Common/Utils.hpp"
#include "pis/Common/runtime.hpp"
#include <algorithm>
#include <string>
#include <type_traits>
#include <vector>

namespace pis
{
    class lookahead_symbol
    {
    public:
        using lookahead_type = std::vector<std::vector<std::string>>;
        lookahead_symbol()   = default;
        lookahead_symbol(const lookahead_symbol&) = default;
        virtual bool is_same(const lookahead_type& lookahead) const
        {
            return false;
        }
        virtual bool is_empty() const noexcept
        {
            return false;
        }
        virtual std::string to_string() const
        {
            return "";
        }
    };
    template <typename List>
    class lookahead_list: public lookahead_symbol
    {
    public:
        lookahead_list()                      = default;
        lookahead_list(const lookahead_list&) = default;
        bool is_same(const lookahead_type& lookahead) const override
        {
            _end = lookahead.cend();
            return do_comp<List>(lookahead.cbegin());
        }
        std::string to_string() const override
        {
            std::string result = "<";
            dump<List>(result);
            return result + ">";
        }

    private:
        template <typename L>
        bool do_comp(lookahead_type::const_iterator it) const
        {
            if constexpr (std::is_same_v<L, type_list<>>)
            {
                return true;
            }
            else if (it == _end || it->size() == 0)
            {
                return false;
            }
            else
            {
                auto cur_item = pis::to_string(type_list_top<L>{});
                if (std::find(it->cbegin(), it->cend(), cur_item) != it->cend())
                {
                    return do_comp<type_list_tail<L>>(it + 1);
                }
                return false;
            }
        }
        template <typename L>
        void dump(std::string& str) const
        {
            if constexpr (std::is_same_v<L, type_list<>>)
            {
                str.pop_back();
                return;
            }
            else
            {
                str += pis::to_string(type_list_top<L>()) + ",";
                return dump<type_list_tail<L>>(str);
            }
        }
        mutable lookahead_type::const_iterator _end;
    };
    template <>
    class lookahead_list<empty_symbol>: public lookahead_symbol
    {
    public:
        lookahead_list()                      = default;
        lookahead_list(const lookahead_list&) = default;
        bool is_empty() const noexcept override
        {
            return true;
        }
    };
} // namespace pis

#endif
