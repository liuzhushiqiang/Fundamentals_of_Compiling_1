#include <iostream>
#include<fstream>
#include "touwenjian.h"

void main(){
	ifstream ifs("input.txt", ios::in);
	Status s = NULL;	//ʹ��֮ǰһ��Ҫ��ΪNULL
	Status t = NULL;
	Status p = NULL;

	int a[10] = {1,2,3,4};
	status_create(s, a, 4);
	status_display(s);
}