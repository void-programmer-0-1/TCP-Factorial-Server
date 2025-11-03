#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "queue.h"
#include "factorial.h"

#define WORKER_COUNT 4
#define BACKLOG 128
#define MAX_LINE 4096

static socket_queue_t queue;
static volatile sig_atomic_t stop_flag = 0;
static pthread_mutex_t active_m = PTHREAD_MUTEX_INITIALIZER;
static int active = 0;

void sigint_handler(int signo) {
    (void)signo;
    stop_flag = 1;
    pthread_mutex_lock(&queue.m);
    queue.shutdown = 1;
    pthread_cond_broadcast(&queue.nonempty);
    pthread_mutex_unlock(&queue.m);
}

char *read_line(int fd) {
    char *buf = malloc(MAX_LINE);
    int pos = 0;
    while (pos < MAX_LINE - 1) {
        ssize_t r = recv(fd, buf + pos, 1, 0);
        if (r <= 0) break;
        if (buf[pos++] == '\n') break;
    }
    buf[pos] = '\0';
    return buf;
}

ssize_t write_all(int fd, const void *b, size_t n) {
    size_t s = 0; const char *p = b;
    while (s < n) {
        ssize_t w = send(fd, p + s, n - s, 0);
        if (w <= 0) return -1;
        s += w;
    }
    return (ssize_t)s;
}

void *worker(void *arg) {
    (void)arg;
    for (;;) {
        int fd;
        if (!queue_pop(&queue, &fd)) break;

        pthread_mutex_lock(&active_m); active++; pthread_mutex_unlock(&active_m);

        char *line = read_line(fd);
        long n = strtol(line, NULL, 10);
        free(line);

        bigint_t b;
        compute_factorial_big((int)n, &b);
        char *out = bigint_to_str(&b);
        write_all(fd, out, strlen(out));
        close(fd);
        free(out);

        pthread_mutex_lock(&active_m); active--; pthread_mutex_unlock(&active_m);
    }
    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "Usage: %s <port>\n", argv[0]); return 1; }
    int port = atoi(argv[1]);

    signal(SIGINT, sigint_handler);
    queue_init(&queue);

    pthread_t tids[WORKER_COUNT];
    for (int i = 0; i < WORKER_COUNT; ++i)
        pthread_create(&tids[i], NULL, worker, NULL);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1; setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(port), .sin_addr.s_addr = INADDR_ANY };
    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sockfd, BACKLOG);

    printf("Server on port %d\n", port);
    printf("Queue size: %d\n", queue.count);

    while (!stop_flag) {
        int cfd = accept(sockfd, NULL, NULL);
        if (cfd < 0) continue;
        int r = queue_push(&queue, cfd);
        if (r == 0) { write_all(cfd, "BUSY\n", 5); close(cfd); }
    }

    pthread_mutex_lock(&queue.m);
    queue.shutdown = 1;
    pthread_cond_broadcast(&queue.nonempty);
    pthread_mutex_unlock(&queue.m);

    for (int i = 0; i < WORKER_COUNT; ++i)
        pthread_join(tids[i], NULL);

    close(sockfd);
    return 0;
}
