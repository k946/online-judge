#include<stdio.h>

int main(){
	int a[1024000] = {0};
	for(int i = 0; i < 1024000; i++){
		for(int j = 0; j < 1000; j++)
			a[i] = i;
	}

	return 0;

}