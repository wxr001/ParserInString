// ParserInString -*- C++ -*-

// Copyright (C) 2019  Scott Brown

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

#ifndef PARSER_IN_STRING_MATCHER_COMPILER
#define PARSER_IN_STRING_MATCHER_COMPILER

#include "Common/Identity.hpp"
#include "Common/TAVLHelper.hpp"
#include "Common/Tfuncs.hpp"
#include "Common/Tlist.hpp"
#include "Common/Tstring.hpp"
#include "Common/Utils.hpp"
#include "Matcher/FirstFollow.hpp"
#include "Rules/CodeToRules.hpp"
#include "Rules/EBNF.hpp"
#include "TAVL.hpp"
#include <type_traits>
#define CONCAT_IMPL(a, b)   #a #b
#define CONCAT(a, b)        CONCAT_IMPL(a, b)
#define CONCATID_IMPL(a, b) a##b
#define CONCATID(a, b)      CONCATID_IMPL(a, b)
#define DEF(a)                                             \
    static constexpr const char CONCATID(r, a)[] = CONCAT( \
        Primary,                                           \
        a) " = Primary, [\"+\", Primary] | Primary, {\"-\"}, Primary | [Primary] | ;"
static constexpr const char long_rules[] = R"(
Primary = Primary, ["+", Primary] | Primary, {"-"}, Primary | [Primary] | ;)";
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
DEF(__COUNTER__);
namespace Compiler
{
    // clang-format off
    using type = code_to_rules_t<long_rules,
    	r0, r1, r2, r3, r4, r5, r6, r7, r8, r9,
    	r10, r11, r12, r13, r14, r15, r16, r17, r18, r19,
    	r20, r21, r22, r23, r24, r25, r26, r27, r28, r29,
    	r30, r31, r32, r33, r34, r35, r36, r37, r38, r39,
    	r40, r41, r42, r43, r44, r45, r46, r47, r48, r49,
    	r50, r51, r52, r53, r54, r55, r56, r57, r58, r59,
    	r60, r61, r62, r63, r64, r65, r66, r67, r68, r69,
    	r70, r71, r72, r73, r74, r75, r76, r77, r78, r79,
    	r80, r81, r82, r83, r84, r85, r86, r87, r88, r89,
    	r90, r91, r92, r93, r94, r95, r96, r97, r98, r99,
    	r100, r101, r102, r103, r104, r105, r106, r107, r108, r109,
    	r110, r111, r112, r113, r114, r115, r116, r117, r118, r119,
    	r120, r121, r122, r123, r124, r125, r126, r127, r128, r129,
    	r130, r131, r132, r133, r134, r135, r136, r137, r138, r139,
    	r140, r141, r142, r143, r144, r145, r146, r147, r148, r149,
    	r150, r151, r152, r153, r154, r155, r156, r157, r158, r159,
    	r160, r161, r162, r163, r164, r165, r166, r167, r168, r169,
    	r170, r171, r172, r173, r174, r175, r176, r177, r178, r179,
    	r180, r181, r182, r183, r184, r185, r186, r187, r188, r189,
    	r190, r191, r192, r193, r194, r195, r196, r197, r198, r199,
    	r200, r201, r202, r203, r204, r205, r206, r207, r208, r209,
    	r210, r211, r212, r213, r214, r215, r216, r217, r218, r219,
    	r220, r221, r222, r223, r224, r225, r226, r227, r228, r229,
    	r230, r231, r232, r233, r234, r235, r236, r237, r238, r239,
    	r240, r241, r242, r243, r244, r245, r246, r247, r248, r249,
    	r250, r251, r252, r253, r254, r255, r256, r257, r258, r259,
    	r260, r261, r262, r263, r264, r265, r266, r267, r268, r269,
    	r270, r271, r272, r273, r274, r275, r276, r277, r278, r279,
    	r280, r281, r282, r283, r284, r285, r286, r287, r288, r289,
    	r290, r291, r292, r293, r294, r295, r296, r297, r298, r299,
    	r300, r301, r302, r303, r304, r305, r306, r307, r308, r309,
    	r310, r311, r312, r313, r314, r315, r316, r317, r318, r319>;
    // clang-format on
    static_assert(!std::is_same_v<type, tavl::empty_node>);
} // namespace Compiler

#endif
