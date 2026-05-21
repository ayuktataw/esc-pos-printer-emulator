#include "test.h"

int main(){
	//open the /tmp/pos_printer file in write mode for binary writing
	FILE *fp = fopen("/tmp/pos_printer", "wb");
	if(fp == NULL){
		perror("Failed to open file");
		return 1;
	}
	//
	fn_test_underline(fp);
	fclose(fp);
	return 0;
}
