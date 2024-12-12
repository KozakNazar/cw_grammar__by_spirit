#define _CRT_SECURE_NO_WARNINGS  // for using sscanf in VS
/*******************************************************************
* N.Kozak // Lviv'2024 // example syntax analysis by boost::spirit *
*                         file: cwgrammar2.cpp                     *
*                                           (0.04v/draft version ) *
********************************************************************/
#include <iostream>
#include <sstream>  // for std::ostringstream
#include <string>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp> //

#define DEBUG__IF_ERROR

#define MAX_TEXT_SIZE 8192

namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

#define SAME_RULE(RULE) ((RULE) | (RULE))

template <typename Iterator>
struct cwgrammar : qi::grammar<Iterator> {
    cwgrammar(std::ostringstream& error_stream) : cwgrammar::base_type(program), error_stream_(error_stream) {
        //
        labeled_point = label >> tokenCOLON;
        goto_label = tokenGOTO >> label;
        program_name = SAME_RULE(ident);
        value_type = SAME_RULE(tokenINTEGER16);
        other_declaration_ident = tokenCOMMA >> ident;
        declaration = value_type >> ident >> *other_declaration_ident;
        //
        unary_operator = tokenNOT | tokenMINUS | tokenPLUS;
        unary_operation = unary_operator >> expression;
        binary_operator = tokenAND | tokenOR | tokenEQUAL | tokenNOTEQUAL | tokenLESSOREQUAL | tokenGREATEROREQUAL | tokenPLUS | tokenMINUS | tokenMUL | tokenDIV | tokenMOD;
        binary_action = binary_operator >> expression;
        //
        left_expression = group_expression | unary_operation | ident | value;
        expression = left_expression >> *binary_action;
        //
        group_expression = tokenGROUPEXPRESSIONBEGIN >> expression >> tokenGROUPEXPRESSIONEND;
        //
        bind_right_to_left = ident >> tokenRLBIND >> expression;
        bind_left_to_right = expression >> tokenLRBIND >> ident;
        //
        if_expression = SAME_RULE(expression);
        body_for_true = *statement >> tokenSEMICOLON;
        body_for_false = tokenELSE >> *statement >> tokenSEMICOLON;
        cond_block = tokenIF >> tokenGROUPEXPRESSIONBEGIN >> if_expression >> tokenGROUPEXPRESSIONEND >> body_for_true >> (-body_for_false);
        //
        cycle_begin_expression = SAME_RULE(expression);
        cycle_counter = SAME_RULE(ident);
        cycle_counter_rl_init = cycle_counter >> tokenRLBIND >> cycle_begin_expression;
        cycle_counter_lr_init = cycle_begin_expression >> tokenLRBIND >> cycle_counter;
        cycle_counter_init = cycle_counter_rl_init | cycle_counter_lr_init;
        cycle_counter_last_value = SAME_RULE(value);
        cycle_body = tokenDO >> statement >> *statement;
        forto_cycle = tokenFOR >> cycle_counter_init >> tokenTO >> cycle_counter_last_value >> cycle_body >> tokenSEMICOLON;
        //
        continue_while = tokenCONTINUE >> tokenWHILE;
        exit_while = tokenEXIT >> tokenWHILE;
        statement_in_while_body = statement | continue_while | exit_while;
        while_cycle_head_expression = SAME_RULE(expression);
        while_cycle = tokenWHILE >> while_cycle_head_expression >> *statement_in_while_body >> tokenEND >> tokenWHILE;
        //
        repeat_until_cycle_cond = SAME_RULE(group_expression);
        repeat_until_cycle = tokenREPEAT >> *statement >> tokenUNTIL >> repeat_until_cycle_cond;
        //
        input =
#ifdef DEBUG__IF_ERROR
            qi::eps >
#endif
            tokenGET >> tokenGROUPEXPRESSIONBEGIN >> ident >> tokenGROUPEXPRESSIONEND;
#ifdef DEBUG__IF_ERROR
        input.name("input");
        tokenGET.name("tokenGET");
        tokenGROUPEXPRESSIONBEGIN.name("tokenGROUPEXPRESSIONBEGIN");
        ident.name("ident");
        tokenGROUPEXPRESSIONEND.name("tokenGROUPEXPRESSIONEND");
#endif
        output = tokenPUT >> tokenGROUPEXPRESSIONBEGIN >> expression >> tokenGROUPEXPRESSIONEND;
        statement = bind_right_to_left | bind_left_to_right | cond_block | forto_cycle | while_cycle | repeat_until_cycle | labeled_point | goto_label | input | output;
        program = tokenNAME >> program_name >> tokenSEMICOLON >> tokenBODY >> tokenDATA >> (-declaration) >> tokenSEMICOLON >> *statement >> tokenEND;
        //
        digit = digit_0 | digit_1 | digit_2 | digit_3 | digit_4 | digit_5 | digit_6 | digit_7 | digit_8 | digit_9;
        non_zero_digit = digit_1 | digit_2 | digit_3 | digit_4 | digit_5 | digit_6 | digit_7 | digit_8 | digit_9;
        unsigned_value = ((non_zero_digit >> *digit) | digit_0) >> BOUNDARIES;
        value = (-sign) >> unsigned_value >> BOUNDARIES;
        letter_in_lower_case = a | b | c | d | e | f | g | h | i | j | k | l | m | n | o | p | q | r | s | t | u | v | w | x | y | z;
        letter_in_upper_case = A | B | C | D | E | F | G | H | I | J | K | L | M | N | O | P | Q | R | S | T | U | V | W | X | Y | Z;
        ident = tokenUNDERSCORE >> letter_in_upper_case >> letter_in_upper_case >> letter_in_upper_case >> letter_in_upper_case >> letter_in_upper_case >> letter_in_upper_case >> letter_in_upper_case >> STRICT_BOUNDARIES;
        label = letter_in_lower_case >> *letter_in_lower_case >> STRICT_BOUNDARIES;
        //
        sign = sign_plus | sign_minus;
        sign_plus = '-' >> BOUNDARIES;
        sign_minus = '+' >> BOUNDARIES;
        //
        digit_0 = '0';
        digit_1 = '1';
        digit_2 = '2';
        digit_3 = '3';
        digit_4 = '4';
        digit_5 = '5';
        digit_6 = '6';
        digit_7 = '7';
        digit_8 = '8';
        digit_9 = '9';
        //
        tokenCOLON = ":" >> BOUNDARIES;
        tokenGOTO = "GOTO" >> STRICT_BOUNDARIES;
        tokenINTEGER16 = "INTEGER16" >> STRICT_BOUNDARIES;
        tokenCOMMA = "," >> BOUNDARIES;
        tokenNOT = "NOT" >> STRICT_BOUNDARIES;
        tokenAND = "AND" >> STRICT_BOUNDARIES;
        tokenOR = "OR" >> STRICT_BOUNDARIES;
        tokenEQUAL = "==" >> BOUNDARIES;
        tokenNOTEQUAL = "!=" >> BOUNDARIES;
        tokenLESSOREQUAL = "<=" >> BOUNDARIES;
        tokenGREATEROREQUAL = ">=" >> BOUNDARIES;
        tokenPLUS = "+" >> BOUNDARIES;
        tokenMINUS = "-" >> BOUNDARIES;
        tokenMUL = "*" >> BOUNDARIES;
        tokenDIV = "DIV" >> STRICT_BOUNDARIES;
        tokenMOD = "MOD" >> STRICT_BOUNDARIES;
        tokenGROUPEXPRESSIONBEGIN = "(" >> BOUNDARIES;
        tokenGROUPEXPRESSIONEND = ")" >> BOUNDARIES;
        tokenRLBIND = "<<" >> BOUNDARIES;
        tokenLRBIND = ">>" >> BOUNDARIES;
        tokenELSE = "ELSE" >> STRICT_BOUNDARIES;
        tokenIF = "IF" >> STRICT_BOUNDARIES;
        tokenDO = "DO" >> STRICT_BOUNDARIES;
        tokenFOR = "FOR" >> STRICT_BOUNDARIES;
        tokenTO = "TO" >> STRICT_BOUNDARIES;
        tokenWHILE = "WHILE" >> STRICT_BOUNDARIES;
        tokenCONTINUE = "CONTINUE" >> STRICT_BOUNDARIES;
        tokenEXIT = "EXIT" >> STRICT_BOUNDARIES;
        tokenREPEAT = "REPEAT" >> STRICT_BOUNDARIES;
        tokenUNTIL = "UNTIL" >> STRICT_BOUNDARIES;
        tokenGET = "GET" >> STRICT_BOUNDARIES;
        tokenPUT = "PUT" >> STRICT_BOUNDARIES;
        tokenNAME = "NAME" >> STRICT_BOUNDARIES;
        tokenBODY = "BODY" >> STRICT_BOUNDARIES;
        tokenDATA = "DATA" >> STRICT_BOUNDARIES;
        tokenEND = "END" >> STRICT_BOUNDARIES;
        tokenSEMICOLON = ";" >> BOUNDARIES;
        // 
        //
        STRICT_BOUNDARIES = (BOUNDARY >> *(BOUNDARY)) | (!(qi::alpha | qi::char_("_")));
        BOUNDARIES = (BOUNDARY >> *(BOUNDARY) | NO_BOUNDARY);
        BOUNDARY = BOUNDARY_SPACE | BOUNDARY_TAB | BOUNDARY_CARRIAGE_RETURN | BOUNDARY_LINE_FEED | BOUNDARY_NULL;
        BOUNDARY_SPACE = " ";
        BOUNDARY_TAB = "\t";
        BOUNDARY_CARRIAGE_RETURN = "\r";
        BOUNDARY_LINE_FEED = "\n";
        BOUNDARY_NULL = "\0";
        NO_BOUNDARY = "";
        //
        tokenUNDERSCORE = "_";
        //
        A = "A";
        B = "B";
        C = "C";
        D = "D";
        E = "E";
        F = "F";
        G = "G";
        H = "H";
        I = "I";
        J = "J";
        K = "K";
        L = "L";
        M = "M";
        N = "N";
        O = "O";
        P = "P";
        Q = "Q";
        R = "R";
        S = "S";
        T = "T";
        U = "U";
        V = "V";
        W = "W";
        X = "X";
        Y = "Y";
        Z = "Z";
        //
        a = "a";
        b = "b";
        c = "c";
        d = "d";
        e = "e";
        f = "f";
        g = "g";
        h = "h";
        i = "i";
        j = "j";
        k = "k";
        l = "l";
        m = "m";
        n = "n";
        o = "o";
        p = "p";
        q = "q";
        r = "r";
        s = "s";
        t = "t";
        u = "u";
        v = "v";
        w = "w";
        x = "x";
        y = "y";
        z = "z";
        //
#ifdef DEBUG__IF_ERROR
        qi::on_error<qi::fail>(input,
            phx::ref(error_stream_) << "Error expecting " << qi::_4 << " here: \n"
            << phx::construct<std::string>(qi::_3, qi::_2) << "\n\n"
            );
#endif
    }
    std::ostringstream& error_stream_;

    qi::rule<Iterator>
        labeled_point,
        goto_label,
        program_name,
        value_type,
        other_declaration_ident,
        declaration,
        unary_operator,
        unary_operation,
        binary_operator,
        binary_action,
        left_expression,
        expression,
        group_expression,
        bind_right_to_left,
        bind_left_to_right,
        if_expression,
        body_for_true,
        body_for_false,
        cond_block,
        cycle_begin_expression,
        cycle_counter,
        cycle_counter_rl_init,
        cycle_counter_lr_init,
        cycle_counter_init,
        cycle_counter_last_value,
        cycle_body,
        forto_cycle,
        while_cycle_head_expression,
        while_cycle,
        continue_while,
        exit_while,
        statement_in_while_body,
        repeat_until_cycle_cond,
        repeat_until_cycle,
        input,
        output,
        statement,
        program,
        //
        tokenCOLON, tokenGOTO, tokenINTEGER16, tokenCOMMA, tokenNOT, tokenAND, tokenOR, tokenEQUAL, tokenNOTEQUAL, tokenLESSOREQUAL,
        tokenGREATEROREQUAL, tokenPLUS, tokenMINUS, tokenMUL, tokenDIV, tokenMOD, tokenGROUPEXPRESSIONBEGIN, tokenGROUPEXPRESSIONEND, tokenRLBIND, tokenLRBIND,
        tokenELSE, tokenIF, tokenDO, tokenFOR, tokenTO, tokenWHILE, tokenCONTINUE, tokenEXIT, tokenREPEAT, tokenUNTIL, tokenGET, tokenPUT, tokenNAME, tokenBODY, tokenDATA, tokenEND, tokenSEMICOLON,
        //
        STRICT_BOUNDARIES, BOUNDARIES, BOUNDARY, BOUNDARY_SPACE, BOUNDARY_TAB, BOUNDARY_CARRIAGE_RETURN, BOUNDARY_LINE_FEED, BOUNDARY_NULL,
        NO_BOUNDARY,
        //
        sign, sign_plus, sign_minus,
        digit_0, digit_1, digit_2, digit_3, digit_4, digit_5, digit_6, digit_7, digit_8, digit_9,
        digit, non_zero_digit, value, unsigned_value,
        letter_in_upper_case, letter_in_lower_case, ident,
        label,
        tokenUNDERSCORE,
        a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z;
};

template <typename Iterator>
struct cwgrammar_2 : qi::grammar<Iterator> {
#define ADDON_RULES_LIST \
value_type__ident,\
other_declaration_ident_____iteration
    cwgrammar_2(std::ostringstream& error_stream) : cwgrammar_2::base_type(program), error_stream_(error_stream) {
        //
        labeled_point = label >> tokenCOLON; // +
        goto_label = tokenGOTO >> label; // +
        program_name = SAME_RULE(ident); // + (!)
        value_type = SAME_RULE(tokenINTEGER16); // + (!)
        other_declaration_ident = (tokenCOMMA >> ident); // + (!)
        other_declaration_ident_____iteration = other_declaration_ident >> other_declaration_ident_____iteration // + (!)
            | tokenCOMMA >> ident; // + (!)
        value_type__ident = value_type >> ident; // + (!)
        declaration = value_type__ident >> other_declaration_ident_____iteration; // +
        //
        unary_operator = tokenNOT                       // + (!)
            | tokenMINUS                    // + (!)
            | tokenPLUS;                    // + (!)
        unary_operation = unary_operator >> expression; // +
        binary_operator = tokenAND                      // + (!)
            | tokenOR                      // + (!)
            | tokenEQUAL                   // + (!)
            | tokenNOTEQUAL                // + (!)
            | tokenLESSOREQUAL             // + (!)
            | tokenGREATEROREQUAL          // + (!)
            | tokenPLUS                    // + (!)
            | tokenMINUS                   // + (!)
            | tokenMUL                     // + (!)
            | tokenDIV                     // + (!)
            | tokenMOD;                    // + (!)
        binary_action = binary_operator >> expression;  // +
        //
        left_expression = group_expression | unary_operation | ident | value;
        expression = left_expression >> *binary_action;
        //
        group_expression = tokenGROUPEXPRESSIONBEGIN >> expression >> tokenGROUPEXPRESSIONEND;
        //
        bind_right_to_left = ident >> tokenRLBIND >> expression;
        bind_left_to_right = expression >> tokenLRBIND >> ident;
        //
        if_expression = SAME_RULE(expression);
        body_for_true = *statement >> tokenSEMICOLON;
        body_for_false = tokenELSE >> *statement >> tokenSEMICOLON;
        cond_block = tokenIF >> tokenGROUPEXPRESSIONBEGIN >> if_expression >> tokenGROUPEXPRESSIONEND >> body_for_true >> (-body_for_false);
        //
        cycle_begin_expression = SAME_RULE(expression);
        cycle_counter = SAME_RULE(ident);
        cycle_counter_rl_init = cycle_counter >> tokenRLBIND >> cycle_begin_expression;
        cycle_counter_lr_init = cycle_begin_expression >> tokenLRBIND >> cycle_counter;
        cycle_counter_init = cycle_counter_rl_init | cycle_counter_lr_init;
        cycle_counter_last_value = SAME_RULE(value);
        cycle_body = tokenDO >> statement >> *statement;
        forto_cycle = tokenFOR >> cycle_counter_init >> tokenTO >> cycle_counter_last_value >> cycle_body >> tokenSEMICOLON;
        //
        continue_while = tokenCONTINUE >> tokenWHILE;
        exit_while = tokenEXIT >> tokenWHILE;
        statement_in_while_body = statement | continue_while | exit_while;
        while_cycle_head_expression = SAME_RULE(expression);
        while_cycle = tokenWHILE >> while_cycle_head_expression >> *statement_in_while_body >> tokenEND >> tokenWHILE;
        //
        repeat_until_cycle_cond = SAME_RULE(group_expression);
        repeat_until_cycle = tokenREPEAT >> *statement >> tokenUNTIL >> repeat_until_cycle_cond;
        //
        input =
#ifdef DEBUG__IF_ERROR
            qi::eps >
#endif
            tokenGET >> tokenGROUPEXPRESSIONBEGIN >> ident >> tokenGROUPEXPRESSIONEND;
#ifdef DEBUG__IF_ERROR
        input.name("input");
        tokenGET.name("tokenGET");
        tokenGROUPEXPRESSIONBEGIN.name("tokenGROUPEXPRESSIONBEGIN");
        ident.name("ident");
        tokenGROUPEXPRESSIONEND.name("tokenGROUPEXPRESSIONEND");
#endif
        output = tokenPUT >> tokenGROUPEXPRESSIONBEGIN >> expression >> tokenGROUPEXPRESSIONEND;
        statement = bind_right_to_left | bind_left_to_right | cond_block | forto_cycle | while_cycle | repeat_until_cycle | labeled_point | goto_label | input | output;
        program = tokenNAME >> program_name >> tokenSEMICOLON >> tokenBODY >> tokenDATA >> (-declaration) >> tokenSEMICOLON >> *statement >> tokenEND;
        //
        digit = digit_0 | digit_1 | digit_2 | digit_3 | digit_4 | digit_5 | digit_6 | digit_7 | digit_8 | digit_9;
        non_zero_digit = digit_1 | digit_2 | digit_3 | digit_4 | digit_5 | digit_6 | digit_7 | digit_8 | digit_9;
        unsigned_value = ((non_zero_digit >> *digit) | digit_0) >> BOUNDARIES;
        value = (-sign) >> unsigned_value >> BOUNDARIES;
        letter_in_lower_case = a | b | c | d | e | f | g | h | i | j | k | l | m | n | o | p | q | r | s | t | u | v | w | x | y | z;
        letter_in_upper_case = A | B | C | D | E | F | G | H | I | J | K | L | M | N | O | P | Q | R | S | T | U | V | W | X | Y | Z;
        ident = tokenUNDERSCORE >> letter_in_upper_case >> letter_in_upper_case >> letter_in_upper_case >> letter_in_upper_case >> letter_in_upper_case >> letter_in_upper_case >> letter_in_upper_case >> STRICT_BOUNDARIES;
        label = letter_in_lower_case >> *letter_in_lower_case >> STRICT_BOUNDARIES;
        //
        sign = sign_plus | sign_minus;
        sign_plus = '-' >> BOUNDARIES;
        sign_minus = '+' >> BOUNDARIES;
        //
        digit_0 = '0';
        digit_1 = '1';
        digit_2 = '2';
        digit_3 = '3';
        digit_4 = '4';
        digit_5 = '5';
        digit_6 = '6';
        digit_7 = '7';
        digit_8 = '8';
        digit_9 = '9';
        //
        tokenCOLON = ":" >> BOUNDARIES;
        tokenGOTO = "GOTO" >> STRICT_BOUNDARIES;
        tokenINTEGER16 = "INTEGER16" >> STRICT_BOUNDARIES;
        tokenCOMMA = "," >> BOUNDARIES;
        tokenNOT = "NOT" >> STRICT_BOUNDARIES;
        tokenAND = "AND" >> STRICT_BOUNDARIES;
        tokenOR = "OR" >> STRICT_BOUNDARIES;
        tokenEQUAL = "==" >> BOUNDARIES;
        tokenNOTEQUAL = "!=" >> BOUNDARIES;
        tokenLESSOREQUAL = "<=" >> BOUNDARIES;
        tokenGREATEROREQUAL = ">=" >> BOUNDARIES;
        tokenPLUS = "+" >> BOUNDARIES;
        tokenMINUS = "-" >> BOUNDARIES;
        tokenMUL = "*" >> BOUNDARIES;
        tokenDIV = "DIV" >> STRICT_BOUNDARIES;
        tokenMOD = "MOD" >> STRICT_BOUNDARIES;
        tokenGROUPEXPRESSIONBEGIN = "(" >> BOUNDARIES;
        tokenGROUPEXPRESSIONEND = ")" >> BOUNDARIES;
        tokenRLBIND = "<<" >> BOUNDARIES;
        tokenLRBIND = ">>" >> BOUNDARIES;
        tokenELSE = "ELSE" >> STRICT_BOUNDARIES;
        tokenIF = "IF" >> STRICT_BOUNDARIES;
        tokenDO = "DO" >> STRICT_BOUNDARIES;
        tokenFOR = "FOR" >> STRICT_BOUNDARIES;
        tokenTO = "TO" >> STRICT_BOUNDARIES;
        tokenWHILE = "WHILE" >> STRICT_BOUNDARIES;
        tokenCONTINUE = "CONTINUE" >> STRICT_BOUNDARIES;
        tokenEXIT = "EXIT" >> STRICT_BOUNDARIES;
        tokenREPEAT = "REPEAT" >> STRICT_BOUNDARIES;
        tokenUNTIL = "UNTIL" >> STRICT_BOUNDARIES;
        tokenGET = "GET" >> STRICT_BOUNDARIES;
        tokenPUT = "PUT" >> STRICT_BOUNDARIES;
        tokenNAME = "NAME" >> STRICT_BOUNDARIES;
        tokenBODY = "BODY" >> STRICT_BOUNDARIES;
        tokenDATA = "DATA" >> STRICT_BOUNDARIES;
        tokenEND = "END" >> STRICT_BOUNDARIES;
        tokenSEMICOLON = ";" >> BOUNDARIES;
        // 
        //
        STRICT_BOUNDARIES = (BOUNDARY >> *(BOUNDARY)) | (!(qi::alpha | qi::char_("_")));
        BOUNDARIES = (BOUNDARY >> *(BOUNDARY) | NO_BOUNDARY);
        BOUNDARY = BOUNDARY_SPACE | BOUNDARY_TAB | BOUNDARY_CARRIAGE_RETURN | BOUNDARY_LINE_FEED | BOUNDARY_NULL;
        BOUNDARY_SPACE = " ";
        BOUNDARY_TAB = "\t";
        BOUNDARY_CARRIAGE_RETURN = "\r";
        BOUNDARY_LINE_FEED = "\n";
        BOUNDARY_NULL = "\0";
        NO_BOUNDARY = "";
        //
        tokenUNDERSCORE = "_";
        //
        A = "A";
        B = "B";
        C = "C";
        D = "D";
        E = "E";
        F = "F";
        G = "G";
        H = "H";
        I = "I";
        J = "J";
        K = "K";
        L = "L";
        M = "M";
        N = "N";
        O = "O";
        P = "P";
        Q = "Q";
        R = "R";
        S = "S";
        T = "T";
        U = "U";
        V = "V";
        W = "W";
        X = "X";
        Y = "Y";
        Z = "Z";
        //
        a = "a";
        b = "b";
        c = "c";
        d = "d";
        e = "e";
        f = "f";
        g = "g";
        h = "h";
        i = "i";
        j = "j";
        k = "k";
        l = "l";
        m = "m";
        n = "n";
        o = "o";
        p = "p";
        q = "q";
        r = "r";
        s = "s";
        t = "t";
        u = "u";
        v = "v";
        w = "w";
        x = "x";
        y = "y";
        z = "z";
        //
#ifdef DEBUG__IF_ERROR
        qi::on_error<qi::fail>(input,
            phx::ref(error_stream_) << "Error expecting " << qi::_4 << " here: \n"
            << phx::construct<std::string>(qi::_3, qi::_2) << "\n\n"
            );
#endif
    }
    std::ostringstream& error_stream_;

    qi::rule<Iterator>
        ADDON_RULES_LIST,
        labeled_point,
        goto_label,
        program_name,
        value_type,
        other_declaration_ident,
        declaration,
        unary_operator,
        unary_operation,
        binary_operator,
        binary_action,
        left_expression,
        expression,
        group_expression,
        bind_right_to_left,
        bind_left_to_right,
        if_expression,
        body_for_true,
        body_for_false,
        cond_block,
        cycle_begin_expression,
        cycle_counter,
        cycle_counter_rl_init,
        cycle_counter_lr_init,
        cycle_counter_init,
        cycle_counter_last_value,
        cycle_body,
        forto_cycle,
        while_cycle_head_expression,
        while_cycle,
        continue_while,
        exit_while,
        statement_in_while_body,
        repeat_until_cycle_cond,
        repeat_until_cycle,
        input,
        output,
        statement,
        program,
        //
        tokenCOLON, tokenGOTO, tokenINTEGER16, tokenCOMMA, tokenNOT, tokenAND, tokenOR, tokenEQUAL, tokenNOTEQUAL, tokenLESSOREQUAL,
        tokenGREATEROREQUAL, tokenPLUS, tokenMINUS, tokenMUL, tokenDIV, tokenMOD, tokenGROUPEXPRESSIONBEGIN, tokenGROUPEXPRESSIONEND, tokenRLBIND, tokenLRBIND,
        tokenELSE, tokenIF, tokenDO, tokenFOR, tokenTO, tokenWHILE, tokenCONTINUE, tokenEXIT, tokenREPEAT, tokenUNTIL, tokenGET, tokenPUT, tokenNAME, tokenBODY, tokenDATA, tokenEND, tokenSEMICOLON,
        //
        STRICT_BOUNDARIES, BOUNDARIES, BOUNDARY, BOUNDARY_SPACE, BOUNDARY_TAB, BOUNDARY_CARRIAGE_RETURN, BOUNDARY_LINE_FEED, BOUNDARY_NULL,
        NO_BOUNDARY,
        //
        sign, sign_plus, sign_minus,
        digit_0, digit_1, digit_2, digit_3, digit_4, digit_5, digit_6, digit_7, digit_8, digit_9,
        digit, non_zero_digit, value, unsigned_value,
        letter_in_upper_case, letter_in_lower_case, ident,
        label,
        tokenUNDERSCORE,
        a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z;
};

// after using this function use free(void *) function to release text buffer
size_t loadSource(char** text, char* fileName) {
    if (!fileName) {
        printf("No input file name\r\n");
        return 0;
    }

    FILE* file = fopen(fileName, "rb");

    if (file == NULL) {
        printf("File not loaded\r\n");
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long fileSize_ = ftell(file);
    if (fileSize_ >= MAX_TEXT_SIZE) {
        printf("the file(%ld bytes) is larger than %d bytes\r\n", fileSize_, MAX_TEXT_SIZE);
        fclose(file);
        exit(2); // TODO: ...
        //return 0;
    }
    size_t fileSize = fileSize_;
    rewind(file);

    if (!text) {
        printf("Load source error\r\n");
        return 0;
    }
    *text = (char*)malloc(sizeof(char) * (fileSize + 1));
    if (*text == NULL) {
        fputs("Memory error", stderr);
        fclose(file);
        exit(2); // TODO: ...
        //return 0;
    }

    size_t result = fread(*text, sizeof(char), fileSize, file);
    if (result != fileSize) {
        fputs("Reading error", stderr);
        fclose(file);
        exit(3); // TODO: ...
        //return 0;
    }
    (*text)[fileSize] = '\0';

    fclose(file);

    return fileSize;
}

int main() {
    char* text_;
    char fileName[64] = "file1.cwl";
    std::cout << "Enter file name(Enter \"f\" to use default \"file1.cwl\"):\n";
    std::cin >> fileName;
    if (fileName[0] == 'f' && fileName[1] == '\0') {
        fileName[1] = 'i';
    }
    size_t sourceSize = loadSource(&text_, (char*)"file1.cwl");
    if (!sourceSize) {
        printf("Press Enter to exit . . .");
        (void)getchar();
        return 0;
    }
    std::string text(text_);

    typedef std::string     str_t;
    typedef str_t::iterator str_t_it;

    std::ostringstream error_stream;
    cwgrammar_2<str_t_it> cwg(error_stream);

    str_t_it begin = text.begin(), end = text.end();


    bool success = qi::parse(begin, end, cwg);

    if (!success) {
        std::cout << "\nParsing failed!\n";
        std::cout << "Error message: " << error_stream.str();
    }
    else if (begin != end) {
        std::cout << "\nUnknown fragment ofter successs parse at: \"" << str_t(begin, end) << "\"\n";
    }
    else {
        std::cout << "\nParsing success!\n";
    }

    free(text_);
}
