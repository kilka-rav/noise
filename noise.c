#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define prints(string) fprintf(stderr, "%s\n", string)
#define BUFLEN 4096

void check_arguments(int arg) {
    if ( arg != 3 ) {
        prints("ERROR IN ARG");
        exit(1);
    }
}

void check(int arg) {
    if ( arg < 0 ) {
        prints(strerror(errno));
        exit(2);
    }
}

char* binary;
float signal, noise;

float call_random() {
    float reply = rand() % 1000 + rand() % 900 + 537;
    reply = reply * noise / 1000;
    int znak = rand() % 2;
    if ( znak == 0 ) {
        reply = reply * -1;
    }
    return reply;
}


void write_bin(int a, int number) {
    int del = 128;
    int i = 0;
    while(i < 8) {
        float ran = call_random();
        if ( a >= del ) {
            a = a % del;
            if ( ran + signal >= 0.2 * signal ) {
                binary[8 * number + i] = '1';
            }
            else {
                binary[8 * number + i] = '0';
            }
        }
        del /= 2;
        i++;
    }
}

void encode(int sz) {
    for(int i = 0; i < sz - 1; ++i) {
        int del = 128;
        int a = 0;
        for(int j = 0; j < 8; ++j) {
            if ( binary[i * 8 + j] == '1' ) {
                a += del;
            }
            del /= 2;
        }
        char c = a;
        printf("%c", c);
    }
    printf("\n");
}

void convert(char* msg, size_t sz) {
    binary = (char*) malloc(8 * sz);
    int count = 0;
    while(count < sz - 1) {
        int a = msg[count];
        write_bin(a, count);
        //printf("%d ", a);
        count++;
    }
    prints(msg);
    //printf("\n");
    //prints(binary);
    //printf("\n");
    encode(sz);
    free(binary);
    return;
}

void decode(char* name) {
    int fd = open(name, O_RDWR);
    check(fd);
    struct stat buffer;
    int book = fstat(fd, &buffer);
    check(book);
    size_t sz = buffer.st_size;
    char* msg = (char*) malloc(sz);
    int note = read(fd, msg, sz);
    check(note);
    close(fd);
    convert(msg, sz);
    free(msg);
}

void data_noise(char* file) {
    FILE* f = fopen(file, "r");
    fscanf(f, "%f", &signal);
    fscanf(f, "%f", &noise);
    //signal = 1.0;
    //noise = 0.37;
    fclose(f);
}


int main(int argc, char** argv) {
    check_arguments(argc);
    srand(time(NULL));
    data_noise(argv[2]);
    decode(argv[1]);
    return 0;
}
