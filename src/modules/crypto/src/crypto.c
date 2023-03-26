/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2023
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | crypto.c | LuaRT crypto module
*/

#include <Cipher.h>
#include <Buffer.h>
#include <stdlib.h>

HCRYPTPROV hProv = 0;
UNCRYPT uncrypt = NULL;

/* Karl Malbrain's compact CRC-32. See "A compact CCITT crc16 and crc32 C
 * implementation that balances processor cache usage against speed":
 * http://www.geocities.com/malbrain/ */
ULONG32 crc32(ULONG crc, const UINT8 *ptr, size_t buf_len)
{
    static const UINT32 s_crc32[16] = { 0, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
                                            0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c, 0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c };
    UINT32 crcu32 = (UINT32)crc;
    if (!ptr)
        return 0;
    crcu32 = ~crcu32;
    while (buf_len--)
    {
        UINT8 b = *ptr++;
        crcu32 = (crcu32 >> 4) ^ s_crc32[(crcu32 & 0xF) ^ (b & 0xF)];
        crcu32 = (crcu32 >> 4) ^ s_crc32[(crcu32 & 0xF) ^ (b >> 4)];
    }
    return ~crcu32;
}

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
	lua_pushinteger(L, crc32(0, b, len));
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
