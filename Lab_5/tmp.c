#include <stdio.h>
int main(int argc, char** argv) {
	if(argc == 1){
		printf("%s\n",argv[0]);
	} else {
	 	printf("%s\n",argv[1]);	
	}
	char gowno[256];
	scanf("%s",gowno);
	printf("%s\n",gowno);
}
