#include <iostream>
#include <string>
#include <conio.h> //getch()
#include <windows.h> // system()
#include <cmath> // 절대값(abs), pow(제곱근)
#include <algorithm> // copy()

using namespace std;

class ALU {
private:
	const static int LEN = 32;   // 32비트의 레지스터 길이
	const static int MSB = 0;   // 2^31 위치에 있는 2진수의 값
	const static int LSB = 31;   // 2^0 위치에 있는 2진수의 값
	int C; //올림수(Carry)
	int S; //부호(Sign)
	int Z; //0(Zero)
	int V; //오버플로우(Overflow)
	int A; // 10진법 A
	int B; // 10진법 B
	int Result; // 10진법 Result

	int *A_binary; // A의 2진법 담는 배열 주소
	int *B_binary; // B의 2진법 담는 배열 주소
	int *A2_binary;//A레지스터
	int *Q_binary; // 승수 or 피젯수를 담는 배열 주소
	int *M_binary; // 피승수 or 젯수를 담는 배열 주소
	int *Carry_binary; // 캐리 배열
	int *Result_binary; // 계산 결과를 담는 배열 주소

	int* MTemp; //M의 보수값을 담는 배열 주소
	int* QTemp; //몫의 보수값을 담는 배열 주소_몫 반환시, 양음수를위해
	int* Regis_A; // A레지스터를 담는 배열 주소
	int* Regis_Q; // 승수 or 피젯수를 담는 배열 주소
	int* Regis_M; // 피승수 or 젯수를 담는 배열 주소
public:
	ALU(int A, int B);
	void print_binary();
	void To_binary(int num, int *num_binary); // 10진수 num 을 배열num_binary에 2진수로 변경
	int To_Ten(int *num_binary); // 2진수 배열을 10진수로 반환
	void change_binary(); // A, B값을 이진법으로 변경
	int Full_adder(int a, int b, int *carry); // 전가산기
	void Parallel_adder(int *A_binary, int *B_binary, int *Result_binary); //병렬 가산기
	void complementer(int *num_binary); // 보수기(배열을 2의보수로 변환)
	int right_shift(int *A2_binary, int *Q_binary);   //우측쉬프트(상원형)

	void RegisterView(int a[], char A);	//레지스터 상태 출력
	void left_shift(int* A, int* Q); //좌측시프트(종우)
	int ZeroTest();


	void ADD();
	void SUB();
	void MUL();
	void DIV();
};
int main() {

	//system("mode con lines=60 cols=100");
	int A; // 10진법 A 첫번째 인풋
	int B; // 10진법 B 두번째 인풋
	//char sign; // +, -, *, /	//연산자 받는게 아니고 숫자 두개 들어가면 모든 연산 결과 보여줘야함

	while (1) {
		cout << "정수를 입력하세요(x y) : ";
		cin >> A >> B;
		ALU ALU1(A, B);
		//ALU1.change_binary();		//메인에서 초기화 하면 덧셈,뺄셈,곱셈,나눗셈 순으로 하면서 A_binary B_binary값이 변함
		//ALU1.print_binary();		//10진수값만 저장해두고 각 연산 함수 내부에서 각각 다른 레지스터에 2진수로 할당해줘야 함


		ALU1.ADD();
		ALU1.SUB();
		ALU1.MUL();	//오류나서 일단 주석처리
		ALU1.DIV();


		cout << "아무 키나 입력하세요." << endl;
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
	cout << "-------------------------------A의 10진법----------------------------------- \n" << A << endl;
	cout << "-------------------------------A의 2진법------------------------------------ \n" << endl;
	cout << "  ";
	for (int i = 0; i < LEN; i++) {
		cout << A_binary[i] << " ";
	}
	cout << "\n\n";
	cout << "-------------------------------B의 10진법----------------------------------- \n" << B << endl;
	cout << "-------------------------------B의 2진법------------------------------------ \n" << endl;
	cout << "  ";
	for (int i = 0; i < LEN; i++) {
		cout << B_binary[i] << " ";
	}
	cout << "\n\n";
}

void ALU::complementer(int *num_binary) {
	for (int i = 0; i < LEN; i++)
		num_binary[i] = num_binary[i] ^ 1;   //비트 반전

	int add[LEN] = { 0 };
	add[LSB] = 1; // 1의 2진코드
	int *shadow;
	shadow = new int[LEN];

	copy(num_binary, num_binary + LEN, shadow);
	Parallel_adder(shadow, add, num_binary);//비트 반전 후 +1Bit

	delete[] shadow;
}

void ALU::To_binary(int num, int *num_binary) {
	int *binary_length = new int;
	*binary_length = LEN;

	if (num >= 0) {                                      //num이 양수일 때
		while (num > 0) {
			num_binary[*binary_length - 1] = num % 2;
			num /= 2;
			*binary_length -= 1;
		}
	}
	else {                                                  //num이 음수일 때
		int absnum = abs(num);
		while (absnum > 0) {                        // abs(num)을 배열에놓고
			num_binary[*binary_length - 1] = absnum % 2;
			absnum /= 2;
			*binary_length -= 1;
		}

		complementer(num_binary);                  //2의 보수
	}

	delete binary_length;
}

int ALU::To_Ten(int *num_binary) {
	int sum = 0; //반환값
	//int what;
	if (num_binary[MSB] == 0) {                              // 2진수가 양수일 때,
		for (int i = 0; i < LEN - 1; i++) {
			sum += (int)pow(2.0, (double)i) * num_binary[LSB - i];
		}
	}
	else {                                        // 2진수가 음수일 때
		int *shadow;
		shadow = new int[LEN];
		copy(num_binary, num_binary + LEN, shadow);
		complementer(shadow); //보수;
		for (int i = 0; i < LEN - 1; i++) {
			sum += (int)pow(2.0, (double)i) * shadow[LSB - i];
		}
		sum = -sum; // 마이너스 부호

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
	//S = Result_binary[MSB]; //결과값의 최상위비트
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
	cout << "------------------다음은  (" << A << ") + (" << B << ")  의 계산과정입니다---------------------" << endl;
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
	cout << "------------------------------A + B의 10진법--------------------------------- \n" << endl;
	if (V == 1) {
		cout << "오버플로우 발생!!" << endl;
	}
	else
	cout << Result << endl;
	cout << "------------------------------A + B의 2진법---------------------------------- \n" << endl;
	if (V == 1) {
		cout << "오버플로우 발생!!"<<endl;
	}
	else {
		cout << "  ";
		for (int i = 0; i < LEN; i++)
		{
			cout << Result_binary[i] << " ";
		}
	}
	cout << "\n\n";
	cout << "--------------------------------V레지스터의 값------------------------------ " << endl;
	cout << "V레지스터 :  " << V << endl;
	cout << "--------------------------------Z레지스터의 값------------------------------ " << endl;
	cout << "Z레지스터 :  " << Z << endl;
	cout << "--------------------------------S레지스터의 값------------------------------ " << endl;
	cout << "S레지스터 :  " << S << endl;
	cout << "--------------------------------C레지스터의 값------------------------------ " << endl;
	cout << "C레지스터 :  " << C << endl;
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
	cout << "------------------다음은  (" << A << ") - (" << B << ")  의 계산과정입니다---------------------" << endl;
	cout << "============================================================================\n" << endl;
	complementer(B_binary); // 다시 B의 값
	cout << "  B (" << B << ") : ";
	for (int i = 0; i < LEN; i++) {
		cout << B_binary[i] << " ";
	}
	cout << "\n\n";
	cout << "----------------------------- B의 2의보수 -----------------------------------\n" << endl;
	complementer(B_binary);
	cout << " -B (" << -B << ") : ";
	for (int i = 0; i < LEN; i++) {
		cout << B_binary[i] << " ";
	}
	cout << "\n\n";
	cout << "-------------------------- A - B = A + (-B) ---------------------------------\n" << endl;

	Parallel_adder(A_binary, B_binary, Result_binary); // 캐리값때매 한번더씀 (보수변환에서 캐리값이 들어가버려서)
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
	cout << "------------------------------A - B의 10진법--------------------------------- \n" << endl;
	if (V == 1) {
		cout << "오버플로우 발생!!" << endl;
	}
	else
		cout << Result << endl;
	cout << "------------------------------A - B의 2진법---------------------------------- \n" << endl;
	if (V == 1) {
		cout << "오버플로우 발생!!" << endl;
	}
	else {
		cout << "  ";
		for (int i = 0; i < LEN; i++)
		{
			cout << Result_binary[i] << " ";
		}
	}
	cout << "\n\n";
	cout << "--------------------------------V레지스터의 값------------------------------ " << endl;
	cout << "V레지스터 :  " << V << endl;
	cout << "--------------------------------Z레지스터의 값------------------------------ " << endl;
	cout << "Z레지스터 :  " << Z << endl;
	cout << "--------------------------------S레지스터의 값------------------------------ " << endl;
	cout << "S레지스터 :  " << S << endl;
	cout << "--------------------------------C레지스터의 값------------------------------ " << endl;
	cout << "C레지스터 :  " << C << endl;
	cout << "---------------------------------------------------------------------------- " << endl;
	cout << "\n";
}

int ALU::right_shift(int *A2_binary, int *Q_binary)
{
	int preQ;

	//if (direction == 0) //좌측 시프트
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
	C = 0; // 일단 0으로해뒀어요
	cout << "곱셈" << endl;
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
			copy(Result_binary, Result_binary + LEN, A2_binary); //복사
			complementer(M_binary); //M
		}
		else if (Q_binary[31] == 0 && preQ == 1)
		{
			Parallel_adder(A2_binary, M_binary, Result_binary);   //A <- A + M
			copy(Result_binary, Result_binary + LEN, A2_binary); //복사
		}
		else    //00 11
			C = 0;

		//연산 결과 출력
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

		preQ = right_shift(A2_binary, Q_binary);   //산술적 우측-시프트

		for (int i = 0; i < LEN; i++)
			cout << A2_binary[i];
		cout << "   ";
		for (int i = 0; i < LEN; i++)
			cout << Q_binary[i];
		cout << "   " << preQ << "   ";

		cout << "이동      ";
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

	cout << "결과: " << x << endl << endl;
	//곱셈 결과 출력, 상태 비트 출력
}

void ALU::DIV() {

	cout << "\n\n<나눗셈>" << endl;

	To_binary(A, Regis_Q);		//여기서 2진수 할당해줌
	To_binary(B, Regis_M);

	bool Mcheck = false; //제수와 피젯수의 부호로 몫 부호판단

	//피젯수의 부호에 따른 A레지스터의 값 판단
	if (Regis_Q[0] == 1) { //A값, 즉 Q에 들어갈 값이 음수라면 최상위비트가 1
		for (int i = 0; i < LEN; i++)
			Regis_A[i] = 1;
	}
	else {
		for (int i = 0; i < LEN; i++)
			Regis_A[i] = 0;
	}


	if (Regis_Q[0] == Regis_M[0])  //피젯수와 제수의 부호가 같을때
		Mcheck = true; //몫은 양수

	cout << "초기상태" << endl;
	RegisterView(Regis_A, 'A');
	RegisterView(Regis_Q, 'Q');
	RegisterView(Regis_M, 'M');

	for (int i = 0; i < LEN; i++) {
		MTemp[i] = Regis_M[i];
	}
	complementer(MTemp); //M레지스터 값의 보수를 MTemp에 저장

	int count = 1;					// 싸이클 횟수 나타냄

	//연산
	for (int i = 0; i < LEN; i++) {
		cout << "\n\n<" << count << "번째 싸이클>" << endl;
		count++;

		//좌측시프트
		left_shift(Regis_A, Regis_Q);
		RegisterView(Regis_A, 'A');
		RegisterView(Regis_Q, 'Q');
		cout <<   "     좌측 시프트" << endl;
		

		if (Regis_A[0] == Regis_M[0]) { //A와 M의 부호가 같다면 뺄셈

			Parallel_adder(Regis_A, MTemp, Result_binary);
			//상태 출력
			RegisterView(Result_binary, 'A');
			RegisterView(Regis_Q, 'Q');
			cout <<  "     (A와 M의 부호가 같으므로 뺄셈)A <- A-M" << endl;
			
						
			
			
		}
		else {//A,M의부호가 다르다면 덧셈
			Parallel_adder(Regis_A, Regis_M, Result_binary);
			RegisterView(Result_binary, 'A');
			RegisterView(Regis_Q, 'Q');
			cout <<  "     (A와 M의 부호가 다르므로 덧셈)A <- A+M" << endl;
			
			//상태 출력
		
			
		}

		if (Regis_A[0] == Result_binary[0]) {
			
			for (int i = 0; i < LEN; i++) //연산이 성공했으니, 값 갱신
				Regis_A[i] = Result_binary[i];
			Regis_Q[LSB] = 1; //Q의 최하위비트에 1저장

			RegisterView(Regis_A, 'A');
			RegisterView(Regis_Q, 'Q');
			cout << "      연산이 성공했으므로 Q0 = 1로 저장 및 A레지스터 갱신" << endl;
			


		}
		else {
			Regis_Q[LSB] = 0;
			
			RegisterView(Regis_A, 'A');
			RegisterView(Regis_Q, 'Q');
			cout << "     연산이 실패했으므로 Q0 = 0으로 저장 및 A레지스터 값 복원" << endl;
		}
	}

	//나머지가 0이 되는 경우 찾기
	bool dividable = false;		//최종 나머지가 제수로 한번더 연산 가능한지 확인하는 인자


	if (Mcheck) {
		Parallel_adder(Regis_A, MTemp, Result_binary);		//나머지-제수 해서 0나오면
		if (ZeroTest())	dividable = true;
	}
	else {
		Parallel_adder(Regis_A, Regis_M, Result_binary);		//나머지+제수 해서 0나오면 
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

	//결과 출력
	for (int i = 0; i < LEN; i++) {
		QTemp[i] = Regis_Q[i];
	}
	complementer(QTemp); //Q레지스터 값의 보수를 QTemp에 저장

	cout << endl << "<나눗셈 결과>" << endl;

	if (Mcheck) {			// 제수와 피젯수의 부호가 같다면 양수로 몫을 반환
		cout << "  몫  : ";
		RegisterView(Regis_Q, 'Q');
	}
	else {
		cout << "  몫  : ";
		RegisterView(QTemp, 'Q');
		cout << "	(피제수와 제수의 부호가 다르므로 몫은 2의 보수를 취함)" << endl;
	}

	cout << endl;
	cout << "나머지: ";
	RegisterView(Regis_A, 'A');

	int r = To_Ten(Regis_A);
	int q;
	if (Mcheck)
		q = To_Ten(Regis_Q);
	else
		q = To_Ten(QTemp);
	cout << endl << endl << "십진수 몫-> " << q << " 나머지-> " << r << endl;

	cout << endl;
	cout << " V Flag : " << V << endl;
	cout << " C Flag : " << C << endl;
	cout << " S Flag : " << S << endl;
	cout << " Z Flag : " << Z << endl;
}

void ALU::RegisterView(int a[], char N) { //레지스터 출력

	cout << N << "레지스터: ";
	for (int i = 1; i < LEN + 1; i++) {
		cout << a[i - 1];
		if (i % 8 == 0)
			cout << " ";
	}
	
}

void ALU::left_shift(int* A, int* Q) {
	for (int i = 0; i < LSB; i++)//A레지스터값 좌측시프트
		A[i] = A[i + 1];
	A[LSB] = Q[0]; //A 최하위 비트에 Q최상위 비트값 저장

	for (int i = 0; i < LSB; i++)//Q레지스터값 좌측시프트
		Q[i] = Q[i + 1];
	Q[LSB] = 0; //Q 최하위 비트에 0저장
}

int ALU::ZeroTest() {
	int zero = 0;
	for (int i = 0; i < LEN; i++)
		zero = (zero || Result_binary[i]);

	if (zero == 0) return 1;
	else return 0;
}
