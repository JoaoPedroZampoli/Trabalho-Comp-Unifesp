int x;
int y[10];

int soma(int a, int b) {
    return a + b;
}

int fatorial(int n) {
    if (n <= 1)
        return 1;
    else
        return n * fatorial(n - 1);
}

void main(void) {
    int i;
    int resultado;
    
    x = 5;
    y[0] = 10;
    
    resultado = soma(x, y[0]);
    
    i = 0;
    while (i < 10) {
        y[i] = i * 2;
        i = i + 1;
    }
    
    resultado = fatorial(5);
}
