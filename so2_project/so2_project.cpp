#include <iostream>
#include <thread>

using namespace std;

void PrintHelloWorld()
{
    cout << "Hello world!" << endl;
}

int main()
{
    thread th1(PrintHelloWorld);
    th1.join();
    return 0;
}