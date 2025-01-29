/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2025
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | Cipher.c | LuaRT Cipher object implementation
*/

#include <luart.h>
#include <Cipher.h>
#include <Buffer.h>
#include <stdlib.h>

luart_type TCipher;

typedef struct {
    BLOBHEADER hdr;
    DWORD dwKeySize;
    BYTE KeyData[32];
} AESBLOB;

/* -- crypt library functions ----------------------------------------------- */

const DWORD KEY_MODES_VAL[] = { CRYPT_MODE_CBC, CRYPT_MODE_CFB, CRYPT_MODE_ECB, CRYPT_MODE_CTS};
const char *KEY_MODES[] = {"cbc", "cfb", "ecb", "cts", NULL};

static const crypt_Reg crypt_funcs[] = {
  { "aes128",	16,	16,		16,	CALG_AES_128 },
  { "aes192",	24,	24,		16,	CALG_AES_192 },
  { "aes256",	32,	32,		16,	CALG_AES_256 },
  { "des",		7, 	7,		8,	CALG_DES },
  { "2des",		14, 14,		8,	CALG_3DES_112 },
  { "3des",		21, 21,		8,	CALG_3DES },
  { "rc2",		5,	16,		8,	CALG_RC2 },
  { "rc4",		5,	16, 	0,	CALG_RC4 },
  { NULL,		0, 	0,    	0, 0}
};

int get_algo(lua_State *L, const crypt_Reg *funcs, int idx) {
	const crypt_Reg *c;
	const char *algo = luaL_checkstring(L, idx);
	int i = 0;

	for (c = funcs; c->name; c++) {
		if (strcmp(c->name, algo) == 0)
			return i;
		i++;
	}
	return luaL_error(L, "unknown '%s' algorithm", algo);
}

static Cipher *construct(lua_State *L, const crypt_Reg *reg, luart_type type) {
	int i = get_algo(L, reg, 2);
	Cipher *cipher = calloc(1, sizeof(Cipher));
	cipher->algo = (crypt_Reg*)&reg[i];
	cipher->type = type;
	return cipher;
}

LUA_CONSTRUCTOR(Cipher) {
	Cipher *cipher = construct(L, crypt_funcs, TCipher);
	size_t keylen;
	const char *key;
	int n = lua_gettop(L);
	AESBLOB AESBLOB = {0};
	DWORD mode;

	key = luaL_tolstring(L, 3, &keylen);
	if (keylen > cipher->algo->maxkeysize)
		keylen = cipher->algo->maxkeysize;
	if (keylen < cipher->algo->minkeysize)
		luaL_error(L, "invalid key length (expecting a minimum of %d bytes for '%s')", cipher->algo->minkeysize, cipher->algo->name);
	AESBLOB.hdr.bType = PLAINTEXTKEYBLOB;
    AESBLOB.hdr.bVersion = CUR_BLOB_VERSION;
    AESBLOB.hdr.reserved = 0;
    AESBLOB.hdr.aiKeyAlg = cipher->algo->algo;
    AESBLOB.dwKeySize = keylen + (AESBLOB.hdr.aiKeyAlg == CALG_DES) + (AESBLOB.hdr.aiKeyAlg == CALG_3DES)*3 + (AESBLOB.hdr.aiKeyAlg == CALG_3DES_112)*2;
	memcpy(AESBLOB.KeyData, key, keylen);

	if (CryptImportKey(hProv, (BYTE*)&AESBLOB, sizeof(AESBLOB), 0, CRYPT_NO_SALT, &cipher->key)) {
		if (n > 3 && !lua_isnil(L, 4) && cipher->algo->blocksize) {
			if (lua_isstring(L, 4) || luaL_checkcinstance(L, 4, Buffer)){
				const char *buff = luaL_tolstring(L, 4, &keylen);
				if (keylen != cipher->algo->blocksize)
					luaL_error(L, "wrong initialization vector length (expected %d bytes, found %d)", cipher->algo->blocksize, keylen);
				if (!CryptSetKeyParam(cipher->key, KP_IV, (BYTE*)buff, 0)) {
			err:	luaL_getlasterror(L, GetLastError()); 
					luaL_error(L, "key error (%s)", lua_tostring(L, -1));
				}
			} else luaL_typeerror(L, 3, "Buffer or string");
		}
		if (n > 4  && !lua_isnil(L, 5)) {
			int opt = lua_optstring(L, 5, KEY_MODES, -1);		
			if (opt == -1)
				luaL_error(L, "unknown cipher mode '%s'", lua_tostring(L, 5));
			mode = KEY_MODES_VAL[opt];
			if (!CryptSetKeyParam(cipher->key, KP_MODE, (const BYTE *)&mode, 0))
				goto err;
		}
		mode = PKCS5_PADDING;
		if (!CryptSetKeyParam(cipher->key, KP_PADDING, (const BYTE *)&mode, 0))
			goto err;
	} else goto err;
	lua_newinstance(L, cipher, Cipher);
	return 1;
}

LUA_METHOD(Cipher, encrypt) {
	Cipher *cipher = lua_self(L, 1, Cipher);
	DWORD size, length, remaining = 0, datalen = 0;
	size_t result;
	const char *data = luaL_tolstring(L, 2, &result);
	
	datalen = result;
	cipher->ebuff = cipher->esize ? realloc(cipher->ebuff, cipher->esize + datalen) : malloc(datalen);
	memcpy(cipher->ebuff+cipher->esize, data, datalen);
	if (!cipher->algo->blocksize) {
		length = datalen;
		goto crypt;
	}
	remaining = cipher->esize % cipher->algo->blocksize;	
	if ((length = remaining + datalen) >= cipher->algo->blocksize ) {
		length -= length % cipher->algo->blocksize;
crypt:	size = length;
		result = 0;
		if (CryptEncrypt(cipher->key, 0, FALSE, 0, NULL, &length, 0))
			if (CryptEncrypt(cipher->key, 0, FALSE, 0, cipher->ebuff+cipher->esize-remaining, &size, length))
				result = size;
		if (!result)
			goto done;
	}
	cipher->esize += datalen;
done:
	lua_pushinteger(L, result);
	return 1;
}

LUA_METHOD(Cipher, decrypt) {
	Cipher *cipher = lua_self(L, 1, Cipher);
	DWORD size, length, datalen, remaining = 0;
	size_t result = 0;
	const char *data = luaL_tolstring(L, 2, (size_t*)&datalen);

	cipher->dbuff = cipher->dsize ? realloc(cipher->dbuff, cipher->dsize + datalen) : malloc(datalen);
	memcpy(cipher->dbuff+cipher->dsize, data, datalen);	
	if (!cipher->algo->blocksize) {
		result = datalen;
		goto done;
	}
	remaining = cipher->dsize % cipher->algo->blocksize;
	if ((length = remaining + datalen - (datalen % cipher->algo->blocksize))) {
		size = length;
		if (uncrypt(cipher->key, 0, FALSE, 0, NULL, &length)) {
			cipher->dsize -= remaining;			
			if (remaining)
				cipher->dbuff = realloc(cipher->dbuff, cipher->dsize+length);
			if (uncrypt(cipher->key, 0, FALSE, 0, cipher->dbuff+cipher->dsize, &size))
				result = datalen;
		}
	}
done:
	cipher->dsize += datalen;
	lua_pushinteger(L, result);
	return 1;
}

LUA_PROPERTY_GET(Cipher, algorithm) {
	lua_pushstring(L, lua_self(L, 1, Cipher)->algo->name);
	return 1;
}

LUA_PROPERTY_GET(Cipher, encrypted) {
	Cipher *cipher = lua_self(L, 1, Cipher);
	DWORD length, remaining;
	
	if (cipher->algo->blocksize && (remaining = cipher->esize % cipher->algo->blocksize)) {
		length = remaining;
		if (CryptEncrypt(cipher->key, 0, TRUE, 0, NULL, &length, 0)) {
			cipher->esize -= remaining;
			cipher->ebuff = realloc(cipher->ebuff, cipher->esize+length);
			if (CryptEncrypt(cipher->key, 0, TRUE, 0, cipher->ebuff+cipher->esize, &remaining, length))
				cipher->esize += length;
		}
	}
	lua_toBuffer(L, cipher->ebuff, cipher->esize);
	return 1;
}

LUA_PROPERTY_GET(Cipher, decrypted) {
	Cipher *cipher = lua_self(L, 1, Cipher);
	DWORD length, remaining = 0;
	
	if (!cipher->decrypted) {
		remaining = cipher->algo->blocksize ? cipher->dsize % cipher->algo->blocksize : cipher->dsize;
		length = remaining;
		if (uncrypt(cipher->key, 0, TRUE, 0, NULL, &length)) {
			if (cipher->algo->blocksize) {
				cipher->dsize -= remaining;
				cipher->dbuff = realloc(cipher->dbuff, cipher->dsize+length);
				if (uncrypt(cipher->key, 0, TRUE, 0, cipher->dbuff+cipher->dsize, &remaining))
					cipher->dsize += remaining;
				else 
					goto err;
			} else if (!uncrypt(cipher->key, 0, TRUE, 0, cipher->dbuff, &remaining)) {
err:				lua_pushboolean(L, FALSE);
					return 1;
				}
		}
	}
	cipher->decrypted = TRUE;
	lua_toBuffer(L, cipher->dbuff, cipher->dsize);
	return 1;
}

LUA_METHOD(Cipher, reset) {
	Cipher *cipher = lua_self(L, 1, Cipher);
	free(cipher->ebuff);
	free(cipher->dbuff);
	cipher->esize = 0;
	cipher->dsize = 0;
	cipher->decrypted = FALSE;
	return 0;
}

LUA_METHOD(Cipher, __gc) {
	Cipher *cipher = lua_self(L, 1, Cipher);
	free(cipher->ebuff);
	free(cipher->dbuff);
	if (cipher->key)
		CryptDestroyKey(cipher->key);
	if (cipher->hash)
		CryptDestroyHash(cipher->hash);
	free(cipher);
	return 0;
}

const luaL_Reg Cipher_metafields[] = {
	{"__gc", Cipher___gc},
	{NULL, NULL}
};

const luaL_Reg Cipher_methods[] = {        
	{"encrypt", Cipher_encrypt},
	{"decrypt", Cipher_decrypt},
	{"reset", 	Cipher_reset},
	{"get_algorithm",  Cipher_getalgorithm}, 
	{"get_encrypted",  Cipher_getencrypted},  
	{"get_decrypted",  Cipher_getdecrypted},  
	{NULL, NULL}
};