#define LUA_LIB
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

//--- String constants for transformations
#define STR_TRY_HEAD      "do local __ok, __err = pcall(function()\n"
#define STR_TRY_CATCH     "end); if not __ok then local "
#define STR_TRY_END       " end end"

#define STR_FN_SELF       "function(self"

#define STR_TOSTRING      "tostring("
#define STR_OBJECT_HEAD   " = Object("
#define STR_OBJECT_TAIL   "})"

#define STR_AWAIT_WRAP    "await(function"
#define STR_AWAIT         "await("
#define STR_TASK_WRAP     "sys.TaskFactory(function"

#define STR_IMPORT_FMT    "local %s = require(\"%s\"); "

extern int original_searcher_ref = LUA_NOREF;

static inline int is_idchar(char c) {
    return isalnum((unsigned char)c) || c == '_';
}

static inline void skip_whitespace(const char* src, size_t* i) {
    while (src[*i] && isspace((unsigned char)src[*i])) (*i)++;
}

#define skipandkeep_whitespace(src, i) \
     while (src[*i] && isspace((unsigned char)src[*i])) out[(*ri)++] = src[(*i)++];

static size_t skip_string_or_comment(const char* src, size_t i) {
    if (src[i] == '"' || src[i] == '\'') {
        char quote = src[i++];
        while (src[i] && src[i] != quote) {
            if (src[i] == '\\') i += 2;
            else i++;
        }
        return src[i] ? i + 1 : i;
    }

    if (src[i] == '-' && src[i + 1] == '-') {
        i += 2;
        if (src[i] == '[') {
            size_t j = i + 1;
            while (src[j] == '=') j++;
            if (src[j] == '[') {
                size_t eqs = j - i - 1;
                j++;
                while (src[j]) {
                    if (src[j] == ']') {
                        size_t k = j + 1, match = 0;
                        while (src[k + match] == '=' && match < eqs) match++;
                        if (match == eqs && src[k + match] == ']') return k + match + 1;
                    }
                    j++;
                }
                return j;
            }
        }
        while (src[i] && src[i] != '\n') i++;
        return i;
    }

    if (src[i] == '[') {
        size_t j = i + 1;
        while (src[j] == '=') j++;
        if (src[j] == '[') {
            size_t eqs = j - i - 1;
            j++;
            while (src[j]) {
                if (src[j] == ']') {
                    size_t k = j + 1, match = 0;
                    while (src[k + match] == '=' && match < eqs) match++;
                    if (match == eqs && src[k + match] == ']') return k + match + 1;
                }
                j++;
            }
            return j;
        }
    }

    return i;
}

static size_t get_block_content_end(const char* src, size_t block_end, size_t min_pos) {
    size_t e = block_end;
    while (e > min_pos && isspace((unsigned char)src[e - 1])) e--;  
    if (e - min_pos >= 3 && strncmp(&src[e - 3], "end", 3) == 0) {
        e -= 3; 
    }
    return e;
}

static bool is_kw(const char* src, size_t i, const char* kw, size_t len) {
    if (strncmp(&src[i], kw, len) != 0) return false;
    if (is_idchar(src[i + len])) return false;
    if (i > 0 && (is_idchar(src[i - 1]) || src[i - 1] == '.' || src[i - 1] == ':')) return false;
    return true;
}

static bool syntax_error(lua_State *L, const char* msg, const char* src, size_t* i) {
    int line = 1;
    for (size_t j = 0; j < *i && src[j]; j++)
        if (src[j] == '\n') line++;
    lua_pushfstring(L, ":%d: %s", line, msg);
    return false;
}

typedef enum {
    BLOCK_FUNCTION,
    BLOCK_FOR,
    BLOCK_DO,
    BLOCK_TRY,
    BLOCK_UNKNOWN
} BlockType;

static const char *block_names[] = {
    "function",
    "for",
    "do",
    "try",
    "block"
};

static size_t find_block_end(lua_State *L, const char* src, size_t i, BlockType root) {
    size_t start = i;
    BlockType stack[64];
    int sp = 0;
    stack[sp++] = root;

    while (src[i]) {
        size_t skip = skip_string_or_comment(src, i);
        if (skip != i) { i = skip; continue; }

        if (is_kw(src, i, "function", 8)) { stack[sp++] = BLOCK_FUNCTION; i += 8; continue; }
        if (is_kw(src, i, "if", 2))       { stack[sp++] = BLOCK_UNKNOWN; i += 2; continue; } 
        if (is_kw(src, i, "do", 2))       { stack[sp++] = BLOCK_DO;       i += 2; continue; }
        if (is_kw(src, i, "try", 3))      { stack[sp++] = BLOCK_TRY;      i += 3; continue; }

        if (is_kw(src, i, "end", 3) && sp > 0) {
            BlockType popped = stack[--sp]; i += 3;
            if (sp == 0 && popped == root) return i;
            continue;
        }

        i++;
    }
    if (sp > 0) {
        char msg[64];
        snprintf(msg, sizeof(msg), "unterminated '%s' block", block_names[root]);
        syntax_error(L, msg, src, &start);
        return 0;
    }
    return i;
}

typedef enum {
    CONTEXT_NORMAL,
    CONTEXT_CLASS_TABLE
} PreprocessContext;

static bool preprocess_segment(lua_State *L, const char* src, size_t start, size_t end, char* out, size_t* ri, PreprocessContext ctx);

static bool handle_interpolation(lua_State *L, const char* src, size_t* i, char* out, size_t* ri) {
    char quote = src[(*i)++];
    bool in_escape = false;
    bool has_interpolation = false;

    char buffer[256]; 
    size_t bi = 0;

    out[(*ri)++] = '(';

    while (src[*i] && src[*i] != quote) {
        if (!in_escape && src[*i] == '\\') {
            in_escape = true;
            buffer[bi++] = src[(*i)++];
            continue;
        }

        if (in_escape) {
            buffer[bi++] = src[(*i)++];
            in_escape = false;
            continue;
        }

        if (src[*i] == '$' && src[*i + 1] == '{') {
            size_t j = *i + 2;
            int depth = 0;
            bool in_str = false;
            char q = 0;

            while (src[j]) {
                if (in_str) {
                    if (src[j] == '\\') j++;
                    else if (src[j] == q) in_str = false;
                    j++;
                } else {
                    if (src[j] == '"' || src[j] == '\'') {
                        in_str = true;
                        q = src[j];
                        j++;
                    } else if (src[j] == '{') {
                        depth++;
                        j++;
                    } else if (src[j] == '}') {
                        if (depth > 0) depth--;
                        else break; 
                        j++;
                    } else {
                        j++;
                    }
                }
            }

            if (src[j] == '}') {
                if (bi > 0) {
                    buffer[bi] = '\0';
                    *ri += sprintf(&out[*ri], "%c%s%c .. ", quote, buffer, quote);
                    bi = 0;
                } else if (!has_interpolation) {
                    *ri += sprintf(&out[*ri], "\"\" .. ");
                }

                *ri += sprintf(&out[*ri], "tostring(");
                
                size_t len = j - (*i + 2);
                memcpy(&out[*ri], &src[*i + 2], len);
                *ri += len;

                out[(*ri)++] = ')';
                
                *i = j + 1;
                has_interpolation = true;

                if (src[*i] && src[*i] != quote) {
                    *ri += sprintf(&out[*ri], " .. ");
                }

                continue;
            }
        }

        buffer[bi++] = src[(*i)++];
    }

    if (bi > 0) {
        buffer[bi] = '\0';
        if (has_interpolation) {
             *ri += sprintf(&out[*ri], "%c%s%c", quote, buffer, quote);
        } else {
            *ri += sprintf(&out[*ri], "%c%s%c", quote, buffer, quote);
        }
    } else if (!has_interpolation) {
        out[(*ri)++] = quote;
        out[(*ri)++] = quote;
    }

    out[(*ri)++] = ')';

    if (src[*i] == quote) (*i)++;
    
    return true;
}

static bool handle_async(lua_State* L, const char* src, size_t* i, char* out, size_t* ri) {
    if (!is_kw(src, *i, "function", 8))
        return syntax_error(L, "'function' expected after 'async'", src, i);

    *i += 8;
    skip_whitespace(src, i);

    char fname[128]; size_t fi = 0;
    bool is_method = false;
    char *p_colon = NULL;
    fname[0] = '\0';

    if (is_idchar(src[*i])) {
        while (src[*i] && (is_idchar(src[*i]) || src[*i] == '.' || src[*i] == ':') && fi < sizeof(fname) - 1) {
            if (src[*i] == ':') {
                is_method = true;
                p_colon = &fname[fi];
            }
            fname[fi++] = src[(*i)++];
        }
        fname[fi] = '\0';
        skip_whitespace(src, i);
    }

    if (src[*i] != '(')
        return syntax_error(L, "expected '(' after async function", src, i);

    bool in_expression = false;
    size_t last = *ri;
    while (last > 0 && isspace((unsigned char)out[last - 1])) last--;
    if (last > 0) {
        char c = out[last - 1];
        if (c == '=' || c == '(' || c == ',' || c == '{' || c == '[' || (last >= 6 && strncmp(&out[last-6], "return", 6) == 0 && (last == 6 || !is_idchar(out[last-7]))))
            in_expression = true;
    }

    if (in_expression) {
        *ri += sprintf(&out[*ri], STR_TASK_WRAP);
    } else if (fname[0]) {
        if (is_method && p_colon) *p_colon = '.';
        *ri += sprintf(&out[*ri], "%s = " STR_TASK_WRAP, fname);
    } else {
        *ri += sprintf(&out[*ri], STR_TASK_WRAP);
    }

    if (src[*i] == '(') {
        (*i)++; 
        if (is_method) {
            *ri += sprintf(&out[*ri], "(self");
            skip_whitespace(src, i);
            if (src[*i] != ')') {
                out[(*ri)++] = ',';
            }
        } else {
            out[(*ri)++] = '(';
        }
    }

    int paren_depth = 1;
    while (src[*i] && paren_depth > 0) {
        if (src[*i] == '(') paren_depth++;
        if (src[*i] == ')') paren_depth--;
        out[(*ri)++] = src[(*i)++];
    }

    size_t body_start = *i;
    size_t body_end = find_block_end(L, src, *i, BLOCK_FUNCTION);
    if (body_end == 0)
        return false;
    
    size_t body_process_end = get_block_content_end(src, body_end, body_start);

    if (!preprocess_segment(L, src, body_start, body_process_end, out, ri, CONTEXT_NORMAL))
        return false;

    memcpy(&out[*ri], "end)", 4);
    *ri += 4;
    *i = body_end;
    return true;
}

static size_t find_expression_end(lua_State* L, const char* src, size_t i) {
    int parens = 0, braces = 0, brackets = 0;
    bool started = false;

    while (src[i]) {
        while (src[i] && isspace((unsigned char)src[i])) {
            if ((src[i] == '\n' || src[i] == '\r') && parens == 0 && braces == 0 && brackets == 0 && started) {
                break;
            }
            i++;
        }

        size_t next = skip_string_or_comment(src, i);
        if (next != i) {
            i = next;
            continue;
        }

        char c = src[i];

        if (c == '(') {
            parens++;
            started = true;
        } else if (c == '{') {
            braces++;
            started = true;
        } else if (c == '[') {
            brackets++;
            started = true;
        } else if (c == '}') {
            if (braces > 0) {
                braces--;
                started = true;
                if (braces == 0 && parens == 0 && brackets == 0) {
                    i++;
                    return i;
                }
            }
        } else if (c == ']') {
            if (brackets > 0) {
                brackets--;
                started = true;
                if (brackets == 0 && parens == 0 && braces == 0) {
                    i++;
                    return i;
                }
            }
        } else if (c == ')') {
            if (parens > 0) {
                parens--;
                started = true;
                
                if (parens == 0 && braces == 0 && brackets == 0) {
                    i++; 
                    size_t lookahead = i;
                    
                    while (src[lookahead] && isspace((unsigned char)src[lookahead])) {
                        if (src[lookahead] == '\n' || src[lookahead] == '\r') break;
                        lookahead++;
                    }
                    
                    if ((src[lookahead] == '.' || src[lookahead] == ':') && is_idchar(src[lookahead + 1])) {
                        i = lookahead + 2;
                        while (is_idchar(src[i])) i++;
                        continue;
                    }
                    return i;
                }
            } else {
                return i; 
            }
        } else if ((c == '\n' || c == ';') && parens == 0 && braces == 0 && brackets == 0 && started) {
            return i;
        }
        started = true;
        i++;
    }

    return i;
}

static inline void emit_str(char* out, size_t* ri, const char* s, size_t len) {
    memcpy(&out[*ri], s, len);
    *ri += len;
}

static bool handle_await(lua_State *L, const char* src, size_t* i, char* out, size_t* ri) {
    skip_whitespace(src, i);

    if (is_kw(src, *i, "function", 8)) {
        emit_str(out, ri, STR_AWAIT_WRAP, sizeof(STR_AWAIT_WRAP) - 1);
        *i += 8;
        size_t body_start = *i;
        size_t body_end = find_block_end(L, src, *i, BLOCK_FUNCTION);
        if (body_end == 0) return false;

        if (!preprocess_segment(L, src, body_start, body_end, out, ri, CONTEXT_NORMAL))
            return false;

        out[(*ri)++] = ')';
        *i = body_end;
        return true;
    }
    size_t expr_start = *i;
    size_t expr_end = find_expression_end(L, src, *i);

    if (expr_end <= expr_start)
        return syntax_error(L, "expression expected after 'await'", src, i);
    emit_str(out, ri, STR_AWAIT, sizeof(STR_AWAIT) - 1);
    while (*i < expr_end)
        out[(*ri)++] = src[(*i)++];
    out[(*ri)++] = ')';

    return true;
}

static bool handle_import(lua_State *L, const char* src, size_t* i, char* out, size_t* ri) {
    while (src[*i]) {
        if (!is_idchar(src[*i]))
            return syntax_error(L, "<name> expected near 'import'", src, i);

        char path[128], name[64];
        size_t pi = 0, ni = 0;

        while (src[*i] && (is_idchar(src[*i]) || src[*i] == '.') && pi < sizeof(path) - 1)
            path[pi++] = src[(*i)++];
        path[pi] = '\0';

        const char* last = strrchr(path, '.');
        const char* local = last ? last + 1 : path;

        strncpy(name, local, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';

        *ri += sprintf(&out[*ri], STR_IMPORT_FMT, name, path);

        skip_whitespace(src, i);
        if (src[*i] == ',') {
            (*i)++;
            skip_whitespace(src, i);
        } else {
            out[(*ri)++] = '\n';
            break;
        }
    }

    out[(*ri)++] = '\n';
    return true;
}

static bool handle_trycatch(lua_State *L, const char* src, size_t* i, char* out, size_t* ri) {
    size_t body_start = *i;

    while (src[*i]) {
        if (is_kw(src, *i, "catch", 5)) break;
        size_t skip = skip_string_or_comment(src, *i);
        if (skip != *i) { *i = skip; continue; }
        (*i)++;
    }

    if (!is_kw(src, *i, "catch", 5))
        return syntax_error(L, "'try' without matching 'catch' block", src, &body_start);

    size_t body_end = *i;
    *i += 5;
    skip_whitespace(src, i);

    if (!is_idchar(src[*i]))
        return syntax_error(L, "<name> expected after 'catch'", src, i);

    char errname[64]; size_t ei = 0;
    while (src[*i] && is_idchar(src[*i]) && ei < sizeof(errname) - 1)
        errname[ei++] = src[(*i)++];
    errname[ei] = '\0';

    skip_whitespace(src, i);
    size_t catch_start = *i;
    size_t full_end = find_block_end(L, src, *i, BLOCK_TRY);
    if (full_end == 0)
        return false;

    size_t catch_body_end = get_block_content_end(src, full_end, catch_start);

    memcpy(&out[*ri], STR_TRY_HEAD, sizeof(STR_TRY_HEAD) - 1);
    *ri += sizeof(STR_TRY_HEAD) - 1;

    if (!preprocess_segment(L, src, body_start, body_end, out, ri, CONTEXT_NORMAL))
        return false;

    memcpy(&out[*ri], STR_TRY_CATCH, sizeof(STR_TRY_CATCH) - 1);
    *ri += sizeof(STR_TRY_CATCH) - 1;
    *ri += sprintf(&out[*ri], "%s = __err\n", errname);

    if (!preprocess_segment(L, src, catch_start, catch_body_end, out, ri, CONTEXT_NORMAL))
        return false;

    memcpy(&out[*ri], STR_TRY_END, sizeof(STR_TRY_END) - 1);
    *ri += sizeof(STR_TRY_END) - 1;

    *i = full_end;
    return true;
}

static size_t find_table_block_end(const char* src, size_t i) {
    int depth = 0;
    while (src[i]) {
        size_t skip = skip_string_or_comment(src, i);
        if (skip != i) {
            i = skip;
            continue;
        }

        if (src[i] == '{') depth++;
        else if (src[i] == '}') {
            depth--;
            if (depth == 0) return i + 1;
        }

        i++;
    }
    return i;
}

static bool handle_class(lua_State *L, const char* src, size_t* i, char* out, size_t* ri) {
    if (!is_idchar(src[*i]))
        return syntax_error(L, "<name> expected after 'class'", src, i);

    char cname[64]; size_t ci = 0;
    while (src[*i] && is_idchar(src[*i]) && ci < sizeof(cname) - 1)
        cname[ci++] = src[(*i)++];
    cname[ci] = '\0';
    
    if (src[*i] == '(') {
        *ri += sprintf(&out[*ri], "%s = Object", cname);
        size_t depth = 0;
        do {
            out[(*ri)++] = src[(*i)++];
            if (src[*i - 1] == '(') depth++;
            else if (src[*i - 1] == ')') depth--;
        } while (src[*i] && depth > 0);
        return true;
    }

    int start = *i;
    skip_whitespace(src, i);

    if (src[*i] == '{') {
        size_t block_start = *i;
        size_t block_end = find_table_block_end(src, block_start);
        if (src[block_end - 1] != '}')
            return syntax_error(L, "unterminated class table block", src, i);

        *ri += sprintf(&out[*ri], "%s" STR_OBJECT_HEAD, cname);
        while (src[start] && isspace(src[start])) out[(*ri)++] = src[start++];
        if (!preprocess_segment(L, src, *i, block_end - 1, out, ri, CONTEXT_CLASS_TABLE))
            return false;

        memcpy(&out[*ri], STR_OBJECT_TAIL, sizeof(STR_OBJECT_TAIL) - 1);
        *ri += sizeof(STR_OBJECT_TAIL) - 1;

        *i = block_end;
        return true;
    }

    return syntax_error(L, "(...) or {...} expected after class name", src, i);
}

typedef struct {
    const char* keyword;
    size_t len;
    bool (*handler)(lua_State *L, const char*, size_t*, char*, size_t*);
} Macro;

#define REGISTER_MACRO(name, fn) { name, sizeof(name) - 1, fn }

static Macro macros[] = {
    REGISTER_MACRO("async", handle_async),
    REGISTER_MACRO("await", handle_await),
    REGISTER_MACRO("import", handle_import),
    REGISTER_MACRO("class", handle_class),
    REGISTER_MACRO("try", handle_trycatch),
};

static bool preprocess_segment(lua_State *L, const char* src, size_t start, size_t end, char* out, size_t* ri, PreprocessContext ctx) {
    size_t i = start;

    while (i < end) {
        if (src[i] == '"' || src[i] == '\'') {
            if (!handle_interpolation(L, src, &i, out, ri))
                return false;
            continue;
        }
        size_t skip = skip_string_or_comment(src, i);
        if (skip != i) {
            while (i < skip) out[(*ri)++] = src[i++];
            continue;
        }
        if (ctx == CONTEXT_CLASS_TABLE) {
            if (src[i] == '{') {
                out[(*ri)++] = src[i++];
                continue;
            }
            skipandkeep_whitespace(src, &i);

            char ident[64]; size_t idlen = 0;
            size_t idstart = i;
            while (src[i] && is_idchar(src[i]) && idlen < sizeof(ident) - 1)
                ident[idlen++] = src[i++];
            ident[idlen] = '\0';

            skipandkeep_whitespace(src, &i);
            if (strcmp(ident, "async") == 0) {
                skipandkeep_whitespace(src, &i);
                char next_ident[64]; size_t nidlen = 0;
                while (src[i] && is_idchar(src[i]) && nidlen < sizeof(next_ident) - 1)
                    next_ident[nidlen++] = src[i++];
                next_ident[nidlen] = '\0';
                if (strcmp(next_ident, "function") != 0)
                    return syntax_error(L, "'function' expected after 'async'", src, &i);

                skipandkeep_whitespace(src, &i);
                char fname[64]; size_t fi = 0;
                while (src[i] && is_idchar(src[i]) && fi < sizeof(fname) - 1)
                    fname[fi++] = src[i++];
                fname[fi] = '\0';

                skipandkeep_whitespace(src, &i);
                if (src[i] != '(')
                    return syntax_error(L, "expected '(' after function name", src, &i);

                *ri += sprintf(&out[*ri], "%s = " STR_TASK_WRAP "(self", fname);
                i++; 
                skipandkeep_whitespace(src, &i);
                if (src[i] != ')') {
                    out[(*ri)++] = ',';
                    while (src[i] && src[i] != ')')
                        out[(*ri)++] = src[i++];
                }
                if (src[i] == ')') out[(*ri)++] = src[i++];

                size_t body_end = find_block_end(L, src, i, BLOCK_FUNCTION);
                if (body_end == 0) return false;

                if (!preprocess_segment(L, src, i, body_end, out, ri, CONTEXT_NORMAL))
                    return false;

                i = body_end;
                out[(*ri)++] = ')';
                out[(*ri)++] = ',';
                continue;
            }
            if (strcmp(ident, "function") == 0) {
                char fname[64]; size_t fi = 0;
                while (src[i] && is_idchar(src[i]) && fi < sizeof(fname) - 1)
                    fname[fi++] = src[i++];
                fname[fi] = '\0';

                skipandkeep_whitespace(src, &i);
                if (src[i] != '(')
                    return syntax_error(L, "expected '(' after function name", src, &i);
                *ri += sprintf(&out[*ri], "%s = function(self", fname);
                i++; 
                skipandkeep_whitespace(src, &i);
                if (src[i] != ')') {
                    out[(*ri)++] = ',';
                    while (src[i] && src[i] != ')')
                        out[(*ri)++] = src[i++];
                }
                if (src[i] == ')') out[(*ri)++] = src[i++];

                size_t body_end = find_block_end(L, src, i, BLOCK_FUNCTION);
                if (body_end == 0) return false;

                if (!preprocess_segment(L, src, i, body_end, out, ri, CONTEXT_NORMAL))
                    return false;

                i = body_end;
                out[(*ri)++] = ',';
                continue;
            }
            if (src[i] == '=') {
            if (idlen == 0)
                return syntax_error(L, "expected identifier before '='", src, &i);
                *ri += sprintf(&out[*ri], "%s = ", ident);
                i++;
                skip_whitespace(src, &i);

                if (is_kw(src, i, "function", 8)) {
                    i += 8;
                    skip_whitespace(src, &i);
                    memcpy(&out[*ri], STR_FN_SELF, sizeof(STR_FN_SELF) - 1);
                    *ri += sizeof(STR_FN_SELF) - 1;
                    if (src[i] == '(') {
                        i++;
                        skip_whitespace(src, &i);
                        if (src[i] != ')') {
                            out[(*ri)++] = ',';
                            while (src[i] && src[i] != ')')
                                out[(*ri)++] = src[i++];
                        }
                        if (src[i] == ')') out[(*ri)++] = src[i++];
                    }
                    size_t body_end = find_block_end(L, src, i, BLOCK_FUNCTION);
                    if (body_end == 0) return false;

                    if (!preprocess_segment(L, src, i, body_end, out, ri, CONTEXT_NORMAL))
                        return false;

                    i = body_end;
                    out[(*ri)++] = ',';
                    continue;
                }
                while (i < end && src[i] != '\n')
                    out[(*ri)++] = src[i++];
                if (i < end && src[i] == '\n') i++;
                out[(*ri)++] = ','; 
                out[(*ri)++] = '\n';
                continue;
            }
            while (i < end && src[i] != '\n')
                out[(*ri)++] = src[i++];
            if (i < end && src[i] == '\n') out[(*ri)++] = src[i++];
            continue;
        }
        int matched = 0;
        for (int m = 0; m < sizeof(macros)/sizeof(macros[0]); m++) {
            Macro* macro = &macros[m];
            if (is_kw(src, i, macro->keyword, macro->len)) {
                i += macro->len;
                skip_whitespace(src, &i);
                if (!macro->handler(L, src, &i, out, ri))
                    return false;
                matched = 1;
                break;
            }
        }
        if (!matched)
            out[(*ri)++] = src[i++];
    }
    return true;
}

char* preprocess_lua(lua_State *L, const char* src) {
    size_t len = strlen(src);

    if (strncmp(src, "--! luart-extensions", 20) == 0) {
        char *out = malloc(len * 2);
        if (out) {              
            char *src_copy = malloc(len + 1);
            size_t k = 0;
            for (long j = 0; j < len; j++) {
                if (src[j] == '\r' && src[j + 1] == '\n') continue;
                src_copy[k++] = src[j];
            }
            src_copy[k] = '\0';
            size_t ri = 0;
            if (!preprocess_segment(L, src_copy, 0, len, out, &ri, CONTEXT_NORMAL)) {
                free(src_copy);
                free(out);
                return NULL;
            }
            out[ri] = '\0';
            free(src_copy);
        } else lua_pushliteral(L, "memory allocation failed");
        return out;
    }
    return strdup(src);
}

LUA_API char* preprocess_file(lua_State *L, const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        lua_pushfstring(L, "cannot open file '%s'", filename);
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buffer = malloc(len + 1);
    if (!buffer) {
        fclose(f);
        lua_pushliteral(L, "memory allocation failed");
        return NULL;
    }
    fread(buffer, 1, len, f);
    buffer[len] = '\0';
    fclose(f);
    char* result = preprocess_lua(L, buffer);
    free(buffer);
    return result;
}

LUA_API int luaL_loadfilep(lua_State* L, const char* filename, const char* mode) {
    char* transformed = preprocess_file(L, filename);
    if (!transformed) {
        lua_pushstring(L, filename);
        lua_insert(L, -2);
        lua_concat(L, 2);
        return LUA_ERRSYNTAX;
    }
    char chunkname[256];
    snprintf(chunkname, sizeof(chunkname), "@%s", filename);
    int status = luaL_loadbufferx(L, transformed, strlen(transformed), chunkname, mode);
    free(transformed);
    return status;
}

int lua_preload_searcher(lua_State* L) {
    const char* modname = luaL_checkstring(L, 1);

    lua_rawgeti(L, LUA_REGISTRYINDEX, original_searcher_ref);
    lua_pushstring(L, modname);
    if (lua_pcall(L, 1, 2, 0) != LUA_OK) {
        lua_pushfstring(L, "\n\tsearcher error: %s", lua_tostring(L, -1));
        return 1;
    }

    if (!lua_isfunction(L, -2) || !lua_isstring(L, -1)) return 2;

    const char* resolved = lua_tostring(L, -1);
    lua_pop(L, 1);

    FILE* f = fopen(resolved, "rb");
    if (!f) {
        lua_pushfstring(L, "\n\tunable to open file '%s'", resolved);
        return 1;
    }

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = malloc(len + 1);
    if (!buffer) {
        fclose(f);
        lua_pushliteral(L, "memory allocation failed");
        return LUA_ERRMEM;
    }

    fread(buffer, 1, len, f);
    buffer[len] = '\0';
    fclose(f);

    char* transformed = preprocess_lua(L, buffer);
    free(buffer);
    if (!transformed) {
        lua_pushstring(L, resolved);
        lua_insert(L, -2);
        lua_concat(L, 2);
        return LUA_ERRSYNTAX;
    }

    size_t res_len = strlen(resolved);
    char* chunkname = (char*)malloc(res_len + 2);
    chunkname[0] = '@';
	memcpy(chunkname + 1, resolved, res_len + 1);

    if (luaL_loadbuffer(L, transformed, strlen(transformed), chunkname) != LUA_OK) {
        free(transformed);
        return lua_error(L);
    }
	free(chunkname);

    free(transformed);
    return 1;
}
