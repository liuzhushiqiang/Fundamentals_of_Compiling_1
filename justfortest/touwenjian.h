using namespace std;

#define M 1000	//״̬��
#define N 100	//��ĸ���С
#define END_MAX 20	//��̬�ĸ���
#define STACK_SIZE 50

typedef struct Element{
	int index;		//״̬��Ԫ�ص��±�
	struct Element * next_element;
	int end_flag;	//��ʾ��̬����Ӧ�ļǺŵı�ţ�����̬��ֵΪ-1.
} Element, *Status;		//�����ʹ���״̬�е�Ԫ�أ�ָ������͵�ָ�����һ��״̬��

typedef struct {
	Status status[M];	//����״̬
	int status_size;	//״̬����
	Status transfer_table[M][N];	//״̬ת�Ʊ�
	char letters[N];	//������ĸ
	int letters_size;	//��ĸ����
	int the_end[END_MAX];	//��̬��status������±�	
	int the_end_size;
} Transfer_Info, *Transfer_Info_Ptr;

typedef struct{
	Transfer_Info_Ptr tip[STACK_SIZE];	//�±�Ϊ0��λ�ò��ã���Ϊ�ڱ����ж�ջ���ã�
	int head;
} Calculating_Stack;	//�洢��������м�����ת����Ϣ����Transfer_Info��

typedef struct{
	char c[STACK_SIZE];	//�±�Ϊ0��λ�ò��ã���Ϊ�ڱ����ж�ջ���ã�
	int head;
} Reg_Exp_Letter_Stack;		//�洢����ʽ�������

/************************************************************************/
/*    functions about Status                                                                  */
/************************************************************************/
void status_init(Status& s1, int index, int end_flag, Status s2){
	s1->index = index;
	s1->next_element = s2;
	s1->end_flag = end_flag;
}

Status status_union(Status s1, Status s2){		//���ϲ�����,����һ���µļ���
	Status ret = NULL;
	if(s1 == NULL && s2 == NULL){
		return ret;
	}
	if(s1 == NULL){
		ret = new Element();
		ret->index = s2->index;
		ret->next_element = NULL;
		Status q = ret;
		for(Status p = s2->next_element; p != NULL; p = p->next_element, q = q->next_element){
			Status t = new Element();
			t->index = p->index;
			t->next_element = NULL;
			q->next_element = t;
		}
		return ret;
	}
	if(s2 == NULL){
		ret = new Element();
		ret->index = s1->index;
		ret->next_element = NULL;
		Status q = ret;
		for(Status p = s1->next_element; p != NULL; p = p->next_element, q = q->next_element){
			Status t = new Element();
			t->index = p->index;
			t->next_element = NULL;
			q->next_element = t;
		}
		return ret;
	}
	Status s3 = s1;
	Status s4 = s2;
	Status tail = NULL;

	if(s3->index > s4->index){		//�ҵ�һ��Ԫ��
		Status s5 = new Element();
		s5->index = s4->index;
		s5->next_element = NULL;
		ret = s5;
		tail = s5;
		s4 = s4->next_element;
	}else if(s3->index == s4->index){
		Status s5 = new Element();
		s5->index = s3->index;
		s5->next_element = NULL;
		ret = s5;
		tail = s5;
		s3 = s3->next_element;
		s4 = s4->next_element;
	}else if(s3->index < s4->index){
		Status s5 = new Element();
		s5->index = s3->index;
		s5->next_element = NULL;
		ret = s5;
		tail = s5;
		s3 = s3->next_element;
	}

	while(s3 != NULL && s4 != NULL){
		if(s3->index > s4->index){
			Status s5 = new Element();
			s5->index = s4->index;
			s5->next_element = NULL;
			tail->next_element = s5;
			tail = s5;
			s4 = s4->next_element;
		}else if(s3->index == s4->index){
			Status s5 = new Element();
			s5->index = s3->index;
			s5->next_element = NULL;
			tail->next_element = s5;
			tail = s5;
			s3 = s3->next_element;
			s4 = s4->next_element;
		}else if(s3->index < s4->index){
			Status s5 = new Element();
			s5->index = s3->index;
			s5->next_element = NULL;
			tail->next_element = s5;
			tail = s5;
			s3 = s3->next_element;
		}
	}

	if(s3 == NULL){
		while(s4 != NULL){
			Status s5 = new Element();
			s5->index = s4->index;
			s5->next_element = NULL;
			tail->next_element = s5;
			tail = s5;
			s4 = s4->next_element;
		}
	}

	if(s4 == NULL){
		while(s3 != NULL){
			Status s5 = new Element();
			s5->index = s3->index;
			s5->next_element = NULL;
			tail->next_element = s5;
			tail = s5;
			s3 = s3->next_element;
		}
	}

	return ret;
}

Status status_intersect(Status s1, Status s2){		//���Ͻ����㣬����һ���µļ���
	Status ret = NULL;
	if(s1 == NULL){
		return ret;
	}
	if(s2 == NULL){
		return ret;
	}
	Status s3 = s1;
	Status s4 = s2;
	Status tail = NULL;
	while(s3 != NULL && s4 != NULL){
		if(s3->index > s4->index){
			s4 = s4->next_element;
		}else if(s3->index == s4->index){
			Status s5 = new Element();
			s5->index = s3->index;
			s5->next_element = NULL;
			if(ret == NULL){
				ret = s5;
				tail = s5;
			}else{
				tail->next_element = s5;
				tail = s5;
			}
			s3 = s3->next_element;
			s4 = s4->next_element;
		}else if(s3->index < s4->index){
			s3 = s3->next_element;
		}
	}
	tail->next_element = NULL;
	return ret;
}

int status_compare(Status s1, Status s2){		//���ϵıȽϣ���ȷ���1������ȷ���0
	Status p = s1;
	Status q = s2;
	while(p != NULL && q != NULL){
		if(p->index != q->index){
			return 0;
		}
		p = p->next_element;
		q = q->next_element;
	}
	if(p == q){
		return 1; 
	}else{
		return 0;
	}
}

int status_add_element(Status &s1, int n){		//�ڼ��������һ��Ԫ�أ��ɹ�����1�����ɹ�����0����Ԫ���Ѿ����ڣ�
	Status p = s1;
	Status e = new Element();
	e->index = n;
	e->next_element = NULL;
	Status q = NULL;
	if(p == NULL){
		s1 = e;
		return 1;
	}
	if(p->index > n){
		e->next_element = p;
		s1 = e;
		return 1;
	}
	while(p != NULL && p->index < n){
		q = p;
		p = p->next_element;
	}
	if(p == NULL){
		q->next_element = e;
		e->next_element = p;
		return 1;
	}else if(p->index == n){
		return 0;
	}else if(p->index > n){
		q->next_element = e;
		e->next_element = p;
		return 1;
	}
}

int status_delete_element(Status &s1, int n){		//�ڼ�����ɾ��һ��Ԫ�أ��ɹ�����1�� ���ɹ�����0����Ԫ�ز����ڣ�
	Status p = s1;
	Status q = NULL;
	if(p->index == n){
		s1 = s1->next_element;
		delete(p);
		return 1;
	}
	while(p != NULL && p->index != n){
		q = p;
		p = p->next_element;
	}
	if(p == NULL){
		return 0;
	}else{
		q->next_element = p->next_element;
		delete(p);
		return 1;
	}
}

void status_destroy(Status &s){		//���ټ���
	Status p = s;
	s = NULL;
	while(p == NULL){
		return;
	}
	Status q = p->next_element;
	while (q != NULL)
	{
		delete(p);
		p = q;
		q = q->next_element;
	}
	delete(p);
};

void status_display(Status p){		//��ӡ�����е�Ԫ��
	if(p == NULL){
		cout<<"( ) ";
		return;
	}
	cout<<"(";
	while(p != NULL){
		cout<<p->index;
		p = p->next_element;
	}
	cout<<") ";
}

void status_create(Status &s, int* a, int n){		//���������е�Ԫ�ش���һ�����ϣ�aΪ���飬�����е�ֵΪ������������Ԫ�ص��±꣬nΪ�����С
	for(int i = 0; i < n; i++){
		status_add_element(s, a[i]);
	}
}

/************************************************************************/
/*   functions about Transfer_Info                                                                 */
/************************************************************************/


void transfer_info_init(Transfer_Info_Ptr& tip1, char* letters, int n, char c){
	tip1->status_size = 2;
	tip1->the_end_size = 1;

	tip1->status[0] = new Element();
	status_init(tip1->status[0], 0, -1, NULL);

	tip1->status[1] = new Element();
	status_init(tip1->status[1], 1, -1,  NULL);

	tip1->the_end[0] = 1;
	
	tip1->letters_size = n;
	for(int i = 0; i < n; i++){
		tip1->letters[i] = letters[i];
	}

	for(int i = 0; i < tip1->status_size; i++){
		for(int j = 0; j < tip1->letters_size; j++){
			if(i == 0 && letters[j] == c){
				Status s = new Element();
				status_init(s, 1, -1, NULL);
				tip1->transfer_table[i][j] = s;
			}else{
				tip1->transfer_table[i][j] = NULL;
			}
		}
	}
}

void transfer_info_add_index(Transfer_Info_Ptr& tip, int added_value){
	for(int i = 0; i < tip->status_size; i++){
		if(tip->status[i] != NULL){
			for(Status s = tip->status[i]; s != NULL; s = s->next_element){
				s->index += added_value;
			}
		}
		
		for(int j = 0; j < tip->letters_size; j++){
			if(tip->transfer_table[i][j] != NULL){
				for(Status s = tip->transfer_table[i][j]; s != NULL; s = s->next_element){
					s->index += added_value;
				}
			}
		}
	}
}

void status_add_index(Status& s, int added_value){	
	for(Status p = s; p != NULL; p = p->next_element){
		p->index += added_value;
	}
}

void transfer_info_merge(Transfer_Info_Ptr& tip1, Transfer_Info_Ptr& tip2, char reg_exp_letter){	//����һЩ����ϵ��ϰ�����ʱ�����Ƕ����̬���������ŵ�Ƕ�׵�����Ҫ���ϲ��ӽṹ�������⣨���ϲ���ʱ��ֻ�������Ե���չ��������Ĭ��transfer_table�е�һ���ǳ�̬�����һ������̬��
	if(reg_exp_letter == '.'){
		int tip1_status_size = tip1->status_size;
		int tip2_status_size = tip2->status_size;
		
		//��tip2����Ԥ����
		transfer_info_add_index(tip2, tip1->status_size - 1);

		//��tip2�ĳ�̬��tip1����̬�ϲ�
		for(int i = 0; i < tip1->letters_size; i++){
				tip1->transfer_table[tip1->the_end[0]][i] = status_union(tip1->transfer_table[tip1->the_end[0]][i], tip2->transfer_table[0][i]);
		}

		//��tip2��transfer_table�е�����״̬��Ϊ��״̬���뵽tip1��transfer_table��
		for(int i = 1; i < tip2->status_size; i++){
			tip1->status[tip1->status_size + i - 1] = tip2->status[i];
			for (int j = 0; j < tip1->letters_size; j++){
				tip1->transfer_table[tip1->status_size + i - 1][j] = tip2->transfer_table[i][j];
			}
		}

		//�޸�������Ϣ
		tip1->the_end[0] = tip2->the_end[0] + tip1->status_size - 1;
		tip1->status_size = tip1->status_size + tip2->status_size - 1;
		
	}else if(reg_exp_letter == '|'){
		//��tip1����̬����������λ��
		tip1->status[tip1->the_end[0] + tip2->status_size - 2] = status_union(NULL,tip1->status[tip1->the_end[0]]);
		status_add_index(tip1->status[tip1->the_end[0] + tip2->status_size - 2], tip2->status_size - 2);
		for(int i = 0; i < tip1->letters_size; i++){
			tip1->transfer_table[tip1->the_end[0] + tip2->status_size - 2][i] = status_union(NULL,tip1->transfer_table[tip1->the_end[0]][i]);
		}


		//��tip1��ָ��ԭ����̬�Ľ������ָ���µ���̬
		for(int i = 0; i < tip1->status_size - 1; i++){
			for(int j = 0; j < tip1->letters_size; j++){
				if(status_compare(tip1->transfer_table[i][j], tip1->status[tip1->the_end[0]])){
					tip1->transfer_table[i][j] = status_union(NULL, tip1->status[tip1->the_end[0] + tip2->status_size - 2]);
				}else{
					continue;
				}
			}
		}
		tip1->the_end[0] = tip1->the_end[0] + tip2->status_size - 2;

		//tip2�ĳ�̬�ϲ���tip1�ĳ�̬
		for(int i = 0; i < tip1->letters_size; i++){
			if(status_compare(tip2->transfer_table[0][i], tip2->status[0])){
				tip1->transfer_table[0][i] = status_union(tip1->transfer_table[0][i], tip1->status[0]);
			}else if(status_compare(tip2->transfer_table[0][i], tip2->status[tip2->the_end[0]])){
				tip1->transfer_table[0][i] = status_union(tip1->transfer_table[0][i], tip1->status[tip1->the_end[0]]);

			}else{
				status_add_index(tip2->transfer_table[0][i], tip1->status_size - 2);
				tip1->transfer_table[0][i] = status_union(tip1->transfer_table[0][i], tip2->transfer_table[0][i]);
			}
		}

		//tip2����̬�ϲ���tip1����̬
		for(int i = 0; i < tip1->letters_size; i++){
			if(status_compare(tip2->transfer_table[tip2->the_end[0]][i], tip2->status[0])){
				tip1->transfer_table[tip1->the_end[0]][i] = status_union(tip1->transfer_table[tip1->the_end[0]][i], tip1->status[0]);
			}else if(status_compare(tip2->transfer_table[tip2->the_end[0]][i], tip2->status[tip2->the_end[0]])){
				tip1->transfer_table[tip1->the_end[0]][i] = status_union(tip1->transfer_table[tip1->the_end[0]][i], tip1->status[tip1->the_end[0]]);

			}else{
				status_add_index(tip2->transfer_table[tip2->the_end[0]][i], tip1->status_size - 2);
				tip1->transfer_table[tip1->the_end[0]][i] = status_union(tip1->transfer_table[tip1->the_end[0]][i], tip2->transfer_table[tip2->the_end[0]][i]);
			}
		}
		


		//�ϲ�����(�ǳ�̬�ͷ���̬)
		for(int i = 1; i < tip2->status_size - 1; i++){
			for(int j = 0; j < tip2->letters_size; j++){
				if(status_compare(tip2->transfer_table[i][j], tip2->status[0])){
					tip1->transfer_table[i + tip1->status_size - 2][j] = status_union(NULL, tip1->status[0]);
				}else if(status_compare(tip2->transfer_table[i][j], tip2->status[tip2->the_end[0]])){
					tip1->transfer_table[i + tip1->status_size - 2][j] = status_union(NULL, tip1->status[tip1->the_end[0]]);

				}else{
					status_add_index(tip2->transfer_table[i][j], tip1->status_size - 2);
					tip1->transfer_table[i + tip1->status_size - 2][j] = status_union(NULL, tip2->transfer_table[i][j]);
				}
			}
		}


		//�޸�������Ϣ
		for(int i = 1; i < tip2->status_size - 1; i++){
			tip1->status[tip1->status_size + i - 2] = tip2->status[i];
		}
		tip1->status_size = tip1->status_size + tip2->status_size - 2;
	}
}

void transfer_info_calculate_closure(Transfer_Info_Ptr& tip){	//������ü���򵥣�ֻ�ܴ���a*�����ܴ���ab��*�����ٶ�tip��ԭ��ֻ������״̬
	for(int i = 0; i < tip->letters_size; i++){
		if(tip->transfer_table[0][i] != NULL){
			tip->transfer_table[0][i] = status_union(NULL, tip->status[0]);
		}
	}
	tip->the_end[0] = 0;
	tip->status_size = 1;
}

void transfer_info_destroy(Transfer_Info_Ptr& tip){
	delete[] (tip->status);
	for(int i = 0; i < tip->status_size; i++){
		delete[] tip->transfer_table[i];
	}

	delete tip;		//���delete tip��
	
}






/************************************************************************/
/* functions about Calculating_Stack                                                                */
/************************************************************************/
void calculating_stack_init(Calculating_Stack& cs){
	cs.head = 0;
}

int calculating_stack_push(Calculating_Stack& cs, Transfer_Info_Ptr tip){
	if(cs.head + 1 < STACK_SIZE){
		cs.head++;
		cs.tip[cs.head] = tip;   
		return 1;
	}else{
		return 0;
	}
}

int calculating_stack_pop(Calculating_Stack& cs, Transfer_Info_Ptr& tip){
	if(cs.head > 0){
		tip = cs.tip[cs.head];
		cs.tip[cs.head] = NULL;
		cs.head--;
		return 1;
	}else{
		return 0;
	}
}

int calculating_stack_gettop(Calculating_Stack& cs, Transfer_Info_Ptr& tip){
	if(cs.head > 0){
		tip = cs.tip[cs.head];
		return 1;
	}else{
		return 0;
	}
}


/************************************************************************/
/*    function of Reg_Exp_Letter_Stack                                                                  */
/************************************************************************/

void reg_exp_letter_stack_init(Reg_Exp_Letter_Stack& rels){
	rels.head = 0;
}

int reg_exp_letter_stack_push(Reg_Exp_Letter_Stack& rels, char c){
	if(rels.head + 1 < STACK_SIZE){
		rels.head++;
		rels.c[rels.head] = c;   
		return 1;
	}else{
		return 0;
	}
}

int reg_exp_letter_stack_pop(Reg_Exp_Letter_Stack& rels, char& c){
	if(rels.head > 0){
		c = rels.c[rels.head]; 
		rels.c[rels.head] = ' ';
		rels.head--;
		return 1;
	}else{
		return 0;
	}
}

int reg_exp_letter_stack_gettop(Reg_Exp_Letter_Stack& rels, char& c){
	if(rels.head > 0){
		c = rels.c[rels.head]; 
		return 1;
	}else{
		return 0;
	}
}