#include <iostream>
#include<fstream>
#include "touwenjian.h"

int condition1(char c){		
	if(c == '|' || c == '('){
		return 1;
	}else{
		return 0;
	}
}

int condition2(char c){
	if(c == '|' || c == '*' || c == ')' || c == '\0'){
		return 1;
	}else{
		return 0;
	}
}

char* format_reg_exp(char* reg_exp){
	char* ret = new char[100];
	int index = 0;
	ret[0] = '#';
	index++;
	for(int i = 0; i < strlen(reg_exp); i++){
		if(!condition1(reg_exp[i]) && !condition2(reg_exp[i+1])){
			ret[index] = reg_exp[i];
			index++;
			ret[index] = '.';
			index++;
		}else{
			ret[index] = reg_exp[i]; 
			index++;
		}
	}
	ret[index] = '#';
	index++;
	ret[index] = '\0';
	index++;
	return ret;
}



void main(){
	ifstream ifs("input.txt", ios::in);
	char reg_exp[100];
	ifs.getline(reg_exp, 100, '\n');
	char* reg_exp_formated = format_reg_exp(reg_exp);
	cout<<reg_exp_formated<<endl;


	delete reg_exp_formated;
	cout<<reg_exp_formated<<endl;
}