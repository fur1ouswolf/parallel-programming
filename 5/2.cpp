#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <cmath>
#include <map>
#include <vector>

enum class EventType {
    FACTORIAL_READY,
    POWER_READY
};

class Event {
public:
    Event() : triggered(false) {}

    void wait() {
        std::unique_lock<std::mutex> lock(mutex);
        while (!triggered) {
            cv.wait(lock);
        }
    }

    void signal() {
        std::unique_lock<std::mutex> lock(mutex);
        triggered = true;
        cv.notify_all();
    }

    void reset() {
        std::unique_lock<std::mutex> lock(mutex);
        triggered = false;
    }

private:
    std::mutex mutex;
    std::condition_variable cv;
    bool triggered;
};

class EventHandler {
public:
    void trigger(EventType type) {
        events[type].signal();
    }

    void wait(EventType type) {
        events[type].wait();
    }

    void reset(EventType type) {
        events[type].reset();
    }

private:
    std::map<EventType, Event> events;
};

class Rendezvous {
public:
    Rendezvous() {}

    void set_factorial_result(double result) {
        std::lock_guard<std::mutex> lock(mutex);
        factorial_result = result;
        eventHandler.trigger(EventType::FACTORIAL_READY);
    }

    void set_power_result(double result) {
        std::lock_guard<std::mutex> lock(mutex);
        power_result = result;
        eventHandler.trigger(EventType::POWER_READY);
    }

    std::pair<double, double> get_results() {
        eventHandler.wait(EventType::FACTORIAL_READY);
        eventHandler.wait(EventType::POWER_READY);

        std::lock_guard<std::mutex> lock(mutex);
        auto results = std::make_pair(factorial_result, power_result);

        eventHandler.reset(EventType::FACTORIAL_READY);
        eventHandler.reset(EventType::POWER_READY);

        return results;
    }

private:
    std::mutex mutex;
    double factorial_result;
    double power_result;
    EventHandler eventHandler;
};

double factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

double power(double base, int exp) {
    double result = 1;
    for (int i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}

double calculate_ex_term(double x, int n, Rendezvous& rendez) {
    std::thread factorial_thread([&] {
        double fact = factorial(n);
        rendez.set_factorial_result(fact);
    });

    std::thread power_thread([&] {
        double pow_x = power(x, n);
        rendez.set_power_result(pow_x);
    });

    auto [fact, pow_x] = rendez.get_results();
    double term = pow_x / fact;

    factorial_thread.join();
    power_thread.join();

    return term;
}

int main() {
    double x = 2.0;
    int n = 10;

    Rendezvous rendez;
    double result = 0;

    for (int i = 0; i <= n; i++) {
        result += calculate_ex_term(x, i, rendez);
    }

    std::cout << "e^x = " << result << std::endl;
    std::cout << "std::exp: " << std::exp(x) << std::endl;

    return 0;
}