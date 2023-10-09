int sum(int N) {
    int result = 0;
    for (int i = 1; i <= N; i++) {
        result += i;
    }
    return result;
}


int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}


int fibonacci(int n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

