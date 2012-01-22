#include <unistd.h>
#include <stdio.h>


void long2RGB( const long &pattern, int &r, int &g , int &b)
{
	r = (pattern & 0xff0000) >> 16;
	g = (pattern & 0x00ff00) >> 8;
	b = pattern & 0x0000ff;	
}




