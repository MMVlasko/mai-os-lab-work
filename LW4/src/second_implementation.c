float Pi(const int K) {
    float pi = 1.0;
    for (int i = 1; i <= K; i++) {
        pi *= (4.0 * i * i) / (4.0 * i * i - 1);
    }
    return pi * 2;
}

float E(const int x) {
    float e = 1.0;
    float fact = 1.0;

    for (int n = 1; n <= x; n++) {
        fact *= n;
        e += 1.0 / fact;
    }
    return e;
}