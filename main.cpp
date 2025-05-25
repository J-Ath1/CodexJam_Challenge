#include <vector>
#include <string>
#include <cstdint>
#include <tuple>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <random>

// Challenge - 1gb RAM cap, single thread only, speed challenge - Fastest Time Only
// Rules -
// NO compilation optimisations allowed
// NO max/min
// NO 'advanced' data structures - array/buffer ONLY (asides for return format)

class Solutions {
public:
    static int32_t char_find(std::vector<uint32_t> inputs);
    static uint64_t depleting_odds(std::vector<uint16_t> inputs);
    static uint64_t combo_cool(uint64_t k, uint64_t n);
    static std::tuple<std::string, uint32_t> card_match(std::vector<uint64_t> numbers);
    static double road_illumination(uint64_t road_length, std::vector<uint64_t> poles);
};
    int32_t Solutions::char_find(std::vector<uint32_t> inputs) {
        static uint64_t BITVEC[1ULL << 26];
        const uint32_t* data = inputs.data();
        const size_t size = inputs.size();

        for (size_t idx = 0; idx < size; ++idx)
        {
            uint32_t v = data[idx];
            
            uint64_t &word = BITVEC[v >> 6];
            uint64_t mask = 1ULL << (v & 63);

            if (word & mask)
                return static_cast<int32_t>(idx);

            word |= mask;
        }
        return -1;
    }


    uint64_t Solutions::depleting_odds(std::vector<uint16_t> inputs) {
        const uint16_t* cur  = inputs.data();
        const uint16_t* last = cur + inputs.size();

        uint64_t  odd_sum = 0;
        uint16_t  min_v   = 0xFFFF;
        uint16_t  max_v   = 0;

        while (cur != last)
        {
            uint16_t v = *cur++;

            min_v = (v < min_v) ? v : min_v;
            max_v = (v > max_v) ? v : max_v;

            if (v & 1) odd_sum += v;
        }

        min_v |= 1U;
        max_v -= static_cast<uint16_t>(~max_v & 1U);

        if (min_v > max_v) return 0;

        uint64_t count = (static_cast<uint64_t>(max_v - min_v) >> 1) + 1;
        uint64_t full  = (count * static_cast<uint64_t>(max_v + min_v)) >> 1;

        return full - odd_sum;  
    }

    uint64_t Solutions::combo_cool(uint64_t k, uint64_t n) {
        static const uint64_t MOD = 10000000000000061ULL;
        k %= MOD;
        uint64_t k_pow = 1;
        uint64_t ans   = 0;

        while (n)
        {
            if (n & 1ULL)
            {
                ans += k_pow;
                if (ans >= MOD) ans -= MOD;
            }
            n >>= 1ULL;

            uint64_t a = k_pow;
            uint64_t b = k;
            uint64_t res = 0;

            while (b)
            {
                if (b & 1ULL)
                {
                    res += a;
                    if (res >= MOD) res -= MOD;
                }
                a <<= 1ULL;
                if (a >= MOD) a -= MOD;
                b >>= 1ULL;
            }
            k_pow = res;
        }
        return ans;
    }

    std::tuple<std::string, uint32_t> Solutions::card_match(std::vector<uint64_t> numbers) {
        uint64_t number_count = numbers.size();
        if (numbers.empty()) {
            return {"Tie", 0};
        }
        std::sort(numbers.begin(), numbers.end());

        uint32_t player1_score = 0;
        uint32_t player2_score = 0;

        bool player1_turn = true;
        for (int i = 0; i < number_count; i++) {
            uint64_t popped = numbers[number_count - 1 - i];
            if (player1_turn) {
                if ((popped & 1) == 0) {
                    player1_score += popped;
                }
                player1_turn = !(player1_turn);
            } else {
                if ((popped & 1) == 1) {
                    player2_score += popped;
                }
                player1_turn = !(player1_turn);
            }
        }
        if (player1_score == player2_score) {
            return {"Tie", 0};
        }
        if (player1_score > player2_score) {
            return {"Player 1", player1_score};
        } else {
            return {"Player 2", player2_score};
        }

    }

    double Solutions::road_illumination(uint64_t road_length, std::vector<uint64_t> poles) {
        uint64_t lamp_count = poles.size();
        if (lamp_count == 0) return 0.0;
        if (lamp_count == 1) {
            uint64_t start_dist = poles[0];
            uint64_t end_dist = road_length - start_dist;
            if (start_dist > end_dist) return start_dist;
            else return end_dist;
        }
        std::sort(poles.begin(), poles.end());
        uint64_t largest = 0;
        uint64_t init = poles[0];
        uint64_t final = road_length - poles[lamp_count - 1];
        if (init > final) largest = init * 2;
        else largest = final * 2;

        const uint64_t* p   = poles.data();
        const uint64_t* end = p + lamp_count - 1;
        while (p != end) {
            uint64_t gap = p[1] - p[0];
            if (gap > largest) largest = gap;
            ++p;
        }

        return largest * 0.5;
    }

int main() {
        using namespace std;
        using namespace std::chrono;
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // RNG setup
    mt19937_64 rng(12345);
    uniform_int_distribution<uint32_t> dist32(0, 1000000);
    uniform_int_distribution<uint16_t> dist16(0, 10000);
    uniform_int_distribution<uint64_t> dist64(0, 1000000);

    // 1) char_find
    {
        vector<uint32_t> v;
        const size_t N = 1'000'000;
        v.reserve(N);
        for (size_t i = 0; i < N; ++i)
            v.push_back(dist32(rng));
        // introduce a duplicate at the end
        v.push_back(v[ N/2 ]);

        auto t0 = high_resolution_clock::now();
        int32_t idx = Solutions::char_find(v);
        auto t1 = high_resolution_clock::now();

        cout << "char_find returned " << idx
             << " in "
             << duration_cast<microseconds>(t1 - t0).count()
             << " micro s\n";
    }

    // 2) depleting_odds
    {
        vector<uint16_t> v;
        const size_t N = 2'000'000;
        v.reserve(N);
        for (size_t i = 0; i < N; ++i)
            v.push_back(dist16(rng));

        auto t0 = high_resolution_clock::now();
        uint64_t sum_missing = Solutions::depleting_odds(v);
        auto t1 = high_resolution_clock::now();

        cout << "depleting_odds returned " << sum_missing
             << " in "
             << duration_cast<milliseconds>(t1 - t0).count()
             << " ms\n";
    }

    // 3) combo_cool
    {
        uint64_t k = 1234567;
        uint64_t n = (1ULL<<40) + 12345;  // some large-ish n

        auto t0 = high_resolution_clock::now();
        uint64_t cool = Solutions::combo_cool(k, n);
        auto t1 = high_resolution_clock::now();

        cout << "combo_cool returned " << cool
             << " in "
             << duration_cast<microseconds>(t1 - t0).count()
             << " micro s\n";
    }

    // 4) card_match
    {
        vector<uint64_t> v;
        const size_t N = 1'000'000;
        v.reserve(N);
        for (size_t i = 0; i < N; ++i)
            v.push_back(dist64(rng));
        // force one match
        v.push_back(v[N/3]);

        auto t0 = high_resolution_clock::now();
        auto [winner, score] = Solutions::card_match(v);
        auto t1 = high_resolution_clock::now();

        cout << "card_match returned (" << winner << ", " << score << ")"
             << " in "
             << duration_cast<milliseconds>(t1 - t0).count()
             << " ms\n";
    }

    // 5) road_illumination
    {
        const uint64_t L = 1'000'000;
        const size_t M = 500'000;
        vector<uint64_t> poles;
        poles.reserve(M);
        for (size_t i = 0; i < M; ++i)
            poles.push_back(dist64(rng) % (L+1));

        auto t0 = high_resolution_clock::now();
        double R = Solutions::road_illumination(L, poles);
        auto t1 = high_resolution_clock::now();

        cout << "road_illumination returned " << R
             << " in "
             << duration_cast<milliseconds>(t1 - t0).count()
             << " ms\n";
    }

}
