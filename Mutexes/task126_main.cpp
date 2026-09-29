#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;


class someData {
public:
    string name;
    string surname;
    string address;
    int age;

    // Конструктор за замовчуванням
    someData() : name(""), surname(""), address(""), age(0) {}
};


class exchangePerson {
public:
    someData data;
    mutex mtx;

    
    static void JohnDoe(exchangePerson& p) {
        // Гарантуємо ізольованість доступу за допомогою lock_guard
        lock_guard<mutex> lock(p.mtx);
        p.data.name = "John";
        p.data.surname = "Doe";
        p.data.address = "Unknown";
        p.data.age = 120;
    }

    
    static void JacobSmith(exchangePerson& p) {
        
        lock_guard<mutex> lock(p.mtx);
        p.data.name = "Jacob";
        p.data.surname = "Smith";
        p.data.address = "Known";
        p.data.age = 1;
    }

   
    static void Swap(exchangePerson& p1, exchangePerson& p2) {
        // Виключаємо можливість роботи з об'єктами за однаковим посиланням
        if (&p1 == &p2) {
            cout << "Помилка: спроба обміняти об'єкт сам із собою!" << endl;
            return;
        }

        //lock гарантує відсутність взаємного блокування (deadlock) при захопленні кількох м'ютексів
        lock(p1.mtx, p2.mtx);

        //adopt_lock вказує lock_guard, що м'ютекси вже заблоковані
        lock_guard<mutex> lock1(p1.mtx, adopt_lock);
        lock_guard<mutex> lock2(p2.mtx, adopt_lock);

        // Вивід даних до обміну
        cout << "--- До обміну ---" << endl;
        cout << "Об'єкт 1: " << p1.data.name << " " << p1.data.surname 
             << " | Адреса: " << p1.data.address << " | Вік: " << p1.data.age << endl;
        cout << "Об'єкт 2: " << p2.data.name << " " << p2.data.surname 
             << " | Адреса: " << p2.data.address << " | Вік: " << p2.data.age << endl;

        // Здійснюємо обмін даними (обмінюємо вміст полів someData)
        swap(p1.data, p2.data);

        // Вивід даних після обміну
        cout << "\n--- Після обміну ---" << endl;
        cout << "Об'єкт 1: " << p1.data.name << " " << p1.data.surname 
             << " | Адреса: " << p1.data.address << " | Вік: " << p1.data.age << endl;
        cout << "Об'єкт 2: " << p2.data.name << " " << p2.data.surname 
             << " | Адреса: " << p2.data.address << " | Вік: " << p2.data.age << endl;
    }
};

int main() {
    
    exchangePerson person1;
    exchangePerson person2;

    
    // Використовуємо ref для передачі об'єктів за посиланням
    thread t1(exchangePerson::JohnDoe, ref(person1));
    thread t2(exchangePerson::JacobSmith, ref(person2));

    
    t1.detach();
    t2.detach();

    // Невелика затримка, щоб від'єднані потоки встигли записати дані до того, як розпочнеться обмін
    this_thread::sleep_for(chrono::milliseconds(100));

    
    thread t3(exchangePerson::Swap, ref(person1), ref(person2));

    // Гарантуємо отримання результатів роботи потоку t3 до завершення головного
    t3.join();

    return 0;
}