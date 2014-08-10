#ifndef __Z2H_PARSER__
#define __Z2H_PARSER__ = 1

#include <cmath>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stddef.h>
#include <sys/stat.h>
#include <functional>

#include "token.hpp"
#include "symbol.hpp"
#include "binder.hpp"

using namespace std::placeholders;

namespace z2h {

    template <typename TAst>
    class Token;

    template <typename TAst>
    class Symbol;

    class ParserException : public std::exception {
        const char          *_file;
        size_t              _line;
        const std::string   _message;
        std::string         _what;
    public:
        ParserException(const char *file, size_t line, const std::string &message)
            : _file(file)
            , _line(line)
            , _message(message) {
            std::ostringstream out;
            out << _file << ":" << _line << " " << _message << std::endl;
            _what = out.str();
        }
        virtual const char * what() const throw() {
            return _what.c_str();
        }
    };

    template <typename TAst, typename TParser>
    struct Parser : public Binder<TAst, TParser> {

        std::string                 source;
        size_t                      position;
        std::vector<Token<TAst> *>  tokens;
        size_t                      index;

        ~Parser() {
            while (!tokens.empty())
                delete tokens.back(), tokens.pop_back();
        }

        Parser()
            : source("")
            , position(0)
            , tokens({})
            , index(0) {
        }

        // Symbols must be defined by the inheriting parser
        virtual std::vector<Symbol<TAst> *> Symbols() = 0;

        std::string Open(const std::string &filename) {
            struct stat buffer;
            if (stat(filename.c_str(), &buffer) != 0)
                ParserException(__FILE__, __LINE__, filename + " doesn't exist or is unreadable");
            std::ifstream file(filename);
            std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            return text;
        }

        Token<TAst> * Scan() {

            auto eof = Symbols()[0];
            Token<TAst> *match = nullptr;
            if (position < source.length()) {
                for (auto symbol : Symbols()) {
                    auto token = symbol->Scan(symbol, source.substr(position, source.length() - position), position);
                    if (nullptr == match) {
                        match = token;
                    }
                    else if (nullptr != token && (token->length > match->length || (token->symbol->lbp > match->symbol->lbp && token->length == match->length))) {
                        delete match;
                        match = token;
                    } else {
                        delete token;
                    }
                }
                if (nullptr == match) {
                    throw ParserException(__FILE__, __LINE__, "Parser::Scan: invalid symbol");
                }
                return match;
            }
            return new Token<TAst>(eof, "EOF", position, 0, false); //eof
        }

        Token<TAst> * LookAhead(size_t &distance, bool skips = false) {
            Token<TAst> *token = nullptr;
            auto i = index;
            while (distance) {
                if (i < tokens.size()) {
                    token = tokens[i];
                }
                else {
                    token = Scan();
                    position += token->length;
                    tokens.push_back(token);
                }
                if (skips || !token->skip)
                    --distance;
                ++i;
            }
            distance = i - index;
            return token;
        }

        Token<TAst> * Consume(Symbol<TAst> *expected = nullptr, const std::string &message = "") {
            size_t distance = 1;
            auto token = LookAhead(distance);
            index += distance;
            if (nullptr != expected && *expected != *token->symbol)
                throw ParserException(__FILE__, __LINE__, message);
            return token;
        }

        std::vector<Token<TAst> *> TokenizeFile(const std::string &filename) {
            auto source = Open(filename);
            return Tokenize(source);
        }

        std::vector<Token<TAst> *> Tokenize(std::string source) {
            this->index = 0;
            this->source = source;
            auto eof = Symbols()[0];
            auto token = Consume();
            while (*eof != *token->symbol) {
                token = Consume();
            }
            return tokens;
        }

        TAst ParseFile(const std::string &filename) {
            auto source = Open(filename);
            return Parse(source);
        }

        TAst Parse(std::string source) {
            this->index = 0;
            this->source = source;
            return Expression();
        }

        TAst Expression(size_t rbp = 0) {

            auto *curr = Consume();
            if (nullptr == curr->symbol->Nud) {
                std::ostringstream out;
                out << "unexpected: nullptr==Nud curr=" << *curr;
                throw ParserException(__FILE__, __LINE__, out.str());
            }

            TAst left = curr->symbol->Nud(curr);

            size_t distance = 1;
            auto *next = LookAhead(distance);
            while (rbp < next->symbol->lbp) {
                next = Consume();
                left = next->symbol->Led(left, next);
            }

            return left;
        }

        TAst Statement() {
            size_t distance = 1;
            auto *la1 = LookAhead(distance);
            if (la1->symbol->Std) {
                Consume();
                return la1->symbol->Std();
            }
            auto ast = Expression();
            Consume(1, "EndOfStatement expected!");
            return ast;
        }

        size_t Line() {
            return 0;
        }

        size_t Column() {
            return 0;
        }
        
    };

}

#endif /*__Z2H_PARSER__*/
