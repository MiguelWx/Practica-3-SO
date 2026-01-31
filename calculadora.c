#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define STACK_SIZE 256
#define TOKEN_LEN 128

static double stack[STACK_SIZE];
static int top = 0;

void push(double v) {
    if (top >= STACK_SIZE) {
        fprintf(stderr, "Error: pila llena\n");
        return;
    }
    stack[top++] = v;
}

double pop(void) {
    if (top <= 0) {
        fprintf(stderr, "Error: pila vacia\n");
        return 0.0;
    }
    return stack[--top];
}

double peek(void) {
    if (top <= 0) {
        fprintf(stderr, "Error: pila vacia\n");
        return 0.0;
    }
    return stack[top - 1];
}

void print_stack(void) {
    if (top == 0) {
        printf("(pila vacia)\n");
        return;
    }
    printf("Pila (base -> tope):\n");
    for (int i = 0; i < top; ++i) {
        printf("  [%d] %g\n", i, stack[i]);
    }
}

int is_number(const char *s, double *out) {
    char *endptr;
    if (s == NULL || *s == '\0') return 0;
    double v = strtod(s, &endptr);
    if (endptr == s) return 0;
    // aceptar formatos como "1e-3" etc
    *out = v;
    return 1;
}

int main(void) {
    char token[TOKEN_LEN];

    printf("Calculadora RPN simple\n");
    printf("Ingrese tokens separados por espacios o saltos de linea\n");
    printf("Operadores: + - * / ^    Funciones: sin cos tan sqrt exp ln\n");
    printf("Comandos: p (print top), f (full stack), c (clear), dup, swap, q o quit (salir)\n");

    while (1) {
        printf("rpn> ");
        if (scanf("%127s", token) != 1) { // EOF o error
            putchar('\n');
            break;
        }

        // salir
        if (strcmp(token, "q") == 0 || strcmp(token, "quit") == 0) break;

        // comandos simples
        if (strcmp(token, "p") == 0) {
            if (top > 0) printf("%g\n", peek());
            else printf("(pila vacia)\n");
            continue;
        } else if (strcmp(token, "f") == 0) {
            print_stack();
            continue;
        } else if (strcmp(token, "c") == 0) {
            top = 0;
            printf("Pila limpiada\n");
            continue;
        } else if (strcmp(token, "dup") == 0) {
            if (top <= 0) { fprintf(stderr, "Error: nada que duplicar\n"); continue; }
            push(peek());
            continue;
        } else if (strcmp(token, "swap") == 0) {
            if (top < 2) { fprintf(stderr, "Error: se necesitan al menos 2 elementos para swap\n"); continue; }
            double a = pop();
            double b = pop();
            push(a);
            push(b);
            continue;
        }

        // numero?
        double val;
        if (is_number(token, &val)) {
            push(val);
            continue;
        }

        // operadores binarios de un solo caracter
        if (strlen(token) == 1) {
            char op = token[0];
            if (op == '+' || op == '-' || op == '*' || op == '/' || op == '^') {
                if (top < 2) { fprintf(stderr, "Error: operandos insuficientes para '%c'\n", op); continue; }
                double b = pop();
                double a = pop();
                double res = 0.0;
                if (op == '+') res = a + b;
                else if (op == '-') res = a - b;
                else if (op == '*') res = a * b;
                else if (op == '/') {
                    if (b == 0.0) { fprintf(stderr, "Error: division por cero\n"); push(a); push(b); continue; }
                    res = a / b;
                } else if (op == '^') {
                    res = pow(a, b);
                }
                push(res);
                continue;
            }
        }

        // funciones unarias por nombre
        if (strcmp(token, "sin") == 0) {
            if (top < 1) { fprintf(stderr, "Error: operandos insuficientes para sin\n"); continue; }
            double a = pop();
            push(sin(a));
            continue;
        } else if (strcmp(token, "cos") == 0) {
            if (top < 1) { fprintf(stderr, "Error: operandos insuficientes para cos\n"); continue; }
            double a = pop();
            push(cos(a));
            continue;
        } else if (strcmp(token, "tan") == 0) {
            if (top < 1) { fprintf(stderr, "Error: operandos insuficientes para tan\n"); continue; }
            double a = pop();
            push(tan(a));
            continue;
        } else if (strcmp(token, "sqrt") == 0) {
            if (top < 1) { fprintf(stderr, "Error: operandos insuficientes para sqrt\n"); continue; }
            double a = pop();
            if (a < 0.0) { fprintf(stderr, "Error: sqrt de numero negativo\n"); push(a); continue; }
            push(sqrt(a));
            continue;
        } else if (strcmp(token, "exp") == 0) {
            if (top < 1) { fprintf(stderr, "Error: operandos insuficientes para exp\n"); continue; }
            double a = pop();
            push(exp(a));
            continue;
        } else if (strcmp(token, "ln") == 0 || strcmp(token, "log") == 0) {
            if (top < 1) { fprintf(stderr, "Error: operandos insuficientes para ln\n"); continue; }
            double a = pop();
            if (a <= 0.0) { fprintf(stderr, "Error: log de numero no positivo\n"); push(a); continue; }
            push(log(a));
            continue;
        }

        // token desconocido
        fprintf(stderr, "Token desconocido: '%s'\n", token);
    }

    printf("Saliendo\n");
    return 0;
}
