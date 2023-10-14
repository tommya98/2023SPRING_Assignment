#include <stdio.h>
extern int add(int x, int y);
extern int fibo(int n);

int main(void) {
	int x = 4;
	int y = 6;
	
	x = add(x, y);
	x = fibo(x);
	
	return 0;
}
