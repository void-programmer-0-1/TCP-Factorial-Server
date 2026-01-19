#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>

#include <cstring>
#include <iostream>
#include <unordered_map>
#include <thread>

#include "big_factorial.hpp"
#include "thread_pool.hpp"

constexpr int PORT = 9000;
constexpr int MAX_EVENTS = 1024;
constexpr int MAX_N = 5000;

/* ----------------- Utilities ----------------- */

int make_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// Safe write: handles partial writes correctly
bool write_all(int fd, const char* buf, size_t len) {
    size_t written = 0;
    while (written < len) {
        ssize_t n = write(fd, buf + written, len - written);
        if (n > 0) {
            written += static_cast<size_t>(n);
        } else if (n == -1 && errno == EINTR) {
            continue; // retry
        } else if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            // socket buffer full, try again
            continue;
        } else {
            return false; // fatal error
        }
    }
    return true;
}

/* ----------------- Server ----------------- */

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET,
               SO_REUSEADDR | SO_REUSEPORT,
               &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 4096);
    make_nonblocking(server_fd);

    int epfd = epoll_create1(0);

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev);

    epoll_event events[MAX_EVENTS];
    std::unordered_map<int, std::string> buffers;

    // Thread pool per process
    ThreadPool pool(std::thread::hardware_concurrency());

    std::cout << "C++17 factorial server PID "
              << getpid() << " listening on port "
              << PORT << std::endl;

    while (true) {
        int nfd = epoll_wait(epfd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfd; ++i) {
            int fd = events[i].data.fd;

            // ---------- Accept new connections ----------
            if (fd == server_fd) {
                while (true) {
                    int client = accept(server_fd, nullptr, nullptr);
                    if (client < 0) break;

                    make_nonblocking(client);

                    epoll_event cev{};
                    cev.events = EPOLLIN;
                    cev.data.fd = client;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, client, &cev);

                    buffers[client];
                }
            }
            // ---------- Read client data ----------
            else {
                char buf[1024];
                ssize_t n = read(fd, buf, sizeof(buf));

                if (n <= 0) {
                    close(fd);
                    buffers.erase(fd);
                    continue;
                }

                buffers[fd].append(buf, static_cast<size_t>(n));

                size_t pos;
                while ((pos = buffers[fd].find('\n')) != std::string::npos) {
                    std::string line = buffers[fd].substr(0, pos);
                    buffers[fd].erase(0, pos + 1);

                    int value;
                    try {
                        value = std::stoi(line);
                    } catch (...) {
                        std::string err = "ERROR\n";
                        write_all(fd, err.c_str(), err.size());
                        continue;
                    }

                    // ---------- Offload compute ----------
                    pool.submit([fd, value] {
                        std::string out;
                        if (value < 0 || value > MAX_N) {
                            out = "ERROR\n";
                        } else {
                            out = factorial(value) + "\n";
                        }
                        write_all(fd, out.c_str(), out.size());
                    });
                }
            }
        }
    }
}
