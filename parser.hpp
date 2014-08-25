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

#include "symbol.hpp"
#include "token.hpp"
#include "binder.hpp"

using namespace std::placeholders;

namespace z2h {

    template <typename TParser>
    struct Parser : public Binder<TParser> {

        std::string                 source;
        size_t                      position;
        std::vector<Token *>        tokens;
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

        // Exception must be defined by the inheriting parser, throw exceptions defined there
        virtual std::exception Exception(const char *file, size_t line, const std::string &message = "") = 0;

        // Symbols must be defined by the inheriting parser
        virtual std::vector<Symbol *> Symbols() = 0;

        // the default for the eof symbol is first in in the list of symbols
        virtual Symbol * EofSymbol() {
            return Symbols()[0];
        }

        // the default for the eos symbol is second in in the list of symbols
        virtual Symbol * EosSymbol() {
            return Symbols()[1];
        }

        std::string Open(const std::string &filename) {
            struct stat buffer;
            if (stat(filename.c_str(), &buffer) != 0)
                Exception(__FILE__, __LINE__, filename + " doesn't exist or is unreadable");
            std::ifstream file(filename);
            std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            return text;
        }

        std::vector<Token *> TokenizeFile(const std::string &filename) {
            auto source = Open(filename);
            return Tokenize(source);
        }

        std::vector<Token *> Tokenize(std::string source) {
            this->index = 0;
            this->source = source;
            auto eof = EofSymbol();
            auto token = Consume();
            while (*eof != *token->symbol) {
                token = Consume();
            }
            return tokens;
        }

        virtual std::vector<Ast *> ParseFile(const std::string &filename) {
            auto source = Open(filename);
            return Parse(source);
        }

        virtual std::vector<Ast *> Parse(std::string source) {
            this->index = 0;
            this->source = source;
            return {Statement()};
            //return Statements(); FIXME: return this back to normal after fixing the bug... -sai
        }

        virtual Token * Scan() {

            auto eof = EofSymbol();
            Token *match = nullptr;
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
                    throw Exception(__FILE__, __LINE__, "Parser::Scan: invalid symbol");
                }
                return match;
            }
            std::cout << "FAILED SCAN" << std::endl;
            return new Token(eof, "EOF", position, 0, false); //eof
        }

        virtual Token * LookAhead(size_t &distance, bool skips = false) {
            Token *token = nullptr;
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
                if (skips || !token->skip) {
                    --distance;
                }
                ++i;
            }
            distance = i - index;
            return token;
        }

        Token * LookAhead1() {
            size_t distance = 1;
            return this->LookAhead(distance);
        }

        Token * LookAhead2() {
            size_t distance = 2;
            return this->LookAhead(distance);
        }

        virtual Token * Consume(Symbol *expected = nullptr) {
            if (expected) {
                return Consume({expected});
            }
            return Consume({});
        }

        virtual Token * Consume(std::initializer_list<Symbol *> expectations) {
            size_t distance = 1;
            auto token = LookAhead(distance);
            if (expectations.size()) {
                for (auto expectation : expectations) {
                    if (expectation) {
                        if (expectation == token->symbol) {
                            index += distance;
                            return token;
                        }
                    }
                    /*
                    if (expected && expected == token->symbol) {
                        index += distance;
                        return token;
                    }
                    */
                }
                return nullptr;
            }
            index += distance;
            return token;
        }

        virtual Ast * Expression(size_t rbp = 0) {

            auto *curr = Consume();
            size_t distance = 1;
            auto *next = LookAhead(distance);
            Ast *left = curr->symbol->Nud(curr);
            while (rbp < next->symbol->lbp) {
                curr = Consume();
                if (nullptr == curr->symbol->Led) {
                    std::cout << __LINE__ << "no Led: curr=" << *curr << std::endl;
                    std::ostringstream out;
                    out << "unexpected: nullptr==Led curr=" << *curr;
                    throw Exception(__FILE__, __LINE__, out.str());
                }
                left = curr->symbol->Led(left, curr);
                size_t distance = 1;
                next = LookAhead(distance);
            }
            return left;
        }

        virtual Ast * Statement() {
            size_t distance = 1;
            auto *la1 = LookAhead(distance);
            if (nullptr != la1->symbol->Std) {
                Consume();
                return la1->symbol->Std();
            }
            auto ast = Expression();
            return ast;
        }

        virtual std::vector<Ast *> Statements() {
            std::vector<Ast *> statements;
            auto eos = EosSymbol();
            auto statement = Statement();
            while (statement) {
                statements.push_back(statement);
                statement = Consume(eos) ? Statement() : nullptr;
            }
            return statements;
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
