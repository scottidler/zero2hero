#ifndef __Z2H_PARSER__
#define __Z2H_PARSER__ = 1

#include <cmath>
#include <string>
#include <vector>
#include <fstream>
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
        std::string _message;
    public:
        ParserException(const std::string &message)
            : _message(message) {}
        virtual const char * what() const throw() {
            return _message.c_str();
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

        std::string Load(const std::string &filename) {
            struct stat buffer;
            if (stat (filename.c_str(), &buffer) != 0)
                ParserException(filename + " doesn't exist or is unreadable");
            std::ifstream file(filename);
            std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            return text;
        }

        Token<TAst> * Take(Token<TAst> *token) {
            tokens.push_back(token);
            return token;
        }

        Token<TAst> * Emit() {
            if (index < tokens.size()) {
                return tokens[index++];
            }
            return Scan();
        }

        Token<TAst> * Scan() {

            size_t end = position;
            Symbol<TAst> *match = nullptr;
            bool skip = false;
            if (position < source.length()) {
                std::cout << "position: " << position << std::endl;
                std::cout << "Symbols().size(): " << Symbols().size() << std::endl;
                for (auto symbol : Symbols()) {
                    std::cout << "symbol: " << *symbol << std::endl;
                    long delta = symbol->Scan(symbol, source.substr(position, source.length() - position), position);
                    if (delta) {
                        std::cout << "found: " << source.substr(position, abs(delta) ) << std::endl;
                        if (abs(delta) > 0 || (match != nullptr && symbol->lbp > match->lbp && position + abs(delta) == end)) {
                            match = symbol;
                            end = position + abs(delta);
                            skip = delta < 0;
                        }
                    }
                }
                if (position == end) {
                    throw ParserException("Parser::Scan: invalid symbol");
                }
                std::cout << "match: " << *match << std::endl;
                return new Token<TAst>(match, source, position, end - position, skip);
            }
            std::cout << "failed to match anything" << std::endl;
            return new Token<TAst>(); //eof
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
            if (curr->symbol->type) {
                std::cout << "first used" << std::endl;
                return nullptr;
            }

            TAst left = curr->symbol->Nud(curr);

            auto *next = LookAhead(1);
            if (next->symbol->type) {
                std::cout << "second used" << std::endl;
                return left;
            }

            while (rbp < next->symbol->lbp) {
                next = Consume();
                if (next->symbol->type) {
                    std::cout << "third used" << std::endl;
                    return left;
                }

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

        Token<TAst> * LookAhead(size_t distance) {
            if (distance == 0)
                return tokens[index];

            while(distance > tokens.size() - index) {
                auto *token = Scan();
                if (token->skip) {
                    position += token->length;
                    continue;
                }
                Take(Scan());
            }

            return Emit();
        }

        Token<TAst> * Consume() {
            LookAhead(1);
            auto curr = Emit();
            position += curr->length;
            return curr;
        }

        Token<TAst> * Consume(const size_t &expected, const std::string &message) {
            auto token = Consume();
            if (token->symbol->type != expected)
                throw ParserException(message);
            return token;
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
