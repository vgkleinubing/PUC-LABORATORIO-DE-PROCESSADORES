#include <stdio.h>
#include <math.h>
int main()
{
double x;
int k;
for(k=0; k<64; k++){
	x= (k+0.5)*M_PI/128;
	printf("%3.0lf,",510*sin(x));
	}
printf("};\n");
return 0;
}
