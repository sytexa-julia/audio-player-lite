/*
	BASSFLAC 2.4 C/C++ header file
	Copyright (c) 2004-2009 Un4seen Developments Ltd.

	See the BASSFLAC.CHM file for more detailed documentation
*/

#ifndef BASSFLAC_H
#define BASSFLAC_H

#include "bass.h"

#if BASSVERSION!=0x204
#error conflicting BASS and BASSFLAC versions
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BASSFLACDEF
#define BASSFLACDEF(f) WINAPI f
#endif

// BASS_CHANNELINFO type
#define BASS_CTYPE_STREAM_FLAC		0x10900
#define BASS_CTYPE_STREAM_FLAC_OGG	0x10901

HSTREAM BASSFLACDEF(BASS_FLAC_StreamCreateFile)(BOOL mem, const void *file, QWORD offset, QWORD length, DWORD flags);
HSTREAM BASSFLACDEF(BASS_FLAC_StreamCreateURL)(const char *url, DWORD offset, DWORD flags, DOWNLOADPROC *proc, void *user);
HSTREAM BASSFLACDEF(BASS_FLAC_StreamCreateFileUser)(DWORD system, DWORD flags, const BASS_FILEPROCS *procs, void *user);

#ifdef __cplusplus
}
#endif

#endif
