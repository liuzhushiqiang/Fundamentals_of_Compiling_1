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
char tokens[300][15] = {"char", "int", "short", "long", "signed", "unsigned", "float", "double", "const", "void", "enum", "struct", "union", "typeof", "static", "if", "else", "switch", "case", "default", "while", "do", "for", "break", "continue", "return", "sizeof", "#include", "#define", "+",  "++",  "+=", "-", "-=", "--", "*", "*=", "/", "/=", "<", "<=", ">", ">=", "=", "==", ">>", "<<", "!=", "&", "&&", "|", "||", "%", "%=", ";", "[", "]", "(", ")", "{", "}", "\'", "\"", ":", ".", ",", "����", "������", "��ʶ��"};	//�洢�Ǻŵ���𣬼Ǻ��������15���ַ�
int token_count = 69;	//Ԥ����Ǻŵ�������
int line_position[100];		//�洢Դ���򾭹�����֮��ÿһ�п�ʼ�ַ���λ����Ϣ���������100��,�ӡ���0�С���ʼ����
int no_use1 = 0;		//����������������line_total�ᱻ�ĳ�0������֣�Ӧ�����������Խ�硣������Ŀǰ��������line_positiont������Խ�硣
int line_total = 0;		//������
Transfer_Info_Ptr tip1 = NULL;	//�ؼ��֡���������ָ�����ת����Ϣ
Transfer_Info_Ptr tip2 = NULL;	//����������С��������ת����Ϣ
Transfer_Info_Ptr tip3 = NULL;	//��ʶ����ת����Ϣ
char loop_buffer[200];		//˫���壬�����ݴ����Ĳ���Դ����Դ���򾭹���ʽ����
char no_use2;	//loop_buffer������Ϊ��Խ��һ���ַ��������һ���ַ��ᱻ��\0'���ǵ�
typedef struct{//�Ǻ�����
	int index;
	char token[100];
}Lexical_Analysis_Result;
Lexical_Analysis_Result results[1000];
int results_size = 0;	//���н���мǺŵ��ܸ���

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

	
	//���쳣����ת�Ʊ�������С����
	tip2 = new Transfer_Info();
	transfer_info_init1(tip2);
	
	Status s1 = new Element();
	status_init(s1, tip2->status_size, -1, NULL);
	s1->end_flag = 67;
	tip2->status[tip2->status_size] = s1;
	tip2->status_size++;
	int index_presenet = 0;
	for(int i = 0; i < 9; i++){
		(tip2->transfer_table[index_presenet]['1' - ' ' + i]) = status_union(NULL, tip2->status[tip2->status_size - 1]);	
	}
	index_presenet = tip2->status_size - 1;
	for(int i = 0; i < 10; i++){
		(tip2->transfer_table[index_presenet]['0' - ' ' + i]) = status_union(NULL, tip2->status[tip2->status_size - 1]);	
	}
	Status s2 = new Element();
	status_init(s2, tip2->status_size, -1, NULL);
	s2->end_flag = 68;
	tip2->status[tip2->status_size] = s2;
	tip2->status_size++;
	(tip2->transfer_table[index_presenet]['.' - ' ']) = status_union(NULL, tip2->status[tip2->status_size - 1]);
	index_presenet = tip2->status_size - 1;
	for(int i = 0; i < 10; i++){
		(tip2->transfer_table[index_presenet]['0' - ' ' + i]) = status_union(NULL, tip2->status[tip2->status_size - 1]);	
	}

	//��������0��0.x�������
	Status s3 = new Element();
	status_init(s3, tip2->status_size, -1, NULL);
	s3->end_flag = 67;
	tip2->status[tip2->status_size] = s3;
	tip2->status_size++;
	index_presenet = 0;
	(tip2->transfer_table[index_presenet]['0' - ' ']) = status_union(NULL, tip2->status[tip2->status_size - 1]);
	index_presenet = tip2->status_size - 1;
	Status s4 = new Element();
	status_init(s4, tip2->status_size, -1, NULL);
	s4->end_flag = 68;
	tip2->status[tip2->status_size] = s4;
	tip2->status_size++;
	(tip2->transfer_table[index_presenet]['.' - ' ']) = status_union(NULL, tip2->status[tip2->status_size - 1]);
	index_presenet = tip2->status_size - 1;
	for(int i = 0; i < 10; i++){
		(tip2->transfer_table[index_presenet]['0' - ' ' + i]) = status_union(NULL, tip2->status[tip2->status_size - 1]);	
	}


	//�����ʶ����ת�Ʊ�
	tip3 = new Transfer_Info();
	transfer_info_init1(tip3);

	Status s5 = new Element();
	status_init(s5, tip3->status_size, -1, NULL);
	s5->end_flag = 69;
	tip3->status[tip3->status_size] = s5;
	tip3->status_size++;
	index_presenet = 0;
	for(int i = 0; i < 26; i++){
		(tip3->transfer_table[index_presenet]['A' - ' ' + i]) = status_union(NULL, tip3->status[tip3->status_size - 1]);	
	}
	for(int i = 0; i < 26; i++){
		(tip3->transfer_table[index_presenet]['a' - ' ' + i]) = status_union(NULL, tip3->status[tip3->status_size - 1]);	
	}
	index_presenet = tip3->status_size - 1;
	for(int i = 0; i < 26; i++){
		(tip3->transfer_table[index_presenet]['A' - ' ' + i]) = status_union(NULL, tip3->status[tip3->status_size - 1]);	
	}
	for(int i = 0; i < 26; i++){
		(tip3->transfer_table[index_presenet]['a' - ' ' + i]) = status_union(NULL, tip3->status[tip3->status_size - 1]);	
	}
	for(int i = 0; i < 10; i++){
		(tip3->transfer_table[index_presenet]['0' - ' ' + i]) = status_union(NULL, tip3->status[tip3->status_size - 1]);	
	}
}

void input_file_format(char* input_file){
	ifstream ifs = ifstream(input_file, ios::in);
	ofstream ofs = ofstream("temp.txt", ios::out | ios::app);		//temp.txtÿ�ζ�Ҫ�ֶ����
	char line_str[200];		//����Դ�ļ���ÿ�����200���ַ�
	char* word_str;		//����Դ�ļ���ÿ���������ַ������200���ַ�
	char delim[] = " \t";
	line_position[0] = 0;
	int i = 0;
	for( ; ifs.getline(line_str, 200, '\n'); i++){
		line_position[i + 1] = line_position[i];
		word_str = strtok(line_str, delim);
		while(word_str != NULL){
			ofs<<word_str<<" ";
			line_position[i + 1] += strlen(word_str) + 1;
			word_str = strtok(NULL, delim);
		}
		//cout<<line_position[i]<<endl;
	}
	line_total = i;
	ifs.close();
	ofs.close();
}

int calculate_line_position(int n){	//�����±�nλ�õ��ַ��������к�
	for(int i = 0; i < line_total; i++){
		if(line_position[i] > n){
			return i;		//ʵ�ʵ��к��Ǵ�1��ʼ��,������Ϊ����ıȽ������ǡ�>��������ֱ��return i��������return i + 1
		}
	}
}

int cifafenxi(char* input_file){
	input_file_format(input_file);
	ifstream ifs = ifstream("temp.txt", ios::in);
	int flag1 = 0;
	int flag2 = 0;
	int flag3 = 0;
	int position_count = 0;		//��¼��ǰ��������ַ����������ڳ����ʱ������кŵ�ȷ��
	//��ʼ������һ�ε�ʱ��ִֻ������ġ����ļ���������whileѭ���еġ����ļ������ᱻִ��
	char *p_start = loop_buffer;
	char *p_end = loop_buffer;
	
	ifs.get(loop_buffer, 101);		//��Ϊ������������������101,  ������һ��bug����Դ�ļ��ܳ���С��100��ʱ��������ӵ�whileѭ������ִ�С�
	//cout<<loop_buffer<<endl;
	while (!ifs.eof())
	{
		int flag10 = 0;		//����Ϊ�˴���ifs.get(loop_buffer + 100, 101);��֮��loop_buffer + 100 = p_end���µĲ��ܽ���forѭ��������
		if(p_end - loop_buffer == 100){
			ifs.get(loop_buffer + 100, 101);//��ʱ��p_end�պ�ָ��loop_buffer + 100,���Բ�Ҫ�޸�p_end��ֵ
			//cout<<loop_buffer + 100<<endl;
			position_count += 100;
			flag10 = 1;
		}else if(p_end - loop_buffer == 200){
			ifs.get(loop_buffer, 101);
			//cout<<loop_buffer<<endl;
			p_end = loop_buffer;
			position_count += 100;
		}
		for( ; (p_end - loop_buffer) != 200 && ((p_end - loop_buffer) != 100 || flag10 == 1); p_end++){
			if(p_end[0] == '\0'){
				return 1;
			}
			//�ֱ�����ת�Ʊ���ȥ���ң����ת�Ƴɹ��������ת��ʧ���򱨴����������̬�������ҵ��¼Ǻ�	
			if (flag1 != -1){
				if(tip1->transfer_table[flag1][p_end[0] - ' '] == NULL){
					if(tip1->status[flag1]->end_flag == -1){
						flag1 = -1;
					}else{
						if(p_end - p_start < 0){
							char temp1[100] = "";
							char temp2[100] = "";
							strncpy(temp1, p_start, loop_buffer + 200 - p_start);
							strncpy(temp2, loop_buffer, p_end - loop_buffer);
							strcat(results[results_size].token, temp1);
							strcat(results[results_size].token, temp2);
							results[results_size].index = tip1->status[flag1]->end_flag;
							cout<<"<"<<results[results_size].index<<", "<<results[results_size].token<<">"<<endl;
							results_size++;
						}else{
							strncpy(results[results_size].token, p_start, p_end - p_start);
							results[results_size].index = tip1->status[flag1]->end_flag;
							cout<<"<"<<results[results_size].index<<", "<<results[results_size].token<<">"<<endl;
							results_size++;
						}
						flag1 = 0;
						flag2 = 0;
						flag3 = 0;
						if(p_end[0] != ' '){
							p_end--;	//ѭ����ʱ���1�պ��ν���
							p_start = p_end + 1;
						}else{
							p_start = p_end + 1;
						}
						continue;
					}
				}else{
					flag1 = tip1->transfer_table[flag1][p_end[0] - ' ']->index;
				}
			}
			if(flag2 != -1){
				if(tip2->transfer_table[flag2][p_end[0] - ' '] == NULL){
					if(tip2->status[flag2]->end_flag == -1){
						flag2 = -1;
					}else{
						if(p_end - p_start < 0){
							char temp1[100] = "";
							char temp2[100] = "";
							strncpy(temp1, p_start, loop_buffer + 200 - p_start);
							strncpy(temp2, loop_buffer, p_end - loop_buffer);
							strcat(results[results_size].token, temp1);
							strcat(results[results_size].token, temp2);
							results[results_size].index = tip2->status[flag2]->end_flag;
							cout<<"<"<<results[results_size].index<<", "<<results[results_size].token<<">"<<endl;
							results_size++;
						}else{
							strncpy(results[results_size].token, p_start, p_end - p_start);
							results[results_size].index = tip2->status[flag2]->end_flag;
							cout<<"<"<<results[results_size].index<<", "<<results[results_size].token<<">"<<endl;
							results_size++;
						}
						flag1 = 0;
						flag2 = 0;
						flag3 = 0;
						if(p_end[0] != ' '){
							p_end--;	//ѭ����ʱ���1�պ��ν���
							p_start = p_end + 1;
						}else{
							p_start = p_end + 1;
						}
						continue;
					}
				}else{
					flag2 = tip2->transfer_table[flag2][p_end[0] - ' ']->index;
				}
			}
			if(flag3 != -1){
				if(tip3->transfer_table[flag3][p_end[0] - ' '] == NULL){
					if(tip3->status[flag3]->end_flag == -1){
						flag3 = -1;
					}else{
						if(p_end - p_start < 0){
							char temp1[100] = "";
							char temp2[100] = "";
							strncpy(temp1, p_start, loop_buffer + 200 - p_start);
							strncpy(temp2, loop_buffer, p_end - loop_buffer);
							strcat(results[results_size].token, temp1);
							strcat(results[results_size].token, temp2);
							results[results_size].index = tip3->status[flag3]->end_flag;
							cout<<"<"<<results[results_size].index<<", "<<results[results_size].token<<">"<<endl;
							results_size++;
						}else{
							strncpy(results[results_size].token, p_start, p_end - p_start);
							results[results_size].index = tip3->status[flag3]->end_flag;
							cout<<"<"<<results[results_size].index<<", "<<results[results_size].token<<">"<<endl;
							results_size++;
						}
						flag1 = 0;
						flag2 = 0;
						flag3 = 0;
						if(p_end[0] != ' '){
							p_end--;	//ѭ����ʱ���1�պ��ν���
							p_start = p_end + 1;
						}else{
							p_start = p_end + 1;
						}
						continue;
					}
				}else{
					flag3 = tip3->transfer_table[flag3][p_end[0] - ' ']->index;
				}
			}
			if(flag1 == -1 && flag2 == -1 && flag3 == -1){
				char temp_print[100] = "";
				
				cout<<"ERROR: illegal identifier in line "<<calculate_line_position(position_count + ((p_start - loop_buffer) % 100))<<endl;
				return 0;
			}
		}
	}
	ifs.close();
}


void main(){

	init_to_create_all_transfer_table();

	cifafenxi("yuanchengxu.txt");
}