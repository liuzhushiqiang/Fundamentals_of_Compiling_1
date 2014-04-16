#include <iostream>
#include<fstream>
#include "touwenjian.h"

int condition1(char c){		//用于format_reg_exp（）
	if(c == '|' || c == '('){
		return 1;
	}else{
		return 0;
	}
}

int condition2(char c){		//用于format_reg_exp（）
	if(c == '|' || c == '*' || c == ')' || c == '\0'){
		return 1;
	}else{
		return 0;
	}
}

int condition3(char c){		//用于主函数
	if(c == '.' || c == '|' || c == '(' || c == ')' || c == '#'){
		return 1;
	}else{
		return 0;
	}
}

int compare_talbe[5][5] = {{1,1,-1,1,1}, 
{-1,1,-1,1,1}, {-1,-1,-1,0,-2}, {1,1,-2,1,1}, {-1,-1,-1,-2,0}};
char op_letters[5] = {'.', '|', '(', ')', '#'};

int condition4(Reg_Exp_Letter_Stack& rels, char c){		//用于主函数，用于比较当前正规操作符和栈顶正规操作符的优先级
	char stack_top_char = ' ';
	reg_exp_letter_stack_gettop(rels, stack_top_char);
	int index1;
	int index2;
	for(int i = 0; i < 5; i++){
		if(op_letters[i] == stack_top_char){
			index1 = i;
			break;
		}
	}
	for(int i = 0; i < 5; i++){
		if(op_letters[i] == c){
			index2 = i;
			break;
		}
	}
	return compare_talbe[index1][index2];
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
测试用
*/
char letters[N] = {'a', 'b'};
int letters_size = 2;
Calculating_Stack cs;
Reg_Exp_Letter_Stack rels;


void main(){
	//读文件
	ifstream ifs("input.txt", ios::in);
	char reg_exp[100];
	ifs.getline(reg_exp, 100, '\n');

	//对正规式的字符串进行格式化
	char* reg_exp_formated = format_reg_exp(reg_exp);
	cout<<reg_exp_formated<<endl;
	/*********************************************************/

	//初始化两个栈
	calculating_stack_init(cs);
	reg_exp_letter_stack_init(rels);

	//把第一个‘#’压栈
	reg_exp_letter_stack_push(rels, '#');

	//对正规式的字符串（已经格式化）进行扫描，逐步构造NFA
	for(int i = 1; reg_exp_formated[i] != '\0'; i++){
		if(reg_exp_formated[i] == '*'){		//一元操作符‘*’单独处理
			Transfer_Info_Ptr tip = NULL;
			calculating_stack_pop(cs, tip);
			transfer_info_calculate_closure(tip);
			calculating_stack_push(cs, tip);
		}else if(!condition3(reg_exp_formated[i])){		//不是操作符
			Transfer_Info_Ptr tip = NULL;
			tip = new Transfer_Info();
			transfer_info_init(tip, letters, letters_size, reg_exp_formated[i]);
			calculating_stack_push(cs, tip);
		}else if(condition4(rels, reg_exp_formated[i]) == 1){	//是操作符，但优先级小于栈顶元素
			Transfer_Info_Ptr tip1 = NULL;
			Transfer_Info_Ptr tip2 = NULL;
			calculating_stack_pop(cs, tip1);
			calculating_stack_pop(cs, tip2);
			char c = ' ';
			reg_exp_letter_stack_pop(rels, c);
			transfer_info_merge(tip2, tip1, c);
			calculating_stack_push(cs, tip2);
			i = i - 1;	//让下次循环的i值不变
		}else if(condition4(rels, reg_exp_formated[i]) == 0){	//是操作符，但优先级等于栈顶元素
			char c = ' ';
			reg_exp_letter_stack_pop(rels, c);
		}else{		//是操作符，但优先级大于栈顶元素
			reg_exp_letter_stack_push(rels, reg_exp_formated[i]);
		}
	}

	//打印最后的状态转移表
	Transfer_Info_Ptr tip = NULL;
	calculating_stack_pop(cs, tip);
	for(int i = 0; i < tip->status_size; i++){
		for(int j = 0; j < tip->letters_size; j++){
			cout<<i<<"行"<<j<<"列的状态值是：";
			status_display((tip->transfer_table)[i][j]);
		}
	}
	/*********************************************************/
	delete reg_exp_formated;
}