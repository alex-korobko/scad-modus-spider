/*******************************************************************************
Programming - Alexander Klimenko
Project - MetroNET
Started at 27.12.00
Last updated at 03.01.01
Copyright (C) 2000, 2001 SCAD Ltd. (software development group)
*******************************************************************************/

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <Pt.h>
#include <stdarg.h>

//  #define WORD_OF (data, i) (word)((data>>i*16) & 0xFFFF)
#define LOBYTE(data)	(data & 0x00FF)
#define HIBYTE(data)	(data & 0xFF00) >> 8
word CRC(const byte* buffer, int size);

//#define ERROR(S) error_msg(__FILE__, __LINE__, S)
//#define ERROR error_msg(__FILE__, __LINE__)
#define ERRORS(S)
//#define INFO(S)
#define ERROR_MEMORY error_msg(__FILE__, __LINE__, "Fail to allocate memory")

#define ERROR_BOX(S) PtNotice(ABW_main_wnd, NULL, "Ошибка", ApGetImageRes(widget_dbase, "stopMsg"), S, (const char*)g_msgDialogFont, "Ok", (const char*)g_msgDialogFont, Pt_MODAL | Pt_CENTER)
#define INFO_BOX(S) PtNotice(ABW_main_wnd, NULL, "Информация", ApGetImageRes(widget_dbase, "infoMsg"), S, (const char*)g_msgDialogFont, "Ok", (const char*)g_msgDialogFont, Pt_MODAL | Pt_CENTER)
#define WARN_BOX(S) PtNotice(ABW_main_wnd, NULL, "Внимание", ApGetImageRes(widget_dbase, "warnMsg"), S, (const char*)g_msgDialogFont, "Ok", (const char*)g_msgDialogFont, Pt_MODAL | Pt_CENTER)


typedef unsigned long dword;
typedef unsigned short word;
typedef unsigned char byte;

void AddSysLogMessage(int type, const char* msg, int printErr = 0);
void error_msg(char* file_name, int line, const char* msg = NULL);
void SysMessage(int type, const char* message, ...);
long file_size(FILE *fp);

int extract_string(const char* src, char* dst, char first, char last);
int translate_string(const char* src, char* dst, struct PxTransCtrl* trans_set);

// Widget helpers
unsigned long get_widget_scalar(PtWidget_t *widget, long type);

//Save and Load escalator directions from/into file
int SaveDirections(const char* fileName);
int LoadDirections(const char* fileName);

#endif
