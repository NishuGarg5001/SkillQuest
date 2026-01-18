#include <random>

std::mt19937 rng{ std::random_device{}() };

int random_int(int min, int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}