/*
 | LuaRT - A Windows programming framework for Lua
 | Luart.org, Copyright (c) Tine Samir 2022.
 | See Copyright Notice in LICENSE.TXT
 |-------------------------------------------------
 | compression.c | LuaRT compression module implementation
*/

#include <luart.h>
#include <File.h>
#include <Buffer.h>
#include <Zip.h>
#include <compression\lib\zip.h>

LUA_METHOD(compression, deflate) {
	size_t len; 
	const unsigned char *str = (const unsigned char *)luaL_tolstring(L, 1, &len);
	mz_ulong cmp_len = compressBound(len);
	unsigned char *buff = malloc(cmp_len+sizeof(size_t));
	int result;
	
	lua_pop(L, 1);
	result = compress2(buff+sizeof(size_t), &cmp_len, str, len, (int)luaL_optinteger(L, 2, MZ_DEFAULT_COMPRESSION));
	if ( result == Z_OK ) {
		*((size_t *)buff) = len;
		lua_toBuffer(L, buff, cmp_len+sizeof(size_t));
	}
	free(buff);
	return result == Z_OK;
}

LUA_METHOD(compression, inflate) {
	size_t len;
	const unsigned char *str = (const unsigned char *)luaL_tolstring(L, 1, &len);
	mz_ulong uncmp_len = *((size_t *)(str));
	unsigned char *buff = malloc(uncmp_len);
	int result = uncompress(buff, &uncmp_len, str+sizeof(size_t), len);
	if ( result == Z_OK )
		lua_toBuffer(L, buff, uncmp_len);
	free(buff);
	return result == Z_OK;
}

LUA_METHOD(compression, isZip) {
	wchar_t *fname = luaL_checkFilename(L, 1);
	struct zip_t *z = NULL;
	BOOL isvalid = FALSE;
	if ( (_waccess(fname, 0) == 0) && (z = zip_open(fname, MZ_DEFAULT_COMPRESSION, 'r')) ) {
		isvalid = TRUE;
		zip_close(z);
	}
	free(fname);

	lua_pushboolean(L, isvalid);
	return 1;
}

#define  	GZIP_HEADER_MINSIZE 10
#define  	GZIP_FOOTER_SIZE 8
#define  	GZIP_COMPRESSION_DEFLATE 8
#define 	CHUNK 16384

LUA_METHOD(compression, gunzip) {
	int ret;
    unsigned int have;
    z_stream stream = {0};
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];
	wchar_t *fname;
	FILE *filefrom, *fileto;
	wchar_t path[MAX_PATH], tmp[MAX_PATH];

    if (mz_inflateInit2(&stream, -MZ_DEFAULT_WINDOW_BITS) != MZ_OK)
		luaL_error(L, "Failed to initialize deflate compression");
 	fname = luaL_checkFilename(L, 1);
	if (!(filefrom = _wfopen(fname, L"rb")))
		luaL_error(L, "File not found");
	GetTempPathW(MAX_PATH, path);
	GetTempFileNameW(path, NULL, 0, tmp);
	if (!(fileto = _wfopen(tmp, L"wb")))
		luaL_error(L, "Cannot create temporary uncompressed file");
	fread(in, 1, GZIP_HEADER_MINSIZE, filefrom);
    do {
        stream.avail_in = fread(in, 1, CHUNK, filefrom);
        if (ferror(filefrom)) {
ioerror: 	inflateEnd(&stream);
            luaL_error(L, strerror(errno));
        }
        if (stream.avail_in == 0)
            break;
        stream.next_in = in;
        do {
            stream.avail_out = CHUNK;
            stream.next_out = out;
            ret = mz_inflate(&stream, MZ_NO_FLUSH);
			if (ret < MZ_OK || ret > MZ_STREAM_END)
				luaL_error(L, "Error during decompression");
            have = CHUNK - stream.avail_out;
            if (fwrite(out, 1, have, fileto) != have || ferror(fileto)) 
				goto ioerror;
        } while (stream.avail_out == 0);

    } while (ret != Z_STREAM_END);
    (void)inflateEnd(&stream);
	fclose(filefrom);
	fclose(fileto);
	lua_pushwstring(L, tmp);
	lua_pushinstance(L, File, 1);
	free(fname);
    return ret == Z_STREAM_END;
}

LUA_METHOD(compression, gzip) {
    int flush;
    unsigned int have;
    z_stream stream = {0};
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];
	wchar_t *fname;
	FILE *filefrom, *fileto;
	wchar_t path[MAX_PATH], tmp[MAX_PATH];
	static unsigned char gzip_header[] = { 0x1F, 0x8B, 0x08, 0, 0, 0, 0, 0, 0, 0x0B };
	mz_ulong crc = 0;
	LONGLONG fsize = 0;

	if (mz_deflateInit2(&stream, MZ_DEFAULT_LEVEL, MZ_DEFLATED, -MZ_DEFAULT_WINDOW_BITS, 1, 0) != MZ_OK)
		luaL_error(L, "Failed to initialize deflate compression");
 	fname = luaL_checkFilename(L, 1);
	if (!(filefrom = _wfopen(fname, L"rb")))
		luaL_error(L, "File not found");
	GetTempPathW(MAX_PATH, path);
	GetTempFileNameW(path, NULL, 0, tmp);
	if (!(fileto = _wfopen(tmp, L"wb")))
		luaL_error(L, "Cannot create temporary gzip file");
	fwrite(gzip_header, 1, GZIP_HEADER_MINSIZE, fileto);
    do {
        stream.avail_in = fread(in, 1, CHUNK, filefrom);
		fsize += stream.avail_in;
		crc = mz_crc32(crc, in, stream.avail_in);
        if (ferror(filefrom)) {
            deflateEnd(&stream);
            luaL_error(L, strerror(errno));
        }
        flush = feof(filefrom) ? Z_FINISH : Z_NO_FLUSH;
        stream.next_in = in;
        do {
            stream.avail_out = CHUNK;
            stream.next_out = out;
            deflate(&stream, flush);
            have = CHUNK - stream.avail_out;
            if (fwrite(out, 1, have, fileto) != have || ferror(fileto)) {
                deflateEnd(&stream);
                luaL_error(L, strerror(errno));
            }
        } while (stream.avail_out == 0);
    } while (flush != Z_FINISH);

    deflateEnd(&stream);
	fclose(filefrom);
	
	for (int i = 0; i < 4; i++)
		fputc(crc >> (i*8), fileto);
	for (int i = 0; i < 4; i++)
		fputc(fsize >> (i*8), fileto);
	fclose(fileto);
	lua_pushwstring(L, tmp);
	lua_pushinstance(L, File, 1);
	free(fname);
    return 1;
}

static const luaL_Reg compression_properties[] = {
	{NULL, NULL}
};

static const luaL_Reg compressionlib[] = {
	{"isZip",		compression_isZip},
	{"inflate",		compression_inflate},
	{"deflate",		compression_deflate},
	{"gzip",		compression_gzip},
	{"gunzip",		compression_gunzip},
	{NULL, NULL}
};

LUAMOD_API int luaopen_compression(lua_State *L) {
	lua_regmodule(L, compression);
	lua_regobjectmt(L, Zip);
	return 1;
}