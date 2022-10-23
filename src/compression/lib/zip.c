/*
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#define __STDC_WANT_LIB_EXT1__ 1

#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include "lrtapi.h"

#if defined(_WIN32) || defined(__WIN32__) || defined(_MSC_VER) ||              \
    defined(__MINGW32__)
/* Win32, DOS, MSVC, MSVS */
#include <direct.h>


#define MKDIR(DIRNAME) _mkdir(DIRNAME)
#define STRCLONE(STR) ((STR) ? _strdup(STR) : NULL)
#define HAS_DEVICE(P)                                                          \
  ((((P)[0] >= 'A' && (P)[0] <= 'Z') || ((P)[0] >= 'a' && (P)[0] <= 'z')) &&   \
   (P)[1] == ':')
#define FILESYSTEM_PREFIX_LEN(P) (HAS_DEVICE(P) ? 2 : 0)

#else

#include <unistd.h> // needed for symlink() on BSD
int symlink(const char *target, const char *linkpath); // needed on Linux

#define MKDIR(DIRNAME) mkdir(DIRNAME, 0755)
#define STRCLONE(STR) ((STR) ? strdup(STR) : NULL)

#endif

#include <luart.h>
#include "miniz.h"
#include "zip.h"

#ifndef HAS_DEVICE
#define HAS_DEVICE(P) 0
#endif

#ifndef FILESYSTEM_PREFIX_LEN
#define FILESYSTEM_PREFIX_LEN(P) 0
#endif

#ifndef ISSLASH
#define ISSLASH(C) ((C) == '/' || (C) == '\\')
#endif

#ifndef ISSLASHW
#define ISSLASHW(C) ((C) == L'/' || (C) == L'\\')
#endif

#define CLEANUP(ptr) free(ptr);
/*                                                          \
  do {                                                                         \
    if (ptr) {                                                                 \
      free((void *)ptr);                                                       \
      ptr = NULL;                                                              \
    }                                                                          \
  } while (0) */

static const char *base_name(const char *name) {
  char const *p;
  char const *base = name += FILESYSTEM_PREFIX_LEN(name);
  int all_slashes = 1;

  for (p = name; *p; p++) {
    if (ISSLASH(*p))
      base = p + 1;
    else
      all_slashes = 0;
  }

  /* If NAME is all slashes, arrange to return `/'. */
  if (*base == '\0' && ISSLASH(*name) && all_slashes)
    --base;

  return base;
}

struct zip_t *zip_open(const wchar_t *zipname, int level, char mode) {
  struct zip_t *zip = NULL;

  if (!zipname || wcslen(zipname) < 1) {
    // zip_t archive name is empty or NULL
    goto cleanup;
  }

  if (level < 0)
    level = MZ_DEFAULT_LEVEL;
  if ((level & 0xF) > MZ_UBER_COMPRESSION) {
    // Wrong compression level
    goto cleanup;
  }
  zip = (struct zip_t *)calloc((size_t)1, sizeof(struct zip_t));
  if (!zip)
    goto cleanup;
  zip->level = (mz_uint)level;
  switch (mode) {
  case 'w':
    // Create a new archive.
    if (!mz_zip_writer_init_file(&(zip->archive), zipname, 0)) {
      // Cannot initialize zip_archive writer*
      goto cleanup;
    }
	break;

  case 'r':
  case 'a':
    if (!mz_zip_reader_init_file(
            &(zip->archive), zipname,
            zip->level | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY)) {
      // An archive file does not exist or cannot initialize
      // zip_archive reader
      goto cleanup;
    }
    if (mode == 'a' &&
        !mz_zip_writer_init_from_reader(&(zip->archive), zipname)) {
      mz_zip_reader_end(&(zip->archive));
      goto cleanup;
    }
    break;

  default:
    goto cleanup;
  }

  return zip;

cleanup:
  CLEANUP(zip);
  return NULL;
}

void zip_close(struct zip_t *zip) {
  if (zip) {
    // Always finalize, even if adding failed for some reason, so we have a
    // valid central directory.
    mz_zip_writer_finalize_archive(&(zip->archive));

    mz_zip_writer_end(&(zip->archive));
    mz_zip_reader_end(&(zip->archive));
    free(zip);
  }
}

int zip_is64(struct zip_t *zip) {
  if (!zip) {
    // zip_t handler is not initialized
    return -1;
  }

  if (!zip->archive.m_pState) {
    // zip state is not initialized
    return -1;
  }

  return (int)zip->archive.m_pState->m_zip64;
}

static void normalize(struct zip_t *zip, const char *entryname, mz_uint16 len) {
  char *p = strdup(entryname);
  char c;

	zip->entry.name = p;
	zip->entry.namelen = len;

	while ((c = *entryname++))
		if (c == '\\')
			*p++ = '/';
}

int zip_entry_open(struct zip_t *zip, const char *entryname) {
  mz_zip_archive *pzip = NULL;
  mz_uint level;
  mz_uint64 num_alignment_padding_bytes;
  mz_zip_archive_file_stat stats;

  if (!zip || !entryname) {
    return -1;
  }


  /*
    .ZIP File Format Specification Version: 6.3.3

    4.4.17.1 The name of the file, with optional relative path.
    The path stored MUST not contain a drive or
    device letter, or a leading slash.  All slashes
    MUST be forward slashes '/' as opposed to
    backwards slashes '\' for compatibility with Amiga
    and UNIX file systems etc.  If input came from standard
    input, there is no file name field.
  */
  //zip->entry.name = entryname; //strrpl(entryname, entrylen, '\\', '/');
  
  normalize(zip, entryname, strlen(entryname));
  if (!zip->entry.name || (zip->entry.namelen < 1))
    return -1;

  pzip = &(zip->archive);
  if (pzip->m_zip_mode == MZ_ZIP_MODE_READING) {
    zip->entry.index =
        mz_zip_reader_locate_file(pzip, zip->entry.name, NULL, 0);
    if (zip->entry.index < 0) {
      goto cleanup;
    }

    if (!mz_zip_reader_file_stat(pzip, (mz_uint)zip->entry.index, &stats)) {
      goto cleanup;
    }

    zip->entry.comp_size = stats.m_comp_size;
    zip->entry.uncomp_size = stats.m_uncomp_size;
    zip->entry.uncomp_crc32 = stats.m_crc32;
    zip->entry.offset = stats.m_central_dir_ofs;
    zip->entry.header_offset = stats.m_local_header_ofs;
    zip->entry.method = stats.m_method;
    zip->entry.external_attr = stats.m_external_attr;
    zip->entry.m_time = stats.m_time;
    return 0;
  }

  zip->entry.index = (int)zip->archive.m_total_files;
  zip->entry.comp_size = 0;
  zip->entry.uncomp_size = 0;
  zip->entry.uncomp_crc32 = MZ_CRC32_INIT;
  zip->entry.offset = zip->archive.m_archive_size;
  zip->entry.header_offset = zip->archive.m_archive_size;
  memset(zip->entry.header, 0, MZ_ZIP_LOCAL_DIR_HEADER_SIZE * sizeof(mz_uint8));
  zip->entry.method = 0;

  // UNIX or APPLE
#if MZ_PLATFORM == 3 || MZ_PLATFORM == 19
  // regular file with rw-r--r-- persmissions
  zip->entry.external_attr = (mz_uint32)(0100644) << 16;
#else
  zip->entry.external_attr = 0;
#endif

  num_alignment_padding_bytes =
      mz_zip_writer_compute_padding_needed_for_file_alignment(pzip);

  if (!pzip->m_pState || (pzip->m_zip_mode != MZ_ZIP_MODE_WRITING)) {
    // Wrong zip mode
    goto cleanup;
  }

  if (zip->level & MZ_ZIP_FLAG_COMPRESSED_DATA) {
    // Wrong zip compression level
    goto cleanup;
  }

  if (!mz_zip_writer_write_zeros(pzip, zip->entry.offset,(mz_uint32)(
                                 num_alignment_padding_bytes +
                                     sizeof(zip->entry.header)))) {
    // Cannot memset zip entry header
    goto cleanup;
  }

  zip->entry.header_offset += num_alignment_padding_bytes;
  if (pzip->m_file_offset_alignment) {
    MZ_ASSERT(
        (zip->entry.header_offset & (pzip->m_file_offset_alignment - 1)) == 0);
  }
  zip->entry.offset += num_alignment_padding_bytes + sizeof(zip->entry.header);

  if (pzip->m_pWrite(pzip->m_pIO_opaque, zip->entry.offset, zip->entry.name,
                     zip->entry.namelen) != zip->entry.namelen) {
    // Cannot write data to zip entry
    goto cleanup;
  }

  zip->entry.offset += zip->entry.namelen;
  level = zip->level & 0xF;
  if (level) {
    zip->entry.state.m_pZip = pzip;
    zip->entry.state.m_cur_archive_file_ofs = zip->entry.offset;
    zip->entry.state.m_comp_size = 0;

    if (tdefl_init(&(zip->entry.comp), mz_zip_writer_add_put_buf_callback,
                   &(zip->entry.state),
                   (int)tdefl_create_comp_flags_from_zip_params(
                       (int)level, -15, MZ_DEFAULT_STRATEGY)) !=
        TDEFL_STATUS_OKAY) {
      // Cannot initialize the zip compressor

      goto cleanup;
    }
  }

  zip->entry.m_time = time(NULL);
  return 0;

cleanup:
  CLEANUP(zip->entry.name);
  return -1;
}

int zip_entry_openbyindex(struct zip_t *zip, int index) {
  mz_zip_archive *pZip = NULL;
  mz_zip_archive_file_stat stats;
  const mz_uint8 *pHeader;
  char *pFilename;

  if (!zip) {
    // zip_t handler is not initialized
    return -1;
  }

  pZip = &(zip->archive);
  if (pZip->m_zip_mode != MZ_ZIP_MODE_READING) {
    // open by index requires readonly mode
    return -1;
  }

  if (index < 0 || (mz_uint)index >= pZip->m_total_files) {
    // index out of range
    return -1;
  }

  if (!(pHeader = &MZ_ZIP_ARRAY_ELEMENT(
            &pZip->m_pState->m_central_dir, mz_uint8,
            MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir_offsets,
                                 mz_uint32, index)))) {
    // cannot find header in central directory
    return -1;
  }
  pFilename = (char*)pHeader + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE;

  /*
    .ZIP File Format Specification Version: 6.3.3

    4.4.17.1 The name of the file, with optional relative path.
    The path stored MUST not contain a drive or
    device letter, or a leading slash.  All slashes
    MUST be forward slashes '/' as opposed to
    backwards slashes '\' for compatibility with Amiga
    and UNIX file systems etc.  If input came from standard
    input, there is no file name field.
  */
  normalize(zip, pFilename, MZ_READ_LE16(pHeader + MZ_ZIP_CDH_FILENAME_LEN_OFS));

 if (!zip->entry.name) {
    // local entry name is NULL
    return -1;
  }
  if (!mz_zip_reader_file_stat(pZip, (mz_uint)index, &stats)) {
    return -1;
  }

  zip->entry.index = index;
  zip->entry.comp_size = stats.m_comp_size;
  zip->entry.uncomp_size = stats.m_uncomp_size;
  zip->entry.uncomp_crc32 = stats.m_crc32;
  zip->entry.offset = stats.m_central_dir_ofs;
  zip->entry.header_offset = stats.m_local_header_ofs;
  zip->entry.method = stats.m_method;
  zip->entry.external_attr = stats.m_external_attr;
  zip->entry.m_time = stats.m_time;

  return 0;
}

int zip_entry_close(struct zip_t *zip) {
  mz_zip_archive *pzip = NULL;
  mz_uint level;
  tdefl_status done;
  mz_uint16 entrylen;
  mz_uint16 dos_time, dos_date;
  int status = -1;

  if (!zip) {
    // zip_t handler is not initialized
    goto cleanup;
  }

  pzip = &(zip->archive);
  if (pzip->m_zip_mode == MZ_ZIP_MODE_READING) {
    status = 0;
    goto cleanup;
  }

  level = zip->level & 0xF;
  if (level) {
    done = tdefl_compress_buffer(&(zip->entry.comp), "", 0, TDEFL_FINISH);
    if (done != TDEFL_STATUS_DONE && done != TDEFL_STATUS_OKAY) {
      // Cannot flush compressed buffer
      goto cleanup;
    }
    zip->entry.comp_size = zip->entry.state.m_comp_size;
    zip->entry.offset = zip->entry.state.m_cur_archive_file_ofs;
    zip->entry.method = MZ_DEFLATED;
  }

  entrylen = (mz_uint16)zip->entry.namelen;

  mz_zip_time_t_to_dos_time(zip->entry.m_time, &dos_time, &dos_date);
  if (!mz_zip_writer_create_local_dir_header(
          pzip, zip->entry.header, entrylen, 0, zip->entry.uncomp_size,
          zip->entry.comp_size, zip->entry.uncomp_crc32, zip->entry.method, 0,
          dos_time, dos_date)) {
    // Cannot create zip entry header
    goto cleanup;
  }

  if (pzip->m_pWrite(pzip->m_pIO_opaque, zip->entry.header_offset,
                     zip->entry.header,
                     sizeof(zip->entry.header)) != sizeof(zip->entry.header)) {
    // Cannot write zip entry header
    goto cleanup;
  }

  if (!mz_zip_writer_add_to_central_dir(
          pzip, zip->entry.name, entrylen, NULL, 0, "", 0,
          zip->entry.uncomp_size, zip->entry.comp_size, zip->entry.uncomp_crc32,
          zip->entry.method, 0, dos_time, dos_date, zip->entry.header_offset,
          zip->entry.external_attr, NULL, 0)) {
    // Cannot write to zip central dir
    goto cleanup;
  }

  pzip->m_total_files++;
  pzip->m_archive_size = zip->entry.offset;
  status = 0;

cleanup:
  if (zip) {
    zip->entry.m_time = 0;
    CLEANUP(zip->entry.name);
  }
  return status;
}

const char *zip_entry_name(struct zip_t *zip, size_t *len) {
  if (!zip) {
    // zip_t handler is not initialized
    return NULL;
  }
  *len = zip->entry.namelen;
  return zip->entry.name;
}

int zip_entry_index(struct zip_t *zip) {
  if (!zip) {
    // zip_t handler is not initialized
    return -1;
  }

  return zip->entry.index;
}

int zip_entry_isdir(struct zip_t *zip) {
  if (!zip) {
    // zip_t handler is not initialized
    return -1;
  }

  if (zip->entry.index < 0) {
    // zip entry is not opened
    return -1;
  }

  return (int)mz_zip_reader_is_file_a_directory(&zip->archive,
                                                (mz_uint)zip->entry.index);
}

unsigned long long zip_entry_size(struct zip_t *zip) {
  return zip ? zip->entry.uncomp_size : 0;
}

unsigned int zip_entry_crc32(struct zip_t *zip) {
  return zip ? zip->entry.uncomp_crc32 : 0;
}

int zip_entry_write(struct zip_t *zip, const void *buf, size_t bufsize) {
  mz_uint level;
  mz_zip_archive *pzip = NULL;
  tdefl_status status;

  if (!zip) {
    // zip_t handler is not initialized
    return -1;
  }

  pzip = &(zip->archive);
  if (buf && bufsize > 0) {
    zip->entry.uncomp_size += bufsize;
    zip->entry.uncomp_crc32 = (mz_uint32)mz_crc32(
        zip->entry.uncomp_crc32, (const mz_uint8 *)buf, bufsize);

    level = zip->level & 0xF;
    if (!level) {
      if ((pzip->m_pWrite(pzip->m_pIO_opaque, zip->entry.offset, buf,
                          bufsize) != bufsize)) {
        // Cannot write buffer
        return -1;
      }
      zip->entry.offset += bufsize;
      zip->entry.comp_size += bufsize;
    } else {
      status = tdefl_compress_buffer(&(zip->entry.comp), buf, bufsize,
                                     TDEFL_NO_FLUSH);
      if (status != TDEFL_STATUS_DONE && status != TDEFL_STATUS_OKAY) {
        // Cannot compress buffer
        return -1;
      }
    }
  }

  return 0;
}

int zip_entry_fwrite(struct zip_t *zip, const wchar_t *filename) {
  int status = 0;
  size_t n = 0;
  FILE *stream = NULL;
  mz_uint8 buf[MZ_ZIP_MAX_IO_BUF_SIZE];
  struct MZ_FILE_STAT_STRUCT file_stat;

  if (!zip) {
    // zip_t handler is not initialized
    return -1;
  }

  memset(buf, 0, MZ_ZIP_MAX_IO_BUF_SIZE);
  memset((void *)&file_stat, 0, sizeof(struct MZ_FILE_STAT_STRUCT));
  if (MZ_FILE_STAT(filename, &file_stat) != 0) {
    // problem getting information - check errno
    return -1;
  }

  if ((file_stat.st_mode & 0200) == 0) {
    // MS-DOS read-only attribute
    zip->entry.external_attr |= 0x01;
  }
  zip->entry.external_attr |= (mz_uint32)((file_stat.st_mode & 0xFFFF) << 16);
  zip->entry.m_time = file_stat.st_mtime;

  if (_wfopen_s(&stream, filename, L"rb"))
  {
    // Cannot open filename
    return -1;
  }

  while ((n = fread(buf, sizeof(mz_uint8), MZ_ZIP_MAX_IO_BUF_SIZE, stream)) >
         0) {
    if (zip_entry_write(zip, buf, n) < 0) {
      status = -1;
      break;
    }
  }
  fclose(stream);

  return status;
}

ssize_t zip_entry_read(struct zip_t *zip, void **buf, size_t *bufsize) {
  mz_zip_archive *pzip = NULL;
  mz_uint idx;
  size_t size = 0;

  if (!zip) 
    return -1;
  
  pzip = &(zip->archive);
  if (pzip->m_zip_mode != MZ_ZIP_MODE_READING || zip->entry.index < 0)
    return -1;

  idx = (mz_uint)zip->entry.index;
  if (mz_zip_reader_is_file_a_directory(pzip, idx))
    return -1;
  
  *buf = mz_zip_reader_extract_to_heap(pzip, idx, &size, 0);
  
  if (*buf && bufsize)
    *bufsize = size;
  return size;
}

ssize_t zip_entry_noallocread(struct zip_t *zip, void *buf, size_t bufsize) {
  mz_zip_archive *pzip = NULL;

  if (!zip)
    return -1;
  pzip = &(zip->archive);
  if (pzip->m_zip_mode != MZ_ZIP_MODE_READING || zip->entry.index < 0)
    return -1;
  if (!mz_zip_reader_extract_to_mem_no_alloc(pzip, (mz_uint)zip->entry.index, buf, bufsize, 0, NULL, 0))
    return -1;

  return (ssize_t)zip->entry.uncomp_size;
}

int zip_entry_fread(struct zip_t *zip, const wchar_t *filename) {
  mz_zip_archive *pzip = NULL;
  mz_uint idx;
  mz_zip_archive_file_stat info;

  if (!zip) {
    // zip_t handler is not initialized
    return -1;
  }

  memset((void *)&info, 0, sizeof(mz_zip_archive_file_stat));
  pzip = &(zip->archive);
  if (pzip->m_zip_mode != MZ_ZIP_MODE_READING || zip->entry.index < 0) {
    // the entry is not found or we do not have read access
    return -1;
  }

  idx = (mz_uint)zip->entry.index;
  if (mz_zip_reader_is_file_a_directory(pzip, idx)) {
    // the entry is a directory
	return -1;
  }

  if (!mz_zip_reader_extract_to_file(pzip, idx, filename, 0)) {
    return -1;
}
  return 0;
}

int zip_entry_extract(struct zip_t *zip,
                      size_t (*on_extract)(void *arg, unsigned long long offset,
                                           const void *buf, size_t bufsize),
                      void *arg) {
  mz_zip_archive *pzip = NULL;
  mz_uint idx;

  if (!zip) {
    // zip_t handler is not initialized
    return -1;
  }

  pzip = &(zip->archive);
  if (pzip->m_zip_mode != MZ_ZIP_MODE_READING || zip->entry.index < 0) {
    // the entry is not found or we do not have read access
    return -1;
  }

  idx = (mz_uint)zip->entry.index;
  return (mz_zip_reader_extract_to_callback(pzip, idx, on_extract, arg, 0))
             ? 0
             : -1;
}

int zip_total_entries(struct zip_t *zip) {
  if (!zip) {
    // zip_t handler is not initialized
    return -1;
  }

  return (int)zip->archive.m_total_files;
}

int zip_create(const wchar_t *zipname, const char *filenames[], size_t len) {
  int status = 0;
  size_t i;
  mz_zip_archive zip_archive;
  struct MZ_FILE_STAT_STRUCT file_stat;

  if (!zipname || wcslen(zipname) < 1) {
    // zip_t archive name is empty or NULL
    return -1;
  }

  // Create a new archive.
  if (!memset(&(zip_archive), 0, sizeof(zip_archive))) {
    // Cannot memset zip archive
    return -1;
  }

  if (!mz_zip_writer_init_file(&zip_archive, zipname, 0)) {
    // Cannot initialize zip_archive writer
    return -1;
  }

  memset((void *)&file_stat, 0, sizeof(struct MZ_FILE_STAT_STRUCT));

  for (i = 0; i < len; ++i) { 
    const char *name = filenames[i];
	int len = strlen(name);
	wchar_t *wname = utf8_towchar(name, &len);
    if (!name) {
      status = -1;
      break;
    }

   if (!mz_zip_writer_add_file(&zip_archive, base_name(name), wname, "", 0,
                                ZIP_DEFAULT_COMPRESSION_LEVEL)) {
      // Cannot add file to zip_archive
      status = -1;
      break;
    }
	free(wname);
  }

  mz_zip_writer_finalize_archive(&zip_archive);
  mz_zip_writer_end(&zip_archive);
  return status;
}

const char *zip_lasterror(struct zip_t *zip) {
	return mz_zip_get_error_string(mz_zip_get_last_error(&zip->archive));
}
