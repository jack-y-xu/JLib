
#ifndef SAFER_RANDOM
#define SAFER_RANDOM

#include "doctest.h"

#include <concepts>
#include <type_traits>
#include <ranges>
#include <numeric>
#include <random>
#include <stdexcept>

namespace SaferRandom {

    using namespace std::ranges;

    template <typename T>
    concept true_floating = !std::integral<T> && std::floating_point<T>;

    namespace internal {
        struct GeneratorInitWrapper {
            unsigned int seed;
            std::mt19937 generator;
            GeneratorInitWrapper() {
                auto rd = std::random_device();
                seed = rd();
                generator = std::mt19937(seed);
            };
            GeneratorInitWrapper(unsigned int s): seed(s), generator(s) {};
        };
    }

    class ParallelRandom {

    public:

        // C++ linker doesn't work with static thread_local as class variable.
        // This is a temporary workaround for static thread_local
        inline static internal::GeneratorInitWrapper& getGen() {
            static thread_local internal::GeneratorInitWrapper gen;
            return gen;
        }

        inline static unsigned int getSeed() {
            return getGen().seed;
        }

        inline static std::mt19937& getGenerator() {
            return getGen().generator;
        }

        template <true_floating T>
        inline static T random() {
            static std::uniform_real_distribution<T> dist(0, 1);
            return dist(getGenerator());
        }

        template<true_floating T>
        inline static T random(T low, T high) {
            return (high - low) * random<T>() + low;
        }

        inline static int randint(int low, int high_inclusive) {
            std::uniform_int_distribution<> dist(low, high_inclusive);
            return dist(getGenerator());
        }

        template <true_floating T>
        inline static bool bernoulli(T p) {
            return random<T>() <= p;
        }

        template <range Probs, range Elements, typename T>
        requires true_floating<range_value_t<Probs>> && std::same_as<range_value_t<Elements>, T>
        static T weighted_select(const Probs& p, const Elements& e, T fallback, bool safety_check=true) {
            if (safety_check) {
                // size runtime is O(n) if not using sized_range
                if (size(p) != size(e))
                    throw std::runtime_error("Range sizes must be the same");

                if (std::any_of(p.begin(), p.end(), [](auto& x) {return x < 0 || x > 1;}))
                    throw std::runtime_error("All probabilities must be between 0 and 1");

                double sum = std::accumulate(p.begin(), p.end(), 0.0);
                if (sum > 1 || sum < 0)
                    throw std::runtime_error("Probabilities must sum up to between 0 and 1");
            }
            double rand = random<double>(), partial = 0;
            auto pi = p.begin(), ei = e.begin();

            while (pi != p.end()) {
                partial += *pi;
                if (rand <= partial)
                    return *ei;
                pi++;
                ei++;
            }

            return fallback;
        }
    };
}

#endif
