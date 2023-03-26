/*
** $Id: lstrlib.c $
** Standard library for string operations and pattern-matching
** See Copyright Notice in lua.h
*/

#define lstrlib_c
#define LUA_LIB

#include <lua\lprefix.h>


#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lua\lua.h>

#include <lua\lauxlib.h>
#include <lua\lualib.h>


/*
** maximum number of captures that a pattern can do during
** pattern-matching. This limit is arbitrary, but must fit in
** an unsigned char.
*/
#if !defined(LUA_MAXCAPTURES)
#define LUA_MAXCAPTURES		32
#endif


/* macro to 'unsign' a character */
#define uchar(c)	((unsigned char)(c))


/*
** Some sizes are better limited to fit in 'int', but must also fit in
** 'size_t'. (We assume that 'lua_Integer' cannot be smaller than 'int'.)
*/
#define MAX_SIZET	((size_t)(~(size_t)0))

#define MAXSIZE  \
	(sizeof(size_t) < sizeof(int) ? MAX_SIZET : (size_t)(INT_MAX))




static int str_len (lua_State *L) {
  size_t l;
  luaL_checklstring(L, 1, &l);
  lua_pushinteger(L, (lua_Integer)l);
  return 1;
}


/*
** translate a relative initial string position
** (negative means back from end): clip result to [1, inf).
** The length of any string in Lua must fit in a lua_Integer,
** so there are no overflows in the casts.
** The inverted comparison avoids a possible overflow
** computing '-pos'.
*/
static size_t posrelatI (lua_Integer pos, size_t len) {
  if (pos > 0)
    return (size_t)pos;
  else if (pos == 0)
    return 1;
  else if (pos < -(lua_Integer)len)  /* inverted comparison */
    return 1;  /* clip to 1 */
  else return len + (size_t)pos + 1;
}


/*
** Gets an optional ending string position from argument 'arg',
** with default value 'def'.
** Negative means back from end: clip result to [0, len]
*/
static size_t getendpos (lua_State *L, int arg, lua_Integer def,
                         size_t len) {
  lua_Integer pos = luaL_optinteger(L, arg, def);
  if (pos > (lua_Integer)len)
    return len;
  else if (pos >= 0)
    return (size_t)pos;
  else if (pos < -(lua_Integer)len)
    return 0;
  else return len + (size_t)pos + 1;
}


static int str_sub (lua_State *L) {
  size_t l;
  const char *s = luaL_checklstring(L, 1, &l);
  size_t start = posrelatI(luaL_checkinteger(L, 2), l);
  size_t end = getendpos(L, 3, -1, l);
  if (start <= end)
    lua_pushlstring(L, s + start - 1, (end - start) + 1);
  else lua_pushliteral(L, "");
  return 1;
}


static int str_reverse (lua_State *L) {
  size_t l, i;
  luaL_Buffer b;
  const char *s = luaL_checklstring(L, 1, &l);
  char *p = luaL_buffinitsize(L, &b, l);
  for (i = 0; i < l; i++)
    p[i] = s[l - i - 1];
  luaL_pushresultsize(&b, l);
  return 1;
}

static int str_byte (lua_State *L) {
  size_t l;
  const char *s = luaL_checklstring(L, 1, &l);
  lua_Integer pi = luaL_optinteger(L, 2, 1);
  size_t posi = posrelatI(pi, l);
  size_t pose = getendpos(L, 3, pi, l);
  int n, i;
  if (posi > pose) return 0;  /* empty interval; return no values */
  if (l_unlikely(pose - posi >= (size_t)INT_MAX))  /* arithmetic overflow? */
    return luaL_error(L, "string slice too long");
  n = (int)(pose -  posi) + 1;
  luaL_checkstack(L, n, "string slice too long");
  for (i=0; i<n; i++)
    lua_pushinteger(L, uchar(s[posi+i-1]));
  return n;
}


static int str_char (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  int i;
  luaL_Buffer b;
  char *p = luaL_buffinitsize(L, &b, n);
  for (i=1; i<=n; i++) {
    lua_Unsigned c = (lua_Unsigned)luaL_checkinteger(L, i);
    luaL_argcheck(L, c <= (lua_Unsigned)UCHAR_MAX, i, "value out of range");
    p[i - 1] = uchar(c);
  }
  luaL_pushresultsize(&b, n);
  return 1;
}

/*
** {======================================================
** PATTERN MATCHING
** =======================================================
*/

#define CAP_UNFINISHED	(-1)
#define CAP_POSITION	(-2)


typedef struct MatchState {
  const char *src_init;  /* init of source string */
  const char *src_end;  /* end ('\0') of source string */
  const char *p_end;  /* end ('\0') of pattern */
  lua_State *L;
  int matchdepth;  /* control for recursive depth (to avoid C stack overflow) */
  unsigned char level;  /* total number of captures (finished or unfinished) */
  struct {
    const char *init;
    ptrdiff_t len;
  } capture[LUA_MAXCAPTURES];
} MatchState;


/* recursive function */
static const char *match (MatchState *ms, const char *s, const char *p);


/* maximum recursion depth for 'match' */
#if !defined(MAXCCALLS)
#define MAXCCALLS	200
#endif


#define L_ESC		'%'
#define SPECIALS	"^$*+?.([%-"


static int check_capture (MatchState *ms, int l) {
  l -= '1';
  if (l_unlikely(l < 0 || l >= ms->level ||
                 ms->capture[l].len == CAP_UNFINISHED))
    return luaL_error(ms->L, "invalid capture index %%%d", l + 1);
  return l;
}


static int capture_to_close (MatchState *ms) {
  int level = ms->level;
  for (level--; level>=0; level--)
    if (ms->capture[level].len == CAP_UNFINISHED) return level;
  return luaL_error(ms->L, "invalid pattern capture");
}


static const char *classend (MatchState *ms, const char *p) {
  switch (*p++) {
    case L_ESC: {
      if (l_unlikely(p == ms->p_end))
        luaL_error(ms->L, "malformed pattern (ends with '%%')");
      return p+1;
    }
    case '[': {
      if (*p == '^') p++;
      do {  /* look for a ']' */
        if (l_unlikely(p == ms->p_end))
          luaL_error(ms->L, "malformed pattern (missing ']')");
        if (*(p++) == L_ESC && p < ms->p_end)
          p++;  /* skip escapes (e.g. '%]') */
      } while (*p != ']');
      return p+1;
    }
    default: {
      return p;
    }
  }
}


static int match_class (int c, int cl) {
  int res;
  switch (tolower(cl)) {
    case 'a' : res = isalpha(c); break;
    case 'c' : res = iscntrl(c); break;
    case 'd' : res = isdigit(c); break;
    case 'g' : res = isgraph(c); break;
    case 'l' : res = islower(c); break;
    case 'p' : res = ispunct(c); break;
    case 's' : res = isspace(c); break;
    case 'u' : res = isupper(c); break;
    case 'w' : res = isalnum(c); break;
    case 'x' : res = isxdigit(c); break;
    case 'z' : res = (c == 0); break;  /* deprecated option */
    default: return (cl == c);
  }
  return (islower(cl) ? res : !res);
}


static int matchbracketclass (int c, const char *p, const char *ec) {
  int sig = 1;
  if (*(p+1) == '^') {
    sig = 0;
    p++;  /* skip the '^' */
  }
  while (++p < ec) {
    if (*p == L_ESC) {
      p++;
      if (match_class(c, uchar(*p)))
        return sig;
    }
    else if ((*(p+1) == '-') && (p+2 < ec)) {
      p+=2;
      if (uchar(*(p-2)) <= c && c <= uchar(*p))
        return sig;
    }
    else if (uchar(*p) == c) return sig;
  }
  return !sig;
}


static int singlematch (MatchState *ms, const char *s, const char *p,
                        const char *ep) {
  if (s >= ms->src_end)
    return 0;
  else {
    int c = uchar(*s);
    switch (*p) {
      case '.': return 1;  /* matches any char */
      case L_ESC: return match_class(c, uchar(*(p+1)));
      case '[': return matchbracketclass(c, p, ep-1);
      default:  return (uchar(*p) == c);
    }
  }
}


static const char *matchbalance (MatchState *ms, const char *s,
                                   const char *p) {
  if (l_unlikely(p >= ms->p_end - 1))
    luaL_error(ms->L, "malformed pattern (missing arguments to '%%b')");
  if (*s != *p) return NULL;
  else {
    int b = *p;
    int e = *(p+1);
    int cont = 1;
    while (++s < ms->src_end) {
      if (*s == e) {
        if (--cont == 0) return s+1;
      }
      else if (*s == b) cont++;
    }
  }
  return NULL;  /* string ends out of balance */
}


static const char *max_expand (MatchState *ms, const char *s,
                                 const char *p, const char *ep) {
  ptrdiff_t i = 0;  /* counts maximum expand for item */
  while (singlematch(ms, s + i, p, ep))
    i++;
  /* keeps trying to match with the maximum repetitions */
  while (i>=0) {
    const char *res = match(ms, (s+i), ep+1);
    if (res) return res;
    i--;  /* else didn't match; reduce 1 repetition to try again */
  }
  return NULL;
}


static const char *min_expand (MatchState *ms, const char *s,
                                 const char *p, const char *ep) {
  for (;;) {
    const char *res = match(ms, s, ep+1);
    if (res != NULL)
      return res;
    else if (singlematch(ms, s, p, ep))
      s++;  /* try with one more repetition */
    else return NULL;
  }
}


static const char *start_capture (MatchState *ms, const char *s,
                                    const char *p, int what) {
  const char *res;
  int level = ms->level;
  if (level >= LUA_MAXCAPTURES) luaL_error(ms->L, "too many captures");
  ms->capture[level].init = s;
  ms->capture[level].len = what;
  ms->level = level+1;
  if ((res=match(ms, s, p)) == NULL)  /* match failed? */
    ms->level--;  /* undo capture */
  return res;
}


static const char *end_capture (MatchState *ms, const char *s,
                                  const char *p) {
  int l = capture_to_close(ms);
  const char *res;
  ms->capture[l].len = s - ms->capture[l].init;  /* close capture */
  if ((res = match(ms, s, p)) == NULL)  /* match failed? */
    ms->capture[l].len = CAP_UNFINISHED;  /* undo capture */
  return res;
}


static const char *match_capture (MatchState *ms, const char *s, int l) {
  size_t len;
  l = check_capture(ms, l);
  len = ms->capture[l].len;
  if ((size_t)(ms->src_end-s) >= len &&
      memcmp(ms->capture[l].init, s, len) == 0)
    return s+len;
  else return NULL;
}


static const char *match (MatchState *ms, const char *s, const char *p) {
  if (l_unlikely(ms->matchdepth-- == 0))
    luaL_error(ms->L, "pattern too complex");
  init: /* using goto's to optimize tail recursion */
  if (p != ms->p_end) {  /* end of pattern? */
    switch (*p) {
      case '(': {  /* start capture */
        if (*(p + 1) == ')')  /* position capture? */
          s = start_capture(ms, s, p + 2, CAP_POSITION);
        else
          s = start_capture(ms, s, p + 1, CAP_UNFINISHED);
        break;
      }
      case ')': {  /* end capture */
        s = end_capture(ms, s, p + 1);
        break;
      }
      case '$': {
        if ((p + 1) != ms->p_end)  /* is the '$' the last char in pattern? */
          goto dflt;  /* no; go to default */
        s = (s == ms->src_end) ? s : NULL;  /* check end of string */
        break;
      }
      case L_ESC: {  /* escaped sequences not in the format class[*+?-]? */
        switch (*(p + 1)) {
          case 'b': {  /* balanced string? */
            s = matchbalance(ms, s, p + 2);
            if (s != NULL) {
              p += 4; goto init;  /* return match(ms, s, p + 4); */
            }  /* else fail (s == NULL) */
            break;
          }
          case 'f': {  /* frontier? */
            const char *ep; char previous;
            p += 2;
            if (l_unlikely(*p != '['))
              luaL_error(ms->L, "missing '[' after '%%f' in pattern");
            ep = classend(ms, p);  /* points to what is next */
            previous = (s == ms->src_init) ? '\0' : *(s - 1);
            if (!matchbracketclass(uchar(previous), p, ep - 1) &&
               matchbracketclass(uchar(*s), p, ep - 1)) {
              p = ep; goto init;  /* return match(ms, s, ep); */
            }
            s = NULL;  /* match failed */
            break;
          }
          case '0': case '1': case '2': case '3':
          case '4': case '5': case '6': case '7':
          case '8': case '9': {  /* capture results (%0-%9)? */
            s = match_capture(ms, s, uchar(*(p + 1)));
            if (s != NULL) {
              p += 2; goto init;  /* return match(ms, s, p + 2) */
            }
            break;
          }
          default: goto dflt;
        }
        break;
      }
      default: dflt: {  /* pattern class plus optional suffix */
        const char *ep = classend(ms, p);  /* points to optional suffix */
        /* does not match at least once? */
        if (!singlematch(ms, s, p, ep)) {
          if (*ep == '*' || *ep == '?' || *ep == '-') {  /* accept empty? */
            p = ep + 1; goto init;  /* return match(ms, s, ep + 1); */
          }
          else  /* '+' or no suffix */
            s = NULL;  /* fail */
        }
        else {  /* matched once */
          switch (*ep) {  /* handle optional suffix */
            case '?': {  /* optional */
              const char *res;
              if ((res = match(ms, s + 1, ep + 1)) != NULL)
                s = res;
              else {
                p = ep + 1; goto init;  /* else return match(ms, s, ep + 1); */
              }
              break;
            }
            case '+':  /* 1 or more repetitions */
              s++;  /* 1 match already done */
              /* FALLTHROUGH */
            case '*':  /* 0 or more repetitions */
              s = max_expand(ms, s, p, ep);
              break;
            case '-':  /* 0 or more repetitions (minimum) */
              s = min_expand(ms, s, p, ep);
              break;
            default:  /* no suffix */
              s++; p = ep; goto init;  /* return match(ms, s + 1, ep); */
          }
        }
        break;
      }
    }
  }
  ms->matchdepth++;
  return s;
}



static const char *lmemfind (const char *s1, size_t l1,
                               const char *s2, size_t l2) {
  if (l2 == 0) return s1;  /* empty strings are everywhere */
  else if (l2 > l1) return NULL;  /* avoids a negative 'l1' */
  else {
    const char *init;  /* to search for a '*s2' inside 's1' */
    l2--;  /* 1st char will be checked by 'memchr' */
    l1 = l1-l2;  /* 's2' cannot be found after that */
    while (l1 > 0 && (init = (const char *)memchr(s1, *s2, l1)) != NULL) {
      init++;   /* 1st char is already checked */
      if (memcmp(init, s2+1, l2) == 0)
        return init-1;
      else {  /* correct 'l1' and 's1' to try again */
        l1 -= init-s1;
        s1 = init;
      }
    }
    return NULL;  /* not found */
  }
}


/*
** get information about the i-th capture. If there are no captures
** and 'i==0', return information about the whole match, which
** is the range 's'..'e'. If the capture is a string, return
** its length and put its address in '*cap'. If it is an integer
** (a position), push it on the stack and return CAP_POSITION.
*/
static size_t get_onecapture (MatchState *ms, int i, const char *s,
                              const char *e, const char **cap) {
  if (i >= ms->level) {
    if (l_unlikely(i != 0))
      luaL_error(ms->L, "invalid capture index %%%d", i + 1);
    *cap = s;
    return e - s;
  }
  else {
    ptrdiff_t capl = ms->capture[i].len;
    *cap = ms->capture[i].init;
    if (l_unlikely(capl == CAP_UNFINISHED))
      luaL_error(ms->L, "unfinished capture");
    else if (capl == CAP_POSITION)
      lua_pushinteger(ms->L, (ms->capture[i].init - ms->src_init) + 1);
    return capl;
  }
}


/*
** Push the i-th capture on the stack.
*/
static void push_onecapture (MatchState *ms, int i, const char *s,
                                                    const char *e) {
  const char *cap;
  ptrdiff_t l = get_onecapture(ms, i, s, e, &cap);
  if (l != CAP_POSITION)
    lua_pushlstring(ms->L, cap, l);
  /* else position was already pushed */
}


static int push_captures (MatchState *ms, const char *s, const char *e) {
  int i;
  int nlevels = (ms->level == 0 && s) ? 1 : ms->level;
  luaL_checkstack(ms->L, nlevels, "too many captures");
  for (i = 0; i < nlevels; i++)
    push_onecapture(ms, i, s, e);
  return nlevels;  /* number of strings pushed */
}


/* check whether pattern has no special characters */
static int nospecials (const char *p, size_t l) {
  size_t upto = 0;
  do {
    if (strpbrk(p + upto, SPECIALS))
      return 0;  /* pattern has a special character */
    upto += strlen(p + upto) + 1;  /* may have more after \0 */
  } while (upto <= l);
  return 1;  /* no special chars found */
}


static void prepstate (MatchState *ms, lua_State *L,
                       const char *s, size_t ls, const char *p, size_t lp) {
  ms->L = L;
  ms->matchdepth = MAXCCALLS;
  ms->src_init = s;
  ms->src_end = s + ls;
  ms->p_end = p + lp;
}


static void reprepstate (MatchState *ms) {
  ms->level = 0;
  lua_assert(ms->matchdepth == MAXCCALLS);
}


static int str_find_aux (lua_State *L, int find) {
  size_t ls, lp;
  const char *s = luaL_checklstring(L, 1, &ls);
  const char *p = luaL_checklstring(L, 2, &lp);
  size_t init = posrelatI(luaL_optinteger(L, 3, 1), ls) - 1;
  if (init > ls) {  /* start after string's end? */
    luaL_pushfail(L);  /* cannot find anything */
    return 1;
  }
  /* explicit request or no special characters? */
  if (find && (lua_toboolean(L, 4) || nospecials(p, lp))) {
    /* do a plain search */
    const char *s2 = lmemfind(s + init, ls - init, p, lp);
    if (s2) {
      lua_pushinteger(L, (s2 - s) + 1);
      lua_pushinteger(L, (s2 - s) + lp);
      return 2;
    }
  }
  else {
    MatchState ms;
    const char *s1 = s + init;
    int anchor = (*p == '^');
    if (anchor) {
      p++; lp--;  /* skip anchor character */
    }
    prepstate(&ms, L, s, ls, p, lp);
    do {
      const char *res;
      reprepstate(&ms);
      if ((res=match(&ms, s1, p)) != NULL) {
        if (find) {
          lua_pushinteger(L, (s1 - s) + 1);  /* start */
          lua_pushinteger(L, res - s);   /* end */
          return push_captures(&ms, NULL, 0) + 2;
        }
        else
          return push_captures(&ms, s1, res);
      }
    } while (s1++ < ms.src_end && !anchor);
  }
  luaL_pushfail(L);  /* not found */
  return 1;
}


static int str_find (lua_State *L) {
  return str_find_aux(L, 1);
}


static int str_match (lua_State *L) {
  return str_find_aux(L, 0);
}


/* state for 'gmatch' */
typedef struct GMatchState {
  const char *src;  /* current position */
  const char *p;  /* pattern */
  const char *lastmatch;  /* end of last match */
  MatchState ms;  /* match state */
} GMatchState;


static int gmatch_aux (lua_State *L) {
  GMatchState *gm = (GMatchState *)lua_touserdata(L, lua_upvalueindex(3));
  const char *src;
  gm->ms.L = L;
  for (src = gm->src; src <= gm->ms.src_end; src++) {
    const char *e;
    reprepstate(&gm->ms);
    if ((e = match(&gm->ms, src, gm->p)) != NULL && e != gm->lastmatch) {
      gm->src = gm->lastmatch = e;
      return push_captures(&gm->ms, src, e);
    }
  }
  return 0;  /* not found */
}


static int gmatch (lua_State *L) {
  size_t ls, lp;
  const char *s = luaL_checklstring(L, 1, &ls);
  const char *p = luaL_checklstring(L, 2, &lp);
  size_t init = posrelatI(luaL_optinteger(L, 3, 1), ls) - 1;
  GMatchState *gm;
  lua_settop(L, 2);  /* keep strings on closure to avoid being collected */
  gm = (GMatchState *)lua_newuserdatauv(L, sizeof(GMatchState), 0);
  if (init > ls)  /* start after string's end? */
    init = ls + 1;  /* avoid overflows in 's + init' */
  prepstate(&gm->ms, L, s, ls, p, lp);
  gm->src = s + init; gm->p = p; gm->lastmatch = NULL;
  lua_pushcclosure(L, gmatch_aux, 3);
  return 1;
}


static void add_s (MatchState *ms, luaL_Buffer *b, const char *s,
                                                   const char *e) {
  size_t l;
  lua_State *L = ms->L;
  const char *news = lua_tolstring(L, 3, &l);
  const char *p;
  while ((p = (char *)memchr(news, L_ESC, l)) != NULL) {
    luaL_addlstring(b, news, p - news);
    p++;  /* skip ESC */
    if (*p == L_ESC)  /* '%%' */
      luaL_addchar(b, *p);
    else if (*p == '0')  /* '%0' */
        luaL_addlstring(b, s, e - s);
    else if (isdigit(uchar(*p))) {  /* '%n' */
      const char *cap;
      ptrdiff_t resl = get_onecapture(ms, *p - '1', s, e, &cap);
      if (resl == CAP_POSITION)
        luaL_addvalue(b);  /* add position to accumulated result */
      else
        luaL_addlstring(b, cap, resl);
    }
    else
      luaL_error(L, "invalid use of '%c' in replacement string", L_ESC);
    l -= p + 1 - news;
    news = p + 1;
  }
  luaL_addlstring(b, news, l);
}


/*
** Add the replacement value to the string buffer 'b'.
** Return true if the original string was changed. (Function calls and
** table indexing resulting in nil or false do not change the subject.)
*/
static int add_value (MatchState *ms, luaL_Buffer *b, const char *s,
                                      const char *e, int tr) {
  lua_State *L = ms->L;
  switch (tr) {
    case LUA_TFUNCTION: {  /* call the function */
      int n;
      lua_pushvalue(L, 3);  /* push the function */
      n = push_captures(ms, s, e);  /* all captures as arguments */
      lua_call(L, n, 1);  /* call it */
      break;
    }
    case LUA_TTABLE: {  /* index the table */
      push_onecapture(ms, 0, s, e);  /* first capture is the index */
      lua_gettable(L, 3);
      break;
    }
    default: {  /* LUA_TNUMBER or LUA_TSTRING */
      add_s(ms, b, s, e);  /* add value to the buffer */
      return 1;  /* something changed */
    }
  }
  if (!lua_toboolean(L, -1)) {  /* nil or false? */
    lua_pop(L, 1);  /* remove value */
    luaL_addlstring(b, s, e - s);  /* keep original text */
    return 0;  /* no changes */
  }
  else if (l_unlikely(!lua_isstring(L, -1)))
    return luaL_error(L, "invalid replacement value (a %s)",
                         luaL_typename(L, -1));
  else {
    luaL_addvalue(b);  /* add result to accumulator */
    return 1;  /* something changed */
  }
}


static int str_gsub (lua_State *L) {
  size_t srcl, lp;
  const char *src = luaL_checklstring(L, 1, &srcl);  /* subject */
  const char *p = luaL_checklstring(L, 2, &lp);  /* pattern */
  const char *lastmatch = NULL;  /* end of last match */
  int tr = lua_type(L, 3);  /* replacement type */
  lua_Integer max_s = luaL_optinteger(L, 4, srcl + 1);  /* max replacements */
  int anchor = (*p == '^');
  lua_Integer n = 0;  /* replacement count */
  int changed = 0;  /* change flag */
  MatchState ms;
  luaL_Buffer b;
  luaL_argexpected(L, tr == LUA_TNUMBER || tr == LUA_TSTRING ||
                   tr == LUA_TFUNCTION || tr == LUA_TTABLE, 3,
                      "string/function/table");
  luaL_buffinit(L, &b);
  if (anchor) {
    p++; lp--;  /* skip anchor character */
  }
  prepstate(&ms, L, src, srcl, p, lp);
  while (n < max_s) {
    const char *e;
    reprepstate(&ms);  /* (re)prepare state for new match */
    if ((e = match(&ms, src, p)) != NULL && e != lastmatch) {  /* match? */
      n++;
      changed = add_value(&ms, &b, src, e, tr) | changed;
      src = lastmatch = e;
    }
    else if (src < ms.src_end)  /* otherwise, skip one character */
      luaL_addchar(&b, *src++);
    else break;  /* end of subject */
    if (anchor) break;
  }
  if (!changed)  /* no changes? */
    lua_pushvalue(L, 1);  /* return original string */
  else {  /* something changed */
    luaL_addlstring(&b, src, ms.src_end-src);
    luaL_pushresult(&b);  /* create and return new string */
  }
  lua_pushinteger(L, n);  /* number of substitutions */
  return 2;
}

/* }====================================================== */

const luaL_Reg strlib[] = {
  {"byte", str_byte},
  {"char", str_char},
  {"find", str_find},
  {"gmatch", gmatch},
  {"gsub", str_gsub},
  {"len", str_len},
  {"match", str_match},
  {"reverse", str_reverse},
  {"sub", str_sub},
  {NULL, NULL}
};