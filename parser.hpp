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
#include "lexer.hpp"
#include "binder.hpp"

namespace z2h {

    template <typename TParser>
    struct Parser : public Binder<TParser> {

        Lexer                           *lexer;
        std::vector<Token *>            tokens;
        std::vector<Symbol *>           symbols;
        size_t                          index;

        virtual ~Parser() {
            while (!tokens.empty())
                delete tokens.back(), tokens.pop_back();
        }

        Parser(Lexer *lexer)
            : lexer(lexer)
            , tokens({})
            , symbols({})
            , index(0) {
        }

        virtual Ast * Parse() = 0;

        Symbol * GetSymbol(size_t type) {
            return symbols[type];
        }

        virtual bool LookAhead(size_t &distance, const size_t &type, bool skips = false) {
            auto token = LookAhead(distance, skips);
            return token->type == type;
        }

        virtual Token * LookAhead(size_t &distance, bool skips = false) {
            Token *token = nullptr;
            auto i = index;
            while (distance) {
                if (i < tokens.size()) {
                    token = tokens[i];
                }
                else {
                    token = lexer->Scan();
                    lexer->position += token->length;
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

        virtual bool LookAhead1(const size_t &type) {
            size_t distance = 1;
            return LookAhead(distance, type);
        }

        virtual Token * LookAhead1() {
            size_t distance = 1;
            return LookAhead(distance);
        }

        virtual bool LookAhead2(const size_t &type) {
            size_t distance = 2;
            return LookAhead(distance, type);
        }

        virtual Token * LookAhead2() {
            size_t distance = 2;
            return LookAhead(distance);
        }

        virtual Token * Consume(size_t type = 0XDEADBEEF) {
            if (type != 0XDEADBEEF) {
                return Consume({type});
            }
            return Consume({});
        }

        virtual Token * Consume(std::initializer_list<size_t> types) {
            size_t distance = 1;
            auto token = LookAhead(distance);
            if (types.size()) {
                for (auto type : types) {
                    if (type != 0XDEADBEEF && type == token->type) {
                        index += distance;
                        return token;
                    }
                }
                return nullptr;
            }
            index += distance;
            return token;
        }

        virtual Ast * Expression(size_t rbp = 0) {

            auto curr = Consume();
            if (!GetSymbol(*curr)->Nud) {
                --index;
                return nullptr;
            }
            Ast *left = GetSymbol(*curr)->Nud(curr);
            auto next = LookAhead1();
            while (rbp < GetSymbol(*next)->lbp) {
                curr = Consume();
                if (!GetSymbol(*curr)->Led) {
                    --index;
                    return nullptr;
                }
                left = GetSymbol(*curr)->Led(left, curr);
                next = LookAhead1();
            }
            return left;
        }

        virtual Ast * Statement() {
            auto la1 = LookAhead1();
            if (nullptr != GetSymbol(*la1)->Std) {
                Consume();
                return GetSymbol(*la1)->Std();
            }
            auto ast = Expression();
            return ast;
        }

        virtual std::vector<Ast *> Statements() {
            std::vector<Ast *> statements;
            size_t eos = 1;
            auto statement = Statement();
            while (statement) {
                statements.push_back(statement);
                statement = Consume(eos) ? Statement() : nullptr;
            }
            return statements;
        }

        virtual size_t Line() {
            return 0;
        }

        virtual size_t Column() {
            return 0;
        }
    };
}

#endif /*__Z2H_PARSER__*/
