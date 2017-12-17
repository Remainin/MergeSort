#include <iostream>
#include <ctime>
#include <fstream>
#include <cassert>
#include <string>
//#include "ExternSort.h"
using namespace std;

//ʹ�ö�·�鲢�������������
//ExternSort.h

/*
* ��������������
* ��·�鲢����
* ��ǧ��������С��������Ϊ��
* һ���Ƚϼ򵥵����ӣ�û�н����ڴ滺����
*/

#ifndef EXTERN_SORT_H
#define EXTERN_SORT_H



#define MIN -1//���￪ʼ��ʱ�������һ��BUG����������MIN���ڵ��ڴ����������������㷨���ִ���
#define MAX 10000000//���ֵ�������ڹ鲢�ļ���β
typedef int* LoserTree;
typedef int* External;

class ExternSort
{
public:
	void sort()
	{
		time_t start = time(NULL);

		//���ļ����ݷֿ����ڴ������򣬲��ֱ�д����ʱ�ļ�
		k = memory_sort();  //

		//�鲢��ʱ�ļ����ݵ�����ļ�
		//merge_sort(file_count);
		ls = new int[k];
		b = new int[k + 1];
		K_Merge();
		delete[]ls;
		delete[]b;

		time_t end = time(NULL);
		printf("��ʱ��:%f\n", (end - start) * 1000.0 / CLOCKS_PER_SEC);
	}

	//input_file:�����ļ���
	//out_file:����ļ���
	//count: ÿ�����ڴ����������������
	ExternSort(const char *in_file, const char * out_file, int count)
	{
		m_in_file = new char[strlen(in_file) + 1];
		m_out_file = new char[strlen(out_file) + 1];
		m_count = count;
		strcpy_s(m_in_file, strlen(in_file) + 1, in_file);
		strcpy_s(m_out_file, strlen(out_file) + 1, out_file);
	}
	virtual ~ExternSort()
	{
		delete[] m_in_file;//����ռ�
		delete[] m_out_file;
	}

private:
	int m_count; //���鳤��
	char *m_in_file;   //�����ļ���·��
	char *m_out_file; //����ļ���·��
	int k;//�鲢������������Ҫ������֮����ܵõ������������ls��b��ֻ�ܶ���Ϊָ��
	LoserTree ls;//�����Ϊָ�룬֮��̬��������
	External b;//�����Ϊָ�룬�ڳ�Ա�����п��԰�����������ʹ��
	//int External[k];
protected:
	int read_data(FILE* f, int a[], int n)
	{
		int i = 0;
		long long sran;//B��
		while (i < n && (fscanf_s(f, "%d %d\n", &a[i], &sran)) != EOF) { i++; }
		printf_s("����:%d ����\n", i);//������������
		return i;
	}
	void write_data(FILE* f, int a[], int n)
	{
		for (int i = 0; i < n; ++i)
			fprintf(f, "%d\n", a[i]);
		fprintf(f, "%d", MAX);//�����д��һ�����ֵ
	}
	char* temp_filename(int index)
	{
		char *tempfile = new char[100];//10^100
		sprintf_s(tempfile, 100, "forNow%d.txt", index);//��ʱ�ļ�����
		return tempfile;
	}
	static int cmp_int(const void *a, const void *b)
	{
		return *(int*)a - *(int*)b;
	}

	int memory_sort()
	{
		FILE*fin;
		fopen_s(&fin, m_in_file, "rt");
		int n = 0, file_count = 0;
		int *array = new int[m_count];

		//ÿ����m_count�����������ڴ�����һ�����򣬲�д����ʱ�ļ�
		while ((n = read_data(fin, array, m_count)) > 0)
		{
			qsort(array, n, sizeof(int), cmp_int);//�Խṹ������
			char *fileName = temp_filename(file_count++);
			FILE*tempFile;
			fopen_s(&tempFile, fileName, "w");
			free(fileName);
			write_data(tempFile, array, n);//MAX��β
			fclose(tempFile);
		}

		delete[] array;
		fclose(fin);

		return file_count;
	}

	void Adjust(int s)
	{//�ش�Ҷ�ӽڵ�b[s]�����ڵ�ls[0]��·������������
		int t = (s + k) / 2;//ls[t]��b[s]��˫�׽ڵ�
		while (t>0)
		{
			if (b[s]>b[ls[t]])//���ʧ�ܣ���ʧ����λ��s���£�sָ���µ�ʤ����
			{
				int tmp = s;
				s = ls[t];
				ls[t] = tmp;
			}
			t = t / 2;
		}
		ls[0] = s;//ls[0]��ŵ��������Сֵ��λ��
	}

	void CreateLoserTree()
	{
		b[k] = MIN;//����Ĵ洢һ����Сֵ
		for (int i = 0; i<k; i++)ls[i] = k;//�ȳ�ʼ��Ϊָ����Сֵ����������ĵ���������ȷ��
		//�����ܱ�֤��Ҷ�ӽڵ㶼�������еġ������֡�
		for (int i = k - 1; i >= 0; i--)
			Adjust(i);//���δ�b[k-1],b[k-2]...b[0]��������������
	}

	void K_Merge()
	{//���ð�������k������鲢�ι鲢���������
		//b��ǰk���������k��������е�ǰ��¼��Ԫ��
		//�鲢��ʱ�ļ�
		FILE*fout;
		fopen_s(&fout, m_out_file, "wt");
		FILE**farray = new FILE*[k];
		int i;
		char *cran = new char[12];
		for (i = 0; i < k; ++i)  //������k·�����ļ�
		{
			char* fileName = temp_filename(i);
			fopen_s(&farray[i], fileName, "rt");
			free(fileName);
		}

		for (i = 0; i < k; ++i)  //��ʼ��ȡ
		{
			if (fscanf_s(farray[i], "%d", &b[i]) == EOF)//��ÿ���ļ��ĵ�һ������data����
			{
				cout << "there is no " << k << " file to merge!" << endl;
				return;
			}
		}
		//    for(int i=0;i<k;i++)input(b[i]);

		CreateLoserTree();
		int q;
		while (b[ls[0]] != MAX)//ÿ��temp�ļ�ĩβ��ΪMAX
		{
			q = ls[0];//q�����洢b����Сֵ��λ�ã�ͬʱҲ��Ӧһ·�ļ�
			//output(q);
			strcpy_s(cran, 12, (to_string(b[q]) + to_string(b[q])).c_str());
			fprintf(fout, "%d %s\n", b[q], cran);
			//fprintf(fout, "%d ", b[q]);
			//input(b[q],q);
			fscanf_s(farray[q], "%d", &b[q]);
			Adjust(q);
		}
		//output(ls[0]);
		//fprintf(fout, "%d ", b[ls[0]]);//MAX

		for (i = 0; i < k; ++i)  //������
		{
			fclose(farray[i]);
		}
		delete[] farray;
		fclose(fout);
	}

};

#endif


//�����������ļ�
/*
* ���ļ�����(AB��ʽ)
* ���ݲ���һ����ȫ��װ���ڴ�
* �����ļ����ж������������֮���ÿո����
*/

//const unsigned int count = 10000000; // �ļ������ݵ�����
const unsigned int number_to_sort = 100000; //���ڴ���һ�����������
const char *unsort_file = "NotSort.txt"; //ԭʼδ������ļ���
const char *sort_file = "Sort.txt"; //��������ļ���
void init_data(unsigned int num); //������������ļ�
void isSorted(const char* filename);//����������ļ�
int main(int argc, char* *argv)
{
	unsigned int count = 1000000;
	srand(time(NULL));
	init_data(count);
	ExternSort extSort(unsort_file, sort_file, number_to_sort);
	extSort.sort();
	isSorted(sort_file);
	system("pause");
	return 0;
}

void init_data(unsigned int num)
{
	FILE *f;
	fopen_s(&f, unsort_file, "wt");
	int ran;//A��
	char *cran = new char[12];//B��
	for (unsigned int i = 0; i < num; ++i)
	{
		ran = rand();
		strcpy_s(cran, 12, (to_string(ran) + to_string(ran)).c_str());
		fprintf(f, "%d %s\n", ran, cran);
	}
	delete[] cran;
	fclose(f);
}

void isSorted(const char* filename) {
	ifstream CheckFile(filename, ios::binary);
	if (CheckFile.is_open() == false) {
		cout << "Fail to open " << filename << "\n";
		return;
	}
	int a, b;
	long long sran;//B��
	long i = 1;
	CheckFile >> a;
	CheckFile >> sran;
	while (CheckFile >> b) {
		if (a > b) {
			cout << i << "th number is wrong\n";
			return;
		}
		else {
			a = b;
			++i;
		}
		CheckFile >> sran;
	}
	CheckFile.close();
	cout << "Check Finished\n";
}