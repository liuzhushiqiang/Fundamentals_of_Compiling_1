#include <iostream>
#include<fstream>
#include <stdlib.h> 
#include "touwenjian.h"

/************************************************************************/
/* 以下是“词法分析器”中的一些函数和变量                                                                     */
/************************************************************************/
char letters[N];
int letters_size;
char tokens[300][15] = {"char", "int", "short", "long", "signed", "unsigned", "float", "double", "const", "void", "enum", "struct", "union", "typeof", "static", "if", "else", "switch", "case", "default", "while", "do", "for", "break", "continue", "return", "sizeof", "#include", "#define", "+",  "++",  "+=", "-", "-=", "--", "*", "*=", "/", "/=", "<", "<=", ">", ">=", "=", "==", ">>", "<<", "!=", "&", "&&", "|", "||", "%", "%=", ";", "[", "]", "(", ")", "{", "}", "\'", "\"", ":", ".", ",", "整数", "浮点数", "标识符"};	//存储记号的类别，记号最长限制在15个字符
int token_count = 69;	//预定义记号的种类数
int line_position[100];		//存储源程序经过整理之后每一行开始字符的位置信息，限制最多100行,从“第0行”开始计数
int line_total = 0;		//总行数
Transfer_Info_Ptr tip1 = NULL;	//关键字、运算符、分隔符的转移信息
Transfer_Info_Ptr tip2 = NULL;	//整数常量和小数常量的转移信息
Transfer_Info_Ptr tip3 = NULL;	//标识符的转移信息
char loop_buffer[201];		//双缓冲，用于暂存读入的部分源程序（源程序经过格式化）
typedef struct{//记号类型
	int index;
	char token[100];
}Lexical_Analysis_Result;
Lexical_Analysis_Result results[1000];
int results_size = 0;	//运行结果中记号的总个数

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

	
	//构造常量的转移表（整数、小数）
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

	//单独处理0或0.x这种情况
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


	//构造标识符的转移表
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
	ofstream ofs1 = ofstream("temp.txt", ios::out);		//temp.txt每次都要手动清空
	ofs1.close();
	ofstream ofs = ofstream("temp.txt", ios::app);		//temp.txt每次都要手动清空
	char line_str[200];		//限制源文件中每行最多200个字符
	char* word_str;		//限制源文件中每个独立的字符串最多200个字符
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

int calculate_line_position(int n){	//计算下标n位置的字符所属的行号
	for(int i = 0; i < line_total; i++){
		if(line_position[i] > n){
			return i;		//实际的行号是从1开始的,但是因为上面的比较运算是“>”，所以直接return i，而不是return i + 1
		}
	}
}

int cifafenxi(char* input_file){
	input_file_format(input_file);
	ifstream ifs = ifstream("temp.txt", ios::in);
	int flag1 = 0;
	int flag2 = 0;
	int flag3 = 0;
	int position_count = 0;		//记录当前处理过的字符个数，用于出错的时候进行行号的确定
	//初始化，第一次的时候只执行这里的“读文件”操作，while循环中的“读文件”不会被执行
	char *p_start = loop_buffer;
	char *p_end = loop_buffer;
	
	ifs.get(loop_buffer, 101);		//因为包括结束符，所以是101,  这里有一个bug，当源文件总长度小于100的时候下面紧接的while循环不会执行。
	//cout<<loop_buffer<<endl;
	while (!ifs.eof())
	{
		int flag10 = 0;		//这是为了处理“ifs.get(loop_buffer + 100, 101);”之后loop_buffer + 100 = p_end导致的不能进入for循环的问题
		if(p_end - loop_buffer == 100){
			ifs.get(loop_buffer + 100, 101);//这时候p_end刚好指向loop_buffer + 100,所以不要修改p_end的值
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
			//分别到三个转移表中去查找，如果转移成功则继续，转移失败则报错或者遇到终态则宣布找到新记号	
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
							p_end--;	//循环的时候加1刚好衔接上
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
							p_end--;	//循环的时候加1刚好衔接上
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
							p_end--;	//循环的时候加1刚好衔接上
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