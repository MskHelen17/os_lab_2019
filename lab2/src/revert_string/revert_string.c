#include "revert_string.h"

void RevertString(char *str)
{
	int len = strlen(str);
    int i = 0;
    while(len>i){
    	char tmp = str[--len];
    	str[len]=str[i];
    	str[i++]=tmp;
    }

}
