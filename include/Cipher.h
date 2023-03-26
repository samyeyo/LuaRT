/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Cipher.h | LuaRT Cipher object header file
*/


#include <luart.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef BOOL ( WINAPI *UNCRYPT )( HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen );

typedef struct crypt_Reg {
  const char *name;
  size_t minkeysize;
  size_t maxkeysize;
  size_t blocksize;
  ALG_ID algo;
} crypt_Reg;

extern HCRYPTPROV hProv;
extern UNCRYPT uncrypt;

int get_algo(lua_State *L, const crypt_Reg *funcs, int idx);


//---------------------------------------- Cipher object
typedef struct {
	luart_type		type;
	union {
		HCRYPTKEY	key;
		HCRYPTHASH 	hash;
	};
	crypt_Reg*		algo;
	BOOL			decrypted;
	BYTE*			dbuff;
	size_t			dsize;
	BYTE*			ebuff;
	size_t			esize;
} Cipher;

extern luart_type TCipher;

LUA_CONSTRUCTOR(Cipher);
extern const luaL_Reg Cipher_methods[];
extern const luaL_Reg Cipher_metafields[];

#ifdef __cplusplus
}
#endif