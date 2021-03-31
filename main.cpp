#include <iostream>
#include <utility>
#include <vector>
#include <cmath>

const long long modulo = 1'000'000'000'000;

bool bit(long long mask, int pos) {
    return (mask >> pos) & 1;
}

bool correct(int n, long long mask1, long long mask2) {
    for (int i = 1; i < n; ++i)
        if (bit(mask1, i) == bit(mask2, i) && bit(mask1, i-1) == bit(mask2, i-1) && bit(mask2, i) == bit(mask2, i - 1))
            return false;
    return true;
}

void to_one(std::vector<std::vector<long long>> &matrix) {
    for (int i = 0; i < matrix.size(); ++i) {
        for (int j = 0; j < matrix.size(); ++j)
            if (i == j) matrix[i][j] = 1;
            else matrix[i][j] = 0;
    }
}

std::vector<std::vector<long long>> dot(std::vector<std::vector<long long>> &m1, std::vector<std::vector<long long>> &m2) {
    std::vector<std::vector<long long>> res(m1.size(), std::vector<long long>(m2.size()));
    for(int i = 0; i < m1.size(); ++i)
        for(int j = 0; j < m2[0].size(); ++j)
            for(int k = 0; k < m2.size(); ++k)
                res[i][j] += m1[i][k] * m2[k][j] % modulo;
    return res;
}

void matrix_exponent(std::vector<std::vector<long long>> &matrix, int n) {
    if (n == 0) {
        to_one(matrix);
        return;
    }
    std::vector<std::vector<long long>> p = matrix;
    to_one(matrix);
    while (n) {
        if (n&1)
            matrix = dot(matrix, p);
        p = dot(p, p);
        n >>= 1;
    }

}

int main() {
    int n, m;
    std::cin >> n >> m;
    if (n > m) std::swap(n,m);
    long long size = pow(2, n);
    std::vector<long long> column(size, 1); // count per masks
    std::vector<std::vector<long long>> matrix(size, std::vector<long long>(size));

    for (long long mask1 = 0; mask1 < size; ++mask1) {
        for (long long mask2 = 0; mask2 < size; ++mask2) {
            if (correct(n, mask1, mask2))
                matrix[mask1][mask2] = 1;
        }
    }

    matrix_exponent(matrix, m - 1);

    long long sum = 0;
    for (int i = 0; i < matrix.size(); ++i) {
        for (int j = 0; j < matrix.size(); ++j)
            sum += matrix[i][j] * column[j];
    }

    std::cout << sum;
    return 0;
}
