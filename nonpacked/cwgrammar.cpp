#define _CRT_SECURE_NO_WARNINGS  // for using sscanf in VS
/*******************************************************************
* N.Kozak // Lviv'2024 // example syntax analysis by boost::spirit *
*                         file: cwsyntax.cpp                       *
*                                                  (draft version) *
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
    cwgrammar(std::ostringstream& error_stream): cwgrammar::base_type(program), error_stream_(error_stream) {
        //
        recursive_descent_end_point = SAME_RULE(tokenRECURSIVEDESCENTENDPOINT);
        //
        value_read = SAME_RULE(value);
        ident_read = SAME_RULE(ident);
        ident_write = SAME_RULE(ident);
        //
        labeled_point = label >> tokenCOLON;
        goto_label = tokenGOTO >> label;
        program_name = SAME_RULE(ident);
        value_type = SAME_RULE(tokenINTEGER16);
        declaration_ident = SAME_RULE(ident);
        other_declaration_ident = tokenCOMMA >> ident;
        declaration = value_type >> declaration_ident >> *other_declaration_ident;
        operation_not = tokenNOT >> inseparable_expression;
        and_action = tokenAND >> inseparable_expression;
        or_action = tokenOR >> high_prioryty_expression;
        equal_action = tokenEQUAL >> middle_prioryty_expression;
        not_equal_action = tokenNOTEQUAL >> middle_prioryty_expression;
        less_or_equal_action = tokenLESSOREQUAL >> middle_prioryty_expression;
        greater_or_equal_action = tokenGREATEROREQUAL >> middle_prioryty_expression;
        add_action = tokenPLUS >> high_prioryty_expression;
        sub_action = tokenMINUS >> high_prioryty_expression;
        mul_action = tokenMUL >> inseparable_expression;
        div_action = tokenDIV >> inseparable_expression;
        mod_action = tokenMOD >> inseparable_expression;
        unary_operation = SAME_RULE(operation_not);
        //
        inseparable_expression = group_expression | unary_operation | ident_read | value_read;
        //
        high_prioryty_left_expression = group_expression | unary_operation | ident_read | value_read;
        high_prioryty_action = recursive_descent_end_point | mul_action | div_action | mod_action | and_action;
        high_prioryty_expression = high_prioryty_left_expression >> *high_prioryty_action;
        //
        middle_prioryty_left_expression = high_prioryty_expression | group_expression | unary_operation | ident_read | value_read;
        middle_prioryty_action = recursive_descent_end_point | add_action | sub_action | or_action;
        middle_prioryty_expression = middle_prioryty_left_expression >> *middle_prioryty_action;
        //
        low_prioryty_left_expression = middle_prioryty_expression | high_prioryty_expression | group_expression | unary_operation | ident_read | value_read;
        low_prioryty_action = recursive_descent_end_point | less_or_equal_action | greater_or_equal_action | equal_action | not_equal_action;
        low_prioryty_expression = low_prioryty_left_expression >> *low_prioryty_action;
        //
        group_expression = tokenGROUPEXPRESSIONBEGIN >> low_prioryty_expression >> groupGROUPEXPRESSIONEND;
        //
        bind = low_prioryty_expression >> tokenBIND >> ident_write;
        //
        if_expression = SAME_RULE(low_prioryty_expression); // !!!
        body_for_true = tokenTHEN >> *statement >> tokenSEMICOLON;
        body_for_false = tokenELSE >> *statement >> tokenSEMICOLON;
        cond_block = tokenIF >> if_expression >> body_for_true >> (-body_for_false);
        //
        cycle_begin_expression = SAME_RULE(low_prioryty_expression);
        cycle_counter = SAME_RULE(ident);
        cycle_counter_last_value = SAME_RULE(value);
        cycle_body = tokenDO >> statement >> *statement;
        forto_cycle = tokenFOR >> cycle_begin_expression >> tokenBIND >> cycle_counter >> tokenTO >> cycle_counter_last_value >> cycle_body >> tokenSEMICOLON;
        //
        while_cycle_head_expression = SAME_RULE(low_prioryty_expression);
        while_cycle = tokenWHILE >> while_cycle_head_expression >> *statement >> tokenSEMICOLON;
        //
        do_while_cycle_cond = SAME_RULE(low_prioryty_expression);
        do_while_cycle = tokenDO >> *statement >> tokenWHILE >> do_while_cycle_cond;
        //
        input =
#ifdef DEBUG__IF_ERROR
            qi::eps > 
#endif
            tokenGET >> tokenGROUPEXPRESSIONBEGIN >> ident_write >> groupGROUPEXPRESSIONEND;
#ifdef DEBUG__IF_ERROR
        input.name("input");
        tokenGET.name("tokenGET");
        tokenGROUPEXPRESSIONBEGIN.name("tokenGROUPEXPRESSIONBEGIN");
        ident_write.name("ident_write");
        groupGROUPEXPRESSIONEND.name("groupGROUPEXPRESSIONEND");
#endif
        output = tokenPUT >> tokenGROUPEXPRESSIONBEGIN >> low_prioryty_expression >> groupGROUPEXPRESSIONEND;
        statement = recursive_descent_end_point | bind | cond_block | forto_cycle | while_cycle | do_while_cycle | labeled_point | goto_label | input | output;
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
        sign_plus  = '-';
        sign_minus = '+';
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
        tokenRECURSIVEDESCENTENDPOINT = "RECURSIVE_DESCENT_END_POINT" >> STRICT_BOUNDARIES; // TODO:...
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
        groupGROUPEXPRESSIONEND = ")" >> BOUNDARIES;
        tokenBIND = ">>" >> BOUNDARIES;
        tokenTHEN = "THEN" >> STRICT_BOUNDARIES;
        tokenELSE = "ELSE" >> STRICT_BOUNDARIES;
        tokenIF = "IF" >> STRICT_BOUNDARIES;
        tokenDO = "DO" >> STRICT_BOUNDARIES;
        tokenFOR = "FOR" >> STRICT_BOUNDARIES;
        tokenTO = "TO" >> STRICT_BOUNDARIES;
        tokenWHILE = "WHILE" >> STRICT_BOUNDARIES;
        tokenGET = "GET" >> STRICT_BOUNDARIES;
        tokenPUT = "PUT" >> STRICT_BOUNDARIES;
        tokenNAME = "NAME" >> STRICT_BOUNDARIES;
        tokenBODY = "BODY" >> STRICT_BOUNDARIES;
        tokenDATA = "DATA" >> STRICT_BOUNDARIES;
        tokenEND  = "END" >> STRICT_BOUNDARIES;
        tokenSEMICOLON = ";" >> BOUNDARIES;
        // 
        //
        STRICT_BOUNDARIES = (BOUNDARY >> *(BOUNDARY)) | (!(qi::alpha | qi::char_("_")));
        BOUNDARIES =  (BOUNDARY >> *(BOUNDARY) | NO_BOUNDARY);
        BOUNDARY = BOUNDARY_SPACE | BOUNDARY_TAB | BOUNDARY_CARRIAGE_RETURN | BOUNDARY_LINE_FEED | BOUNDARY_NULL;
        BOUNDARY_SPACE           = " ";
        BOUNDARY_TAB             = "\t";
        BOUNDARY_CARRIAGE_RETURN = "\r";
        BOUNDARY_LINE_FEED       = "\n";
        BOUNDARY_NULL            = "\0";
        NO_BOUNDARY              = "";
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
        recursive_descent_end_point,
        value_read,
        ident_read,
        ident_write,
        labeled_point,
        goto_label,
        program_name,
        value_type,
        declaration_ident,
        other_declaration_ident,
        declaration,
        operation_not,
        and_action,
        or_action,
        equal_action,
        not_equal_action,
        less_or_equal_action,
        greater_or_equal_action,
        add_action,
        sub_action,
        mul_action,
        div_action,
        mod_action,
        unary_operation,
        inseparable_expression,
        high_prioryty_left_expression,
        high_prioryty_action,
        high_prioryty_expression,
        middle_prioryty_left_expression,
        middle_prioryty_action,
        middle_prioryty_expression,
        low_prioryty_left_expression,
        low_prioryty_action,
        low_prioryty_expression,
        group_expression,
        bind,
        if_expression,
        body_for_true,
        body_for_false,
        cond_block,
        cycle_begin_expression,
        cycle_counter,
        cycle_counter_last_value,
        cycle_body,
        forto_cycle,
        while_cycle_head_expression,
        while_cycle,
        do_while_cycle_cond,
        do_while_cycle,
        input,
        output,
        statement,
        program,
        //
        tokenRECURSIVEDESCENTENDPOINT, tokenCOLON, tokenGOTO, tokenINTEGER16, tokenCOMMA, tokenNOT, tokenAND, tokenOR, tokenEQUAL, tokenNOTEQUAL, tokenLESSOREQUAL, 
        tokenGREATEROREQUAL, tokenPLUS, tokenMINUS, tokenMUL, tokenDIV, tokenMOD, tokenGROUPEXPRESSIONBEGIN, groupGROUPEXPRESSIONEND, tokenBIND, tokenTHEN, 
        tokenELSE, tokenIF, tokenDO, tokenFOR, tokenTO, tokenWHILE, tokenGET, tokenPUT, tokenNAME, tokenBODY, tokenDATA, tokenEND, tokenSEMICOLON,
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

int main(){
    char* text_;
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
    cwgrammar<str_t_it> cwg(error_stream);

    str_t_it begin = text.begin(), end = text.end();

    //unsigned result;
    bool success = qi::parse(begin, end, cwg/*, result*/);

    if(!success){
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