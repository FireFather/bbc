#ifndef NNUE_H
#define NNUE_H

#ifndef __cplusplus
#ifndef _MSC_VER
#include <stdalign.h>
#endif
#endif

#ifdef __cplusplus
#   define EXTERNC extern "C"
#else
#   define EXTERNC
#endif

#if defined (_WIN32)
#   define _CDECL __cdecl
#ifdef DLL_EXPORT
#   define DLLExport EXTERNC __declspec(dllexport)
#else
#   define DLLExport EXTERNC __declspec(dllimport)
#endif
#else
#   define _CDECL
#   define DLLExport EXTERNC
#endif

#include <cstdint>
/*
enum colors {
    white,black
};
*/
enum pieces
{
	blank=0,
	wking,
	wqueen,
	wrook,
	wbishop,
	wknight,
	wpawn,
	bking,
	bqueen,
	brook,
	bbishop,
	bknight,
	bpawn
};

using DirtyPiece = struct DirtyPiece
{
	int dirtyNum;
	int pc[3];
	int from[3];
	int to[3];
};

using Accumulator = struct Accumulator
{
	alignas(64) int16_t accumulation[2][256];
	int computedAccumulation;
};

using NNUEdata = struct NNUEdata
{
	Accumulator accumulator;
	DirtyPiece dirtyPiece;
};

using Position = struct Position
{
	int player;
	int* pieces;
	int* squares;
	NNUEdata* nnue[3];
};

int nnue_evaluate_pos(const Position* pos);

#ifdef __cplusplus
extern "C" {
#endif

void nnue_init(
	const char* eval_file
);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int nnue_evaluate_fen(
	const char* fen
);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int nnue_evaluate(
	int player,
	int* pieces,
	int* squares
);
#ifdef __cplusplus
}
#endif

int _CDECL nnue_evaluate_incremental(
	int player,
	int* pieces,
	int* squares,
	NNUEdata** nnue_data
);

#endif
