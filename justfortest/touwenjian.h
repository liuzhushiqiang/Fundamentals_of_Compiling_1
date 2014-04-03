using namespace std;

#define M 1000	//״̬��
#define N 70	//��ĸ���С

typedef struct Element{
	int index;		//״̬��Ԫ�ص��±�
	struct Element * next_element;
} Element, *Status;		//�����ʹ���״̬�е�Ԫ�أ�ָ������͵�ָ�����һ��״̬��

Status status[M];	//����״̬
Status transfer_table[M][N];	//״̬ת�Ʊ�
char letters[N];	//������ĸ

Status status_union(Status s1, Status s2){		//���ϲ�����,����һ���µļ���
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
	while(p != NULL){
		cout<<p->index<<" ";
		p = p->next_element;
	}
	cout<<endl;
}

void status_create(Status &s, int* a, int n){		//���������е�Ԫ�ش���һ�����ϣ�aΪ���飬�����е�ֵΪ������������Ԫ�ص��±꣬nΪ�����С
	for(int i = 0; i < n; i++){
		status_add_element(s, a[i]);
	}
}