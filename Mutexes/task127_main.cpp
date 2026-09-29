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

    someData() : name(""), surname(""), address(""), age(0) {}
};

class exchangePerson {
public:
    someData data;
    mutex mtx;

    static void JohnDoe(exchangePerson& p) {
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
        if (&p1 == &p2) {
            cout << "Помилка: спроба обміняти об'єкт сам із собою!" << endl;
            return;
        }

        // Створюємо unique_lock з defer_lock.
        // Це зв'язує м'ютекси з обгортками, але НЕ блокує їх одразу.
        unique_lock<mutex> lock1(p1.mtx, defer_lock);
        unique_lock<mutex> lock2(p2.mtx, defer_lock);

        // Блокуємо обидва м'ютекси одночасно без ризику взаємного блокування (deadlock).
        // std::lock вміє працювати з об'єктами unique_lock.
        lock(lock1, lock2);

        cout << "--- До обміну ---" << endl;
        cout << "Об'єкт 1: " << p1.data.name << " " << p1.data.surname 
             << " | Адреса: " << p1.data.address << " | Вік: " << p1.data.age << endl;
        cout << "Об'єкт 2: " << p2.data.name << " " << p2.data.surname 
             << " | Адреса: " << p2.data.address << " | Вік: " << p2.data.age << endl;

        swap(p1.data, p2.data);

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

    thread t1(exchangePerson::JohnDoe, ref(person1));
    thread t2(exchangePerson::JacobSmith, ref(person2));

    t1.detach();
    t2.detach();

    this_thread::sleep_for(chrono::milliseconds(100));

    thread t3(exchangePerson::Swap, ref(person1), ref(person2));
    t3.join();

    return 0;
}