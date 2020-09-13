#ifndef		_STR_H_
#define		_STR_H_

#include "common.h"

void str_trim_crlf(char *str);//去除末尾回车或者换行符
void str_split(const char *str,char *left, char *right, char c);


#endif	/*_STR_H_*/