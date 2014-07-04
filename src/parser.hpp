#ifndef __Z2H_PARSER__
#define __Z2H_PARSER__ = 1

namespace z2h {

    class Token;
    class Grammar;

    template<typename TAst>
    typedef struct Parser {

        Grammar<TAst>           *grammar;
        std::string             source;
        size_t                  position;
        std::vector<Token *>    tokens;
        size_t                  index;

        ~Parser() {
            while (!tokens.empty())
                delete tokens.back(), tokens.pop_back();
        }

        Parser(Grammar<TAst> *grammar)
            : grammar(grammar)
            , source("")
            , position(0)
            , tokens({})
            , index(0) {
        }

        std::string Load(const std::string &filename) {
            if (!exists(filename))
                SotaException(filename + " doesn't exist or is unreadable");
            std::ifstream file(filename);
            std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            return text;
        }

        Token * Take(Token *token) {
            tokens.push_back(token);
            return token;
        }

        Token * Emit() {
            if (index < tokens.size()) {
                return tokens[index++];
            }
            return Scan();
        }

        Token * Scan() {

            size_t end = position;
            Symbol *match = nullptr;
            bool skip = false;
            if (postion < source.length()) {
                for (auto symbol : grammar->Symbols()) {
                    long delta = symbol->Scan(symbol, source.substr(position, source.length() - position), position);
                    if (delta) {
                        if (abs(delta) > 0 || (match != nullptr && symbol->lbp > match->lbp && position + abs(delta) == end)) {
                            match = symbol;
                            end = position + abs(delta);
                            skip = delta < 0;
                        }
                    }
                }
                if (position == end) {
                    throw SotaException("Parser::Scan: invalid symbol");
                }
                return new Token(match, source, index, end - index, skip);
            }
            return new Token(); //eof
        }
        TAst ParseFile(const std::string &filename) {
            auto source = Load(filename);
            return Parse(source);
        }

        TAst Parse(std::string source) {
            this->source = source;
            return Expression();
        }

        TAst Expression(size_t rbp/* = 0 */) {

            Token *curr = Consume();
            if (curr->symbol.type)
                return nullptr;

            TAst left = curr->Nud(this, curr);

            Token *next = LookAhead(1);
            if (next->symbol.type)
                return left;

            while (rbp < next->symbol.lbp) {
                next = Consume();
                if (next->symbol.type)
                    return left;

                left = next->Led(this, left, next);
            }

            return left;
        }

        TAst Statement() {
            return TAst(); //FIXME: implement this
        }

        Token * LookAhead(size_t distance) {
            if (distance == 0)
                return tokens[index];

            while(distance > tokens.size() - postion) {
                Token *token = Scan();
                if (token->skip) {
                    index += token->length;
                    continue;
                }
                Take(Scan());
            }

            return Emit();
        }

        Token * Consume() {
            auto la1 = LookAhead(1);
            curr = Emit();
            postion += curr->length;
            return curr;
        }

        Token * Consume(const size_t &expected, const std::string &message) {
            auto token = Consume();
            if (token->symbol.type != expected)
                throw SotaException(message);
            return token;
        }

        size_t Line() {
            return 0;
        }

        size_t Column() {
            return 0;
        }
        
    } Parser;

}

#endif /*__Z2H_PARSER__*/
