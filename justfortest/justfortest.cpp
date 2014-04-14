#include <iostream>
#include<fstream>
#include "touwenjian.h"

int condition1(char c){		//����format_reg_exp����
	if(c == '|' || c == '('){
		return 1;
	}else{
		return 0;
	}
}

int condition2(char c){		//����format_reg_exp����
	if(c == '|' || c == '*' || c == ')' || c == '\0'){
		return 1;
	}else{
		return 0;
	}
}

int condition3(char c){		//����������
	if(c == '*' || c == '.' || c == '|' || c == '(' || c == ')' || c == '#'){
		return 1;
	}else{
		return 0;
	}
}

int condition4(Reg_Exp_Letter_Stack& rels, char c){		//���������������ڱȽϵ�ǰ�����������ջ����������������ȼ�
	//δ��
	return 0;
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

/*
������
*/
char letters[N] = {'a', 'b'};
int letters_size = 2;
Calculating_Stack cs;
Reg_Exp_Letter_Stack rels;


void main(){
	//���ļ�
	ifstream ifs("input.txt", ios::in);
	char reg_exp[100];
	ifs.getline(reg_exp, 100, '\n');

	//������ʽ���ַ������и�ʽ��
	char* reg_exp_formated = format_reg_exp(reg_exp);
	cout<<reg_exp_formated<<endl;
	/*********************************************************/

	//��ʼ������ջ
	calculating_stack_init(cs);
	reg_exp_letter_stack_init(rels);

	//������ʽ���ַ������Ѿ���ʽ��������ɨ�裬�𲽹���NFA
	for(int i = 0; reg_exp_formated[i] != '\0'; i++){
		if(!condition3(reg_exp_formated[i])){
			Transfer_Info_Ptr tip = NULL;
			transfer_info_init(tip, letters, letters_size);
			calculating_stack_push(cs, tip);
		}else if(condition4(rels, reg_exp_formated[i])){
			Transfer_Info_Ptr tip1 = NULL;
			Transfer_Info_Ptr tip2 = NULL;
			calculating_stack_pop(cs, tip1);
			calculating_stack_pop(cs, tip2);
			char c = ' ';
			reg_exp_letter_stack_pop(rels, c);
			transfer_info_merge(tip1, tip2, c);
			calculating_stack_push(cs, tip1);
			i = i - 1;	//���´�ѭ����iֵ����
		}else{
			reg_exp_letter_stack_push(rels, reg_exp_formated[i]);
		}
	}

	//��ӡ����״̬ת�Ʊ�
	Transfer_Info_Ptr tip = NULL;
	calculating_stack_pop(cs, tip);
	for(int i = 0; i < tip->status_size; i++){
		for(int j = 0; j < tip->letters_size; j++){
			status_display((tip->transfer_table)[i][j]);
		}
	}
	/*********************************************************/
	delete reg_exp_formated;
	cout<<reg_exp_formated<<endl;
}