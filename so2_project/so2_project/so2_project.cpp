#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <random>


using namespace std;


//Definiowanie stałych.
enum {
    //Ilość filozofów
    NUMBER_OF_PHILOSOPHERS = 4
};


//Funkcja zwracająca losową liczbę z podanego zakresu.
int getRandomNumber(int minimalNumber, int maximalNumber) {
    static mt19937 rnd(time(nullptr));
    return uniform_int_distribution<int>(minimalNumber, maximalNumber)(rnd);
}


//Utworzenie mutexu dla funkcji związanych z wyświetlaniem.
mutex mutexCout;


//Funkcja myslenia, parametrem jest numer filozofa.
void think(int philosopherNumber) {
    int time = getRandomNumber(1000, 2000);


    //Zablokowanie mutexu oraz wykonanie "bezpiecznego" kodu. 
    // W tym wypadku zapewnia nam to spójnośc wyświetlanych danych, poprzez zablokowanie dostępu do cout. 
    //Inny wątek przypadkowo nie przerwie wyświetlania w konsoli.
    {
        lock_guard<mutex> lock(mutexCout);
        cout << "Philosopher[" << philosopherNumber << "] thinks " << time << "ms" << endl;
    }


    //Wstrzymanie wątku na określony czas myślenia.
    this_thread::sleep_for(chrono::milliseconds(time));
}


//Funkcja pobierania lewej pałeczki. Parametrem jest numer filozofa.
void takeLeftChopstick(int philosopherNumber, mutex& mutexLeftChopstick, int leftChopstickNumber) {


    //Zablokowanie mutexu oraz wykonanie "bezpiecznego" kodu. 
    // Najpier blokujemy Mutex pałeczki, dopóki się nie zwolni dostęp do pałeczki, wykonanie wątku jest wstrzymane.
    // Po uzyskanie dostępu do pałeczki inny wątek nam jej nie zabierze.
    // Blokujemy kolejny mutex, który zablokowuje nam cout.
    //Inny wątek przypadkowo nie przerwie wyświetlania w konsoli.
    //lock_guard<mutex> lock(mutexLeftChopstick);
    mutexLeftChopstick.lock();
    {
        lock_guard<mutex> lock(mutexCout);
        cout << "\t\t\t\t" << "Philosopher[" << philosopherNumber << "] got left chopstick[" << leftChopstickNumber << "]." << endl;
    }
}


//Funkcja pobierania lewejprawej pałeczki. Parametrem jest numer filozofa.
void takeRightChopstick(int philosopherNumber, mutex& mutexRightChopstick, int rightChopstickNumber) {


    //Zablokowanie mutexu oraz wykonanie "bezpiecznego" kodu. 
    // Najpier blokujemy Mutex pałeczki, dopóki się nie zwolni dostęp do pałeczki, wykonanie wątku jest wstrzymane.
    // Po uzyskanie dostępu do pałeczki inny wątek nam jej nie zabierze.
    // Blokujemy kolejny mutex, który zablokowuje nam cout.
    //Inny wątek przypadkowo nie przerwie wyświetlania w konsoli.
    mutexRightChopstick.lock();
    {
        lock_guard<mutex> lock(mutexCout);
        cout << "\t\t\t\t" << "Philosopher[" << philosopherNumber << "] got right chopstick[" << rightChopstickNumber << "]." << endl;
    }
}


//Funkcja jedzenia. Parametrem jest numer filozofa. Oraz oba mutexy pałeczek
void eat(int philosopherNumber, mutex& mutexLeftChopstick, mutex& mutexRightChopstick) {
    int time = getRandomNumber(1000, 2000);


    //Zablokowanie mutexu oraz wykonanie "bezpiecznego" kodu. 
    // W tym wypadku zapewnia nam to spójnośc wyświetlanych danych, poprzez zablokowanie dostępu do cout. 
    //Inny wątek przypadkowo nie przerwie wyświetlania w konsoli.
    {
        lock_guard<mutex> lock(mutexCout);
        cout << "\t\t\t\t\t\t\t\t\t" << "Philosopher[" << philosopherNumber << "] eats " << time << "ms" << endl;
    }


    //Wstrzymanie wątku na określony czas myślenia.
    this_thread::sleep_for(chrono::milliseconds(time));


    //Odblokowanie obydwu pałeczek.
    mutexLeftChopstick.unlock();
    mutexRightChopstick.unlock();
}


//Funkcja uruchamiana przez wątek każdego filozofa, parametrem jest numer filozofa oraz prawa i lewa pałeczka (oraz ich numery).
void philosopher(int philosopherNumber, mutex& mutexLeftChopstick, int leftChopstickNumber, mutex& mutexRightChopstick, int rightChopstickNumber) {
    

    //Pętla którą powtarzamy w nieskończoność.
    while (true) {

        think(philosopherNumber);

        takeLeftChopstick(philosopherNumber, mutexLeftChopstick, leftChopstickNumber);

        takeRightChopstick(philosopherNumber, mutexRightChopstick, rightChopstickNumber);

        eat(philosopherNumber, mutexLeftChopstick, mutexRightChopstick);
    }
}


int main() {
    srand(time(nullptr));


    //Tworzymy tablicę wątków. Każdy wątek odpowiada filozofowi.
    thread threadsPhilosophers[NUMBER_OF_PHILOSOPHERS];


    //Tworzymy tablicę mutexów, każdy mutex odpwiadam pałeczce do jedzenia.
    mutex mutexChopsticks[NUMBER_OF_PHILOSOPHERS];


    //Uruchamianie poszczegolnych wątków odpowiadających filozofom. (miało być w pętli, ale pętla coś nie działała XD)
    threadsPhilosophers[0] = thread([&] {philosopher(1, mutexChopsticks[0], 1, mutexChopsticks[1], 2); });
    threadsPhilosophers[1] = thread([&] {philosopher(2, mutexChopsticks[1], 2, mutexChopsticks[2], 3); });
    threadsPhilosophers[2] = thread([&] {philosopher(3, mutexChopsticks[2], 3, mutexChopsticks[3], 4); });

    //Odwrócenie pałeczek (lewej z prawą). Rozwiązuje to potencjalny problem zakleszczenia.
    threadsPhilosophers[3] = thread([&] {philosopher(4, mutexChopsticks[0], 1, mutexChopsticks[3], 4); });


    //Oczekiwanie na zakończenie wątków.
    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; ++i) {
        threadsPhilosophers[i].join();
    }
}