#include <iostream>
#include <bitset>
#include <cassert>
#include <stdlib.h>
#include <cmath>

// #####################################################################
//                         ALGORITMO DE BOOTH    
// #####################################################################

template<class T>
class AlgoritmoBooth {
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
    T run(T _Q, T _M);

private:
    T A, M, Q, M2;
    int Q_1, Q0, n;

    void arithmeticAdd(T _M);    // A = A + M
    void arithmeticSub();                  // A = A - M
    void arithmeticShift();                // A = A >> 1
    T change2Complement(T binary); // Cambia el signo de un binario

    void reset();                          // Reinicia los binarios A, M, Q, Q0, Q_1 y M
    void init(T _Q, T _M);             // Inicializa los binarios A, M, Q, Q0, Q_1 y M
};

template<class T>
AlgoritmoBooth<T>::AlgoritmoBooth() {
    n = 0;
    Q_1 = 0;
    Q0 = 0;
}

template<class T>
AlgoritmoBooth<T>::~AlgoritmoBooth() {
    reset();
}

template<class T>
void AlgoritmoBooth<T>::arithmeticAdd(T _M) {
    bool carry = false;

    for (int i = 0; i < n; ++i) {
        int bit = A.test(i) ^ _M.test(i);

        if (carry == true) bit ^= 1;

        carry = ((A.test(i) == 1 && _M.test(i) == 1) ||
            ((A.test(i) == 1 || _M.test(i) == 1) && carry));
        A.set(i, bit);
    }
}

template<class T>
void AlgoritmoBooth<T>::arithmeticSub() {
    arithmeticAdd(M2);
}

template<class T>
void AlgoritmoBooth<T>::arithmeticShift() {
    Q_1 = Q.test(0);
    Q >>= 1;
    Q.set(n - 1, A.test(0));
    Q0 = Q.test(0);
    A >>= 1;
    A.set(n - 1, A.test(n - 2));
}

template<class T>
T AlgoritmoBooth<T>::change2Complement(T binary) {
    binary.flip();

    bool carry = true;
    for (int i = 0; i < n && carry; ++i) {
        if (binary.test(i)) {
            binary.set(i, 0);
        }
        else {
            binary.set(i, 1);
            carry = false;
        }
    }

    return binary;
}

template<class T>
void AlgoritmoBooth<T>::init(T _Q, T _M) {
    Q = _Q;
    M = _M;
    n = Q.size();

    M2 = change2Complement(M);

    Q_1 = 0;
    Q0 = Q.test(0);
}

template<class T>
void AlgoritmoBooth<T>::reset() {
    n = 0;
    Q_1 = 0;
    Q0 = 0;

    A.reset();
    M.reset();
    Q.reset();
    M2.reset();
}

template<class T>
T AlgoritmoBooth<T>::run(T _Q, T _M) {
    init(_Q, _M);

    for (int i = n; i != 0; i--) {
        if (Q0 == 1 && Q_1 == 0) {
            arithmeticSub();
        }
        else if (Q0 == 0 && Q_1 == 1) {
            arithmeticAdd(M);
        }
        arithmeticShift();
    }

    T result(A.to_string());
    reset();

    return result;
}

// #####################################################################
//                     FLOATING POINTER MULTIPLICATION
// #####################################################################

class FPMultiplication {
public:
    FPMultiplication();
    void run(float _a, float _b);

private:
    float a, b;                             // a, b numeros a multiplicar 
    std::bitset<32> fPa, fPb, result;       // fPa, fPb, resultados en binario

    std::bitset<8> exp1, exp2;              // exponentes en binario
    std::bitset<23> mant1, mant2;           // mantissas en binario
    int sig1, sig2;                         // signos de los numeros

    std::bitset<8> result_exp;              // resultado exponente
    std::bitset<23> result_mant;            // resultado mantissa
    int result_sign;                        // resultado signo


    bool isZero();                          // verifica si el numero es cero
    void biasedExponent();                  // convierte el exponente a su representacion biased
    std::bitset<8> sumExponents(std::bitset<8> _a, std::bitset<8>_b); // suma de exponentes
    std::bitset<25> multiplySignificands();                           // multiplicacion de mantissas
    void normalize(std::bitset<25> _mant);                            // normalizacion de mantissa y arreglo del exponente
    bool isOverflow(float _a);                                        // verifica si hay overflow
    bool isUnderflow(float _a);                                       // verifica si hay underflow
    bool isOverUnderFlow(float _a);                                   // verifica si hay overflow o underflow

    void assembleResult();                                            // arma el resultado en su representacion final

    void init();                                                      // inicializa los numeros a,b a su punto flotante
};

FPMultiplication::FPMultiplication() {
    a = b = 0;
    sig1 = sig2 = 0;
    result_sign = 0;
    result_exp = 0;
    result_mant = 0;
    exp1 = exp2 = 0;
    mant1 = mant2 = 0;
    result = fPa = fPb = 0;
}

bool FPMultiplication::isZero() {
    return !a || !b;
}

bool FPMultiplication::isUnderflow(float _a) {
    return _a < 1;
}

bool FPMultiplication::isOverflow(float _a) {
    return _a >= std::pow(2, 8) - 1;
}

bool FPMultiplication::isOverUnderFlow(float _a) {
    if (isOverflow(_a)) {
        std::cout << "Es overflow" << std::endl;
        return 1;
    }

    if (isUnderflow(_a)) {
        std::cout << "Es underflow" << std::endl;
        return 1;
    }
    return 0;
}

std::bitset<8> FPMultiplication::sumExponents(std::bitset<8> _a, std::bitset<8>_b) {
    // Suma de los exponentes
    std::bitset<8> sum(0);
    bool carry = false;

    for (int i = 0; i < 8; ++i) {
        sum[i] = _a[i] ^ _b[i] ^ carry;
        carry = (_a[i] & _b[i]) | (_a[i] & carry) | (carry & _b[i]);
    }

    return sum;
}

void FPMultiplication::biasedExponent() {
    std::bitset<8> sumExp = sumExponents(exp1, exp2);

    // biased exponent
    std::bitset<8> add{ "10000001" };
    result_exp = sumExponents(sumExp, add);
}

void FPMultiplication::normalize(std::bitset<25> _mant) {
    int n = 25;
    int k = 0;

    // normalizacion de mantissa
    for (int i = 0; i < n; ++i) {
        if (_mant.test(n - 1)) break;

        k++;
        _mant <<= 1;
    }

    result_mant = std::bitset<23>((_mant <<= 1).to_string());

    if (k == 2) {
        result_exp = sumExponents(result_exp, std::bitset<8>("00000001"));
    }
}


std::bitset<25> FPMultiplication::multiplySignificands() {
    // Multiplicación de los números binarios
    const int n = 25;
    AlgoritmoBooth<std::bitset<n>> algB;

    std::bitset<n> mant1_("01" + mant1.to_string());
    std::bitset<n> mant2_("01" + mant2.to_string());

    return algB.run(mant1_, mant2_);
}

void FPMultiplication::assembleResult() {
    result = std::bitset<32>("0" + result_exp.to_string() + result_mant.to_string());
    result[31] = result_sign;
}

void FPMultiplication::init() {
    // inicializa los numeros a,b a su punto flotante
    fPa = std::bitset<32>(*reinterpret_cast<unsigned*>(&a));
    fPb = std::bitset<32>(*reinterpret_cast<unsigned*>(&b));

    // signos
    sig1 = fPa[31];
    sig2 = fPb[31];

    // exponentes
    for (int i = 0; i < 8; ++i) {
        exp1[7 - i] = fPa[30 - i];
        exp2[7 - i] = fPb[30 - i];
    }

    // mantissas
    for (int i = 0; i < 23; ++i) {
        mant1[22 - i] = fPa[22 - i];
        mant2[22 - i] = fPb[22 - i];
    }
}

void FPMultiplication::run(float _a, float _b) {
    a = _a;
    b = _b;

    std::cout << " ---------------------------------------- \n\n";
    std::cout << " || " << std::string(5, ' ') << a << " * " << b << "\n\n";
    std::cout << "Caso del lenguaje de programacion: " << a << " * " << b << " = " << a * b << std::endl;
    std::cout << "Resultado del algoritmo: " << std::endl;


    if (isZero()) {
        std::cout << "Al menos uno de los valores ingresados es cero." << std::endl;
        return;
    }

    init();

    result_sign = sig1 ^ sig2;

    biasedExponent();

    if (isOverUnderFlow(exp1.to_ulong() + exp2.to_ulong() - (pow(2, 7) - 1))) return;

    normalize(multiplySignificands());

    assembleResult();


    std::cout << sig1 << " " << exp1 << " " << mant1 << " * " << sig2 << " " << exp2 << " " << mant2 << " = " << std::endl;

    float result_number = *reinterpret_cast<float*>(&result);
    std::cout << result_sign << " " << result_exp << " " << result_mant << " = " << result_number << std::endl;
}


// #####################################################################
//                                 MAIN
// #####################################################################

int main() {
    float a = 4294967297777777777777777777777777777.0, b = 10000000.0;

    FPMultiplication fpm;
    fpm.run(a, b);
    fpm.run(1E-10, 1E-30);
    fpm.run(1E-10, 0);
    fpm.run(1.0, 1.0);
    fpm.run(5.455, -3.54545);
    fpm.run(2444, -157);
    fpm.run(5.0, 3.0);


    std::cout << " ---------------------------------------- " << std::endl;
    std::cout << "Ingrese un numero: " << std::endl;
    std::cin >> a;
    std::cout << "Ingrese otro numero: " << std::endl;
    std::cin >> b;
    fpm.run(a, b);

    return 0;
}