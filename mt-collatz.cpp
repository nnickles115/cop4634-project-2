#include <atomic>
#include <cstring>
#include <iostream>
#include <mutex>
#include <thread>
#include <time.h>
#include <vector>

/**
 * @file mt-collatz.cpp
 * @brief This program calculates the stopping times of the Collatz sequence for numbers 
 * in the range [1, N] using T threads. 
 * 
 * The optional `-nolock` argument can be used to disable thread synchronization for the 
 * shared histogram, allowing for experimentation with race conditions. 
 * It also outputs a histogram of the computed stopping times, as well as the total 
 * elapsed time for the computation, measured in seconds and nanoseconds.
 * 
 * @author Noah Nickles
 * @author Dylan Stephens
 * @date 10/2/2024
 * @details Course COP4634
 */

std::mutex mtx;  // Mutex for synchronized access to global data.
std::atomic<int> COUNTER(1);  // Atomic counter for distributing work.
std::vector<int> histogram(1001, 0);  // Global histogram, initally filled with 0s.
bool use_locks = true;  // By default, use locks for synchronization.

/**
 * This function computes the stopping time of a given number 'n' using the Collatz sequence.
 * The stopping time is the number of steps it takes for 'n' to reach 1.
 * 
 * Collatz sequence:
 * - If 'n' is even, the next number is 'n / 2'.
 * - If 'n' is odd, the next number is '3n + 1'.
 * 
 * The function terminates when 'n' becomes 1, or if it reaches 1000 steps.
 * 
 * @param n The initial number for which the Collatz stopping time is computed.
 * @return The number of steps (stopping time) it takes for 'n' to reach 1.
 */
int compute_stopping_time(int n) {
    int steps = 0;
    while(n != 1 && steps < 1000) {
        if(n % 2 == 0) {
            n /= 2;
        }
        else {
            n = 3 * n + 1;
        }
        steps++;
    }
    return steps;
}

/**
 * Function executed by each thread to compute the Collatz stopping times for numbers 
 * in the range [1, N]. Each thread fetches a unique number from a shared counter 
 * ('COUNTER'), computes its Collatz stopping time, and updates the global histogram
 * with the stopping time frequency.
 * 
 * The function handles thread-safe access to the shared 'COUNTER' using an atomic 
 * operation ('fetch_add'), ensuring that each thread gets a unique number to process.
 * 
 * The function also synchronizes access to the global 'histogram' using a mutex 
 * when 'use_locks' is set to true. If 'use_locks' is false, the function skips
 * synchronization to allow race conditions for experimentation.
 * 
 * @param N The maximum number in the range for which the Collatz stopping time is computed.
 */
void collatz_worker(int N) {
    while(true) {
        // Atomically fetch and increment COUNTER to ensure each thread processes a unique number.
        // This guarantees thread-safe access to COUNTER without needing a mutex.
        int num = COUNTER.fetch_add(1);

        // If the current number exceeds the max (N), end the loop.
        if(num > N) break;

        // Compute the stopping time for the fetched number using the Collatz sequence.
        int stopping_time = compute_stopping_time(num);

        // If locks are enabled, use a mutex to protect access to the shared histogram.
        // std::lock_guard locks the mutex in its constructor and unlocks it when it goes out of scope.
        if(use_locks) {
            std::lock_guard<std::mutex> scoped_lock(mtx);
        }

        // Update the histogram with the computed values.
        if(stopping_time <= 1000) {
            histogram[stopping_time]++;
        }
    }
}

/**
 * Entry point of the program. Tracks the start and end times of the program execution
 * and calculates the total elapsed time. Writes histogram data to stdout and
 * elapsed time data to stderr.
 * 
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line argument strings.
 * @return Returns 0 on successful completion, or 1 if there is an error with the input.
 */
int main(int argc, char** argv) {
    // Output correct usage and return with error if number of args aren't correct.
    if(argc < 3) {
        std::cerr << "Usage: ./mt-collatz <N> <T> [-nolock]" << std::endl;
        return 1;
    }

    int N = std::atoi(argv[1]); // Range of numbers to compute.
    int T = std::atoi(argv[2]); // Number of threads to use.

    // Check for -nolock option (optional argument).
    if(argc == 4 && std::strcmp(argv[3], "-nolock") == 0) {
        use_locks = false;
    }

    // Measure start time.
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_REALTIME, &start_time);

    // Create and launch threads.
    std::vector<std::thread> threads;
    for(int i = 0; i < T; ++i) {
        threads.emplace_back(collatz_worker, N);
    }

    // Join threads.
    for(auto& th : threads) {
        th.join();
    }

    // Measure end time.
    clock_gettime(CLOCK_REALTIME, &end_time);

    // Calculate elapsed time.
    long seconds     = end_time.tv_sec  - start_time.tv_sec;
    long nanoseconds = end_time.tv_nsec - start_time.tv_nsec;
    // Check if the nanosecond difference is negative.
    if(nanoseconds < 0) {
        // Borrow 1 second from the seconds value to normalize the nanoseconds.
        seconds     -= 1;
        // Add 1 billion nanoseconds (1 second) to the nanoseconds value.
        nanoseconds += 1000000000L;
    }

    // Output the histogram.
    for(int i = 0; i <= 1000; ++i) {
        std::cout << i << "," << histogram[i] << std::endl;
    }

    // Output the elapsed time to stderr.
    std::cerr << N << "," << T << "," << seconds << "." << nanoseconds << std::endl;

    return 0;
}