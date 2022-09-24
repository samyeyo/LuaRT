/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | crypto.c | LuaRT crypto module
*/

#include <luart.h>
#include <Cipher.h>
#include <Buffer.h>
#include <stdlib.h>

#define MINIZ_HEADER_FILE_ONLY
#include <zip\lib\miniz.h>

HCRYPTPROV hProv = 0;
UNCRYPT uncrypt = NULL;

/* -- crypt library functions ----------------------------------------------- */

static HINSTANCE dll;

static const crypt_Reg hash_funcs[] = {
  { "md5",		0, 	0,	0,	CALG_MD5 },
  { "sha1",		0, 	0,	0,	CALG_SHA1 },
  { "sha256",	0, 	0,	0,	CALG_SHA_256 },
  { "sha384",	0, 	0,	0,	CALG_SHA_384 },
  { "sha512",	0, 	0,	0,	CALG_SHA_512 },
  { NULL,		0, 	0 ,  0,   0}
};

static int hash(lua_State *L, ALG_ID algo)
{
	HCRYPTHASH hash;
	Buffer *buff = luart_tobuffer(L, 2);
	DWORD len = 0;
	int result = 0;

	if (CryptCreateHash(hProv, algo, 0, 0, &hash)) {
		if (CryptHashData(hash, buff->bytes, buff->size, 0)) {
			if (CryptGetHashParam(hash, HP_HASHVAL, NULL, &len, 0)) {
				BYTE* buff = malloc(len);
				if (CryptGetHashParam(hash, HP_HASHVAL, buff, &len, 0)) {
					lua_toBuffer(L, buff, len);
					result = 1;
				}
				free(buff);
			}
		}
		CryptDestroyHash(hash);
	}
	return result;
}

LUA_METHOD(crypto, hash) {
	return hash(L, hash_funcs[get_algo(L, hash_funcs, 1)].algo);
}

LUA_METHOD(crypto, generate) {
	size_t size = luaL_checkinteger(L, 1);
	BYTE *buff = malloc(sizeof(BYTE)*size);
	CryptGenRandom(hProv, size, buff);
	lua_toBuffer(L, buff, size);
	return 1;
}

LUA_METHOD(crypto, crc32) {
	size_t len;
	const unsigned char *b = (const unsigned char*)luaL_tolstring(L, 1, &len);
	lua_pushinteger(L, mz_crc32(0, b, len));
	return 1;
}

static const luaL_Reg cryptolib[] = {
	{"hash",	crypto_hash},
	{"generate",crypto_generate},
	{"crc32",	crypto_crc32},
	{NULL, NULL}
};

static const luaL_Reg crypto_properties[] = {
	{NULL, NULL}
};

int crypto_finalize(lua_State *L)
{
	FreeLibrary(dll);
	CryptReleaseContext(hProv, 0);
	return 0;
}

LUAMOD_API int luaopen_crypto(lua_State *L)
{
	dll = LoadLibrary("AdvAPI32");
	uncrypt = (void*)GetProcAddress(dll, "CryptDecrypt");
	lua_regmodulefinalize(L, crypto);
	lua_regobjectmt(L, Cipher);
	CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);
	return 1;
}
