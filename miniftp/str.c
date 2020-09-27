#include"str.h"

//去掉命令末尾的回车或者换行
void str_trim_crlf(char *str)
{
	assert(str != NULL);
	char *p = str+(strlen(str)-1);
	while (*p == '\n' || *p == '\r')
	{
		*p-- ='\0';
	}
}

//将命令分割
void str_split(const char *str,char *left, char *right, char c)
{
	assert(str != NULL);
	char *pos = strchr(str,c);
	if (!pos)
		strcpy(left,str);
	else
	{
		strncpy(left,str,pos-str);
		strcpy(right,pos+1);
	}
}

//转化为大写
void str_upper(char *str)
{
	while(*str)
	{
		if(*str>='a' && *str<='z')
			*str -= 32;
		str++;
	}
}