using namespace std;

#define M 1000	//状态数
#define N 70	//字母表大小
#define END_MAX 20	//终态的个数
#define STACK_SIZE 50

typedef struct Element{
	int index;		//状态中元素的下标
	struct Element * next_element;
} Element, *Status;		//该类型代表状态中的元素，指向该类型的指针代表一个状态。

typedef struct {
	Status status[M];	//所有状态
	int status_size;	//状态个数
	Status transfer_table[M][N];	//状态转移表
	char letters[N];	//所有字母
	int letters_size;	//字母个数
	Status the_start;
	Status the_end[END_MAX];	
	int the_end_size;
} Transfer_Info, *Transfer_Info_Ptr;

typedef struct{
	Transfer_Info_Ptr tip[STACK_SIZE];	//下标为0的位置不用，作为哨兵（判断栈空用）
	int head;
} Calculating_Stack;	//存储计算机的中间结果（转移信息，即Transfer_Info）

typedef struct{
	char c[STACK_SIZE];	//下标为0的位置不用，作为哨兵（判断栈空用）
	int head;
} Reg_Exp_Letter_Stack;		//存储正规式的运算符

/************************************************************************/
/*    functions about Status                                                                  */
/************************************************************************/
void status_init(Status& s1, int index, Status s2){
	s1->index = index;
	s1->next_element = s2;
}

Status status_union(Status s1, Status s2){		//集合并运算,返回一个新的集合
	Status ret = NULL;
	if(s1 == NULL){
		return ret = s2;
	}
	if(s2 == NULL){
		return ret = s1;
	}
	Status s3 = s1;
	Status s4 = s2;
	Status tail = NULL;

	if(s3->index > s4->index){		//找第一个元素
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

Status status_intersect(Status s1, Status s2){		//集合交运算，返回一个新的集合
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

int status_compare(Status s1, Status s2){		//集合的比较，相等返回1，不相等返回0
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

int status_add_element(Status &s1, int n){		//在集合中添加一个元素，成功返回1，不成功返回0（该元素已经存在）
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

int status_delete_element(Status &s1, int n){		//在集合中删除一个元素，成功返回1， 不成功返回0（该元素不存在）
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

void status_destroy(Status &s){		//销毁集合
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

void status_display(Status p){		//打印集合中的元素
	while(p != NULL){
		cout<<p->index<<" ";
		p = p->next_element;
	}
	cout<<endl;
}

void status_create(Status &s, int* a, int n){		//根据数组中的元素创建一个集合，a为数组，数组中的值为待创建集合中元素的下标，n为数组大小
	for(int i = 0; i < n; i++){
		status_add_element(s, a[i]);
	}
}

/************************************************************************/
/*   functions about Transfer_Info                                                                 */
/************************************************************************/


void transfer_info_init(Transfer_Info_Ptr& tip1, char* letters, int n){
	tip1->status_size = 0;
	tip1->the_end_size = 0;

	tip1->the_start = new Element();
	status_init(tip1->the_start, 0, NULL);
	
	tip1->status[tip1->status_size] = tip1->the_start;
	tip1->status_size++;
	
	
	tip1->the_end[tip1->the_end_size] = new Element();
	tip1->the_end_size++;
	
	tip1->status[tip1->status_size] = tip1->the_end[tip1->the_end_size];
	tip1->status_size++;
	
	tip1->letters_size = n;
	for(int i = 0; i < n; i++){
		tip1->letters[i] = letters[i];
	}
}

void transfer_info_merge(Transfer_Info_Ptr& tip1, Transfer_Info_Ptr& tip2, char reg_exp_letter){
	
}

void transfer_info_destroy(Transfer_Info_Ptr& tip){
	delete(tip->the_start);
	delete[] (tip->the_end);
	delete[] (tip->status);
	delete tip;
	for(int i = 0; i < tip->status_size; i++){
		delete[] tip->transfer_table[i];
	}
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