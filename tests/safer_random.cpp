#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "safer_random.h"

#include <thread>
#include <vector>
#include <iostream>

void thread_func(unsigned int& record) {
    record = SaferRandom::ParallelRandom::getSeed();
}

TEST_CASE("check that seeds are different across threads") {

    int n = 10;
    std::vector<unsigned int> seeds(n);
    std::vector<std::thread> ts(n);

    for (int i = 0; i < 10; i++) {
        ts[i] = std::thread(thread_func, std::ref(seeds[i]));
    }

    std::cout << "Thread seeds: ";
    for (int i = 0; i < 10; i ++) {
        ts[i].join();
        std::cout << seeds[i] << " ";
    }

    std::cout << '\n';

    CHECK(std::unordered_set<int>(seeds.begin(), seeds.end()).size() != 1);
}
