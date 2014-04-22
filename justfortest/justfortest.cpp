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

void transfer_info_display(Transfer_Info_Ptr tip){
	for(int i = 0; i < tip->status_size; i++){
		cout<<i<<"行"<<"的状态值依次是：";
		for(int j = 0; j < tip->letters_size; j++){
			status_display((tip->transfer_table)[i][j]);
		}
		cout<<endl;
	}
	cout<<endl;
}

void reg_to_NFA(char* letters, int letters_size){
	Calculating_Stack cs;
	Reg_Exp_Letter_Stack rels;

	//读文件
	ifstream ifs("input.txt", ios::in);
	ifs>>letters_size;
	for(int i = 0 ; i < letters_size; i++){
		ifs>>letters[i];
	}

	char reg_exp[100];
	ifs.getline(reg_exp, 100, '\n');	//吸收末尾的换行符
	ifs.getline(reg_exp, 100, '\n');	//读正规式

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
			
			//transfer_info_display(tip2);
			//transfer_info_display(tip1);

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
	transfer_info_display(tip);
	/*********************************************************/
	delete reg_exp_formated;
}

/************************************************************************/
/* 以下是词法分析器中的一些函数和变量                                                                     */
/************************************************************************/
char letters[N];
int letters_size;
char tokens[300][15] = {"char", "int", "short", "long", "signed", "unsigned", "float", "double", "const", "void", "enum", "struct", "union", "typeof", "static", "if", "else", "switch", "case", "default", "while", "do", "for", "break", "continue", "return", "sizeof", "#include", "#define", "+",  "++",  "+=", "-", "-=", "--", "*", "*=", "/", "/=", "<", "<=", ">", ">=", "=", "==", ">>", "<<", "!=", "&", "&&", "|", "||", "%", "%=", ";", "[", "]", "(", ")", "{", "}", "\'", "\"", ":", "?", ",", "整数", "浮点数", "标识符"};	//存储记号的类别，记号最长限制在15个字符
int token_count = 69;
int line_position[100];		//存储源程序经过整理之后每一行开始字符的位置信息，限制最多100行
Transfer_Info_Ptr tip1 = NULL;	//关键字、运算符、分隔符的转移信息
Transfer_Info_Ptr tip2 = NULL;	//整数常量和小数常量的转移信息
Transfer_Info_Ptr tip3 = NULL;	//标识符的转移信息
char loop_buffer[200];

void input_file_format(char* input_file){

}

void transfer_info_init1(Transfer_Info_Ptr& tip){
	tip->letters_size = letters_size;
	for(int i = 0; i < letters_size; i++){
		tip->letters[i] = letters[i];
	}

	tip->status[0] = new Element();
	status_init(tip->status[0], 0, -1, NULL);

	tip->status_size = 1;

	for(int i = 0; i < M; i++){
		for(int j = 0; j < N; j++){
			tip->transfer_table[i][j] = NULL;
		}
	}
	
}

void init_to_create_all_transfer_table(){
	//初始化参数letters和letters_size
	letters_size = 95;
	for(int i = 0; i < letters_size; i++){
		letters[i] = ' ' + i;
	}

	//构造运算符和分隔符的转移表
	tip1 = new Transfer_Info();
	transfer_info_init1(tip1);
	for(int i = 0; i < 66; i++){
		int index_i = 0;	//从0号下标出发
		int index_i_pre = index_i;
		int j = 0;
		for( ; tokens[i][j] != '\0'; j++){
			if((tip1->transfer_table[index_i][tokens[i][j] - ' ']) == NULL){
				Status s = new Element();
				status_init(s, tip1->status_size, -1, NULL);
				(tip1->transfer_table[index_i][tokens[i][j] - ' ']) = s;
				tip1->status[tip1->status_size] = status_union(NULL, s);
				index_i_pre = index_i;
				index_i = tip1->status_size;
				tip1->status_size++;
			}else{
				index_i_pre = index_i;
				index_i = (tip1->transfer_table[index_i][tokens[i][j] - ' '])->index;
			}
		}
		(tip1->transfer_table[index_i_pre][tokens[i][j - 1] - ' '])->end_flag = i;
		(tip1->status[(tip1->transfer_table[index_i_pre][tokens[i][j - 1] - ' '])->index])->end_flag = i;
	}
}

void cifafenxi(char* input_file){

}


void main(){

	init_to_create_all_transfer_table();

	cifafenxi("yuanchengxu.txt");
}