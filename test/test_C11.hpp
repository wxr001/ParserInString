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

#ifndef PARSER_IN_STRING_TEST_C11
#define PARSER_IN_STRING_TEST_C11

#include "Common/Tstring.hpp"
#include "Rules/CodeToRules.hpp"
#include "Rules/EBNF.hpp"
#include "TAVL.hpp"
#include <type_traits>
namespace Compiler
{
    DECL_RULE(identifier, R"(
    identifier =
        ?lexer identifier?,
        (* identifier-nondigit *)
        (?@[_a-zA-Z]? |
        (?@\[uU][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]?,
            [?@[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]?])),
        (* (identifier-nondigit | digit)* *)    
        {?@[_a-zA-Z0-9]? |
        (?@\[uU][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]?,
            [?@[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]?])}   
            ;)");
    DECL_RULE(constant, R"(
    constant =
        integer constant |
        floating constant |
        enumeration constant |
        character constant;)");
    DECL_RULE(integer_constant, R"(
    integer constant =
        ?lexer integer constant?,
        (?@[1-9][0-9]*? |
        ?@0[0-7]*? |
        ?@0[xX][0-9a-fA-F][0-9a-fA-F]*?),
        [(('u' | 'U'), ['l' | 'L']) |
        (('u' | 'U'), ['ll' | 'LL']) |
        (('l' | 'L'), ['u' | 'U']) |
        (('ll' | 'LL'), ['u' | 'U'])];)");
    DECL_RULE(floating_constant, R"(
    floating constant =
        ?lexer floating constant?,
        ((
        (?@[0-9][0-9]*\.? | ?@[0-9]*\.[0-9][0-9]*?),
        [('e'|'E'), ['+'|'-'], ?@[0-9][0-9]*?]) |
        (?@[0-9][0-9]*?, ('e'|'E'), ['+'|'-'], ?@[0-9][0-9]*?) |
        (?@0[xX]?,
            ((?@[0-9a-fA-F][0-9a-fA-F]*\.? | ?@[0-9a-fA-F]*\.[0-9a-fA-F][0-9a-fA-F]*?) |
            (?@[0-9a-fA-F][0-9a-fA-F]*?)),
            ([?@[pP]?, ['+' | '-'], ?@[0-9][0-9]*?]))),
        [?@[flFL]?];)");
    DECL_RULE(enumeration_constant, R"(
    enumeration constant = identifier;)");
    DECL_RULE(character_constant, R"(
    character constant =
        ?lexer character constant?,
        [?@[LuU]?], "'", (
            ?@[!-&(-[\]-~\a\b\f\r\t\v]? |
            '\\', (?@['"\\abfnrtv]? | '?') |
            ('\\', ?oct?, [?oct?, [?oct?]]) |
            ?@\x[0-9a-fA-F][0-9a-fA-F]*? ),
        "'";)");
    DECL_RULE(string_literal, R"(
    string literal =
        ?lexer string literal?,
        ['u8"' | 'u"' | 'U"' | 'L"'],
        (?@[!#-[\]-~\a\b\f\r\t\v]? |
        ('\\', ?@['"\\abfnrtv]? | '?') |
        ('\\', ?oct?, [?oct?, [?oct?]]) |
        ?@\x[0-9a-fA-F][0-9a-fA-F]*?),
        '"';)");
    DECL_RULE(primary, R"(
    primary expression =
        identifier |
        constant |
        string literal |
        '(', expression, ')' |
        generic selection;)");
    DECL_RULE(generic_selection, R"(
    generic selection =
        '_Generic', '(', assignment expression, generic assoc list, ')';)");
    DECL_RULE(generic_assoc_list, R"(
    generic assoc list =
        generic association, {',', generic association};)");
    DECL_RULE(generic_association, R"(
    generic association =
        type name, '"', assignment expression |
        'default', ':', assignment expression;)");
    DECL_RULE(postfix, R"(
    postfix expression =
        (primary expression |
        '(', type name, ')', '{', initializer list, [','], '}'), {
            '[', expression, ']' |
            '(', [argument expression list], ')' |
            '.', identifier |
            '->', identifier |
            '++' |
            '--'
        };)");
    DECL_RULE(argument_expression_list, R"(
    argument expression list =
        assignment expression, {',', assignment expression};)");
    DECL_RULE(unary_expression, R"(
    unary expression =
        postfix expression |
        '++', unary expression |
        '--', unary expression |
        unary operator, cast expression |
        'sizeof', (unary expression | '(', type name, ')') |
        '_Alignof', '(', type name, ')';)");
    DECL_RULE(unary_operator, R"(
    unary operator = '&' | '*' | '+' | '-' | '~' | '!';)");
    DECL_RULE(cast_expression, R"(
    cast expression =
        unary expression |
        '(', type name, ')', cast expression;)");
    DECL_RULE(multiplicative, R"(
    multiplicative expression =
        cast expression, {
            '*', cast expression |
            '/', cast expression |
            '%', cast expression
        };)");
    DECL_RULE(additive, R"(
    additive expression =
        multiplicative expression, {
            '+', multiplicative expression,
            '-', multiplicative expression
        };)");
    DECL_RULE(shift, R"(
    shift expression =
        additive expression, {
            '<<', additive expression,
            '>>', additive expression
        };)");
    DECL_RULE(relational, R"(
    relational expression =
        shift expression, {
            '<=', shift expression |
            '>=', shift expression |
            '<', shift expression |
            '>', shift expression
        };)");
    DECL_RULE(equality_expression, R"(
    equality expression =
        relational expression, {
            '==', relational expression |
            '!=', relational expression
        };
    )");
    DECL_RULE(and_expression, R"(
    and expression =
        equality expression, {'&', equality expression};
    )");
    DECL_RULE(exclusive, R"(
    exclusive or expression =
        and expression, {'^', and expression};)");
    DECL_RULE(inclusive, R"(
    inclusive or expression =
        exclusive or expression, {'|', exclusive or expression};)");
    DECL_RULE(logical_and, R"(
    logical and expression =
        inclusive or expression, {'&&', inclusive or expression};)");
    DECL_RULE(logical_or, R"(
    logical or expression =
        logical and expression, {'||', logical and expression};)");
    DECL_RULE(conditional, R"(
    conditional expression =
        logical or expression,
        ['?', expression, ':', conditional expression];)");
    DECL_RULE(assignment_expression, R"(
    assignment expression =
        conditional expression |
        unary expression, assignment operator, assignment expression;)");
    DECL_RULE(assignment_operator, R"(
    assignment operator =
        '=' |
        '*=' | '/=' | '%=' |
        '+=' | '-=' |
        '<<=' | '>>=' |
        '&=' | '^=' | '|=';)");
    DECL_RULE(expression, R"(
    expression =
        assignment expression, {',', assignment expression};)");
    DECL_RULE(constant_expression, R"(
    constant expression =
        conditional expression;)");
    DECL_RULE(declaration, R"(
    declaration =
        declaration specifiers, [init declarator list], ';' |
        static assert declaration;)");
    DECL_RULE(declaration_specifiers, R"(
    declaration specifiers =
        storage class specifier, [declaration specifiers] |
        type specifier, [declaration specifiers] |
        type qualifier, [declaration specifiers] |
        function specifier, [declaration specifiers] |
        alignment specifier, [declaration specifiers];)");
    DECL_RULE(init_declarator_list, R"(
    init declarator list =
        init declarator, {',', init declarator};)");
    DECL_RULE(init_declarator, R"(
    init declarator =
        declarator, ['=', initializer];)");
    DECL_RULE(storage_class_specifier, R"(
    storage class specifier =
        'typedef' | 'extern' | 'static' | '_Thread_local' | 'auto' | 'register';)");
    DECL_RULE(type_specifier, R"(
    type specifier =
        'void' |
        'char' | 'short' | 'int' | 'long' |
        'float' | 'double' | 'signed' | 'unsigned' |
        '_Bool' | '_Complex' |
        atomic type specifier |
        struct or union specifier |
        enum specifier |
        typedef name;)");
    DECL_RULE(struct_or_union_specifier, R"(
    struct or union specifier =
        struct or union, [identifier], '{', struct declaration list, '}' |
        struct or union, identifier;)");
    DECL_RULE(struct_or_union, R"(
    struct or union = 'struct' | 'union';)");
    DECL_RULE(struct_declaration_list, R"(
    struct declaration list =
        struct declaration, {struct declaration};)");
    DECL_RULE(struct_declaration, R"(
    struct declaration =
        specifier qualifier list, [struct declarator list], ';' |
        static assert declaration;)");
    DECL_RULE(specifier_qualifier_list, R"(
    specifier qualifier list =
        type specifier, [specifier qualifier list] |
        type qualifier, [specifier qualifier list];)");
    DECL_RULE(struct_declarator_list, R"(
    struct declarator list =
        struct declarator, {',', struct declarator};)");
    DECL_RULE(struct_declarator, R"(
    struct declarator =
        declarator |
        [declarator], ':', constant expression;)");
    DECL_RULE(enum_specifier, R"(
    enum specifier =
        'enum', [identifier], '{', enumerator list, [','], '}' |
        'enum', identifier;)");
    DECL_RULE(enumerator_list, R"(
    enumerator list =
        enumerator, {',', enumerator};)");
    DECL_RULE(enumerator, R"(
    enumerator =
        enumeration constant, ['=', constant expression];)");
    DECL_RULE(atomic_type_specifier, R"(
    atomic type specifier =
        '_Atomic', '(', type name, ')';)");
    DECL_RULE(type_qualifier, R"(
    type qualifier =
        'const' |
        'restrict' |
        'volatile' |
        '_Atomic';)");
    DECL_RULE(function_specifier, R"(
    function specifier = 'inline' | '_Noreturn';)");
    DECL_RULE(alignment_specifier, R"(
    alignment specifier =
        '_Alignas', '(', (type name | constant expression), ')';)");
    DECL_RULE(declarator, R"(
    declarator = [pointer], direct declarator;)");
    DECL_RULE(direct_declarator, R"(
    direct declarator =
        (identifier | '(', declarator, ')'),
        {'[', (
            [type qualifier list], [assignment expression] |
            'static', [type qualifier list], assignment expression |
            type qualifier list, 'static', assignment expression |
            [type qualifier list], '*'),']' |
        '(', (parameter type list | [identifier list]), ')'};)");
    DECL_RULE(pointer, R"(
    pointer =
        '*', [type qualifier list], [pointer];)");
    DECL_RULE(type_qualifier_list, R"(
    type qualifier list =
        type qualifier, {type qualifier};)");
    DECL_RULE(parameter_type_list, R"(
    parameter type list =
        parameter list, [',', '...'];)");
    DECL_RULE(parameter_list, R"(
    parameter list =
        parameter declaration, {',', parameter declaration};)");
    DECL_RULE(parameter_declaration, R"(
    parameter declaration =
        declaration specifiers,
        (declarator | [abstract declarator]);)");
    DECL_RULE(identifier_list, R"(
    identifier list =
        identifier, {',', identifier};)");
    DECL_RULE(type_name, R"(
    type name = specifier qualifier list, [abstract declarator];)");
    DECL_RULE(abstract_declarator, R"(
    abstract declarator =
        pointer |
        [pointer], direct abstract declarator;)");
    DECL_RULE(direct_abstract_declarator, R"(
    direct abstract declarator =
        ('(', abstract declarator ,')' |
        '[', [type qualifier list], [assignment expression], ']' |
        '[', 'static', [type qualifier list], assignment expression, ']' |
        '[', type qualifier list, 'static', assignment expression, ']' |
        '[', '*', ']' |
        '(', [parameter type list], ')'), direct abstract declarator ext;)");
    DECL_RULE(direct_abstract_declarator_ext, R"(
    direct abstract declarator ext = {
    '[', [type qualifier list], [assignment expression], ']' |
    '[', 'static', [type qualifier list], assignment expression, ']' |
    '[', type qualifier list, 'static', assignment expression, ']' |
    '[', '*', ']' |
    '(', [parameter type list], ')'};)");
    DECL_RULE(typedef_name, R"(
    typedef name = identifier;)");
    DECL_RULE(initializer, R"(
    initializer =
        assignment expression |
        '{', initializer list, [','], '}';)");
    DECL_RULE(initializer_list, R"(
    initializer list =
        [designation], initializer, {',', [designation], initializer};)");
    DECL_RULE(designation, R"(
    designation = designator list, '=';)");
    DECL_RULE(designator_list, R"(
    designator list = designator, {designator};)");
    DECL_RULE(designator, R"(
    designator =
        '[', constant expression, ']' |
        '.', identifier;)");
    DECL_RULE(static_assert_declaration, R"(
    static assert declaration =
        '_Static_assert', '(', constant expression, ',', string literal, ')', ';';)");
    DECL_RULE(statement, R"(
    statement =
        labeled statement |
        compound statement |
        expression statement |
        selection statement |
        iteration statement |
        jump statement;)");
    DECL_RULE(labeled_statement, R"(
    labeled statement =
        identifier, ':', statement |
        'case', constant expression, ':', statement |
        'default', ':', statement;)");
    DECL_RULE(compound_statement, R"(
    compound statement =
        '{', [block item list], '}';)");
    DECL_RULE(block_item_list, R"(
    block item list = block item, {block item};)");
    DECL_RULE(block_item, R"(
    block item = declaration | statement;)");
    DECL_RULE(expression_statement, R"(
    expression statement = [expression], ';';)");
    DECL_RULE(selection_statement, R"(
    selection statement =
        'if', '(', expression, ')', statement, ['else', statement] |
        'switch', '(', expression, ')', statement;)");
    DECL_RULE(iteration_statement, R"(
    iteration statement =
        'while', '(', expression, ')', statement |
        'do', statement, 'while', '(', expression, ')', ';'; |
        'for', '(',
            (declaration | [expression], ';'),
            [expression], ';',
            [expression], ')',
        statement;)");
    DECL_RULE(jump_statement, R"(
    jump statement =
        'goto', identifier, ';' |
        'continue', ';' |
        'break', ';' |
        'return', [expression], ';';)");
    DECL_RULE(translation_unit, R"(
    translation unit = external declaration, {external declaration};)");
    DECL_RULE(external_declaration, R"(
    external declaration =
        function definition, | declaration;)");
    DECL_RULE(function_definition, R"(
    function definition =
        declaration specifiers, declarator, [declaration list], compound statement;)");
    DECL_RULE(declaration_list, R"(
    declaration list =
        declaration, {declaration};)");
} // namespace Compiler

#endif
