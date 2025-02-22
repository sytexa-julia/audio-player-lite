#ifndef BASSAAC_H
#define BASSAAC_H

#include "bass.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BASSAACDEF
#define BASSAACDEF(f) WINAPI f
#endif

// Additional tags available from BASS_StreamGetTags (for MP4 files)
#define BASS_TAG_MP4 		7	// MP4/iTunes metadata

#define BASS_AAC_STEREO			0x400000 // downmatrix to stereo

// BASS_CHANNELINFO type
#define BASS_CTYPE_STREAM_AAC	0x10b00 // AAC
#define BASS_CTYPE_STREAM_MP4	0x10b01 // MP4


HSTREAM BASSAACDEF(BASS_AAC_StreamCreateFile)(BOOL mem, const void *file, QWORD offset, QWORD length, DWORD flags);
HSTREAM BASSAACDEF(BASS_AAC_StreamCreateURL)(const char *url, DWORD offset, DWORD flags, DOWNLOADPROC *proc, void *user);
HSTREAM BASSAACDEF(BASS_AAC_StreamCreateFileUser)(DWORD system, DWORD flags, const BASS_FILEPROCS *procs, void *user);
HSTREAM BASSAACDEF(BASS_MP4_StreamCreateFile)(BOOL mem, const void *file, QWORD offset, QWORD length, DWORD flags);
HSTREAM BASSAACDEF(BASS_MP4_StreamCreateFileUser)(DWORD system, DWORD flags, const BASS_FILEPROCS *procs, void *user);

#ifdef __cplusplus
}
#endif

#endif
