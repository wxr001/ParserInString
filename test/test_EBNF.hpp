// Universal Compiler Library -*- C++ -*-

// Copyright (C) 2018  Scott Brown

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

#ifndef UNIVERSAL_COMPILER_LIBRARY_GRAMMAR_MATCHER_TESTEBNF
#define UNIVERSAL_COMPILER_LIBRARY_GRAMMAR_MATCHER_TESTEBNF

#include "Common/Identity.hpp"
#include "Common/Tlist.hpp"
#include "Common/Tstring.hpp"
#include "Rules/EBNF.hpp"
#include "TAVL.hpp"
#include <type_traits>

namespace Compiler
{
    namespace InHeaderDebug
    {
        using namespace Impl;
#define EOS_TAG 'o', 't', 'h', 'e', 'r'
        inline namespace IsAlphaTest
        {
            static_assert(is_alpha_v<'A'>, "is_alpha_v tests");
            static_assert(is_alpha_v<'a'>, "is_alpha_v tests");
            static_assert(is_alpha_v<'Z'>, "is_alpha_v tests");
            static_assert(is_alpha_v<'z'>, "is_alpha_v tests");
            static_assert(!is_alpha_v<'A' - 1>,
                          "is_alpha_v tests( off by one)");
        } // namespace IsAlphaTest
        inline namespace IsDigitTest
        {
            static_assert(is_digit_v<'0'>, "is_digit_v tests");
            static_assert(is_digit_v<'9'>, "is_digit_v tests");
            static_assert(!is_digit_v<'0' - 1>,
                          "is_digit_v tests( off by one)");
        } // namespace IsDigitTest
        inline namespace ConcatenateTest
        {
            static_assert(is_concatenate_v<',', EOS_TAG>,
                          "concatenate tests for value");
            static_assert(std::is_same_v<is_concatenate_n<',', EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "concatenate tests for next");
            static_assert(
                std::is_same_v<is_concatenate_n<EOS_TAG>, t_string<EOS_TAG>>,
                "concatenate tests for next");
            static_assert(!is_concatenate_v<EOS_TAG>,
                          "concatenate tests for value");
        } // namespace ConcatenateTest
        inline namespace DefiningTest
        {
            static_assert(is_defining_v<'=', EOS_TAG>,
                          "Defining tests for value");
            static_assert(
                std::is_same_v<is_defining_n<'=', EOS_TAG>, t_string<EOS_TAG>>,
                "Defining tests for next");
            static_assert(
                std::is_same_v<is_defining_n<EOS_TAG>, t_string<EOS_TAG>>,
                "Defining tests for next");
            static_assert(!is_defining_v<EOS_TAG>, "Defining tests for value");
        } // namespace DefiningTest
        inline namespace DefinitionSeparatorTest
        {
            static_assert(is_definition_separator_v<'|', EOS_TAG>,
                          "Definition separator tests for |");
            static_assert(is_definition_separator_v<'/', EOS_TAG>,
                          "Definition separator tests for /");
            static_assert(is_definition_separator_v<'!', EOS_TAG>,
                          "Definition separator tests for !");
            static_assert(
                std::is_same_v<is_definition_separator_n<'/', EOS_TAG>,
                               t_string<EOS_TAG>>,
                "Definition separator tests for next");
            static_assert(std::is_same_v<is_definition_separator_n<EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "Definition separator tests for next");
            static_assert(!is_definition_separator_v<EOS_TAG>,
                          "Definition separator tests for value");
        } // namespace DefinitionSeparatorTest
        inline namespace EndCommentTest
        {
            static_assert(is_end_comment_v<'*', ')', EOS_TAG>,
                          "EndComment tests for *)");
            static_assert(std::is_same_v<is_end_comment_n<'*', ')', EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "EndComment tests for next");
            static_assert(
                std::is_same_v<is_end_comment_n<EOS_TAG>, t_string<EOS_TAG>>,
                "EndComment tests for next");
            static_assert(!is_end_comment_v<EOS_TAG>,
                          "EndComment tests for value");
        } // namespace EndCommentTest
        inline namespace EndGroupTest
        {
            static_assert(is_end_group_v<')', EOS_TAG>, "EndGroup tests for )");
            static_assert(
                std::is_same_v<is_end_group_n<')', EOS_TAG>, t_string<EOS_TAG>>,
                "EndGroup tests for next");
            static_assert(
                std::is_same_v<is_end_group_n<EOS_TAG>, t_string<EOS_TAG>>,
                "EndGroup tests for next");
            static_assert(!is_end_group_v<EOS_TAG>, "EndGroup tests for value");
        } // namespace EndGroupTest
        inline namespace EndOptionTest
        {
            static_assert(is_end_option_v<']', EOS_TAG>,
                          "EndOption tests for ]");
            static_assert(is_end_option_v<'/', ')', EOS_TAG>,
                          "EndOption tests for /)");
            static_assert(std::is_same_v<is_end_option_n<']', EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "EndOption tests for ]");
            static_assert(std::is_same_v<is_end_option_n<'/', ')', EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "EndOption tests for /)");
            static_assert(
                std::is_same_v<is_end_option_n<EOS_TAG>, t_string<EOS_TAG>>,
                "EndOption tests for next");
            static_assert(!is_end_option_v<'/', EOS_TAG>,
                          "EndOption tests for / only");
        } // namespace EndOptionTest
        inline namespace EndRepeatTest
        {
            static_assert(is_end_repeat_v<'}', EOS_TAG>,
                          "EndRepeat tests for }");
            static_assert(is_end_repeat_v<':', ')', EOS_TAG>,
                          "EndRepeat tests for :)");
            static_assert(std::is_same_v<is_end_repeat_n<'}', EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "EndRepeat tests for }");
            static_assert(std::is_same_v<is_end_repeat_n<':', ')', EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "EndRepeat tests for :)");
            static_assert(
                std::is_same_v<is_end_repeat_n<EOS_TAG>, t_string<EOS_TAG>>,
                "EndRepeat tests for next");
            static_assert(!is_end_repeat_v<':', EOS_TAG>,
                          "EndRepeat tests for : only");
        } // namespace EndRepeatTest
        inline namespace ExceptTest
        {
            static_assert(is_except_v<'-', EOS_TAG>, "Except tests for -");
            static_assert(
                std::is_same_v<is_except_n<'-', EOS_TAG>, t_string<EOS_TAG>>,
                "Except tests for next");
            static_assert(
                std::is_same_v<is_except_n<EOS_TAG>, t_string<EOS_TAG>>,
                "Except tests for next");
            static_assert(!is_except_v<EOS_TAG>, "Except tests for value");
        } // namespace ExceptTest
        inline namespace FirstQuoteTest
        {
            static_assert(is_first_quote_v<'\'', EOS_TAG>,
                          "FirstQuote tests for '");
            static_assert(std::is_same_v<is_first_quote_n<'\'', EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "FirstQuote tests for next");
            static_assert(
                std::is_same_v<is_first_quote_n<EOS_TAG>, t_string<EOS_TAG>>,
                "FirstQuote tests for next");
            static_assert(!is_first_quote_v<EOS_TAG>,
                          "FirstQuote tests for value");
        } // namespace FirstQuoteTest
        inline namespace RepetitionTest
        {
            static_assert(is_repetition_v<'*', EOS_TAG>,
                          "Repetition tests for *");
            static_assert(std::is_same_v<is_repetition_n<'*', EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "Repetition tests for next");
            static_assert(
                std::is_same_v<is_repetition_n<EOS_TAG>, t_string<EOS_TAG>>,
                "Repetition tests for next");
            static_assert(!is_repetition_v<EOS_TAG>,
                          "Repetition tests for value");
        } // namespace RepetitionTest
        inline namespace SecondQuoteTest
        {
            static_assert(is_second_quote_v<'"', EOS_TAG>,
                          "SecondQuote tests for \"");
            static_assert(std::is_same_v<is_second_quote_n<'"', EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "SecondQuote tests for next");
            static_assert(
                std::is_same_v<is_second_quote_n<EOS_TAG>, t_string<EOS_TAG>>,
                "SecondQuote tests for next");
            static_assert(!is_second_quote_v<EOS_TAG>,
                          "SecondQuote tests for value");
        } // namespace SecondQuoteTest
        inline namespace SpecialSequenceSymbolTest
        {
            static_assert(is_special_sequence_symbol_v<'?', EOS_TAG>,
                          "SpecialSequenceSymbol tests for ?");
            static_assert(
                std::is_same_v<is_special_sequence_symbol_n<'?', EOS_TAG>,
                               t_string<EOS_TAG>>,
                "SpecialSequenceSymbol tests for next");
            static_assert(std::is_same_v<is_special_sequence_symbol_n<EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "SpecialSequenceSymbol tests for next");
            static_assert(!is_special_sequence_symbol_v<EOS_TAG>,
                          "SpecialSequenceSymbol tests for value");
        } // namespace SpecialSequenceSymbolTest
        inline namespace StartCommentTest
        {
            static_assert(is_start_comment_v<'(', '*', EOS_TAG>,
                          "EndComment tests for (*");
            static_assert(std::is_same_v<is_start_comment_n<'(', '*', EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "EndComment tests for next");
            static_assert(
                std::is_same_v<is_start_comment_n<EOS_TAG>, t_string<EOS_TAG>>,
                "EndComment tests for next");
            static_assert(!is_end_comment_v<EOS_TAG>,
                          "EndComment tests for value");
        } // namespace StartCommentTest
        inline namespace StartOptionTest
        {
            static_assert(is_start_option_v<'[', EOS_TAG>,
                          "StartOption tests for [");
            static_assert(is_start_option_v<'(', '/', EOS_TAG>,
                          "StartOption tests for (/");
            static_assert(std::is_same_v<is_start_option_n<'[', EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "StartOption tests for [");
            static_assert(std::is_same_v<is_start_option_n<'(', '/', EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "StartOption tests for (/");
            static_assert(
                std::is_same_v<is_start_option_n<EOS_TAG>, t_string<EOS_TAG>>,
                "StartOption tests for next");
            static_assert(!is_end_repeat_v<'(', EOS_TAG>,
                          "EndRepeat tests for ( only");
        } // namespace StartOptionTest
        inline namespace StartRepeatTest
        {
            static_assert(is_start_repeat_v<'{', EOS_TAG>,
                          "StartRepeat tests for {");
            static_assert(is_start_repeat_v<'(', ':', EOS_TAG>,
                          "StartRepeat tests for (:");
            static_assert(std::is_same_v<is_start_repeat_n<'{', EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "StartRepeat tests for {");
            static_assert(std::is_same_v<is_start_repeat_n<'(', ':', EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "StartRepeat tests for (:");
            static_assert(
                std::is_same_v<is_start_repeat_n<EOS_TAG>, t_string<EOS_TAG>>,
                "StartRepeat tests for next");
            static_assert(!is_start_repeat_v<'(', EOS_TAG>,
                          "StartRepeat tests for ( only");
        } // namespace StartRepeatTest
        inline namespace TerminatorTest
        {
            static_assert(is_terminator_v<';', EOS_TAG>,
                          "Terminator tests for ;");
            static_assert(is_terminator_v<'.', EOS_TAG>,
                          "Terminator tests for .");
            static_assert(std::is_same_v<is_terminator_n<';', EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "Terminator tests for next");
            static_assert(std::is_same_v<is_terminator_n<'.', EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "Terminator tests for next");
            static_assert(
                std::is_same_v<is_terminator_n<EOS_TAG>, t_string<EOS_TAG>>,
                "Terminator tests for next");
            static_assert(!is_terminator_v<EOS_TAG>,
                          "Terminator tests for value");
        } // namespace TerminatorTest
        inline namespace IntegerSymbolTest
        {
            static_assert(is_integer_symbol_v<'0', EOS_TAG>,
                          "IntegerSymbol tests for 0");
            static_assert(is_integer_symbol_v<'1', '2', '3', EOS_TAG>,
                          "IntegerSymbol tests for non-zero values");
            static_assert(integer_symbol_v<'5', EOS_TAG> == 5,
                          "IntegerSymbol tests for 5");
            static_assert(integer_symbol_v<'1', '2', '9', EOS_TAG> == 129,
                          "IntegerSymbol tests for non-zero values");
            static_assert(
                std::is_same_v<integer_symbol_n<'1', '2', '3', EOS_TAG>,
                               t_string<EOS_TAG>>,
                "IntegerSymbol tests for next");
            static_assert(
                std::is_same_v<integer_symbol_n<EOS_TAG>, t_string<EOS_TAG>>,
                "IntegerSymbol tests for next");
            static_assert(!is_integer_symbol_v<EOS_TAG>,
                          "IntegerSymbol tests for failure");
        } // namespace IntegerSymbolTest
        inline namespace MetaIdentifierCharacterTest
        {
            static_assert(is_meta_identifier_character_v<'a'>,
                          "MetaIdentifierCharacter tests for alphabets");
            static_assert(is_meta_identifier_character_v<'5'>,
                          "MetaIdentifierCharacter tests for digits");
            static_assert(is_meta_identifier_character_v<' '>,
                          "MetaIdentifierCharacter tests for ' '");
            static_assert(is_meta_identifier_character_v<'\t'>,
                          "MetaIdentifierCharacter tests for \\t");
            static_assert(is_meta_identifier_character_v<'\v'>,
                          "MetaIdentifierCharacter tests for \\v");
            static_assert(is_meta_identifier_character_v<'\f'>,
                          "MetaIdentifierCharacter tests for \\f");
            static_assert(!is_meta_identifier_character_v<'.'>,
                          "MetaIdentifierCharacter tests for other characters");
        } // namespace MetaIdentifierCharacterTest
        inline namespace MetaIdentifierTest
        {
            // is_meta_identifier_v
            static_assert(
                is_meta_identifier_v<'a', 'b', 'c', '1', '2', '3', '.'>,
                "MetaIdentifier tests for is_*_v (alnum only)");
            static_assert(is_meta_identifier_v<'a',
                                               'B',
                                               'c',
                                               ' ',
                                               '\t',
                                               '1',
                                               '2',
                                               '3',
                                               '\f',
                                               '.'>,
                          "MetaIdentifier tests for is_*_v (with space)");
            static_assert(
                !is_meta_identifier_v<'.', 'b', 'c', '1', '2', '3', '.'>,
                "MetaIdentifier tests for is_*_v (invalid name)");
            // meta_identifier_v
            static_assert(
                std::is_same_v<
                    meta_identifier_v<'a', 'b', 'c', '1', '2', '3', '.'>,
                    t_string<'a', 'b', 'c', '1', '2', '3'>>,
                "MetaIdentifier tests for *_v (no space, valid name)");
            static_assert(
                std::is_same_v<
                    meta_identifier_v<'a',
                                      'b',
                                      '\f',
                                      'c',
                                      ' ',
                                      '1',
                                      '\t',
                                      '2',
                                      '\v',
                                      '3',
                                      '.'>,
                    t_string<'a', 'b', ' ', 'c', ' ', '1', ' ', '2', ' ', '3'>>,
                "MetaIdentifier tests for *_v (with spaces in the middle of name)");
            static_assert(
                std::is_same_v<
                    meta_identifier_v<'a',
                                      'b',
                                      '\f',
                                      'c',
                                      ' ',
                                      '1',
                                      '\t',
                                      '2',
                                      '\v',
                                      '3',
                                      '.'>,
                    t_string<'a', 'b', ' ', 'c', ' ', '1', ' ', '2', ' ', '3'>>,
                "MetaIdentifier tests for *_v (with spaces in the front of name)");
            static_assert(
                std::is_same_v<
                    meta_identifier_v<'a',
                                      'b',
                                      '\f',
                                      'c',
                                      ' ',
                                      '1',
                                      '\t',
                                      '2',
                                      '\v',
                                      '3',
                                      ' ',
                                      '\t',
                                      '\f',
                                      '.'>,
                    t_string<'a', 'b', ' ', 'c', ' ', '1', ' ', '2', ' ', '3'>>,
                "MetaIdentifier tests for *_v (with spaces in the end of name)");
            static_assert(
                std::is_same_v<meta_identifier_v<'a',
                                                 'b',
                                                 'c',
                                                 ' ',
                                                 '\f',
                                                 ' ',
                                                 '\t',
                                                 '1',
                                                 '2',
                                                 '3',
                                                 '.'>,
                               t_string<'a', 'b', 'c', ' ', '1', '2', '3'>>,
                "MetaIdentifier tests for *_v (merge spaces)");
            static_assert(
                std::is_same_v<
                    meta_identifier_v<'.', 'a', 'b', 'c', '1', '2', '3', '.'>,
                    t_string<>>,
                "MetaIdentifier tests for *_v (invalid name)");
            static_assert(std::is_same_v<meta_identifier_n<' ',
                                                           ' ',
                                                           ' ',
                                                           'a',
                                                           'b',
                                                           '\f',
                                                           'c',
                                                           ' ',
                                                           ' ',
                                                           '1',
                                                           '\t',
                                                           '2',
                                                           '\v',
                                                           '3',
                                                           ' ',
                                                           '\t',
                                                           '\f',
                                                           '.'>,
                                         t_string<'.'>>,
                          "MetaIdentifier tests for next");
            static_assert(std::is_same_v<meta_identifier_v<'1', '2', '3', '.'>,
                                         t_string<>>,
                          "MetaIdentifier tests for *_v (invalid name)");
        } // namespace MetaIdentifierTest
        namespace FirstQuoteStringTest
        {
            static_assert(
                is_first_quote_string_v<'\'', 'a', 'b', '"', '\'', EOS_TAG>,
                "FirstQuoteString tests for is_*_v (normal case)");
            static_assert(!is_first_quote_string_v<'\'', '\'', EOS_TAG>,
                          "FirstQuoteString tests for is_*_v (empty case)");
            static_assert(std::is_same_v<first_quote_string_v<'\'',
                                                              'a',
                                                              ' ',
                                                              'b',
                                                              '"',
                                                              '\'',
                                                              EOS_TAG>,
                                         t_string<'a', ' ', 'b', '"'>>,
                          "FirstQuoteString tests for *_v (normal case)");
            static_assert(
                std::is_same_v<first_quote_string_v<'\'', '\'', EOS_TAG>,
                               t_string<>>,
                "FirstQuoteString tests for *_v (empty case)");
            static_assert(std::is_same_v<first_quote_string_n<'\'',
                                                              'a',
                                                              ' ',
                                                              'b',
                                                              '"',
                                                              '\'',
                                                              EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "FirstQuoteString tests for *_n (normal case)");
            static_assert(
                std::is_same_v<first_quote_string_n<'\'', '\'', EOS_TAG>,
                               t_string<EOS_TAG>>,
                "FirstQuoteString tests for *_n (empty case)");
        } // namespace FirstQuoteStringTest
        namespace SecondQuoteStringTest
        {
            static_assert(
                is_second_quote_string_v<'"', 'a', 'b', '\'', '"', EOS_TAG>,
                "SecondQuoteString tests for is_*_v (normal case)");
            static_assert(!is_second_quote_string_v<'"', '"', EOS_TAG>,
                          "SecondQuoteString tests for is_*_v (empty case)");
            static_assert(std::is_same_v<second_quote_string_v<'"',
                                                               'a',
                                                               ' ',
                                                               'b',
                                                               '\'',
                                                               '"',
                                                               EOS_TAG>,
                                         t_string<'a', ' ', 'b', '\''>>,
                          "SecondQuoteString tests for *_v (normal case)");
            static_assert(
                std::is_same_v<second_quote_string_v<'"', '"', EOS_TAG>,
                               t_string<>>,
                "SecondQuoteString tests for *_v (empty case)");
            static_assert(std::is_same_v<second_quote_string_n<'"',
                                                               'a',
                                                               ' ',
                                                               'b',
                                                               '\'',
                                                               '"',
                                                               EOS_TAG>,
                                         t_string<EOS_TAG>>,
                          "SecondQuoteString tests for *_n (normal case)");
            static_assert(
                std::is_same_v<second_quote_string_n<'"', '"', EOS_TAG>,
                               t_string<EOS_TAG>>,
                "SecondQuoteString tests for *_n (empty case)");
        } // namespace SecondQuoteStringTest
#undef EOS_TAG
    } // namespace InHeaderDebug
} // namespace Compiler
#endif
