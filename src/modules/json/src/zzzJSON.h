#ifndef zj_JSON_H
#define zj_JSON_H

#include <stdbool.h> // 使用其 bool
#include <stdint.h>  // 使用其 uint32_t
#include <stdio.h>   // 使用其 sprintf 函数
#include <stdlib.h>  // 使用其 atof malloc free 函数
#include <string.h>  // 使用其 memcpy 函数

// API模式
// 0 经典模式（ C语言 ），所有API为 zj_ 开头
// 1 短API模式（ C语言 ），所有API没有 zj_ 开头，有可能会跟其它头文件冲突，因为
// C语言 没有命名空间
#ifndef zj_API_MODE
#define zj_API_MODE 1
#endif

// 是否支持 C++ 模式
#ifndef zj_CPLUSPLUS
#ifdef __cplusplus
#define zj_CPLUSPLUS 1
#else
#define zj_CPLUSPLUS 0
#endif
#endif

// C++ 支持命名空间
#if zj_CPLUSPLUS == 1
// 如果是C++模式，那么有名空间，短API开启
#define zj_API_MODE 1
namespace zj {
#endif

// 长命名的 类型 & 常量 & 用作常量的宏，详见《数据结构》
#ifndef zj_SIZE_TYPE
#define zj_SIZE_TYPE uint32_t
#endif
typedef zj_SIZE_TYPE zj_Size;

typedef enum zj_JSONType {
  zj_JSONTypeBool = 1,
  zj_JSONTypeArray = 2,
  zj_JSONTypeObject = 3,
  zj_JSONTypeString = 4,
  zj_JSONTypeNull = 5,
  zj_JSONTypeNumber = 6
} zj_JSONType;

// 内存配置，详见《内存配置》
#ifndef zj_DELTA
#define zj_DELTA 2
#endif
static const zj_Size zj_Delta = zj_DELTA;

#ifndef zj_ALLOCATOR_INIT_MEMSIZE
#define zj_ALLOCATOR_INIT_MEMSIZE 1024 * 4
#endif
static const zj_Size zj_AllocatorInitMemSize = zj_ALLOCATOR_INIT_MEMSIZE;

// Stringify函数初始化字符串的大小
#ifndef zj_STRING_INIT_MEMSIZE
#define zj_STRING_INIT_MEMSIZE 1024
#endif
static const zj_Size zj_StringInitMemSize = zj_STRING_INIT_MEMSIZE;

// 用zj_String作为Cache时初始化内存的大小，后面可能去掉
#ifndef zj_STRING_CACHE_INIT_MEMSIZE
#define zj_STRING_CACHE_INIT_MEMSIZE 128
#endif
static const zj_Size zj_StringCacheInitMemSize = zj_STRING_CACHE_INIT_MEMSIZE;

// 环境适配

#ifndef zj_MEMORY_MODE
#define zj_MEMORY_MODE 1
#endif

#if zj_MEMORY_MODE == 1

static inline void *zj_new(zj_Size size) { return malloc(size); }
static inline void zj_free(void *pointer) { free(pointer); }

#elif zj_MEMORY_MODE == 2
// 测试模式，主要是用来测试内存的分配数和释放数，防止内存泄漏，同时用于观察内存分配次数。
// 通过观察，可以得出较好zj_DELTA和zj_ALLOCATORINITMEMSIZE。

static zj_Size AllocMemorySize = 0, AllocMemoryCount = 0, FreeMemoryCount = 0;
static inline void *zj_new(zj_Size size) {
  return AllocMemorySize += size, AllocMemoryCount += 1, malloc(size);
}
static inline void zj_free(void *ptr) { FreeMemoryCount += 1, free(ptr); }

#elif zj_MEMORY_MODE == 3
// 自定义模式，需要实现zj_New，zj_Free和zj_MemCopy三个函数
#endif

// 分支预测
#ifndef zj_EXPECT_MODE
#if defined(__GNUC__) || defined(__clang__)
#define zj_EXPECT_MODE 1
#else
#define zj_EXPECT_MODE 2
#endif
#endif

#if zj_EXPECT_MODE == 1
// gcc和clang，使用__builtin_expect
#define zj_LIKELY(x) __builtin_expect(x, 1)
#define zj_UNLIKELY(x) __builtin_expect(x, 0)
#elif zj_EXPECT_MODE == 2
// msvc 不需要分支优化
#define zj_LIKELY(x) x
#define zj_UNLIKELY(x) x
#elif zj_EXPECT_MODE == 3
// 自定义分支预测
#endif

// 长命名数据结构，详见《数据结构》
typedef struct zj_Allocator zj_Allocator;
typedef struct zj_Value zj_Value;

// 长命名数据结构，详见《API》
static inline zj_Allocator *zj_NewAllocator();
static inline void zj_ReleaseAllocator(zj_Allocator *root_alloc);
static inline zj_Value *zj_NewValue(zj_Allocator *alloc);
static inline bool zj_ParseFast(zj_Value *v, const char *s);
static inline bool zj_ParseLen(zj_Value *v, const char *s, zj_Size len);
static inline bool zj_Parse(zj_Value *v, const char *s);
static inline const char *zj_Stringify(const zj_Value *v);
static inline const char *zj_GetStrFast(const zj_Value *v, zj_Size *len);
static inline const char *zj_GetUnEscapeStr(zj_Value *v);
static inline const char *zj_GetStr(zj_Value *v);
static inline const char *zj_GetNumFast(const zj_Value *v, zj_Size *len);
static inline const char *zj_GetNumStr(zj_Value *v);
static inline const double *zj_GetNum(zj_Value *v);
static inline const double *zj_GetDouble(zj_Value *v);
static inline const int *zj_GetInt(zj_Value *v);
static inline const long *zj_GetLong(zj_Value *v);
static inline const long long *zj_GetLongLong(zj_Value *v);
static inline const bool *zj_GetBool(const zj_Value *v);
static inline bool zj_IsNull(const zj_Value *v);
static inline const char *zj_GetKey(zj_Value *v);
static inline const char *zj_GetUnEscapeKey(zj_Value *v);
static inline const char *zj_GetKeyFast(const zj_Value *v, zj_Size *len);
static inline zj_Value *zj_ObjGet(const zj_Value *v, const char *key);
static inline zj_Value *zj_ObjGetLen(const zj_Value *v, const char *key,
                                     zj_Size len);
static inline const zj_JSONType *zj_Type(const zj_Value *v);
static inline zj_Size zj_SizeOf(const zj_Value *v);
static inline zj_Value *zj_ArrayGet(const zj_Value *v, zj_Size index);
static inline zj_Value *zj_Begin(const zj_Value *v);
static inline zj_Value *zj_Next(const zj_Value *v);
static inline zj_Value *zj_Copy(const zj_Value *v);
static inline bool zj_Move(zj_Value *v);
static inline void zj_SetNull(zj_Value *v);
static inline void zj_SetBool(zj_Value *v, bool b);
static inline bool zj_SetNumStrFast(zj_Value *v, const char *num);
static inline bool zj_SetNumStrLenFast(zj_Value *v, const char *num,
                                       zj_Size len);
static inline bool zj_SetNumStr(zj_Value *v, const char *num);
static inline bool zj_SetNumStrLen(zj_Value *v, const char *num, zj_Size len);
static inline bool zj_SetNum(zj_Value *v, const double d);
static inline bool zj_SetDouble(zj_Value *v, const double d);
static inline bool zj_SetInt(zj_Value *v, const int n);
static inline bool zj_SetLong(zj_Value *v, const long n);
static inline bool zj_SetLongLong(zj_Value *v, const long long n);
static inline bool zj_SetStrFast(zj_Value *v, const char *str);
static inline bool zj_SetStrLenFast(zj_Value *v, const char *str, zj_Size len);
static inline bool zj_SetStr(zj_Value *v, const char *str);
static inline bool zj_SetStrLen(zj_Value *v, const char *str, zj_Size len);
static inline bool zj_SetStrEscape(zj_Value *v, const char *str);
static inline bool zj_SetStrLenEscape(zj_Value *v, const char *str,
                                      zj_Size len);
static inline bool zj_SetKeyFast(zj_Value *v, const char *key);
static inline bool zj_SetKeyLenFast(zj_Value *v, const char *key, zj_Size len);
static inline bool zj_SetKey(zj_Value *v, const char *key);
static inline bool zj_SetKeyLen(zj_Value *v, const char *key, zj_Size len);
static inline bool zj_SetKeyEscape(zj_Value *v, const char *key);
static inline bool zj_SetKeyLenEscape(zj_Value *v, const char *key,
                                      zj_Size len);
static inline void zj_SetArray(zj_Value *v);
static inline void zj_SetObj(zj_Value *v);
static inline bool zj_SetFast(zj_Value *v, zj_Value *vv);
static inline bool zj_Set(zj_Value *v, const zj_Value *vv);
static inline bool zj_ObjAddFast(zj_Value *v, zj_Value *vv);
static inline bool zj_ObjAdd(zj_Value *v, const zj_Value *vv);
static inline bool zj_ArrayAddFast(zj_Value *v, zj_Value *vv);
static inline bool zj_ArrayAdd(zj_Value *v, const zj_Value *vv);
static inline bool zj_ArrayDel(zj_Value *v, zj_Size index);
static inline bool zj_ObjDel(zj_Value *v, const char *key);

#if zj_API_MODE == 1

// 短命名数据结构，详见《数据结构》
typedef zj_Allocator Allocator;
typedef zj_Value Value;

typedef zj_Size Size;

typedef enum JSONType {
  JSONTypeArray = zj_JSONTypeArray,
  JSONTypeObject = zj_JSONTypeObject,
  JSONTypeString = zj_JSONTypeString,
  JSONTypeNumber = zj_JSONTypeNumber,
  JSONTypeBool = zj_JSONTypeBool,
  JSONTypeNull = zj_JSONTypeNull
} JSONType;

// 短命名API，详见《API》
static inline Allocator *NewAllocator() { return zj_NewAllocator(); }
static inline void ReleaseAllocator(Allocator *root_alloc) {
  zj_ReleaseAllocator(root_alloc);
}
static inline Value *NewValue(Allocator *alloc) { return zj_NewValue(alloc); }
static inline bool ParseFast(Value *v, const char *s) {
  return zj_ParseFast(v, s);
}
static inline bool ParseLen(Value *v, const char *s, Size len) {
  return zj_ParseLen(v, s, len);
}
static inline bool Parse(Value *v, const char *s) { return zj_Parse(v, s); }
static inline const char *Stringify(const Value *v) { return zj_Stringify(v); }
static inline const char *GetStrFast(const Value *v, Size *len) {
  return zj_GetStrFast(v, len);
}
static inline const char *GetUnEscapeStr(Value *v) {
  return zj_GetUnEscapeStr(v);
}
static inline const char *GetStr(Value *v) { return zj_GetStr(v); }
static inline const char *GetNumFast(const Value *v, zj_Size *len) {
  return zj_GetNumFast(v, len);
}
static inline const char *GetNumStr(Value *v) { return zj_GetNumStr(v); }
static inline const double *GetNum(Value *v) { return zj_GetNum(v); }
static inline const double *GetDouble(Value *v) { return zj_GetDouble(v); }
static inline const int *GetInt(Value *v) { return zj_GetInt(v); }
static inline const long *GetLong(Value *v) { return zj_GetLong(v); }
static inline const long long *GetLongLong(Value *v) {
  return zj_GetLongLong(v);
}
static inline const bool *GetBool(const Value *v) { return zj_GetBool(v); }
static inline bool IsNull(const Value *v) { return zj_IsNull(v); }
static inline const char *GetKey(Value *v) { return zj_GetKey(v); }
static inline const char *GetUnEscapeKey(Value *v) {
  return zj_GetUnEscapeKey(v);
}
static inline const char *GetKeyFast(const Value *v, Size *len) {
  return zj_GetKeyFast(v, len);
}
static inline Value *ObjGet(const Value *v, const char *key) {
  return zj_ObjGet(v, key);
}
static inline Value *ObjGetLen(const Value *v, const char *key, Size len) {
  return zj_ObjGetLen(v, key, len);
}
static inline const JSONType *Type(const Value *v) {
  return (const JSONType *)zj_Type(v);
}
static inline Size SizeOf(const Value *v) { return zj_SizeOf(v); }
static inline Value *ArrayGet(const Value *v, Size index) {
  return zj_ArrayGet(v, index);
}
static inline Value *Begin(const Value *v) { return zj_Begin(v); }
static inline Value *Next(const Value *v) { return zj_Next(v); }
static inline Value *Copy(const Value *v) { return zj_Copy(v); }
static inline bool Move(Value *v) { return zj_Move(v); }
static inline void SetNull(Value *v) { zj_SetNull(v); }
static inline void SetBool(Value *v, bool b) { zj_SetBool(v, b); }
static inline bool SetNumStrFast(Value *v, const char *num) {
  return zj_SetNumStrFast(v, num);
}
static inline bool SetNumStrLenFast(Value *v, const char *num, Size len) {
  return zj_SetNumStrLenFast(v, num, len);
}
static inline bool SetNumStr(Value *v, const char *num) {
  return zj_SetNumStr(v, num);
}
static inline bool SetNumStrLen(Value *v, const char *num, Size len) {
  return zj_SetNumStrLen(v, num, len);
}
static inline bool SetNum(Value *v, const double d) { return zj_SetNum(v, d); }
static inline bool SetDouble(Value *v, const double d) {
  return zj_SetDouble(v, d);
}
static inline bool SetInt(Value *v, const int d) { return zj_SetInt(v, d); }
static inline bool SetLong(Value *v, const long d) { return zj_SetLong(v, d); }
static inline bool SetLongLong(Value *v, const long long d) {
  return zj_SetLongLong(v, d);
}
static inline bool SetStrFast(Value *v, const char *str) {
  return zj_SetStrFast(v, str);
}
static inline bool SetStrLenFast(Value *v, const char *str, Size len) {
  return zj_SetStrLenFast(v, str, len);
}
static inline bool SetStr(Value *v, const char *str) {
  return zj_SetStr(v, str);
}
static inline bool SetStrLen(Value *v, const char *str, Size len) {
  return zj_SetStrLen(v, str, len);
}
static inline bool SetStrEscape(Value *v, const char *str) {
  return zj_SetStrEscape(v, str);
}
static inline bool SetStrLenEscape(Value *v, const char *str, Size len) {
  return zj_SetStrLenEscape(v, str, len);
}
static inline bool SetKeyFast(Value *v, const char *key) {
  return zj_SetKeyFast(v, key);
}
static inline bool SetKeyLenFast(Value *v, const char *key, Size len) {
  return zj_SetKeyLenFast(v, key, len);
}
static inline bool SetKey(Value *v, const char *key) {
  return zj_SetKey(v, key);
}
static inline bool SetKeyLen(Value *v, const char *key, Size len) {
  return zj_SetKeyLen(v, key, len);
}
static inline bool SetKeyEscape(Value *v, const char *key) {
  return zj_SetKeyEscape(v, key);
}
static inline bool SetKeyLenEscape(Value *v, const char *key, Size len) {
  return zj_SetKeyLenEscape(v, key, len);
}
static inline void SetArray(Value *v) { zj_SetArray(v); }
static inline void SetObj(Value *v) { zj_SetObj(v); }
static inline bool SetFast(Value *v, Value *vv) { return zj_SetFast(v, vv); }
static inline bool Set(Value *v, const Value *vv) { return zj_Set(v, vv); }
static inline bool ObjAddFast(Value *v, Value *vv) {
  return zj_ObjAddFast(v, vv);
}
static inline bool ObjAdd(Value *v, const Value *vv) {
  return zj_ObjAdd(v, vv);
}
static inline bool ArrayAddFast(Value *v, Value *vv) {
  return zj_ArrayAddFast(v, vv);
}
static inline bool ArrayAdd(Value *v, const Value *vv) {
  return zj_ArrayAdd(v, vv);
}
static inline bool ArrayDel(Value *v, Size index) {
  return zj_ArrayDel(v, index);
}
static inline bool ObjDel(Value *v, const char *key) {
  return zj_ObjDel(v, key);
}
#endif

// 长命名的固字符串，详见《数据结构》
static const char *zj_strTrue = "true";
static const char *zj_strFalse = "false";
static const char *zj_strNull = "null";

static const zj_JSONType zj_jsontype_bool = zj_JSONTypeBool;
static const zj_JSONType zj_jsontype_array = zj_JSONTypeArray;
static const zj_JSONType zj_jsontype_object = zj_JSONTypeObject;
static const zj_JSONType zj_jsontype_string = zj_JSONTypeString;
static const zj_JSONType zj_jsontype_null = zj_JSONTypeNull;
static const zj_JSONType zj_jsontype_number = zj_JSONTypeNumber;

// 内存拷贝函数
static inline void zj_memCopy(const char *src, zj_Size len, char *des) {
  memcpy(des, src, len);
}

// 字符串长度计算函数
static inline zj_Size zj_strLen(const char *str) {
  return (zj_Size)strlen(str);
}

static inline int zj_strToInt(const char *str, zj_Size len) {
  char s[128] = {0};
  memcpy(s, str, len);
  return atoi(s);
}
static inline long zj_strToLong(const char *str, zj_Size len) {
  char s[128] = {0};
  memcpy(s, str, len);
  return atol(str);
}
static inline long long zj_strToLongLong(const char *str, zj_Size len) {
  char s[128] = {0};
  memcpy(s, str, len);
  return atoll(str);
}
static inline double zj_strToDouble(const char *str, zj_Size len) {
  char s[128] = {0};
  memcpy(s, str, len);
  return atof(str);
}

static inline zj_Size zj_intToStr(int n, char *buff) {
  // int 最大长度不超过12
  return snprintf(buff, 12, "%d", n);
}
static inline zj_Size zj_longToStr(long n, char *buff) {
  // long 最大长度不超过24
  return snprintf(buff, 24, "%ld", n);
}
static inline zj_Size zj_longLongToStr(long long n, char *buff) {
  // long long 最大长度不超过24
  return snprintf(buff, 24, "%lld", n);
}
static inline zj_Size zj_doubleToStr(double n, char *buff) {
  // double最大精度是17，使用sprintf转出字符串，%.17g，最大长度不超过32
  return snprintf(buff, 32, "%.17g", n);
}

// 字符串比较，a必须以0结束，len为b的长度。
static inline bool zj_strIsEqual(const char *a, const char *b, zj_Size len) {
  zj_Size i;
  for (i = 0; zj_LIKELY(i < len); ++i) {
    if (zj_LIKELY(a[i] != b[i])) {
      return false;
    }
  }
  // a字符串必须结束才能算相等
  if (zj_LIKELY(a[i] == 0))
    return true;
  return false;
}

// 字符串比较，len为b的长度。
static inline bool zj_strIsEqualLen(const char *a, zj_Size a_len, const char *b,
                                    zj_Size b_len) {
  if (zj_LIKELY(a_len != b_len)) {
    return false;
  }
  zj_Size i;
  for (i = 0; zj_LIKELY(i < a_len); ++i) {
    if (zj_LIKELY(a[i] != b[i])) {
      return false;
    }
  }
  return true;
}

// 内存分配器节点
struct zj_aNode {
  // 数据地址
  char *Data;
  // 数据大小
  zj_Size SizeOf;
  // 使用到的位置
  zj_Size Pos;
  // 下一个节点
  struct zj_aNode *Next;
};

// 内存分配器
// 内存分配器为由内存分配器节点组成的链表，Root为根节点，End总是指向最后一个节点
struct zj_Allocator {
#if zj_CPLUSPLUS == 1
public:
  zj_Allocator() {
    void *ptr = zj_new(sizeof(struct zj_aNode) + zj_AllocatorInitMemSize);
    Root = (struct zj_aNode *)ptr;
    End = Root;
    Root->SizeOf = zj_AllocatorInitMemSize;
    Root->Data = (char *)ptr + sizeof(struct zj_aNode);
    Root->Pos = 0;
    Root->Next = 0;
  }
  ~zj_Allocator() {
    struct zj_aNode *next = Root;
    while (zj_LIKELY(next != 0)) {
      struct zj_aNode *nn = next->Next;
      zj_free((void *)next);
      next = nn;
    }
  }
#endif
  // 根节点
  struct zj_aNode *Root;
  // 最后一个节点
  struct zj_aNode *End;
};

// 函数说明详见《API》
static inline zj_Allocator *zj_NewAllocator() {
  // 分配大块内存
  void *ptr = zj_new(sizeof(zj_Allocator) + sizeof(struct zj_aNode) +
                     zj_AllocatorInitMemSize);
  zj_Allocator *alloc = (zj_Allocator *)ptr;
  alloc->Root = (struct zj_aNode *)((char *)ptr + sizeof(zj_Allocator));
  alloc->End = alloc->Root;

  alloc->Root->SizeOf = zj_AllocatorInitMemSize;
  alloc->Root->Data =
      (char *)ptr + sizeof(zj_Allocator) + sizeof(struct zj_aNode);
  alloc->Root->Pos = 0;
  alloc->Root->Next = 0;
  return alloc;
}

// 函数说明详见《API》
static inline void zj_ReleaseAllocator(zj_Allocator *alloc) {
  // 遍历整个链表，每次释放一块内存
  struct zj_aNode *next = alloc->Root->Next;
  while (zj_LIKELY(next != 0)) {
    struct zj_aNode *nn = next->Next;
    zj_free((void *)next);
    next = nn;
  }
  // 最后释放第一块内存
  zj_free((void *)alloc);
}

// 追加一个大小为 init_size 的节点。
static inline void zj_allocatorAppendChild(zj_Allocator *alloc,
                                           zj_Size init_size) {
  // 每次分配一大块内存，避免多次分配
  void *ptr = zj_new(sizeof(struct zj_aNode) + init_size);
  struct zj_aNode *node = (struct zj_aNode *)ptr;
  node->SizeOf = init_size;
  node->Data = (char *)ptr + sizeof(struct zj_aNode);
  node->Pos = 0;
  node->Next = 0;
  // 在ANode组成的链表最后加一个ANode
  alloc->End->Next = node;
  alloc->End = node;
  return;
}

// 分配大小为size的内存
static inline char *zj_allocatorAlloc(zj_Allocator *alloc, zj_Size size) {
  struct zj_aNode *cur_node = alloc->End;
  zj_Size s = cur_node->SizeOf;
  if (zj_UNLIKELY(cur_node->Pos + size > s)) {
    s *= zj_Delta;
    // 通过循环计算最终需要的空间大小
    // 这里应该有更好的方法，就是直接通过计算所得
    while (zj_UNLIKELY(size > s))
      s *= zj_Delta; // 每次分配内存的大小是上次的zj_Delta倍
    zj_allocatorAppendChild(alloc, s);
    cur_node = alloc->End;
  }
  char *ret = cur_node->Data + cur_node->Pos;
  cur_node->Pos += size;
  return ret;
}

// 字符串
// 字符串由内存分配器分配内存，当追加操作导致内存不够时，直接分配 zj_Delta
// 倍内存，并把旧内存拷贝到新内存中
struct zj_string {
  // 数据
  char *Data;
  // 位置
  zj_Size Pos;
  // 使用内存大小
  zj_Size SizeOf;
  // 分配器
  zj_Allocator *A;
};

static struct zj_string *zj_stringCache = 0;

// 新建一个字符串
static inline struct zj_string *zj_newString(zj_Allocator *alloc,
                                             zj_Size init_size) {
  struct zj_string *str = (struct zj_string *)zj_allocatorAlloc(
      alloc, sizeof(struct zj_string) + init_size);
  str->SizeOf = init_size;
  str->Data = (char *)str + sizeof(struct zj_string);
  str->Pos = 0;
  str->A = alloc;
  return str;
}

// 清空一个字符串
static inline void zj_reset(struct zj_string *str) { str->Pos = 0; }

// 追加字符串
static inline void zj_appendStr(struct zj_string *str, const char *s,
                                zj_Size size) {
  zj_Size src_s = str->SizeOf;
  if (zj_UNLIKELY(str->Pos + size > src_s)) {
    src_s *= zj_Delta;
    while (zj_UNLIKELY(str->Pos + size > src_s))
      src_s *= zj_Delta;
    const char *src_d = str->Data;
    str->Data = (char *)zj_allocatorAlloc(str->A, src_s);
    str->SizeOf = src_s;
    zj_memCopy(src_d, str->Pos, str->Data);
  }
  zj_memCopy(s, size, str->Data + str->Pos);
  str->Pos += size;
}

// 追加字符
static inline void zj_appendChar(struct zj_string *str, const char c) {
  zj_Size src_s = str->SizeOf;
  if (zj_UNLIKELY(str->Pos + 1 > src_s)) {
    src_s *= zj_Delta;
    const char *src_d = str->Data;
    str->Data = (char *)zj_allocatorAlloc(str->A, src_s);
    str->SizeOf = src_s;
    zj_memCopy(src_d, str->Pos, str->Data);
  }
  *(str->Data + str->Pos) = c;
  str->Pos += 1;
}

// 追加结束符
static inline void zj_appendEnd(struct zj_string *str) {
  zj_appendChar(str, 0);
}

// 获得字符串
static inline const char *zj_str(struct zj_string *str) { return str->Data; }

// zzzJSON把文本转化成内存中的一棵树，zj_Node为该数的节点，每个节点对应一个值
struct zj_node {
  // 节点代表的值的类型
  zj_JSONType Type;

  // 节点代表的值的关键字
  const char *Key;
  // 节点代表的值的关键字长度
  zj_Size KeyLen;

  union {
    // 如果节点代表的值的类型为数组或者对象，则表示数组或者对象的第一个值对应的节点
    struct zj_node *Node;
    // 如果节点代表的值的类型为字符串，数字，布尔值，则对应其字符串
    const char *Str;
  } Value;

  // 节点对应的值包含值的个数，如果类型非对象或者数组，则为0
  zj_Size Len;

  // 下一个节点
  struct zj_node *Next;
  // 上一个节点
  struct zj_node *Prev;
  // 父节点
  struct zj_node *Father;
  // 最后一个节点
  struct zj_node *End;
};

// zzzJSON的基本单位：值，包含一个节点和一个内存分配器
struct zj_Value {
#if zj_CPLUSPLUS == 1
private:
  zj_Value() {}

public:
  static Value *NewValue(Allocator *alloc) { return zj_NewValue(alloc); }
  zj_Allocator *Allocator() { return A; }
  inline bool ParseFast(const char *s) { return zj_ParseFast(this, s); }
  inline bool ParseLen(const char *s, Size len) {
    return zj_ParseLen(this, s, len);
  }
  inline bool Parse(const char *s) { return zj_Parse(this, s); }
  inline const char *Stringify() { return zj_Stringify(this); }
  inline const char *GetStrFast(Size *len) { return zj_GetStrFast(this, len); }
  inline const char *GetUnEscapeStr() { return zj_GetUnEscapeStr(this); }
  inline const char *GetStr() { return zj_GetStr(this); }
  inline const char *GetNumFast(zj_Size *len) {
    return zj_GetNumFast(this, len);
  }
  inline const char *GetNumStr() { return zj_GetNumStr(this); }
  inline const double *GetNum() { return zj_GetNum(this); }
  inline const double *GetDouble() { return zj_GetDouble(this); }
  inline const int *GetInt() { return zj_GetInt(this); }
  inline const long *GetLong() { return zj_GetLong(this); }
  inline const long long *GetLongLong() { return zj_GetLongLong(this); }
  inline const bool *GetBool() { return zj_GetBool(this); }
  inline bool IsNull() { return zj_IsNull(this); }
  inline const char *GetKey() { return zj_GetKey(this); }
  inline const char *GetUnEscapeKey() { return zj_GetUnEscapeKey(this); }
  inline const char *GetKeyFast(Size *len) { return zj_GetKeyFast(this, len); }
  inline Value *ObjGet(const char *key) { return zj_ObjGet(this, key); }
  inline Value *ObjGetLen(const char *key, Size len) {
    return zj_ObjGetLen(this, key, len);
  }
  inline const JSONType *Type() { return (const JSONType *)zj_Type(this); }
  inline Size SizeOf() { return zj_SizeOf(this); }
  inline Value *ArrayGet(Size index) { return zj_ArrayGet(this, index); }
  inline Value *Begin() { return zj_Begin(this); }
  inline Value *Next() { return zj_Next(this); }
  inline Value *Copy() { return zj_Copy(this); }
  inline bool Move() { return zj_Move(this); }
  inline void SetNull() { zj_SetNull(this); }
  inline void SetBool(bool b) { zj_SetBool(this, b); }
  inline bool SetNumStrFast(const char *num) {
    return zj_SetNumStrFast(this, num);
  }
  inline bool SetNumStrLenFast(const char *num, Size len) {
    return zj_SetNumStrLenFast(this, num, len);
  }
  inline bool SetNumStr(const char *num) { return zj_SetNumStr(this, num); }
  inline bool SetNumStrLen(const char *num, Size len) {
    return zj_SetNumStrLen(this, num, len);
  }
  inline bool SetNum(const double d) { return zj_SetNum(this, d); }
  inline bool SetDouble(const double d) { return zj_SetDouble(this, d); }
  inline bool SetInt(const int d) { return zj_SetInt(this, d); }
  inline bool SetLong(const long d) { return zj_SetLong(this, d); }
  inline bool SetLongLong(const long long d) { return zj_SetLongLong(this, d); }
  inline bool SetStrFast(const char *str) { return zj_SetStrFast(this, str); }
  inline bool SetStrLenFast(const char *str, Size len) {
    return zj_SetStrLenFast(this, str, len);
  }
  inline bool SetStr(const char *str) { return zj_SetStr(this, str); }
  inline bool SetStrLen(const char *str, Size len) {
    return zj_SetStrLen(this, str, len);
  }
  inline bool SetStrEscape(const char *str) {
    return zj_SetStrEscape(this, str);
  }
  inline bool SetStrLenEscape(const char *str, Size len) {
    return zj_SetStrLenEscape(this, str, len);
  }
  inline bool SetKeyFast(const char *key) { return zj_SetKeyFast(this, key); }
  inline bool SetKeyLenFast(const char *key, Size len) {
    return zj_SetKeyLenFast(this, key, len);
  }
  inline bool SetKey(const char *key) { return zj_SetKey(this, key); }
  inline bool SetKeyLen(const char *key, Size len) {
    return zj_SetKeyLen(this, key, len);
  }
  inline bool SetKeyEscape(const char *key) {
    return zj_SetKeyEscape(this, key);
  }
  inline bool SetKeyLenEscape(const char *key, Size len) {
    return zj_SetKeyLenEscape(this, key, len);
  }
  inline void SetArray() { zj_SetArray(this); }
  inline void SetObj() { zj_SetObj(this); }
  inline bool SetFast(Value *vv) { return zj_SetFast(this, vv); }
  inline bool Set(const Value *vv) { return zj_Set(this, vv); }
  inline bool ObjAddFast(Value *vv) { return zj_ObjAddFast(this, vv); }
  inline bool ObjAdd(const Value *vv) { return zj_ObjAdd(this, vv); }
  inline bool ArrayAddFast(Value *vv) { return zj_ArrayAddFast(this, vv); }
  inline bool ArrayAdd(const Value *vv) { return zj_ArrayAdd(this, vv); }
  inline bool ArrayDel(Size index) { return zj_ArrayDel(this, index); }
  inline bool ObjDel(const char *key) { return zj_ObjDel(this, key); }
#endif
  struct zj_node *N;
  zj_Allocator *A;
};

// 函数说明详见《API》
static inline zj_Value *zj_NewValue(zj_Allocator *alloc) {
  zj_Value *v = (zj_Value *)zj_allocatorAlloc(alloc, sizeof(zj_Value));
  v->A = alloc;
  v->N = 0;
  return v;
}

// 创建一个值，并赋值
static inline zj_Value *zj_innerNewValue(zj_Allocator *alloc,
                                         struct zj_node *n) {
  zj_Value *v = (zj_Value *)zj_allocatorAlloc(alloc, sizeof(zj_Value));
  v->A = alloc;
  v->N = n;
  return v;
}

// 跳过空格、tab、换行符
static inline bool zj_skin(const char c) {
  if (zj_UNLIKELY(zj_UNLIKELY(c == ' ') || zj_UNLIKELY(c == '\t') ||
                  zj_UNLIKELY(c == '\n') || zj_UNLIKELY(c == '\r'))) {
    return true;
  }
  return false;
}

// 下一个有效字符
static inline char zj_peek(const char *s, zj_Size *index) {
  while (zj_UNLIKELY(zj_skin(s[*index])))
    ++(*index);
  return s[(*index)++];
}

// 消费一个字符
static inline bool zj_consume(const char c, const char *s, zj_Size *index) {
  if (s[*index] == c) {
    ++(*index);
    return true;
  }
  return false;
}

// 预期消费一个字符成功
static inline bool zj_likelyConsume(const char c, const char *s,
                                    zj_Size *index) {
  if (zj_LIKELY(s[*index] == c)) {
    ++(*index);
    return true;
  }
  return false;
}

// 预期消费一个字符失败
static inline bool zj_unLikelyConsume(const char c, const char *s,
                                      zj_Size *index) {
  if (zj_UNLIKELY(s[*index] == c)) {
    ++(*index);
    return true;
  }
  return false;
}

// 预期消费下一个有效字符成功
static inline bool zj_likelyPeekAndConsume(const char c, const char *s,
                                           zj_Size *index) {
  while (zj_UNLIKELY(zj_skin(s[*index])))
    ++(*index);
  if (zj_LIKELY(s[*index] == c)) {
    ++(*index);
    return true;
  }
  return false;
}

// 预期消费下一个有效字符失败
static inline bool zj_unLikelyPeekAndConsume(const char c, const char *s,
                                             zj_Size *index) {
  while (zj_UNLIKELY(zj_skin(s[*index])))
    ++(*index);
  if (zj_UNLIKELY(s[*index] == c)) {
    ++(*index);
    return true;
  }
  return false;
}

// 消费False
static inline bool zj_consumeFalse(const char *s, zj_Size *index) {
  if (zj_LIKELY(*((uint32_t *)("alse")) == *((uint32_t *)(s + *index)))) {
    *index += 4;
    return true;
  }
  return false;
}

// 消费True
static inline bool zj_consumeTrue(const char *s, zj_Size *index) {
  if (zj_LIKELY(*((uint32_t *)zj_strTrue) == *((uint32_t *)(s + *index - 1)))) {
    *index += 3;
    return true;
  }
  return false;
}

// 消费Null
static inline bool zj_consumeNull(const char *s, zj_Size *index) {
  if (zj_LIKELY(*((uint32_t *)zj_strNull) == *((uint32_t *)(s + *index - 1)))) {
    *index += 3;
    return true;
  }
  return false;
}

// 十六进制对应的十进制数字
static inline zj_Size zj_hexCodePoint(const char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  else if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  else if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  return 16;
}

// 专为 zj_GetUnEscapeStr 使用
static inline zj_Size zj_hexCodePointForUnEscape(const char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  else if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return c - 'a' + 10;
}

// 消费一个十六进制字符
static inline bool zj_consumeHexOne(const char *s, zj_Size *index,
                                    zj_Size *cp) {
  zj_Size tcp = zj_hexCodePoint(s[*index]);
  if (zj_LIKELY(tcp < 16)) {
    *cp = *cp << 4;
    *cp += tcp;
    ++(*index);
    return true;
  }
  return false;
}

// 专为 zj_GetUnEscapeStr 使用
static inline void zj_consumeHexOneForUnEscape(const char *s, zj_Size *index,
                                               zj_Size *cp) {
  *cp = *cp << 4;
  *cp += zj_hexCodePointForUnEscape(s[*index]);
  ++(*index);
  return;
}

// 消费4个十六进制字符
static inline bool zj_consumeHex(const char *s, zj_Size *index, zj_Size *cp) {
  if (zj_LIKELY(zj_LIKELY(zj_consumeHexOne(s, index, cp)) &&
                zj_LIKELY(zj_consumeHexOne(s, index, cp)) &&
                zj_LIKELY(zj_consumeHexOne(s, index, cp)) &&
                zj_LIKELY(zj_consumeHexOne(s, index, cp)))) {
    return true;
  }
  return false;
}

// 专为 zj_GetUnEscapeStr 使用
static inline void zj_consumeHexForUnEscape(const char *s, zj_Size *index,
                                            zj_Size *cp) {
  zj_consumeHexOneForUnEscape(s, index, cp);
  zj_consumeHexOneForUnEscape(s, index, cp);
  zj_consumeHexOneForUnEscape(s, index, cp);
  zj_consumeHexOneForUnEscape(s, index, cp);
  return;
}

// 专为 zj_GetUnEscapeStr 使用，追加一个字符
static inline void zj_add(char *s, zj_Size *index, char c) {
  s[(*index)++] = c;
}

static inline void zj_addLen(char *s, zj_Size *index, const char *str,
                             zj_Size len) {
  zj_memCopy(str, len, s + (*index));
  *index += len;
}

// 专为 zj_GetUnEscapeStr 使用，追加一个UTF8字符
static inline void zj_addUTF8(char *s, zj_Size *index, zj_Size codepoint) {
  // UTF8的规则，具体请参考 UNICODE 相关文档
  if (codepoint <= 0x7F) {
    zj_add(s, index, (char)(codepoint & 0xFF));
  } else if (codepoint <= 0x7FF) {
    zj_add(s, index, (char)(0xC0 | ((codepoint >> 6) & 0xFF)));
    zj_add(s, index, (char)(0x80 | ((codepoint & 0x3F))));
  } else if (codepoint <= 0xFFFF) {
    zj_add(s, index, (char)(0xE0 | ((codepoint >> 12) & 0xFF)));
    zj_add(s, index, (char)(0x80 | ((codepoint >> 6) & 0x3F)));
    zj_add(s, index, (char)(0x80 | (codepoint & 0x3F)));
  } else {
    zj_add(s, index, (char)(0xF0 | ((codepoint >> 18) & 0xFF)));
    zj_add(s, index, (char)(0x80 | ((codepoint >> 12) & 0x3F)));
    zj_add(s, index, (char)(0x80 | ((codepoint >> 6) & 0x3F)));
    zj_add(s, index, (char)(0x80 | (codepoint & 0x3F)));
  }
}

// 专为 zj_GetUnEscapeStr 使用，加一个结束符号
static inline void zj_addEnd(char *s, zj_Size *index) { zj_add(s, index, 0); }

static inline void zj_unEscapeStr(const char *str, zj_Size len, char *s) {
  zj_Size s_index = 0;
  zj_Size index;
  char c;
  for (index = 0; index < len;) {
    c = str[index];
    // 如果是一个合法的JSON字符串，那么\后面一定有字符，因此，一定不会越界
    if (zj_UNLIKELY(c == '\\')) {
      c = str[index + 1];
      switch (c) {
      case '"': {
        zj_add(s, &s_index, '\"');
        index += 2;
        break;
      }
      case '\\': {
        zj_add(s, &s_index, '\\');
        index += 2;
        break;
      }
      case 'b': {
        zj_add(s, &s_index, '\b');
        index += 2;
        break;
      }
      case 'f': {
        zj_add(s, &s_index, '\f');
        index += 2;
        break;
      }
      case 'n': {
        zj_add(s, &s_index, '\n');
        index += 2;
        break;
      }
      case 'r': {
        zj_add(s, &s_index, '\r');
        index += 2;
        break;
      }
      case 't': {
        zj_add(s, &s_index, '\t');
        index += 2;
        break;
      }
      case '/': {
        zj_add(s, &s_index, '/');
        index += 2;
        break;
      }
      case 'u': {
        index += 2;
        zj_Size cp = 0;
        zj_consumeHexForUnEscape(str, &index, &cp);

        if (zj_UNLIKELY(cp >= 0xD800 && cp <= 0xDBFF)) {
          zj_Size cp1 = 0;
          index += 2;
          zj_consumeHexForUnEscape(str, &index, &cp1);
          cp = (((cp - 0xD800) << 10) | (cp1 - 0xDC00)) + 0x10000;
        }
        zj_addUTF8(s, &s_index, cp);
        break;
      }
      }
    } else {
      zj_add(s, &s_index, c);
      index += 1;
    }
  }
  zj_addEnd(s, &s_index);
  return;
}

struct zj_escapeChar {
  const char *Str;
  zj_Size Len;
};

static const struct zj_escapeChar zj_escapeChars[256] = {
    {"\\u0000", 6}, {"\\u0001", 6}, {"\\u0002", 6}, {"\\u0003", 6},
    {"\\u0004", 6}, {"\\u0005", 6}, {"\\u0006", 6}, {"\\u0007", 6},

    {"\\b", 2},     {"\\t", 2},     {"\\n", 2},     {"\\u000b", 6},
    {"\\f", 2},     {"\\r", 2},     {"\\u000e", 6}, {"\\u000f", 6},

    {"\\u0010", 6}, {"\\u0011", 6}, {"\\u0012", 6}, {"\\u0013", 6},
    {"\\u0014", 6}, {"\\u0015", 6}, {"\\u0016", 6}, {"\\u0017", 6},
    {"\\u0018", 6}, {"\\u0019", 6}, {"\\u001a", 6}, {"\\u001b", 6},
    {"\\u001c", 6}, {"\\u001d", 6}, {"\\u001e", 6}, {"\\u001f", 6},

    {"\x20", 1},    {"\x21", 1},    {"\\\"", 2},    {"\x23", 1},
    {"\x24", 1},    {"\x25", 1},    {"\x26", 1},    {"\x27", 1},

    {"\x28", 1},    {"\x29", 1},    {"\x2a", 1},    {"\x2b", 1},
    {"\x2c", 1},    {"\x2d", 1},    {"\x2e", 1},    {"\x2f", 1},
    {"\x30", 1},    {"\x31", 1},    {"\x32", 1},    {"\x33", 1},
    {"\x34", 1},    {"\x35", 1},    {"\x36", 1},    {"\x37", 1},
    {"\x38", 1},    {"\x39", 1},    {"\x3a", 1},    {"\x3b", 1},
    {"\x3c", 1},    {"\x3d", 1},    {"\x3e", 1},    {"\x3f", 1},
    {"\x40", 1},    {"\x41", 1},    {"\x42", 1},    {"\x43", 1},
    {"\x44", 1},    {"\x45", 1},    {"\x46", 1},    {"\x47", 1},
    {"\x48", 1},    {"\x49", 1},    {"\x4a", 1},    {"\x4b", 1},
    {"\x4c", 1},    {"\x4d", 1},    {"\x4e", 1},    {"\x4f", 1},
    {"\x50", 1},    {"\x51", 1},    {"\x52", 1},    {"\x53", 1},
    {"\x54", 1},    {"\x55", 1},    {"\x56", 1},    {"\x57", 1},

    {"\x58", 1},    {"\x59", 1},    {"\x5a", 1},    {"\x5b", 1},
    {"\\\\", 2},    {"\x5d", 1},    {"\x5e", 1},    {"\x5f", 1},

    {"\x60", 1},    {"\x61", 1},    {"\x62", 1},    {"\x63", 1},
    {"\x64", 1},    {"\x65", 1},    {"\x66", 1},    {"\x67", 1},
    {"\x68", 1},    {"\x69", 1},    {"\x6a", 1},    {"\x6b", 1},
    {"\x6c", 1},    {"\x6d", 1},    {"\x6e", 1},    {"\x6f", 1},
    {"\x70", 1},    {"\x71", 1},    {"\x72", 1},    {"\x73", 1},
    {"\x74", 1},    {"\x75", 1},    {"\x76", 1},    {"\x77", 1},
    {"\x78", 1},    {"\x79", 1},    {"\x7a", 1},    {"\x7b", 1},
    {"\x7c", 1},    {"\x7d", 1},    {"\x7e", 1},    {"\x7f", 1},
    {"\x80", 1},    {"\x81", 1},    {"\x82", 1},    {"\x83", 1},
    {"\x84", 1},    {"\x85", 1},    {"\x86", 1},    {"\x87", 1},
    {"\x88", 1},    {"\x89", 1},    {"\x8a", 1},    {"\x8b", 1},
    {"\x8c", 1},    {"\x8d", 1},    {"\x8e", 1},    {"\x8f", 1},
    {"\x90", 1},    {"\x91", 1},    {"\x92", 1},    {"\x93", 1},
    {"\x94", 1},    {"\x95", 1},    {"\x96", 1},    {"\x97", 1},
    {"\x98", 1},    {"\x99", 1},    {"\x9a", 1},    {"\x9b", 1},
    {"\x9c", 1},    {"\x9d", 1},    {"\x9e", 1},    {"\x9f", 1},
    {"\xa0", 1},    {"\xa1", 1},    {"\xa2", 1},    {"\xa3", 1},
    {"\xa4", 1},    {"\xa5", 1},    {"\xa6", 1},    {"\xa7", 1},
    {"\xa8", 1},    {"\xa9", 1},    {"\xaa", 1},    {"\xab", 1},
    {"\xac", 1},    {"\xad", 1},    {"\xae", 1},    {"\xaf", 1},
    {"\xb0", 1},    {"\xb1", 1},    {"\xb2", 1},    {"\xb3", 1},
    {"\xb4", 1},    {"\xb5", 1},    {"\xb6", 1},    {"\xb7", 1},
    {"\xb8", 1},    {"\xb9", 1},    {"\xba", 1},    {"\xbb", 1},
    {"\xbc", 1},    {"\xbd", 1},    {"\xbe", 1},    {"\xbf", 1},
    {"\xc0", 1},    {"\xc1", 1},    {"\xc2", 1},    {"\xc3", 1},
    {"\xc4", 1},    {"\xc5", 1},    {"\xc6", 1},    {"\xc7", 1},
    {"\xc8", 1},    {"\xc9", 1},    {"\xca", 1},    {"\xcb", 1},
    {"\xcc", 1},    {"\xcd", 1},    {"\xce", 1},    {"\xcf", 1},
    {"\xd0", 1},    {"\xd1", 1},    {"\xd2", 1},    {"\xd3", 1},
    {"\xd4", 1},    {"\xd5", 1},    {"\xd6", 1},    {"\xd7", 1},
    {"\xd8", 1},    {"\xd9", 1},    {"\xda", 1},    {"\xdb", 1},
    {"\xdc", 1},    {"\xdd", 1},    {"\xde", 1},    {"\xdf", 1},
    {"\xe0", 1},    {"\xe1", 1},    {"\xe2", 1},    {"\xe3", 1},
    {"\xe4", 1},    {"\xe5", 1},    {"\xe6", 1},    {"\xe7", 1},
    {"\xe8", 1},    {"\xe9", 1},    {"\xea", 1},    {"\xeb", 1},
    {"\xec", 1},    {"\xed", 1},    {"\xee", 1},    {"\xef", 1},
    {"\xf0", 1},    {"\xf1", 1},    {"\xf2", 1},    {"\xf3", 1},
    {"\xf4", 1},    {"\xf5", 1},    {"\xf6", 1},    {"\xf7", 1},
    {"\xf8", 1},    {"\xf9", 1},    {"\xfa", 1},    {"\xfb", 1},
    {"\xfc", 1},    {"\xfd", 1},    {"\xfe", 1},    {"\xff", 1}};

// str必定不为空，因此，一定会返回一个非空字符串
static inline const char *zj_escapeStr(const char *str, zj_Allocator *a) {
  zj_Size len = 0;
  const char *src = str;
  while (zj_LIKELY(*str != 0)) {
    len += zj_escapeChars[(unsigned char)(*str)].Len;
    ++str;
  }
  char *s = zj_allocatorAlloc(a, len + 1);
  zj_Size index = 0;
  str = src;
  while (zj_LIKELY(*str != 0)) {
    zj_addLen(s, &index, zj_escapeChars[(unsigned char)(*str)].Str,
              zj_escapeChars[(unsigned char)(*str)].Len);
    ++str;
  }
  zj_addEnd(s, &index);
  return s;
}

// str必定不为空，因此，一定会返回一个非空字符串
static inline const char *zj_escapeStrLen(const char *str, zj_Allocator *a,
                                          zj_Size len) {
  zj_Size l = 0;
  const char *src = str;
  zj_Size srcLen = len;
  while (zj_LIKELY(len != 0)) {
    l += zj_escapeChars[(unsigned char)(*str)].Len;
    ++str;
    --len;
  }
  char *s = zj_allocatorAlloc(a, l + 1);
  zj_Size index = 0;
  str = src;
  len = srcLen;
  while (zj_LIKELY(len != 0)) {
    zj_addLen(s, &index, zj_escapeChars[(unsigned char)(*str)].Str,
              zj_escapeChars[(unsigned char)(*str)].Len);
    ++str;
    --len;
  }
  zj_addEnd(s, &index);
  return s;
}

// 消费字符串
static inline bool zj_consumeStr(const char *s, zj_Size *index) {
  char c;
  c = s[(*index)++];
  while (zj_LIKELY(c != 0)) {
    if (zj_UNLIKELY((unsigned char)c <= 0x1f))
      return false;
    if (zj_UNLIKELY(c == '\\')) {
      c = s[(*index)++];
      switch (c) {
      case '"':
      case '\\':
      case 'b':
      case 'f':
      case 'n':
      case 'r':
      case 't':
      case '/':
        c = s[(*index)++];
        continue;
      case 'u': {
        zj_Size cp = 0;
        if (zj_LIKELY(zj_consumeHex(s, index, &cp))) {
          // 这里是UTF16标准，可以参考网上相关资料
          // 搜索关键字 UNICODE
          if (zj_UNLIKELY(cp >= 0xDC00 && cp <= 0xDFFF))
            return false;
          if (zj_UNLIKELY(cp >= 0xD800 && cp <= 0xDBFF)) {
            if (zj_LIKELY(zj_likelyConsume('\\', s, index) &&
                          zj_likelyConsume('u', s, index))) {
              zj_Size cp2 = 0;
              if (zj_LIKELY(zj_consumeHex(s, index, &cp2))) {
                if (zj_UNLIKELY(cp2 < 0xDC00 || cp2 > 0xDFFF))
                  return false;
              } else {
                return false;
              }
            } else {
              return false;
            }
          }
          c = s[(*index)++];
        } else {
          return false;
        }
        continue;
      }
      default:
        return false;
      }
    }
    if (zj_UNLIKELY(c == '"')) {
      return true;
    }
    c = s[(*index)++];
  }
  return false;
}

// 检查一个字符串是否符合JSON标准，主要用于 SetStr
static inline bool zj_checkStr(const char *s, zj_Size *len) {
  zj_Size index = 0;
  char c;
  c = s[index++];
  while (zj_LIKELY(c != 0)) {
    if (zj_UNLIKELY(zj_UNLIKELY((unsigned char)c <= 0x1f) ||
                    zj_UNLIKELY(c == '"'))) {
      return false;
    }
    if (zj_UNLIKELY(c == '\\')) {
      c = s[index++];
      switch (c) {
      case '"':
      case '\\':
      case 'b':
      case 'f':
      case 'n':
      case 'r':
      case 't':
      case '/':
        c = s[index++];
        continue;
      case 'u': {
        zj_Size cp = 0;
        if (zj_LIKELY(zj_consumeHex(s, &index, &cp))) {
          // 这里是UTF16标准，可以参考网上相关资料
          // 搜索关键字 UNICODE
          if (zj_UNLIKELY(cp >= 0xDC00 && cp <= 0xDFFFF))
            return false;
          if (zj_UNLIKELY(cp >= 0xD800 && cp <= 0xDBFF)) {
            if (zj_LIKELY(zj_likelyConsume('\\', s, &index) &&
                          zj_likelyConsume('u', s, &index))) {
              zj_Size cp2 = 0;
              if (zj_LIKELY(zj_consumeHex(s, &index, &cp2))) {
                if (zj_UNLIKELY(cp2 < 0xDC00 || cp2 > 0xDFFF))
                  return false;
              } else {
                return false;
              }
            } else {
              return false;
            }
          }
          c = s[index++];
        } else {
          return false;
        }
        continue;
      }
      default:
        return false;
      }
    }
    c = s[index++];
  }
  *len = index - 1;
  return true;
}
// 较为省事的一种实现方法，后面会实现更高效的算法
static inline bool zj_checkStrLen(zj_Allocator *alloc, const char *s,
                                  zj_Size len) {
  if (zj_UNLIKELY(zj_stringCache == 0)) {
    zj_stringCache = zj_newString(alloc, zj_StringCacheInitMemSize);
  } else {
    zj_reset(zj_stringCache);
  }
  zj_appendStr(zj_stringCache, s, len);
  zj_appendEnd(zj_stringCache);
  zj_Size avail_len;
  if (zj_UNLIKELY(zj_checkStr(zj_str(zj_stringCache), &avail_len) == false)) {
    return false;
  }
  if (zj_UNLIKELY(avail_len != len))
    return false;
  return true;
}

// 消费一个数字
static inline bool zj_consumeNum(const char *s, zj_Size *index) {
  --(*index);

  if (s[*index] == '-')
    ++(*index);

  if (zj_unLikelyConsume('0', s, index)) {
  } else if (zj_LIKELY(zj_LIKELY(s[*index] >= '1') &&
                       zj_LIKELY(s[*index] <= '9'))) {
    char c = s[++(*index)];
    while (zj_LIKELY(zj_LIKELY(c >= '0') && zj_LIKELY(c <= '9'))) {
      c = s[++(*index)];
    }
  } else {
    return false;
  }

  if (zj_consume('.', s, index)) {
    char c = s[*index];
    if ((zj_LIKELY(c >= '0') && zj_LIKELY(c <= '9'))) {
      c = s[++(*index)];
      while (zj_LIKELY(zj_LIKELY(c >= '0') && zj_LIKELY(c <= '9'))) {
        c = s[++(*index)];
      }
    } else {
      return false;
    }
  }

  if (s[*index] == 'e' || s[*index] == 'E') {
    char c = s[++(*index)];
    if (c == '-' || c == '+') {
      ++(*index);
    }
    c = s[*index];
    if (zj_LIKELY(zj_LIKELY(c >= '0') && zj_LIKELY(c <= '9'))) {
      c = s[++(*index)];
      while (zj_LIKELY(zj_LIKELY(c >= '0') && zj_LIKELY(c <= '9'))) {
        c = s[++(*index)];
      }
    } else {
      return false;
    }
  }

  return true;
}

// 检查一个数字是否符合JSON标准，主要用于 SetNum
static inline bool zj_checkNum(const char *s, zj_Size *len) {
  zj_Size index = 0;

  if (s[index] == '-')
    ++(index);

  if (zj_unLikelyConsume('0', s, &index)) {
  } else if (zj_LIKELY(zj_LIKELY(s[index] >= '1') &&
                       zj_LIKELY(s[index] <= '9'))) {
    char c = s[++index];
    while (zj_LIKELY(zj_LIKELY(c >= '0') && zj_LIKELY(c <= '9'))) {
      c = s[++index];
    }
  } else {
    return false;
  }

  if (zj_consume('.', s, &index)) {
    char c = s[index];
    if ((zj_LIKELY(c >= '0') && zj_LIKELY(c <= '9'))) {
      c = s[++index];
      while (zj_LIKELY(zj_LIKELY(c >= '0') && zj_LIKELY(c <= '9'))) {
        c = s[++index];
      }
    } else {
      return false;
    }
  }

  if (s[index] == 'e' || s[index] == 'E') {
    char c = s[++index];
    if (c == '-') {
      ++index;
    } else if (c == '+') {
      ++index;
    }
    c = s[index];
    if (zj_LIKELY(zj_LIKELY(c >= '0') && zj_LIKELY(c <= '9'))) {
      c = s[++index];
      while (zj_LIKELY(zj_LIKELY(c >= '0') && zj_LIKELY(c <= '9'))) {
        c = s[++index];
      }
    } else {
      return false;
    }
  }
  *len = index;
  return zj_likelyConsume(0, s, &index);
}
static inline bool zj_checkNumLen(zj_Allocator *alloc, const char *s,
                                  zj_Size len) {
  if (zj_UNLIKELY(zj_stringCache == 0)) {
    zj_stringCache = zj_newString(alloc, zj_StringCacheInitMemSize);
  } else {
    zj_reset(zj_stringCache);
  }
  zj_appendStr(zj_stringCache, s, len);
  zj_appendEnd(zj_stringCache);
  zj_Size avail_len;
  if (zj_UNLIKELY(zj_checkNum(zj_str(zj_stringCache), &avail_len) == false)) {
    return false;
  }
  if (zj_UNLIKELY(avail_len != len))
    return false;
  return true;
}

// 函数说明详见《API》
static inline bool zj_ParseFast(zj_Value *v, const char *s) {
  struct zj_node *src_node;
  if (zj_LIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Prev = 0;
    v->N->Next = 0;
    v->N->Father = 0;
    v->N->Key = 0;
    src_node = 0;
  } else {
    src_node =
        (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    *src_node = *v->N;
  }
  zj_Size index = 0;
  struct zj_node *node = v->N;

  char c = zj_peek(s, &index);
  switch (c) {
  case '[': {
    node->Type = zj_JSONTypeArray;
    if (zj_unLikelyPeekAndConsume(']', s, &index)) {
      node->Value.Node = 0;
      node->Len = 0;
      break;
    }
    struct zj_node *n =
        (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    n->Father = node;
    n->Prev = 0;
    // n->Next = 0;

    node->Value.Node = n;
    node->End = n;
    node->Len = 1;
    node = n;
    break;
  }
  case '{': {
    node->Type = zj_JSONTypeObject;
    if (zj_unLikelyPeekAndConsume('}', s, &index)) {
      node->Value.Node = 0;
      node->Len = 0;
      break;
    }
    struct zj_node *n =
        (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    n->Father = node;
    n->Prev = 0;
    // n->Next = 0;

    node->Value.Node = n;
    node->End = n;
    node->Len = 1;
    node = n;
    break;
  }
  case 'n':
    if (zj_LIKELY(zj_consumeNull(s, &index))) {
      node->Type = zj_JSONTypeNull;
      node->Value.Str = zj_strNull;
      node->Len = 4;
      break;
    }
    if (zj_LIKELY(src_node == 0))
      v->N = src_node;
    else
      *v->N = *src_node;
    return false;
  case 'f':
    if (zj_LIKELY(zj_consumeFalse(s, &index))) {
      node->Type = zj_JSONTypeBool;
      node->Value.Str = zj_strFalse;
      node->Len = 5;
      break;
    }
    if (zj_LIKELY(src_node == 0))
      v->N = src_node;
    else
      *v->N = *src_node;
    return false;
  case 't':
    if (zj_LIKELY(zj_consumeTrue(s, &index))) {
      node->Type = zj_JSONTypeBool;
      node->Value.Str = zj_strTrue;
      node->Len = 4;
      break;
    }
    if (zj_LIKELY(src_node == 0))
      v->N = src_node;
    else
      *v->N = *src_node;
    return false;
  case '"': {
    zj_Size start = index;
    if (zj_UNLIKELY(zj_unLikelyConsume('"', s, &index))) {
      node->Type = zj_JSONTypeString;
      node->Value.Str = s + index;
      node->Len = 0;
      break;
    }
    if (zj_LIKELY(zj_consumeStr(s, &index))) {
      node->Type = zj_JSONTypeString;
      node->Value.Str = s + start;
      node->Len = index - start - 1;
      break;
    }
    if (zj_LIKELY(src_node == 0))
      v->N = src_node;
    else
      *v->N = *src_node;
    return false;
  }
  default: {
    zj_Size start = index - 1;
    if (zj_LIKELY(zj_consumeNum(s, &index))) {
      node->Type = zj_JSONTypeNumber;
      node->Value.Str = s + start;
      node->Len = index - start;
      break;
    }
    if (zj_LIKELY(src_node == 0))
      v->N = src_node;
    else
      *v->N = *src_node;
    return false;
  }
  }
  while (zj_LIKELY(node != v->N)) {
    if (node->Father->Type == zj_JSONTypeObject) {
      if (zj_UNLIKELY(zj_likelyPeekAndConsume('"', s, &index) == false)) {
        if (zj_LIKELY(src_node == 0))
          v->N = src_node;
        else
          *v->N = *src_node;
        return false;
      }
      zj_Size start = index;
      if (zj_UNLIKELY(zj_unLikelyConsume('"', s, &index))) {
        node->Key = s + start;
        node->KeyLen = 0;
      } else {
        if (zj_UNLIKELY(zj_consumeStr(s, &index) == false)) {
          if (zj_LIKELY(src_node == 0))
            v->N = src_node;
          else
            *v->N = *src_node;
          return false;
        }
        node->Key = s + start;
        node->KeyLen = index - start - 1;
      }
      if (zj_UNLIKELY(zj_likelyPeekAndConsume(':', s, &index) == false)) {
        if (zj_LIKELY(src_node == 0))
          v->N = src_node;
        else
          *v->N = *src_node;
        return false;
      }
    } else {
      node->Key = 0;
    }
    c = zj_peek(s, &index);
    switch (c) {
    case '[': {
      node->Type = zj_JSONTypeArray;
      if (zj_unLikelyPeekAndConsume(']', s, &index)) {
        node->Value.Node = 0;
        node->Len = 0;
        break;
      }
      struct zj_node *n =
          (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
      n->Father = node;
      n->Prev = 0;
      // n->Next = 0;

      node->Value.Node = n;
      node->End = n;
      node->Len = 1;
      node = n;
      continue;
    }
    case '{': {
      node->Type = zj_JSONTypeObject;
      if (zj_unLikelyPeekAndConsume('}', s, &index)) {
        node->Value.Node = 0;
        node->Len = 0;
        break;
      }
      struct zj_node *n =
          (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
      n->Father = node;
      n->Prev = 0;
      // n->Next = 0;

      node->Value.Node = n;
      node->End = n;
      node->Len = 1;
      node = n;
      continue;
    }
    case 'n':
      if (zj_LIKELY(zj_consumeNull(s, &index))) {
        node->Type = zj_JSONTypeNull;
        node->Value.Str = zj_strNull;
        node->Len = 4;
        break;
      }
      if (zj_LIKELY(src_node == 0))
        v->N = src_node;
      else
        *v->N = *src_node;
      return false;
    case 'f':
      if (zj_LIKELY(zj_consumeFalse(s, &index))) {
        node->Type = zj_JSONTypeBool;
        node->Value.Str = zj_strFalse;
        node->Len = 5;
        break;
      }
      if (zj_LIKELY(src_node == 0))
        v->N = src_node;
      else
        *v->N = *src_node;
      return false;
    case 't':
      if (zj_LIKELY(zj_consumeTrue(s, &index))) {
        node->Type = zj_JSONTypeBool;
        node->Value.Str = zj_strTrue;
        node->Len = 4;
        break;
      }
      if (zj_LIKELY(src_node == 0))
        v->N = src_node;
      else
        *v->N = *src_node;
      return false;
    case '"': {
      zj_Size start = index;
      if (zj_UNLIKELY(zj_unLikelyConsume('"', s, &index))) {
        node->Type = zj_JSONTypeString;
        node->Value.Str = s + index;
        node->Len = 0;
        break;
      }
      if (zj_LIKELY(zj_consumeStr(s, &index))) {
        node->Type = zj_JSONTypeString;
        node->Value.Str = s + start;
        node->Len = index - start - 1;
        break;
      }
      if (zj_LIKELY(src_node == 0))
        v->N = src_node;
      else
        *v->N = *src_node;
      return false;
    }
    default: {
      zj_Size start = index - 1;
      if (zj_LIKELY(zj_consumeNum(s, &index))) {
        node->Type = zj_JSONTypeNumber;
        node->Value.Str = s + start;
        node->Len = index - start;
        break;
      }
      if (zj_LIKELY(src_node == 0))
        v->N = src_node;
      else
        *v->N = *src_node;
      return false;
    }
    }
    while (zj_LIKELY(node != v->N)) {
      if (zj_likelyPeekAndConsume(',', s, &index)) {
        struct zj_node *n =
            (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
        n->Father = node->Father;
        n->Prev = node;
        node->Father->End = n;
        ++(node->Father->Len);
        node->Next = n;
        node = n;
        break;
      } else {
        char c = zj_peek(s, &index);
        if (zj_LIKELY(
                (c == '}' &&
                 zj_LIKELY(node->Father->Type == zj_JSONTypeObject)) ||
                zj_LIKELY(zj_LIKELY(c == ']') &&
                          zj_LIKELY(node->Father->Type == zj_JSONTypeArray)))) {
          node->Next = 0;
          node = node->Father;
        } else {
          if (zj_LIKELY(src_node == 0))
            v->N = src_node;
          else
            *v->N = *src_node;
          return false;
        }
      }
    }
  }
  if (zj_LIKELY(zj_likelyPeekAndConsume(0, s, &index))) {
    return true;
  }
  if (zj_LIKELY(src_node == 0))
    v->N = src_node;
  else
    *v->N = *src_node;
  return false;
}

// 函数说明详见《API》
static inline bool zj_ParseLen(zj_Value *v, const char *s, zj_Size len) {
  char *str = zj_allocatorAlloc(v->A, len + 1);
  zj_memCopy(s, len, str);
  str[len] = 0;
  return zj_ParseFast(v, str);
}

// 函数说明详见《API》
static inline bool zj_Parse(zj_Value *v, const char *s) {
  return zj_ParseLen(v, s, zj_strLen(s));
}

// 函数说明详见《API》
static inline const char *zj_Stringify(const zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0))
    return "";
  struct zj_string *str = zj_newString(v->A, zj_StringInitMemSize);
  struct zj_node *node = v->N;

  switch (node->Type) {
  case zj_JSONTypeArray:
    zj_appendChar(str, '[');
    if (node->Value.Node != 0) {
      node = node->Value.Node;
    } else {
      zj_appendChar(str, ']');
    }
    break;
  case zj_JSONTypeObject:
    zj_appendChar(str, '{');
    if (node->Len != 0) {
      node = node->Value.Node;
    } else {
      zj_appendChar(str, '}');
    }
    break;
  case zj_JSONTypeString:
    zj_appendChar(str, '"');
    zj_appendStr(str, node->Value.Str, node->Len);
    zj_appendChar(str, '"');
    break;
  default:
    zj_appendStr(str, node->Value.Str, node->Len);
    break;
  }

  while (zj_LIKELY(node != v->N)) {
    if (node->Key != 0) {
      zj_appendChar(str, '"');
      zj_appendStr(str, node->Key, node->KeyLen);
      zj_appendStr(str, "\":", 2);
    }
    switch (node->Type) {
    case zj_JSONTypeArray:
      zj_appendChar(str, '[');
      if (node->Value.Node != 0) {
        node = node->Value.Node;
        continue;
      } else {
        zj_appendChar(str, ']');
      }
      break;
    case zj_JSONTypeObject:
      zj_appendChar(str, '{');
      if (node->Len != 0) {
        node = node->Value.Node;
        continue;
      } else {
        zj_appendChar(str, '}');
      }
      break;
    case zj_JSONTypeString:
      zj_appendChar(str, '"');
      zj_appendStr(str, node->Value.Str, node->Len);
      zj_appendChar(str, '"');
      break;
    default:
      zj_appendStr(str, node->Value.Str, node->Len);
      break;
    }
    while (zj_LIKELY(node != v->N)) {
      if (zj_LIKELY(node->Next != 0)) {
        zj_appendChar(str, ',');
        node = node->Next;
        break;
      } else {
        node = node->Father;
        if (node->Type == zj_JSONTypeArray) {
          zj_appendChar(str, ']');
        } else {
          zj_appendChar(str, '}');
        }
      }
    }
  }
  zj_appendEnd(str);
  return zj_str(str);
}

// 函数说明详见《API》
static inline const char *zj_GetStrFast(const zj_Value *v, zj_Size *len) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeString))
    return 0;
  *len = v->N->Len;
  return v->N->Value.Str;
}

// 函数说明详见《API》
static inline const char *zj_GetStr(zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeString))
    return 0;
  char *str = zj_allocatorAlloc(v->A, v->N->Len + 1);
  zj_memCopy(v->N->Value.Str, v->N->Len, str);
  str[v->N->Len] = 0;
  return str;
}

// 函数说明详见《API》
static inline const char *zj_GetUnEscapeStr(zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeString))
    return 0;
  // 因为值里面的字符串一定是合法的字符串，因此，可以简化
  // 因为解码后的字符串一定比解码前短，所以申请原字符串+1的长度即可
  char *ret_str = zj_allocatorAlloc(v->A, v->N->Len + 1);
  zj_unEscapeStr(v->N->Value.Str, v->N->Len, ret_str);
  return ret_str;
}

// 函数说明详见《API》
static inline const char *zj_GetNumFast(const zj_Value *v, zj_Size *len) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeNumber))
    return 0;
  *len = v->N->Len;
  return v->N->Value.Str;
}

// 函数说明详见《API》
static inline const char *zj_GetNumStr(zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeNumber))
    return 0;
  char *str = zj_allocatorAlloc(v->A, v->N->Len + 1);
  zj_memCopy(v->N->Value.Str, v->N->Len, str);
  str[v->N->Len] = 0;
  return str;
}

// 函数说明详见《API》
static inline const double *zj_GetNum(zj_Value *v) { return zj_GetDouble(v); }

static inline const double *zj_GetDouble(zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeNumber))
    return 0;
  double *d = (double *)zj_allocatorAlloc(v->A, sizeof(double));
  *d = zj_strToDouble(v->N->Value.Str, v->N->Len);
  return d;
}

static inline const int *zj_GetInt(zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeNumber))
    return 0;
  int *i = (int *)zj_allocatorAlloc(v->A, sizeof(int));
  *i = zj_strToInt(v->N->Value.Str, v->N->Len);
  return i;
}

static inline const long *zj_GetLong(zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeNumber))
    return 0;
  long *l = (long *)zj_allocatorAlloc(v->A, sizeof(long));
  *l = zj_strToLong(v->N->Value.Str, v->N->Len);
  return l;
}

static inline const long long *zj_GetLongLong(zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeNumber))
    return 0;
  long long *ll = (long long *)zj_allocatorAlloc(v->A, sizeof(long long));
  *ll = zj_strToLongLong(v->N->Value.Str, v->N->Len);
  return ll;
}

// 函数说明详见《API》
static inline const bool *zj_GetBool(const zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeBool))
    return 0;
  static bool t = true;
  static bool f = false;
  if (*(v->N->Value.Str) == 't')
    return &t;
  return &f;
}

// 函数说明详见《API》
static inline bool zj_IsNull(const zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0))
    return false;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeNull))
    return false;
  return true;
}

// 函数说明详见《API》
static inline const char *zj_GetKey(zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_UNLIKELY(v->N->Key == 0))
    return 0;
  char *str = zj_allocatorAlloc(v->A, v->N->KeyLen + 1);
  zj_memCopy(v->N->Key, v->N->KeyLen, str);
  str[v->N->KeyLen] = 0;
  return str;
}

static inline const char *zj_GetUnEscapeKey(zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_UNLIKELY(v->N->Key == 0))
    return 0;
  char *str = zj_allocatorAlloc(v->A, v->N->KeyLen + 1);
  zj_unEscapeStr(v->N->Key, v->N->KeyLen, str);
  return str;
}

// 函数说明详见《API》
static inline const char *zj_GetKeyFast(const zj_Value *v, zj_Size *len) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_UNLIKELY(v->N->Key == 0))
    return 0;
  *len = v->N->KeyLen;
  return v->N->Key;
}

// 函数说明详见《API》
static inline zj_Value *zj_ObjGet(const zj_Value *v, const char *key) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeObject))
    return 0;
  struct zj_node *next = v->N->Value.Node;
  while (zj_LIKELY(next != 0)) {
    if (zj_UNLIKELY(zj_strIsEqual(key, next->Key, next->KeyLen) == true)) {
      zj_Value *ret_val = zj_innerNewValue(v->A, next);
      return ret_val;
    }
    next = next->Next;
  }
  return 0;
}

// 函数说明详见《API》
static inline zj_Value *zj_ObjGetLen(const zj_Value *v, const char *key,
                                     zj_Size len) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeObject))
    return 0;
  struct zj_node *next = v->N->Value.Node;
  while (zj_LIKELY(next != 0)) {
    if (zj_UNLIKELY(zj_strIsEqualLen(key, len, next->Key, next->KeyLen) ==
                    true)) {
      zj_Value *ret_val = zj_innerNewValue(v->A, next);
      return ret_val;
    }
    next = next->Next;
  }
  return 0;
}

// 函数说明详见《API》
static inline const zj_JSONType *zj_Type(const zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  switch (v->N->Type) {
  case zj_JSONTypeArray:
    return &zj_jsontype_array;
  case zj_JSONTypeObject:
    return &zj_jsontype_object;
  case zj_JSONTypeString:
    return &zj_jsontype_string;
  case zj_JSONTypeNumber:
    return &zj_jsontype_number;
  case zj_JSONTypeBool:
    return &zj_jsontype_bool;
  case zj_JSONTypeNull:
    return &zj_jsontype_null;
  }
  return 0;
}

// 函数说明详见《API》
static inline zj_Size zj_SizeOf(const zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeObject &&
                  v->N->Type != zj_JSONTypeArray))
    return 0;
  return v->N->Len;
}

// 函数说明详见《API》
static inline zj_Value *zj_ArrayGet(const zj_Value *v, zj_Size index) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeArray))
    return 0;
  struct zj_node *next = v->N->Value.Node;
  zj_Size i = 0;
  while (zj_LIKELY(next != 0)) {
    if (zj_UNLIKELY(i == index)) {
      zj_Value *ret_val = zj_innerNewValue(v->A, next);
      return ret_val;
    }
    next = next->Next;
    ++i;
  }
  return 0;
}

// 函数说明详见《API》
static inline zj_Value *zj_Begin(const zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeObject &&
                  v->N->Type != zj_JSONTypeArray))
    return 0;
  if (zj_LIKELY(v->N->Value.Node != 0)) {
    zj_Value *ret_val = zj_innerNewValue(v->A, v->N->Value.Node);
    return ret_val;
  }
  return 0;
}

// 函数说明详见《API》
static inline zj_Value *zj_Next(const zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0))
    return 0;
  if (zj_LIKELY(v->N->Next != 0)) {
    zj_Value *ret_val = zj_innerNewValue(v->A, v->N->Next);
    return ret_val;
  }
  return 0;
}

// 把vv拷贝到v，非对外API
static inline bool zj_ValueCopyFrom(zj_Value *v, const zj_Value *vv) {
  if (zj_UNLIKELY(vv->N == 0))
    return false;
  zj_Allocator *const A = v->A;
  v->N = (struct zj_node *)zj_allocatorAlloc(A, sizeof(struct zj_node));
  v->N->Prev = 0;
  v->N->Next = 0;
  v->N->Father = 0;

  struct zj_node *node = vv->N;
  struct zj_node *des_node = v->N;

  do {
    des_node->Type = node->Type;
    if (node->Key != 0) {
      char *k = zj_allocatorAlloc(A, node->KeyLen);
      zj_memCopy(node->Key, node->KeyLen, k);
      des_node->Key = k;
      des_node->KeyLen = node->KeyLen;
    } else {
      des_node->Key = 0;
    }
    switch (node->Type) {
    case zj_JSONTypeObject:
    case zj_JSONTypeArray:
      des_node->Len = node->Len;
      if (zj_LIKELY(node->Value.Node != 0)) {
        node = node->Value.Node;
        struct zj_node *n =
            (struct zj_node *)zj_allocatorAlloc(A, sizeof(struct zj_node));
        n->Father = des_node;
        n->Prev = 0;
        des_node->Value.Node = n;
        des_node->End = n;
        des_node = n;
        continue;
      }
      des_node->Value.Node = 0;
      des_node->End = 0;
      break;
    case zj_JSONTypeBool:
    case zj_JSONTypeNull:
      des_node->Value.Str = node->Value.Str;
      des_node->Len = node->Len;
      break;
    case zj_JSONTypeNumber:
    case zj_JSONTypeString: {
      char *s = zj_allocatorAlloc(A, node->Len);
      zj_memCopy(node->Value.Str, node->Len, s);
      des_node->Value.Str = s;
      des_node->Len = node->Len;
    } break;
    }
    while (zj_LIKELY(node != vv->N)) {
      if (zj_LIKELY(node->Next != 0)) {
        node = node->Next;
        struct zj_node *n =
            (struct zj_node *)zj_allocatorAlloc(A, sizeof(struct zj_node));
        n->Father = des_node->Father;
        n->Prev = des_node;
        n->Father->End = n;
        des_node->Next = n;
        des_node = n;
        break;
      } else {
        node = node->Father;
        des_node->Next = 0;
        des_node = des_node->Father;
      }
    }
  } while (zj_LIKELY(node != vv->N));

  return true;
}

// 函数说明详见《API》
static inline zj_Value *zj_Copy(const zj_Value *v) {
  zj_Value *ret_val = zj_NewValue(v->A);
  if (zj_UNLIKELY(zj_ValueCopyFrom(ret_val, v) == false))
    return 0;
  return ret_val;
}

// 函数说明详见《API》
static inline bool zj_Move(zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0))
    return false;
  if (v->N->Father != 0) {
    if (v->N->Prev == 0) {
      v->N->Father->Value.Node = v->N->Next;
    } else {
      v->N->Prev->Next = v->N->Next;
      v->N->Prev = 0;
    }
    if (v->N->Next == 0) {
      v->N->Father->End = v->N->Prev;
    } else {
      v->N->Next->Prev = v->N->Prev;
      v->N->Next = 0;
    }
    --(v->N->Father->Len);
    v->N->Father = 0;
  }
  return true;
}

// 函数说明详见《API》
static inline void zj_SetNull(zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Key = 0;
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
  }
  v->N->Type = zj_JSONTypeNull;
  v->N->Value.Str = zj_strNull;
  v->N->Len = 4;
}

// 函数说明详见《API》
static inline void zj_SetBool(zj_Value *v, bool b) {
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Key = 0;
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
  }
  v->N->Type = zj_JSONTypeBool;
  if (b == false) {
    v->N->Value.Str = zj_strFalse;
    v->N->Len = 5;
    return;
  }
  v->N->Value.Str = zj_strTrue;
  v->N->Len = 4;
}

// 函数说明详见《API》
static inline bool zj_SetNumStrFast(zj_Value *v, const char *num) {
  zj_Size len = 0;
  if (zj_UNLIKELY(zj_checkNum(num, &len) == false))
    return false;
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Key = 0;
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
  }
  v->N->Type = zj_JSONTypeNumber;
  v->N->Value.Str = num;
  v->N->Len = len;
  return true;
}

// 函数说明详见《API》
static inline bool zj_SetNumStrLenFast(zj_Value *v, const char *num,
                                       zj_Size len) {
  if (zj_UNLIKELY(zj_checkNumLen(v->A, num, len) == false))
    return false;
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Key = 0;
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
  }
  v->N->Type = zj_JSONTypeNumber;
  v->N->Value.Str = num;
  v->N->Len = len;
  return true;
}

// 函数说明详见《API》
static inline bool zj_SetNumStr(zj_Value *v, const char *num) {
  zj_Size len = 0;
  if (zj_UNLIKELY(zj_checkNum(num, &len) == false))
    return false;
  char *s = zj_allocatorAlloc(v->A, len);
  zj_memCopy(num, len, s);
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Key = 0;
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
  }
  v->N->Type = zj_JSONTypeNumber;
  v->N->Value.Str = s;
  v->N->Len = len;
  return true;
}

// 函数说明详见《API》
static inline bool zj_SetNumStrLen(zj_Value *v, const char *num, zj_Size len) {
  if (zj_UNLIKELY(zj_checkNumLen(v->A, num, len) == false))
    return false;
  char *s = zj_allocatorAlloc(v->A, len);
  zj_memCopy(num, len, s);
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Key = 0;
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
  }
  v->N->Type = zj_JSONTypeNumber;
  v->N->Value.Str = s;
  v->N->Len = len;
  return true;
}

// 函数说明详见《API》
static inline bool zj_SetNum(zj_Value *v, const double d) {
  return zj_SetDouble(v, d);
}

static inline bool zj_SetDouble(zj_Value *v, const double d) {
  char *num = zj_allocatorAlloc(v->A, 32);
  zj_Size len = zj_doubleToStr(d, num);
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Key = 0;
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
  }
  v->N->Type = zj_JSONTypeNumber;
  v->N->Value.Str = num;
  v->N->Len = len;
  return true;
}

static inline bool zj_SetInt(zj_Value *v, const int n) {
  char *num = zj_allocatorAlloc(v->A, 16);
  zj_Size len = zj_intToStr(n, num);
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Key = 0;
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
  }
  v->N->Type = zj_JSONTypeNumber;
  v->N->Value.Str = num;
  v->N->Len = len;
  return true;
}

static inline bool zj_SetLong(zj_Value *v, const long n) {
  char *num = zj_allocatorAlloc(v->A, 24);
  zj_Size len = zj_longToStr(n, num);
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Key = 0;
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
  }
  v->N->Type = zj_JSONTypeNumber;
  v->N->Value.Str = num;
  v->N->Len = len;
  return true;
}

static inline bool zj_SetLongLong(zj_Value *v, const long long n) {
  char *num = zj_allocatorAlloc(v->A, 24);
  zj_Size len = zj_longLongToStr(n, num);
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Key = 0;
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
  }
  v->N->Type = zj_JSONTypeNumber;
  v->N->Value.Str = num;
  v->N->Len = len;
  return true;
}

static inline bool zj_SetStrEscape(zj_Value *v, const char *str) {
  const char *es = zj_escapeStr(str, v->A);
  return zj_SetStrFast(v, es);
}

static inline bool zj_SetStrLenEscape(zj_Value *v, const char *str,
                                      zj_Size len) {
  const char *es = zj_escapeStrLen(str, v->A, len);
  return zj_SetStrFast(v, es);
}

// 函数说明详见《API》
static inline bool zj_SetStrFast(zj_Value *v, const char *str) {
  zj_Size len = 0;
  if (zj_UNLIKELY(zj_checkStr(str, &len) == false)) {
    return false;
  }
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Key = 0;
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
  }
  v->N->Type = zj_JSONTypeString;
  v->N->Value.Str = str;
  v->N->Len = len;
  return true;
}

// 函数说明详见《API》
static inline bool zj_SetStrLenFast(zj_Value *v, const char *str, zj_Size len) {
  if (zj_UNLIKELY(zj_checkStrLen(v->A, str, len) == false)) {
    return false;
  }
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Key = 0;
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
  }
  v->N->Type = zj_JSONTypeString;
  v->N->Value.Str = str;
  v->N->Len = len;
  return true;
}

// 函数说明详见《API》
static inline bool zj_SetStr(zj_Value *v, const char *str) {
  zj_Size len = 0;
  if (zj_UNLIKELY(zj_checkStr(str, &len) == false)) {
    return false;
  }
  char *s = zj_allocatorAlloc(v->A, len);
  zj_memCopy(str, len, s);
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Key = 0;
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
  }
  v->N->Type = zj_JSONTypeString;
  v->N->Value.Str = s;
  v->N->Len = len;
  return true;
}

// 函数说明详见《API》
static inline bool zj_SetStrLen(zj_Value *v, const char *str, zj_Size len) {
  if (zj_UNLIKELY(zj_checkStrLen(v->A, str, len) == false)) {
    return false;
  }
  char *s = zj_allocatorAlloc(v->A, len);
  zj_memCopy(str, len, s);
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Key = 0;
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
  }
  v->N->Type = zj_JSONTypeString;
  v->N->Value.Str = s;
  v->N->Len = len;
  return true;
}

static inline bool zj_SetKeyEscape(zj_Value *v, const char *key) {
  const char *es = zj_escapeStr(key, v->A);
  return zj_SetKeyFast(v, es);
}

static inline bool zj_SetKeyLenEscape(zj_Value *v, const char *key,
                                      zj_Size len) {
  const char *es = zj_escapeStrLen(key, v->A, len);
  return zj_SetKeyFast(v, es);
}

// 函数说明详见《API》
static inline bool zj_SetKeyFast(zj_Value *v, const char *key) {
  zj_Size len = 0;
  if (zj_UNLIKELY(zj_checkStr(key, &len) == false)) {
    return false;
  }
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
    v->N->Type = zj_JSONTypeNull;
    v->N->Value.Str = zj_strNull;
    v->N->Len = 4;
  } else if (v->N->Father != 0 &&
             zj_UNLIKELY(v->N->Father->Type != zj_JSONTypeObject)) {
    return false;
  }
  v->N->Key = key;
  v->N->KeyLen = len;
  return true;
}

// 函数说明详见《API》
static inline bool zj_SetKeyLenFast(zj_Value *v, const char *key, zj_Size len) {
  if (zj_UNLIKELY(zj_checkStrLen(v->A, key, len) == false)) {
    return false;
  }
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
    v->N->Type = zj_JSONTypeNull;
    v->N->Value.Str = zj_strNull;
    v->N->Len = 4;
  } else if (v->N->Father != 0 &&
             zj_UNLIKELY(v->N->Father->Type != zj_JSONTypeObject)) {
    return false;
  }
  v->N->Key = key;
  v->N->KeyLen = len;
  return true;
}

// 函数说明详见《API》
static inline bool zj_SetKey(zj_Value *v, const char *key) {
  zj_Size len = 0;
  if (zj_UNLIKELY(zj_checkStr(key, &len) == false)) {
    return false;
  }
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
    v->N->Type = zj_JSONTypeNull;
    v->N->Value.Str = zj_strNull;
    v->N->Len = 4;
  } else if (v->N->Father != 0 &&
             zj_UNLIKELY(v->N->Father->Type != zj_JSONTypeObject)) {
    return false;
  }
  char *s = zj_allocatorAlloc(v->A, len);
  zj_memCopy(key, len, s);
  v->N->Key = s;
  v->N->KeyLen = len;
  return true;
}

// 函数说明详见《API》
static inline bool zj_SetKeyLen(zj_Value *v, const char *key, zj_Size len) {
  if (zj_UNLIKELY(zj_checkStrLen(v->A, key, len) == false)) {
    return false;
  }
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
    v->N->Type = zj_JSONTypeNull;
    v->N->Value.Str = zj_strNull;
    v->N->Len = 4;
  } else if (v->N->Father != 0 &&
             zj_UNLIKELY(v->N->Father->Type != zj_JSONTypeObject)) {
    return false;
  }
  char *s = zj_allocatorAlloc(v->A, len);
  zj_memCopy(key, len, s);
  v->N->Key = s;
  v->N->KeyLen = len;
  return true;
}

// 函数说明详见《API》
static inline void zj_SetArray(zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Key = 0;
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
  }
  v->N->Type = zj_JSONTypeArray;
  v->N->Value.Node = 0;
  v->N->Len = 0;
  return;
}

// 函数说明详见《API》
static inline void zj_SetObj(zj_Value *v) {
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = (struct zj_node *)zj_allocatorAlloc(v->A, sizeof(struct zj_node));
    v->N->Key = 0;
    v->N->Prev = 0;
    v->N->Father = 0;
    v->N->Next = 0;
  }
  v->N->Type = zj_JSONTypeObject;
  v->N->Value.Node = 0;
  v->N->Len = 0;
  return;
}

// 函数说明详见《API》
static inline bool zj_SetFast(zj_Value *v, zj_Value *vv) {
  if (zj_UNLIKELY(zj_Move(vv) == false))
    return false;
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = vv->N;
    vv->N = 0;
    return true;
  }
  v->N->Type = vv->N->Type;
  v->N->Value = vv->N->Value;
  v->N->Len = vv->N->Len;
  if (v->N->Type == zj_JSONTypeArray || v->N->Type == zj_JSONTypeObject) {
    v->N->End = vv->N->End;
    struct zj_node *next = v->N->Value.Node;
    while (zj_LIKELY(next != 0)) {
      next->Father = v->N;
      next = next->Next;
    }
  }
  vv->N = 0;
  return true;
}

// 函数说明详见《API》
static inline bool zj_Set(zj_Value *v, const zj_Value *vv) {
  zj_Value *cp = zj_Copy(vv);
  if (zj_UNLIKELY(cp == 0))
    return false;
  if (zj_UNLIKELY(v->N == 0)) {
    v->N = cp->N;
    return true;
  }
  v->N->Type = cp->N->Type;
  v->N->Value = cp->N->Value;
  v->N->Len = cp->N->Len;
  if (v->N->Type == zj_JSONTypeArray || v->N->Type == zj_JSONTypeObject) {
    v->N->End = vv->N->End;
    struct zj_node *next = v->N->Value.Node;
    while (zj_LIKELY(next != 0)) {
      next->Father = v->N;
      next = next->Next;
    }
  }
  return true;
}

// 函数说明详见《API》
static inline bool zj_ObjAddFast(zj_Value *v, zj_Value *vv) {
  if (zj_UNLIKELY(v->N == 0))
    return false;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeObject))
    return false;
  if (zj_UNLIKELY(vv->N == 0))
    return false;
  if (zj_UNLIKELY(vv->N->Key == 0))
    return false;
  if (zj_UNLIKELY(zj_Move(vv) == false))
    return false;
  vv->N->Father = v->N;
  if (zj_UNLIKELY(v->N->Value.Node == 0)) {
    v->N->Value.Node = vv->N;
    v->N->Len = 1;
    v->N->End = vv->N;
  } else {
    v->N->End->Next = vv->N;
    vv->N->Prev = v->N->End;
    v->N->End = vv->N;
    ++v->N->Len;
  }
  vv->N = 0;
  return true;
}

// 函数说明详见《API》
static inline bool zj_ObjAdd(zj_Value *v, const zj_Value *vv) {
  if (zj_UNLIKELY(v->N == 0))
    return false;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeObject))
    return false;
  if (zj_UNLIKELY(vv->N == 0))
    return false;
  if (zj_UNLIKELY(vv->N->Key == 0))
    return false;
  zj_Value *cp = zj_Copy(vv);
  if (zj_UNLIKELY(cp == 0))
    return false;
  cp->N->Father = v->N;
  if (zj_UNLIKELY(v->N->Value.Node == 0)) {
    v->N->Value.Node = cp->N;
    v->N->Len = 1;
    v->N->End = cp->N;
  } else {
    v->N->End->Next = cp->N;
    cp->N->Prev = v->N->End;
    v->N->End = cp->N;
    ++v->N->Len;
  }
  return true;
}

// 函数说明详见《API》
static inline bool zj_ArrayAddFast(zj_Value *v, zj_Value *vv) {
  if (zj_UNLIKELY(v->N == 0))
    return false;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeArray))
    return false;
  if (zj_UNLIKELY(zj_Move(vv) == false))
    return false;
  vv->N->Key = 0;
  vv->N->Father = v->N;
  if (zj_UNLIKELY(v->N->Value.Node == 0)) {
    v->N->Value.Node = vv->N;
    v->N->Len = 1;
    v->N->End = vv->N;
  } else {
    v->N->End->Next = vv->N;
    vv->N->Prev = v->N->End;
    v->N->End = vv->N;
    ++v->N->Len;
  }
  vv->N = 0;
  return true;
}

// 函数说明详见《API》
static inline bool zj_ArrayAdd(zj_Value *v, const zj_Value *vv) {
  if (zj_UNLIKELY(v->N == 0))
    return false;
  if (zj_UNLIKELY(v->N->Type != zj_JSONTypeArray))
    return false;
  zj_Value *cp = zj_Copy(vv);
  if (zj_UNLIKELY(cp == 0))
    return false;
  cp->N->Key = 0;
  cp->N->Father = v->N;
  if (zj_UNLIKELY(v->N->Value.Node == 0)) {
    v->N->Value.Node = cp->N;
    v->N->Len = 1;
    v->N->End = cp->N;
  } else {
    v->N->End->Next = cp->N;
    cp->N->Prev = v->N->End;
    v->N->End = cp->N;
    ++v->N->Len;
  }
  return true;
}

// 函数说明详见《API》
static inline bool zj_ArrayDel(zj_Value *v, zj_Size index) {
  zj_Value *dv = zj_ArrayGet(v, index);
  if (zj_UNLIKELY(dv == 0))
    return false;
  return zj_Move(dv);
}

// 函数说明详见《API》
static inline bool zj_ObjDel(zj_Value *v, const char *key) {
  zj_Value *dv = zj_ObjGet(v, key);
  if (zj_UNLIKELY(dv == 0))
    return false;
  return zj_Move(dv);
}
#if zj_CPLUSPLUS == 1
}
#endif
#endif