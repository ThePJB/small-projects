#include <stdio.h>
#include <string.h>
#include "vla.hpp"
#include "re.hpp"
/*
Can I have some stupid syntax like function application is space
i guess it would be like identifier identifier 
print add 5 4
print add add 5 4 4 -> print((5 + 4) + 4) no surely its not ambiguous
(4 + 5) + (4 + 5) vs (4 + (4 + 5) + 5)
add add 4 5 add 4 5 vs add 4 add 4 5 5

idk if its a problem

just have expressions

*/


/*
OK looks like tokenizing is happening good, thanks re, no thanks makefile
parsing may be working
-- type checking goes here --
now i need to interpret
node.evaluate
    print - assert 1 child, evaluate it, print it as a side effect, return unit
    do I need like a value union that is returned from node.evaluate(), including type?

tbh sounds pretty simple, then I will add operator + and it will be fuggen /on/
*/




/*
dangling else is one thing to look out for
*/

// so for function calls we are gonna need the type of compound expressions, so like tuples

// right now just copying who cares
// could also just have a vla<char>
struct string_slice {
    int length = 0;
    char *text = NULL;

    string_slice(const char *str) {
        const char *c = str;
        int i = 0;
        while (*c != '\0') {
            i++;
            c++;
        }
        length = i;
        text = (char *)malloc(length * sizeof(char));
        memcpy(text, str, length);
    }

    string_slice(){}

    string_slice(const char *str, int len) {
        length = len;
        text = (char *)malloc(length * sizeof(char));
        memcpy(text, str, length);
    }

    bool equals(const char *other) {
        const char *c = other;
        int i = 0;
        while (*c != '\0') {
            if (i >= length) {
                return false;
            }
            if (*c != text[i]) {
                return false;
            }
            c++;
            i++;
        }
        return true;
    }

    int atoi() {
        int acc = 0;
        for (int i = 0; i < length; i++) {
            int base = 1;
            for (int j = 0; j < (length - i - 1); j++) base *= 10; // shitty pow
            acc += (text[i] - '0')*base;
        }
        return acc;
    }

    void print() {
        for (int i = 0; i < length; i++) {
            putc(text[i], stdout);
        }
    }
};

enum token_type {
    TT_WORD, // identifier vs reserve word? no difference
    TT_OPERATOR,
    TT_INT_LITERAL,
};

struct token {
    token_type tag;
    string_slice text;

    token(token_type tag, string_slice text) {
        this->tag = tag;
        this->text = text;
    }

};


// parse expression list
// parse expression
// just try to go as wide as possible

// yeah i can see why its easy in lisp, its always paren operator args paren

enum value_type {
    V_UNIT,
    V_INT,
};

struct value {
    value_type tag;
    int int_val;

    value(value_type tag, int int_val) {
        this->tag = tag;
        this->int_val = int_val;
    }

    static value new_unit() {
        return value(V_UNIT, 0);
    }

    static value new_int(int i) {
        return value(V_INT, i);
    }
};

enum operator_type {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
};

enum ast_node_type {
    AN_EXPRESSION_LIST,
    AN_LITERAL,
    AN_PRINT,
    AN_BINARY_OPERATOR,
};


struct ast_node {
    ast_node_type tag;
    vla<ast_node> children = vla<ast_node>();
    int literal_val;
    operator_type op;

    value evaluate() {
        if (tag == AN_LITERAL) {
            // value is value of literal duh
            return value::new_int(literal_val);
        } else if (tag == AN_PRINT) {
            // evaluate children, print, return type is unit
            auto v = children.items[0].evaluate();
            printf("%d\n", v.int_val);
            return value::new_unit();
        } else if (tag == AN_EXPRESSION_LIST) {
            // just evaluate all children, type will be unit
            // todo make type tuple for function args
            for (int i = 0; i < children.length; i++) {
                children.items[i].evaluate();
            }
            return value::new_unit();
        } else if (tag == AN_BINARY_OPERATOR) {
            if (op == OP_ADD) {
                return value::new_int(children.items[0].evaluate().int_val + children.items[1].evaluate().int_val);
            } else if (op == OP_MUL) {
                return value::new_int(children.items[0].evaluate().int_val * children.items[1].evaluate().int_val);
            } else if (op == OP_SUB) {
                return value::new_int(children.items[0].evaluate().int_val - children.items[1].evaluate().int_val);
            } else if (op == OP_DIV) {
                return value::new_int(children.items[0].evaluate().int_val / children.items[1].evaluate().int_val);
            } else {
            printf("interpreter error - bad operator\n");
            exit(1);
            }
        } else {
            printf("interpreter error\n");
            exit(1);
        }
    }
};

        /*
            type rules
            () T -> T
            T () -> T
            _|_ is fatal
            _|_ | T -> T

            () int int
            vs (() int) int
        */

struct token_stream {
    int front = 0;
    vla<token> tokens = vla<token>();

    static vla<token> tokenize(const char *text) {
        auto tokens = vla<token>();
        int match_length = 0;
        int match_pos = -1;
        
        while (true) {
            if (*text == '\0') { // end of program
                break;
            }

            // remove whitespace
            match_pos = re_match("^\\s+", text, &match_length);
            if (match_pos != -1) {
                text = text + match_pos + match_length;
                continue;
            }
            // int literal
            match_pos = re_match("^\\d+", text, &match_length);
            if (match_pos != -1) {
                tokens.push(token(
                    TT_INT_LITERAL, 
                    string_slice(
                        text + match_pos,
                        match_length
                    )
                ));
                text = text + match_pos + match_length;
                continue;
            }
            // word
            match_pos = re_match("^[a-zA-Z]+", text, &match_length);
            if (match_pos != -1) {
                tokens.push(token(
                    TT_WORD, 
                    string_slice(
                        text + match_pos,
                        match_length
                    )
                ));
                text = text + match_pos + match_length;
                continue;
            }
            // binary operator
            match_pos = re_match("^[\\+\\-\\*\\/]", text, &match_length);
            if (match_pos != -1) {
                tokens.push(token(
                    TT_OPERATOR, 
                    string_slice(
                        text + match_pos,
                        match_length
                    )
                ));
                text = text + match_pos + match_length;
                continue;
            }
            
            printf("bad token %s\n", text);
            break;

        }

        return tokens;
    }

    token_stream(const char* text) {
        tokens = tokenize(text);
    }

    bool has_next() {
        return front < tokens.length;
    }

    bool has_n(int n) {
        return front + n < tokens.length;
    }

    token peek_next() {
        return tokens.items[front];
    }

    token peek_n(int n) {
        return tokens.items[front + n];
    }
    
    token pop_next() {
        front++;
        return tokens.items[front - 1];
    }
};

ast_node parse_literal(token_stream *toks) {
    auto n = ast_node();
    n.tag = AN_LITERAL;
    n.literal_val = toks->pop_next().text.atoi();
    return n;
}

ast_node parse_expression(token_stream *toks) {
    auto n = ast_node();
    // assuming has next
    auto t = toks->peek_next();

    // check print
    if (t.tag == TT_WORD && t.text.equals("print")) {
        n.tag = AN_PRINT;
        toks->pop_next();
        n.children.push(parse_expression(toks));
    
    // check operator
    } else if (toks->has_n(1))  {
        auto t = toks->peek_n(1);
        if (t.tag == TT_OPERATOR) {
            n.children.push(parse_literal(toks)); // kind of assuming a literal on lhs, will be problematic
            n.tag = AN_BINARY_OPERATOR;
            n.op = t.text.equals("+") ? OP_ADD :
                    t.text.equals("-") ? OP_SUB :
                    t.text.equals("*") ? OP_MUL :
                    t.text.equals("/") ? OP_DIV :
                    OP_ADD; // err
            toks->pop_next();
            n.children.push(parse_expression(toks));
        }

    // check literal
    } else if (t.tag == TT_INT_LITERAL) {
        return parse_literal(toks);
    }
    return n;
}
ast_node parse_expression_list(token_stream *toks) {
    auto n = ast_node();
    n.tag = AN_EXPRESSION_LIST;
    while (toks->has_next()) {
        n.children.push(parse_expression(toks));
    }
    return n;
}

// parse operator
// parse literal
// etc

// fatal error when invalid token
// also print out line number etc (todo)

// do we give it an enum and also a text? may as well, so if its an operator or an identifier or whatever


int main(int argc, char** argv) {
    const auto source = "print 100 / 2 + 8";

    /*
R"(print 1
print 2
print 3
print 4
print 5
print 6)";
*/
    const auto source2 = "print 1 + 2";
    const auto source3 = "print 1 + 2 if 3 == 4 else 5 - 6";

    printf("program source:\n\n%s\n\n", source);

    printf("tokenizing...\n");
    auto tokens = token_stream(source);
    printf("tokenizing... done\n");

/*
    for (int i = 0; i < tokens.tokens.length; i++) {
        printf("token %d: %d - ", i, tokens.tokens.items[i].tag);
        tokens.tokens.items[i].text.print(); 
        putc('\n', stdout);
    }
*/
    printf("building AST...\n");
    auto root = parse_expression_list(&tokens);
    printf("building AST... done\n");

    printf("interpreting...\n");
    root.evaluate();
    printf("interpreting... done\n");



    // i guess we parse root in our ast
    // basically we are allowed root and any number of expressions
    // while tokens, parse expression
    // can an expression be a sequence of expressions
    // so you could have () () ()
    // print is i32 -> () rn
    // main is allowed to be print 1 print 1 print 1
    // so thats () () ()


    // parse tokens, vla of string slice
    // then make ast by parsing expressions one at a time. y tree?

    // if expression is fair enough its just ?, for expression?
    // well pre if or not
    // true if load_file == good else false
    // A (condition) B
    // i guess as ur parsing how dont you know that its not just ending at A
    // other options include condition A B (normal if), ? operator



    
    //probably fileutils_open
    // fileutiles_slurp
}