#include <iostream>
#include <string>
#include <conio.h> //getch()
#include <windows.h> // system()
#include <cmath> // ���밪(abs), pow(������)
#include <algorithm> // copy()

using namespace std;

class ALU {
private:
	const static int LEN = 32;   // 32��Ʈ�� �������� ����
	const static int MSB = 0;   // 2^31 ��ġ�� �ִ� 2������ ��
	const static int LSB = 31;   // 2^0 ��ġ�� �ִ� 2������ ��
	int C; //�ø���(Carry)
	int S; //��ȣ(Sign)
	int Z; //0(Zero)
	int V; //�����÷ο�(Overflow)
	int A; // 10���� A
	int B; // 10���� B
	int Result; // 10���� Result

	int *A_binary; // A�� 2���� ��� �迭 �ּ�
	int *B_binary; // B�� 2���� ��� �迭 �ּ�
	int *A2_binary;//A��������
	int *Q_binary; // �¼� or �������� ��� �迭 �ּ�
	int *M_binary; // �ǽ¼� or ������ ��� �迭 �ּ�
	int *Carry_binary; // ĳ�� �迭
	int *Result_binary; // ��� ����� ��� �迭 �ּ�

	int* MTemp; //M�� �������� ��� �迭 �ּ�
	int* QTemp; //���� �������� ��� �迭 �ּ�_�� ��ȯ��, ������������
	int* Regis_A; // A�������͸� ��� �迭 �ּ�
	int* Regis_Q; // �¼� or �������� ��� �迭 �ּ�
	int* Regis_M; // �ǽ¼� or ������ ��� �迭 �ּ�
public:
	ALU(int A, int B);
	void print_binary();
	void To_binary(int num, int *num_binary); // 10���� num �� �迭num_binary�� 2������ ����
	int To_Ten(int *num_binary); // 2���� �迭�� 10������ ��ȯ
	void change_binary(); // A, B���� ���������� ����
	int Full_adder(int a, int b, int *carry); // �������
	void Parallel_adder(int *A_binary, int *B_binary, int *Result_binary); //���� �����
	void complementer(int *num_binary); // ������(�迭�� 2�Ǻ����� ��ȯ)
	int right_shift(int *A2_binary, int *Q_binary);   //��������Ʈ(�����)

	void RegisterView(int a[], char A);	//�������� ���� ���
	void left_shift(int* A, int* Q); //��������Ʈ(����)
	int ZeroTest();


	void ADD();
	void SUB();
	void MUL();
	void DIV();
};
int main() {

	//system("mode con lines=60 cols=100");
	int A; // 10���� A ù��° ��ǲ
	int B; // 10���� B �ι�° ��ǲ
	//char sign; // +, -, *, /	//������ �޴°� �ƴϰ� ���� �ΰ� ���� ��� ���� ��� ���������

	while (1) {
		cout << "������ �Է��ϼ���(x y) : ";
		cin >> A >> B;
		ALU ALU1(A, B);
		//ALU1.change_binary();		//���ο��� �ʱ�ȭ �ϸ� ����,����,����,������ ������ �ϸ鼭 A_binary B_binary���� ����
		//ALU1.print_binary();		//10�������� �����صΰ� �� ���� �Լ� ���ο��� ���� �ٸ� �������Ϳ� 2������ �Ҵ������ ��


		ALU1.ADD();
		ALU1.SUB();
		ALU1.MUL();	//�������� �ϴ� �ּ�ó��
		ALU1.DIV();


		cout << "�ƹ� Ű�� �Է��ϼ���." << endl;
		if (_getch())
			system("cls");
	}
}

ALU::ALU(int A, int B) : A(A), B(B) {
	A_binary = new int[LEN] {0};
	B_binary = new int[LEN] {0};
	A2_binary = new int[LEN] {0};
	Q_binary = new int[LEN] {0};
	M_binary = new int[LEN] {0};
	Carry_binary = new int [LEN] {0};
	Result_binary = new int [LEN] {0};
	Regis_Q = new int[LEN] {0};
	Regis_M = new int[LEN] {0};
	Regis_A = new int[LEN] { 0 };
	MTemp = new int[LEN] {0};
	QTemp = new int[LEN] {0};
}

void ALU::print_binary() {
	cout << "\n\n";
	cout << "-------------------------------A�� 10����----------------------------------- \n" << A << endl;
	cout << "-------------------------------A�� 2����------------------------------------ \n" << endl;
	cout << "  ";
	for (int i = 0; i < LEN; i++) {
		cout << A_binary[i] << " ";
	}
	cout << "\n\n";
	cout << "-------------------------------B�� 10����----------------------------------- \n" << B << endl;
	cout << "-------------------------------B�� 2����------------------------------------ \n" << endl;
	cout << "  ";
	for (int i = 0; i < LEN; i++) {
		cout << B_binary[i] << " ";
	}
	cout << "\n\n";
}

void ALU::complementer(int *num_binary) {
	for (int i = 0; i < LEN; i++)
		num_binary[i] = num_binary[i] ^ 1;   //��Ʈ ����

	int add[LEN] = { 0 };
	add[LSB] = 1; // 1�� 2���ڵ�
	int *shadow;
	shadow = new int[LEN];

	copy(num_binary, num_binary + LEN, shadow);
	Parallel_adder(shadow, add, num_binary);//��Ʈ ���� �� +1Bit

	delete[] shadow;
}

void ALU::To_binary(int num, int *num_binary) {
	int *binary_length = new int;
	*binary_length = LEN;

	if (num >= 0) {                                      //num�� ����� ��
		while (num > 0) {
			num_binary[*binary_length - 1] = num % 2;
			num /= 2;
			*binary_length -= 1;
		}
	}
	else {                                                  //num�� ������ ��
		int absnum = abs(num);
		while (absnum > 0) {                        // abs(num)�� �迭������
			num_binary[*binary_length - 1] = absnum % 2;
			absnum /= 2;
			*binary_length -= 1;
		}

		complementer(num_binary);                  //2�� ����
	}

	delete binary_length;
}

int ALU::To_Ten(int *num_binary) {
	int sum = 0; //��ȯ��
	//int what;
	if (num_binary[MSB] == 0) {                              // 2������ ����� ��,
		for (int i = 0; i < LEN - 1; i++) {
			sum += (int)pow(2.0, (double)i) * num_binary[LSB - i];
		}
	}
	else {                                        // 2������ ������ ��
		int *shadow;
		shadow = new int[LEN];
		copy(num_binary, num_binary + LEN, shadow);
		complementer(shadow); //����;
		for (int i = 0; i < LEN - 1; i++) {
			sum += (int)pow(2.0, (double)i) * shadow[LSB - i];
		}
		sum = -sum; // ���̳ʽ� ��ȣ

		delete[] shadow;

	}

	return sum;
}

void ALU::change_binary() {
	this->To_binary(A, A_binary);
	this->To_binary(B, B_binary);
}

int ALU::Full_adder(int a, int b, int *carry) {
	int sum = (a ^ b) ^ (*carry);
	*(carry - 1) = (a && b) || ((a ^ b) *(*carry));
	return sum;
}

void ALU::Parallel_adder(int *A_binary, int *B_binary, int *Result_binary) {

	Result_binary[LSB] = (A_binary[LSB] ^ B_binary[LSB]);
	Carry_binary[LSB] = (A_binary[LSB]) && (B_binary[LSB]);

	for (int i = LSB - 1; i >= 0; i--)
	{
		Result_binary[i] = Full_adder(A_binary[i], B_binary[i], &Carry_binary[i + 1]);
	}

	//C = Carry_binary[MSB]; //
	//V = (Carry_binary[MSB] ^ Carry_binary[MSB + 1]);
	//S = Result_binary[MSB]; //������� �ֻ�����Ʈ
	//Z = ZeroTest();
}

void ALU::ADD() {
	To_binary(A, A_binary);
	To_binary(B, B_binary);
	Parallel_adder(A_binary, B_binary, Result_binary);

	Result = To_Ten(Result_binary);
	C = Carry_binary[MSB];
	V = (Carry_binary[MSB] ^ Carry_binary[MSB+1]);
	S = Result_binary[MSB];
	int zero = 0;
	for (int i = 0; i < LEN; i++)
		zero = (zero || Result_binary[i]);
	if (zero == 0) Z = 1;
	else Z = 0;

	cout << "============================================================================" << endl;
	cout << "------------------������  (" << A << ") + (" << B << ")  �� �������Դϴ�---------------------" << endl;
	cout << "============================================================================\n" << endl;

	cout << " Carry :    ";
	for (int i = 0; i < LEN; i++)
	{
		cout << Carry_binary[i] << " ";
	}
	cout << "0";
	cout << "\n";
	cout << "     A :      ";
	for (int i = 0; i < LEN; i++) {
		cout << A_binary[i] << " ";
	}
	cout << "\n";
	cout << "  +  B :      ";
	for (int i = 0; i < LEN; i++) {
		cout << B_binary[i] << " ";
	}
	cout << "\n";
	cout << "=============================================================================" << endl;
	cout << "              ";
	for (int i = 0; i < LEN; i++)
	{
		cout << Result_binary[i] << " ";
	}


	cout << "\n\n";
	cout << "------------------------------A + B�� 10����--------------------------------- \n" << endl;
	if (V == 1) {
		cout << "�����÷ο� �߻�!!" << endl;
	}
	else
	cout << Result << endl;
	cout << "------------------------------A + B�� 2����---------------------------------- \n" << endl;
	if (V == 1) {
		cout << "�����÷ο� �߻�!!"<<endl;
	}
	else {
		cout << "  ";
		for (int i = 0; i < LEN; i++)
		{
			cout << Result_binary[i] << " ";
		}
	}
	cout << "\n\n";
	cout << "--------------------------------V���������� ��------------------------------ " << endl;
	cout << "V�������� :  " << V << endl;
	cout << "--------------------------------Z���������� ��------------------------------ " << endl;
	cout << "Z�������� :  " << Z << endl;
	cout << "--------------------------------S���������� ��------------------------------ " << endl;
	cout << "S�������� :  " << S << endl;
	cout << "--------------------------------C���������� ��------------------------------ " << endl;
	cout << "C�������� :  " << C << endl;
	cout << "---------------------------------------------------------------------------- " << endl;
	cout << "\n";
}

void ALU::SUB() {
	To_binary(A, A_binary);
	To_binary(B, B_binary);
	complementer(B_binary);

	Parallel_adder(A_binary, B_binary, Result_binary);


	Result = To_Ten(Result_binary);
	C = Carry_binary[MSB];
	V = (Carry_binary[MSB] ^ Carry_binary[MSB + 1]);
	S = Result_binary[MSB];
	int zero = 0;
	for (int i = 0; i < LEN; i++)
		zero = (zero || Result_binary[i]);
	if (zero == 0) Z = 1;
	else Z = 0;

	cout << "============================================================================" << endl;
	cout << "------------------������  (" << A << ") - (" << B << ")  �� �������Դϴ�---------------------" << endl;
	cout << "============================================================================\n" << endl;
	complementer(B_binary); // �ٽ� B�� ��
	cout << "  B (" << B << ") : ";
	for (int i = 0; i < LEN; i++) {
		cout << B_binary[i] << " ";
	}
	cout << "\n\n";
	cout << "----------------------------- B�� 2�Ǻ��� -----------------------------------\n" << endl;
	complementer(B_binary);
	cout << " -B (" << -B << ") : ";
	for (int i = 0; i < LEN; i++) {
		cout << B_binary[i] << " ";
	}
	cout << "\n\n";
	cout << "-------------------------- A - B = A + (-B) ---------------------------------\n" << endl;

	Parallel_adder(A_binary, B_binary, Result_binary); // ĳ�������� �ѹ����� (������ȯ���� ĳ������ ��������)
	cout << " Carry :    ";
	for (int i = 0; i < LEN; i++)
	{
		cout << Carry_binary[i] << " ";
	}
	cout << "0";
	cout << "\n";
	cout << "     A :      ";
	for (int i = 0; i < LEN; i++) {
		cout << A_binary[i] << " ";
	}
	cout << "\n";
	cout << " +  -B :      ";
	for (int i = 0; i < LEN; i++) {
		cout << B_binary[i] << " ";
	}
	cout << "\n";
	cout << "=============================================================================" << endl;
	cout << "              ";
	for (int i = 0; i < LEN; i++)
	{
		cout << Result_binary[i] << " ";
	}


	cout << "\n\n";
	cout << "------------------------------A - B�� 10����--------------------------------- \n" << endl;
	if (V == 1) {
		cout << "�����÷ο� �߻�!!" << endl;
	}
	else
		cout << Result << endl;
	cout << "------------------------------A - B�� 2����---------------------------------- \n" << endl;
	if (V == 1) {
		cout << "�����÷ο� �߻�!!" << endl;
	}
	else {
		cout << "  ";
		for (int i = 0; i < LEN; i++)
		{
			cout << Result_binary[i] << " ";
		}
	}
	cout << "\n\n";
	cout << "--------------------------------V���������� ��------------------------------ " << endl;
	cout << "V�������� :  " << V << endl;
	cout << "--------------------------------Z���������� ��------------------------------ " << endl;
	cout << "Z�������� :  " << Z << endl;
	cout << "--------------------------------S���������� ��------------------------------ " << endl;
	cout << "S�������� :  " << S << endl;
	cout << "--------------------------------C���������� ��------------------------------ " << endl;
	cout << "C�������� :  " << C << endl;
	cout << "---------------------------------------------------------------------------- " << endl;
	cout << "\n";
}

int ALU::right_shift(int *A2_binary, int *Q_binary)
{
	int preQ;

	//if (direction == 0) //���� ����Ʈ
	//{
	//	for (int i = 0; i < 31; i++) {
	//		A2_binary[i] = A2_binary[i + 1];
	//	}
	//	A2_binary[31] = Q_binary[0];
	//	for (int i = 0; i < 31; i++) {
	//		Q_binary[i] = Q_binary[i + 1];
	//	}
	//	Q_binary[31] = 0;
	//}
	{
		preQ = Q_binary[31];

		for (int i = 31; i > 0; i--) {
			Q_binary[i] = Q_binary[i - 1];
		}
		Q_binary[0] = A2_binary[31];
		for (int i = 31; i > 0; i--) {
			A2_binary[i] = A2_binary[i - 1];
		}
	}

	for (int i = 0; i < LEN; i++)
	{
		if (i == 0)
			Z = A2_binary[31];
		else
			Z = A2_binary[31 - i] || Z;
	}

	S = A2_binary[MSB];
	Z = !Z;

	//if (direction == 0)
	//	return 0;
	//else
		return preQ;
}

void ALU::MUL()
{
	To_binary(A, M_binary);
	To_binary(B, Q_binary);
	int preQ = 0;
	C = 0; // �ϴ� 0�����ص׾��
	cout << "����" << endl;
	cout << "                A                                   Q                preQ                 M" << endl;
	for (int i = 0; i < 32; i++)
		cout << A2_binary[i];
	cout << "   ";
	for (int i = 0; i < 32; i++)
		cout << Q_binary[i];
	cout << "   " << preQ << "   ";
	for (int i = 0; i < 32; i++)
		cout << M_binary[i];
	cout << endl;

	for (int i = 0; i < 32; i++)
	{
		cout << "STEP " << i + 1 << endl;

		if (Q_binary[31] == 1 && preQ == 0)
		{
			complementer(M_binary); //M'
			Parallel_adder(A2_binary, M_binary, Result_binary);   //A <- A - M
			copy(Result_binary, Result_binary + LEN, A2_binary); //����
			complementer(M_binary); //M
		}
		else if (Q_binary[31] == 0 && preQ == 1)
		{
			Parallel_adder(A2_binary, M_binary, Result_binary);   //A <- A + M
			copy(Result_binary, Result_binary + LEN, A2_binary); //����
		}
		else    //00 11
			C = 0;

		//���� ��� ���
		if ((Q_binary[31] == 1 && preQ == 0) || (Q_binary[31] == 0 && preQ == 1))
		{
			for (int i = 0; i < LEN; i++)
				cout << A2_binary[i];
			cout << "   ";
			for (int i = 0; i < LEN; i++)
				cout << Q_binary[i];
			cout << "   " << preQ << "   ";

			if (Q_binary[31] == 1 && preQ == 0)
				cout << "A <- A - M" << endl;
			else if (Q_binary[31] == 0 && preQ == 1)
				cout << "A <- A + M" << endl;
		}

		preQ = right_shift(A2_binary, Q_binary);   //����� ����-����Ʈ

		for (int i = 0; i < LEN; i++)
			cout << A2_binary[i];
		cout << "   ";
		for (int i = 0; i < LEN; i++)
			cout << Q_binary[i];
		cout << "   " << preQ << "   ";

		cout << "�̵�      ";
		cout << "Status Bit : " << " C : " << C << ", S : " << S << ", Z : " << Z << ", V : " << V << endl;
	}
	int x = 0;

	x = To_Ten(Q_binary);
	//if (A2_binary[0] == 0)
	//{
	//	x = To_Ten(A2_binary)*(int)pow(2.0, 32.0) + To_Ten(Q_binary);
	//}
	//else if (A2_binary[0] == 1)
	//{
	//	complementer(A2_binary);
	//	x = To_Ten(A2_binary)*(int)pow(2.0, 32.0) + To_Ten(Q_binary);
	//}

	cout << "���: " << x << endl << endl;
	//���� ��� ���, ���� ��Ʈ ���
}

void ALU::DIV() {

	cout << "\n\n<������>" << endl;

	To_binary(A, Regis_Q);		//���⼭ 2���� �Ҵ�����
	To_binary(B, Regis_M);

	bool Mcheck = false; //������ �������� ��ȣ�� �� ��ȣ�Ǵ�

	//�������� ��ȣ�� ���� A���������� �� �Ǵ�
	if (Regis_Q[0] == 1) { //A��, �� Q�� �� ���� ������� �ֻ�����Ʈ�� 1
		for (int i = 0; i < LEN; i++)
			Regis_A[i] = 1;
	}
	else {
		for (int i = 0; i < LEN; i++)
			Regis_A[i] = 0;
	}


	if (Regis_Q[0] == Regis_M[0])  //�������� ������ ��ȣ�� ������
		Mcheck = true; //���� ���

	cout << "�ʱ����" << endl;
	RegisterView(Regis_A, 'A');
	RegisterView(Regis_Q, 'Q');
	RegisterView(Regis_M, 'M');

	for (int i = 0; i < LEN; i++) {
		MTemp[i] = Regis_M[i];
	}
	complementer(MTemp); //M�������� ���� ������ MTemp�� ����

	int count = 1;					// ����Ŭ Ƚ�� ��Ÿ��

	//����
	for (int i = 0; i < LEN; i++) {
		cout << "\n\n<" << count << "��° ����Ŭ>" << endl;
		count++;

		//��������Ʈ
		left_shift(Regis_A, Regis_Q);
		RegisterView(Regis_A, 'A');
		RegisterView(Regis_Q, 'Q');
		cout <<   "     ���� ����Ʈ" << endl;
		

		if (Regis_A[0] == Regis_M[0]) { //A�� M�� ��ȣ�� ���ٸ� ����

			Parallel_adder(Regis_A, MTemp, Result_binary);
			//���� ���
			RegisterView(Result_binary, 'A');
			RegisterView(Regis_Q, 'Q');
			cout <<  "     (A�� M�� ��ȣ�� �����Ƿ� ����)A <- A-M" << endl;
			
						
			
			
		}
		else {//A,M�Ǻ�ȣ�� �ٸ��ٸ� ����
			Parallel_adder(Regis_A, Regis_M, Result_binary);
			RegisterView(Result_binary, 'A');
			RegisterView(Regis_Q, 'Q');
			cout <<  "     (A�� M�� ��ȣ�� �ٸ��Ƿ� ����)A <- A+M" << endl;
			
			//���� ���
		
			
		}

		if (Regis_A[0] == Result_binary[0]) {
			
			for (int i = 0; i < LEN; i++) //������ ����������, �� ����
				Regis_A[i] = Result_binary[i];
			Regis_Q[LSB] = 1; //Q�� ��������Ʈ�� 1����

			RegisterView(Regis_A, 'A');
			RegisterView(Regis_Q, 'Q');
			cout << "      ������ ���������Ƿ� Q0 = 1�� ���� �� A�������� ����" << endl;
			


		}
		else {
			Regis_Q[LSB] = 0;
			
			RegisterView(Regis_A, 'A');
			RegisterView(Regis_Q, 'Q');
			cout << "     ������ ���������Ƿ� Q0 = 0���� ���� �� A�������� �� ����" << endl;
		}
	}

	//�������� 0�� �Ǵ� ��� ã��
	bool dividable = false;		//���� �������� ������ �ѹ��� ���� �������� Ȯ���ϴ� ����


	if (Mcheck) {
		Parallel_adder(Regis_A, MTemp, Result_binary);		//������-���� �ؼ� 0������
		if (ZeroTest())	dividable = true;
	}
	else {
		Parallel_adder(Regis_A, Regis_M, Result_binary);		//������+���� �ؼ� 0������ 
		if (ZeroTest())	dividable = true;
	}

	if (dividable) {
		for (int i = 0; i < LEN; i++) {
			Regis_A[i] = 0;
		}
		int add[LEN] = { 0 };
		add[LSB] = 1;
		Parallel_adder(add, Regis_Q, Result_binary);
		for (int i = 0; i < LEN; i++) {
			Regis_Q[i] = Result_binary[i];
		}
	}

	//��� ���
	for (int i = 0; i < LEN; i++) {
		QTemp[i] = Regis_Q[i];
	}
	complementer(QTemp); //Q�������� ���� ������ QTemp�� ����

	cout << endl << "<������ ���>" << endl;

	if (Mcheck) {			// ������ �������� ��ȣ�� ���ٸ� ����� ���� ��ȯ
		cout << "  ��  : ";
		RegisterView(Regis_Q, 'Q');
	}
	else {
		cout << "  ��  : ";
		RegisterView(QTemp, 'Q');
		cout << "	(�������� ������ ��ȣ�� �ٸ��Ƿ� ���� 2�� ������ ����)" << endl;
	}

	cout << endl;
	cout << "������: ";
	RegisterView(Regis_A, 'A');

	int r = To_Ten(Regis_A);
	int q;
	if (Mcheck)
		q = To_Ten(Regis_Q);
	else
		q = To_Ten(QTemp);
	cout << endl << endl << "������ ��-> " << q << " ������-> " << r << endl;

	cout << endl;
	cout << " V Flag : " << V << endl;
	cout << " C Flag : " << C << endl;
	cout << " S Flag : " << S << endl;
	cout << " Z Flag : " << Z << endl;
}

void ALU::RegisterView(int a[], char N) { //�������� ���

	cout << N << "��������: ";
	for (int i = 1; i < LEN + 1; i++) {
		cout << a[i - 1];
		if (i % 8 == 0)
			cout << " ";
	}
	
}

void ALU::left_shift(int* A, int* Q) {
	for (int i = 0; i < LSB; i++)//A�������Ͱ� ��������Ʈ
		A[i] = A[i + 1];
	A[LSB] = Q[0]; //A ������ ��Ʈ�� Q�ֻ��� ��Ʈ�� ����

	for (int i = 0; i < LSB; i++)//Q�������Ͱ� ��������Ʈ
		Q[i] = Q[i + 1];
	Q[LSB] = 0; //Q ������ ��Ʈ�� 0����
}

int ALU::ZeroTest() {
	int zero = 0;
	for (int i = 0; i < LEN; i++)
		zero = (zero || Result_binary[i]);

	if (zero == 0) return 1;
	else return 0;
}
