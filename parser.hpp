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

    template <typename TAst>
    class Grammar;

    class ParserException : public std::exception {
        const char *_filename;
        size_t _line;
        const std::string _message;
        std::string _what;
    public:
        ParserException(const char *filename, size_t line, const std::string &message)
            : _filename(filename)
            , _line(line)
            , _message(message) {
            std::ostringstream out;
            out << "filename=" << _filename
                << " line=" << _line
                << " msg=" << _message
                << std::endl;
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

        Token<TAst> * Scan() {

            auto eof = Symbols()[0];
            Symbol<TAst> *match = nullptr;
            if (position < source.length()) {
                size_t end = position;
                bool skip = false;
                for (auto symbol : Symbols()) {
                    long length = symbol->Scan(symbol, source.substr(position, source.length() - position), position);
                    if (position + abs(length) > end || (match != nullptr && symbol->lbp > match->lbp && position + abs(length) == end)) {
                        match = symbol;
                        end = position + abs(length);
                        skip = length < 0;
                    }
                }
                if (position == end) {
                    throw ParserException(__FILE__, __LINE__, "Parser::Scan: invalid symbol");
                }
                return new Token<TAst>(match, source, position, end - position, skip);
            }
            return new Token<TAst>(eof, source, position, 0, false); //eof
        }
/*
        Token<TAst> * LookAhead(size_t distance, bool skips = false) {
            if (distance == 0) {
                return tokens[index];
            }
            Token<TAst> *token = nullptr;
            size_t d = distance;
            while (index + d >= tokens.size()) {
                token = Scan();
                position += token->length;
                tokens.push_back(token);
                if (!token->symbol->type)
                    return token;
                if (!skips && token->skip)
                    ++d;
            }
            if (skips)
                return tokens[index + distance - 1];
            int i = index;
            for (; i < tokens.size() && distance; ++i) {
                if (!tokens[i]->skip)
                    --distance;
            }
            token = tokens[i - 1];
            return token;
        }
*/
        Token<TAst> * LookAhead(size_t distance, bool skips = false) {
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
            return token;
        }

        Token<TAst> * Consume(Symbol<TAst> *expected = nullptr, const std::string &message = "") {
            auto token = LookAhead(1);
            while (token->position != tokens[index++]->position);
            if (nullptr != expected && *expected != *token->symbol)
                throw ParserException(__FILE__, __LINE__, message);
            return token;
        }

        std::vector<Token<TAst> *> Tokenize() {
            auto eof = Symbols()[0];
            auto token = Consume();
            while (*eof != *token->symbol) {
                token = Consume();
            }
            return tokens;
        }

        std::string Load(const std::string &filename) {
            struct stat buffer;
            if (stat (filename.c_str(), &buffer) != 0)
                ParserException(__FILE__, __LINE__, filename + " doesn't exist or is unreadable");
            std::ifstream file(filename);
            std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            return text;
        }

        TAst ParseFile(const std::string &filename) {
            auto source = Load(filename);
            return Parse(source);
        }

        TAst Parse(std::string source) {
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

            auto *next = LookAhead(1);
            while (rbp < next->symbol->lbp) {
                next = Consume();
                left = next->symbol->Led(left, next);
            }

            return left;
        }

        TAst Statement() {
            auto *la1 = LookAhead(1);
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
