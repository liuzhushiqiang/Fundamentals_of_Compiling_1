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
	if(c == '.' || c == '|' || c == '(' || c == ')' || c == '#'){
		return 1;
	}else{
		return 0;
	}
}

int compare_talbe[5][5] = {{1,1,-1,1,1}, 
{-1,1,-1,1,1}, {-1,-1,-1,0,-2}, {1,1,-2,1,1}, {-1,-1,-1,-2,0}};
char op_letters[5] = {'.', '|', '(', ')', '#'};

int condition4(Reg_Exp_Letter_Stack& rels, char c){		//���������������ڱȽϵ�ǰ�����������ջ����������������ȼ�
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
		cout<<i<<"��"<<"��״ֵ̬�����ǣ�";
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

	//���ļ�
	ifstream ifs("input.txt", ios::in);
	ifs>>letters_size;
	for(int i = 0 ; i < letters_size; i++){
		ifs>>letters[i];
	}

	char reg_exp[100];
	ifs.getline(reg_exp, 100, '\n');	//����ĩβ�Ļ��з�
	ifs.getline(reg_exp, 100, '\n');	//������ʽ

	//������ʽ���ַ������и�ʽ��
	char* reg_exp_formated = format_reg_exp(reg_exp);
	cout<<reg_exp_formated<<endl;
	/*********************************************************/

	//��ʼ������ջ
	calculating_stack_init(cs);
	reg_exp_letter_stack_init(rels);

	//�ѵ�һ����#��ѹջ
	reg_exp_letter_stack_push(rels, '#');

	//������ʽ���ַ������Ѿ���ʽ��������ɨ�裬�𲽹���NFA
	for(int i = 1; reg_exp_formated[i] != '\0'; i++){
		if(reg_exp_formated[i] == '*'){		//һԪ��������*����������
			Transfer_Info_Ptr tip = NULL;
			calculating_stack_pop(cs, tip);
			transfer_info_calculate_closure(tip);
			calculating_stack_push(cs, tip);
		}else if(!condition3(reg_exp_formated[i])){		//���ǲ�����
			Transfer_Info_Ptr tip = NULL;
			tip = new Transfer_Info();
			transfer_info_init(tip, letters, letters_size, reg_exp_formated[i]);
			calculating_stack_push(cs, tip);
		}else if(condition4(rels, reg_exp_formated[i]) == 1){	//�ǲ������������ȼ�С��ջ��Ԫ��
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
			i = i - 1;	//���´�ѭ����iֵ����
		}else if(condition4(rels, reg_exp_formated[i]) == 0){	//�ǲ������������ȼ�����ջ��Ԫ��
			char c = ' ';
			reg_exp_letter_stack_pop(rels, c);
		}else{		//�ǲ������������ȼ�����ջ��Ԫ��
			reg_exp_letter_stack_push(rels, reg_exp_formated[i]);
		}
	}

	//��ӡ����״̬ת�Ʊ�
	Transfer_Info_Ptr tip = NULL;
	calculating_stack_pop(cs, tip);
	transfer_info_display(tip);
	/*********************************************************/
	delete reg_exp_formated;
}

/************************************************************************/
/* �����Ǵʷ��������е�һЩ�����ͱ���                                                                     */
/************************************************************************/
char letters[N];
int letters_size;
char tokens[300][15] = {"char", "int", "short", "long", "signed", "unsigned", "float", "double", "const", "void", "enum", "struct", "union", "typeof", "static", "if", "else", "switch", "case", "default", "while", "do", "for", "break", "continue", "return", "sizeof", "#include", "#define", "+",  "++",  "+=", "-", "-=", "--", "*", "*=", "/", "/=", "<", "<=", ">", ">=", "=", "==", ">>", "<<", "!=", "&", "&&", "|", "||", "%", "%=", ";", "[", "]", "(", ")", "{", "}", "\'", "\"", ":", "?", ",", "����", "������", "��ʶ��"};	//�洢�Ǻŵ���𣬼Ǻ��������15���ַ�
int token_count = 69;
int line_position[100];		//�洢Դ���򾭹�����֮��ÿһ�п�ʼ�ַ���λ����Ϣ���������100��
Transfer_Info_Ptr tip1 = NULL;	//�ؼ��֡���������ָ�����ת����Ϣ
Transfer_Info_Ptr tip2 = NULL;	//����������С��������ת����Ϣ
Transfer_Info_Ptr tip3 = NULL;	//��ʶ����ת����Ϣ
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
	//��ʼ������letters��letters_size
	letters_size = 95;
	for(int i = 0; i < letters_size; i++){
		letters[i] = ' ' + i;
	}

	//����������ͷָ�����ת�Ʊ�
	tip1 = new Transfer_Info();
	transfer_info_init1(tip1);
	for(int i = 0; i < 66; i++){
		int index_i = 0;	//��0���±����
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