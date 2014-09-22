#ifndef __Z2H_LEXER__
#define __Z2H_LEXER__ = 1

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

namespace z2h {

    struct Lexer {
        std::string source;
        size_t      position;

        virtual ~Lexer() {}
        Lexer(std::string source)
            : source(source)
            , position(0) {}

        virtual Token * Scan() = 0;
    };
}

#endif /*__Z2H_LEXER__*/
