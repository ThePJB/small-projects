#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define assert(X) if (!X) printf("Assertion failed: %s:%d\n", __FILE__, __LINE__), exit(1);



/*
cause match can start anywhere in input right
maybe states like escape=true or not

escapes: do literally the next thing
exceptions, stuff like newline
it can be a different state
*/

/*
probably want verifying to be a separate step to evaluating
caus rn aa\ aaaaaa will just not match because it hits end of pattern, not becasue \ bad
*/

/*
subexpression ()
bracket expr [] [abc] [a-z]
negate bracket expr [^]
*/

typedef struct {
    bool match;
    bool bad_pattern;
    int len;
} re_matchinfo;

bool re_match_escaped_char(char pchar, char ichar) {
    if (pchar == ichar) return true;
    return false;
}

bool re_match_char(char pchar, char ichar) {
    if (pchar == '.' ||
        pchar == ichar) {
        return true;
    }
    return false;
}

re_matchinfo re_match_here(char *pattern, char *input) {

    re_matchinfo ret = {0};

    printf("re match -- pattern: %s, input %s\n", pattern, input);

    char *curr_pattern = pattern, *curr_input = input;

    if (*pattern == '?' || *pattern == '*' || *pattern == '+') {
        ret.bad_pattern = true;
        return ret;
    }

    while (*pattern != '\0') {
        printf("p[%c], i[%c]\n", *pattern, *input);
        if (*input == '\0') return ret;
        if (*pattern == '\\') {
            printf("escape branch\n");
            pattern++;
            if (*pattern == '\0') {
                // terminal escape
                printf("bad escape branch\n");
                ret.bad_pattern = true;
                return ret;
            } else if (re_match_escaped_char(*pattern, *input)) {
                input++;
                pattern++;
                ret.len++;
            } else {
                return ret;
            }
        } else {
            if (re_match_char(*pattern, *input)) {
                input++;
                pattern++;
                ret.len++;
            } else if ((*pattern == '+' || *pattern == '*')
                    && re_match_char(*(pattern-1), *input)) {
                input++;
                ret.len++;
            } else {
                // ? 1 case / done case
                if (*pattern == '?' ||
                    *pattern == '*' ||
                    *pattern == '+') {
                    pattern++;
                    continue;
                }

                pattern++;
                if (*pattern == '\0') return ret;

                // ? 0 case
                if (*pattern == '?' ||
                    *pattern == '*') {
                    pattern++;
                } else {
                    return ret;
                }
            }
        }
    }
    ret.match = true;
    printf("true\n");
    return ret;
}

void re_test() {
    // plain
    assert(re_match_here("spagett", "spagett").match);
    assert(re_match_here("spag", "spagett").match);
    assert(!re_match_here("spagetteh", "spagett").match);
    assert(!re_match_here("asd", "asp").match);

    // .
    assert(re_match_here("spag.tt", "spagett").match);
    assert(re_match_here("s....tt", "spagett").match);
    assert(!re_match_here("s.....tt", "spagett").match);
    assert(re_match_here("spagett.", "spagett.").match);

    // escapes
    assert(re_match_here("spagett\\.", "spagett.").match);
    assert(!re_match_here("spagett\\.", "spagetty").match);
    assert(re_match_here("\\h\\e\\l\\l\\o", "hello").match);
    assert(re_match_here("he\\\\\\\\o", "he\\\\o").match);
    assert(re_match_here("\\\\\\\\", "\\\\").match);
    assert(re_match_here("\\\\a\\s\\s\\\\", "\\ass\\").match);
    re_matchinfo trailing_escape = re_match_here("asd\\", "asdasdasdasd");
    assert(!trailing_escape.match);
    assert(trailing_escape.bad_pattern);

    // theres probably a reason these come after, you would have to peek the next thing on
    // a fail to confirm if its a fail

    // ? -- zero or one
    assert(re_match_here("spag?ett", "spaett").match);
    assert(re_match_here("spag?ett", "spagett").match);
    assert(!re_match_here("spag?ett", "spaggett").match);
    assert(!re_match_here("?spagett", "spagett").match);
    assert(re_match_here("?spagett", "spagett").bad_pattern);

    // * -- zero or more
    assert(re_match_here("spag*ett", "spaett").match);
    assert(re_match_here("spag*ett", "spagett").match);
    assert(re_match_here("spag*ett", "spagggggett").match);
    assert(!re_match_here("*spagett", "spagett").match);
    assert(re_match_here("*spagett", "spagett").bad_pattern);
    assert(re_match_here("a.*b", "aspagettb").match);
    // i guess this is like try to advance the pattern if you can
    // maybe check for letter first
    // rn its 'possessive'

    // + -- one or more
    assert(!re_match_here("spag+ett", "spaett").match);
    assert(re_match_here("spag+ett", "spagett").match);
    assert(re_match_here("spag+ett", "spagggggett").match);
    assert(!re_match_here("+spagett", "spagett").match);
    assert(re_match_here("+spagett", "spagett").bad_pattern);

}

int main(int argc, char** argv) {
    re_test();
}
