#include <stdio.h>
#include <string.h>

int main(void){
	char history[10][20];
	printf("%lu\n", sizeof(*history));

	for (int i = 0; i < 10; i++){
		strcpy(history[i], "hello");
	}

	strcpy(history[0], "hello");
	printf("%lu\n", sizeof(&history));
	for (int i = 0; i < 10; i++){
		printf("%s\n", history[i]);
	}
}