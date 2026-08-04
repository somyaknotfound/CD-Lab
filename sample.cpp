// sample.cpp - test input for the Week 3 lexical analyzers
#include <iostream>
using namespace std;

const double PI = 3.14159;
int globalCount = 0;
float rates[3];
char grade = 'A';

int add(int a, int b);                 /* prototype */
double average(int arr[], int n, bool skipZero);
void banner();

int add(int a, int b) {
    int sum = a + b;
    return sum;
}

double average(int arr[], int n, bool skipZero) {
    double total = 0.0;
    int used = 0;
    for (int i = 0; i < n; i++) {
        if (skipZero && arr[i] == 0) continue;
        total += arr[i];
        used++;
    }
    return used ? total / used : 0.0;
}

void banner() {
    cout << "Lexical Analysis Demo" << endl;
}

int main() {
    int octal = 0755;
    int hexa = 0x1F4;
    int decimal = 250;
    float ratio = 2.5e-3;
    bool flag = true;

    int values[3] = {10, 0, 20};
    double avg = average(values, 3, flag);

    if (decimal >= hexa && !flag) {
        globalCount++;
    } else {
        globalCount--;
    }

    decimal += add(octal, hexa);
    ratio *= 2;

    banner();
    cout << avg << " " << ratio << " " << PI << endl;
    return 0;
}
