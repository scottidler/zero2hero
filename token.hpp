#ifndef __Z2H_TOKEN__
#define __Z2H_TOKEN__ = 1

#include <string>
#include <iostream>

namespace z2h {

    inline std::string escape(std::string text) {
        std::string result = "";
        for (char c : text) {
            switch (c) {
                case '\n':
                    result += "\\n";
                    break;
                case '\r':
                    result += "\\r";
                    break;
                case '\t':
                    result += "\\t";
                    break;
                default:
                    result += c;
            }
        }
        return result;
    }

    struct Token {
        size_t          type;
        std::string     value;
        size_t          position;
        size_t          length;
        bool            skip;

        Token()
            : type(0)
            , value("")
            , position(0)
            , length(0)
            , skip(false) {
        }

        Token(size_t type, std::string value, size_t position, size_t length, bool skip = false)
            : type(type)
            , value(value)
            , position(position)
            , length(length)
            , skip(skip) {
        }

        operator size_t() { return type; }

        operator bool() {
           return type;
        }

        friend std::ostream & operator<<(std::ostream &out, const Token &token) {
            return out
                << "(Token: type=" << token.type
                << " value=" << escape(token.value)
                << " position=" << token.position
                << " length=" << token.length
                << " skip=" << (token.skip ? "true" : "false")
                <<  ")";
        }
    };

}

#endif /*__Z2H_TOKEN__*/
