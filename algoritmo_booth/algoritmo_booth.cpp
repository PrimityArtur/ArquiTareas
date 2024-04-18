#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <queue>
#include <stack>

using BinaryQueue = std::queue<int>;
using BinaryStack = std::stack<int>;

class AlgoritmoBooth{
    /*
    A = numero binario auxiliar
    M = numero binario multiplicando
    Q = numero binario multiplicador
    Q_1 = LSB of Q antes de arrastar Q
    Q0 = LSB of Q 
    n = numero de bits 
    */
public:
    AlgoritmoBooth();
    ~AlgoritmoBooth();
    void run(int _Q, int _M);

private:
    BinaryQueue A, M, Q, M2; 
    int Q_1, Q0, n; 

    BinaryQueue convertBinary(int a);      // Conversor de decimal a binario
    void arithmeticAdd(BinaryQueue _M);    // A = A + M
    void arithmeticSub();                  // A = A - M
    void arithmeticShift();                // A = A >> 1
    BinaryQueue change2Complement(BinaryQueue binary); // Cambia el signo de un binario

    void print(BinaryQueue a);             // Imprime un binario
    void printBinarys(std::string &text);   // Imprime un texto y los binarios A, M, Q, Q0, Q_1 y M
    void bitEqualizer(BinaryQueue &a);     // igualador de bits a n bits

    void reset();                          // Reinicia los binarios A, M, Q, Q0, Q_1 y M
    void delBinary(BinaryQueue &a);        // Elimina un binario
    void init(int _Q, int _M);             // Inicializa los binarios A, M, Q, Q0, Q_1 y M
};

AlgoritmoBooth::AlgoritmoBooth(){
    n = 0;
    Q_1 = 0;
    Q0 = 0;
}
AlgoritmoBooth::~AlgoritmoBooth(){
    reset();
}

BinaryQueue AlgoritmoBooth::convertBinary(int a){
    BinaryQueue binary;   
    bool negative = a < 0; 
    
    if (negative) a *= -1;

    while(a/2 > 0){
        binary.push(a%2);
        a /= 2;
    }    
    
    if(a == 0 || a == 1) binary.push(a);

    bitEqualizer(binary);
    
    if (negative) {
        return change2Complement(binary);
    }
    
    return binary;
}

void AlgoritmoBooth::bitEqualizer(BinaryQueue &a){
    while (a.size() < n){
        a.push(0);    
    }
}

void AlgoritmoBooth::arithmeticAdd(BinaryQueue _M){
    bool count = false;
    A.push(2);

    while (A.front() != 2) {
        int bit = A.front() ^ _M.front();
        
        if (count == true) bit ^= 1;
        
        count = ( (A.front() == 1 && _M.front() == 1) || 
                ((A.front() == 1 || _M.front() == 1) && count));        
        A.push(bit);
        A.pop(); _M.pop();
    }

    A.pop();
}

void AlgoritmoBooth::arithmeticSub(){
    arithmeticAdd(M2);
}

void AlgoritmoBooth::arithmeticShift(){
    Q_1 = Q.front();
    Q.pop();
    Q.push(A.front());
    Q0 = Q.front();
    A.pop();
    A.push(A.back());    
}

BinaryQueue AlgoritmoBooth::change2Complement(BinaryQueue binary){
   
    bool count = true;
    binary.push(2);
    
    while (binary.front() != 2) {
        int bit = binary.front() ^ 1;
        binary.pop();
        
        if (count) {
            bit ^= 1;
            if (bit == 1) count = false;
        }
        binary.push(bit);
    }
        
    binary.pop();
    return binary;
}

void AlgoritmoBooth::print(BinaryQueue a){
    BinaryStack aux;
    while(!a.empty()){
        aux.push(a.front());
        a.pop();
    }
    
    while(!aux.empty()){
        std::cout << aux.top();
        aux.pop();
    }
}

void AlgoritmoBooth::printBinarys(std::string &text){
    print(A);
    std::cout << " ";
    print(Q);
    std::cout << " " << Q0 << "  " << Q_1 << "   ";
    print(M);
    std::cout << text << std::endl;
}

void AlgoritmoBooth::init(int _Q, int _M){
    int a = std::abs( std::abs(_Q) > std::abs(_M) ? _Q : _M );
    n = static_cast<int>( std::ceil( std::log2( static_cast<float>(a) ) + 1)) ;

    M = convertBinary(_M);
    Q = convertBinary(_Q);

    bitEqualizer(A);

    M2 = change2Complement(M);

    Q_1 = 0;
    Q0 = Q.front();
}

void AlgoritmoBooth::reset(){
    n = 0;
    Q_1 = 0;
    Q0 = 0;

    delBinary(M);
    delBinary(Q);
    delBinary(A);
    delBinary(M2);
}

void AlgoritmoBooth::delBinary(BinaryQueue &a){
    while(!a.empty()){
        a.pop();
    }
}

void AlgoritmoBooth::run(int _Q, int _M){
    init( _Q, _M);

    std::string text = std::string(n,' '), 
                liner = std::string(n*3+9,'-');

    std::cout <<"\n" << liner <<"\n";
    std::cout << "A" << text << "Q" << text << "Q0 Q-1 M" <<  text << "| " << _Q << " x " << _M << " |\n";
    std::cout << liner <<"\n";
    printBinarys(text);

    for( ; n != 0; n--){
        std::cout << liner <<"\n";
        if (Q0 == 1 && Q_1 == 0){
            arithmeticSub();
            printBinarys(text = " sub");
        }
        else if (Q0 == 0 && Q_1 == 1){
            arithmeticAdd(M);
            printBinarys(text = " add");
        }

        arithmeticShift();        
        printBinarys(text = " shift");        
    }    

    reset();
}

int main() {
	AlgoritmoBooth a;
        
    a.run(15, 3);
    a.run(3, 15);
    a.run(15, -3);
    a.run(-3, 15);
    a.run(-15, -3);

    a.run(1,2);
    a.run(18,8);
    a.run(33,12);
}