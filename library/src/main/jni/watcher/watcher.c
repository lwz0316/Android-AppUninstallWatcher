/*
 * File        : watcher.c
 * Author      : Vincent Cheung
 * Date        : Jan. 21, 2015
 * Description : This is used to watch if the app is uninstall.
 *
 * Copyright (C) Vincent Chueng<coolingfall@gmail.com>
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/inotify.h>
#include <sys/wait.h>

#include <sys/timeb.h>

#include "chttp.h"
#include "common.h"

#define BUFFER_SIZE     4 * 1024
#define LOG_TAG         "Watcher"

/* child process signal function */
static void sig_child() {
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0);

    return;
}

long long getSystemTime() {
    struct timeb t;
    ftime(&t);
    return 1000 * t.time + t.millitm;
}

int main(int argc, char *argv[]) {
    int i;
    int should_open_browser = 0;
    char *package_name = NULL;
    char *url = NULL;
    char *url_file_path = NULL;
    char *http_post_params = NULL;
    struct curl_slist *header_list = NULL;

   LOGI(LOG_TAG, "Copyright (c) 2015, Vincent Cheung<coolingfall@gmail.com>");

    for (i = 0; i < argc; i++) {
        if (!strcmp("-p", argv[i])) {
            package_name = argv[i + 1];
            LOGD(LOG_TAG, "package name: %s", package_name);
        }

        if (!strcmp("-u", argv[i])) {
            url = argv[i + 1];
            LOGD(LOG_TAG, "url: %s", url);
        }

        if (!strcmp("-f", argv[i])) {
            url_file_path = argv[i + 1];
            LOGD(LOG_TAG, "url file path: %s", url_file_path);
        }

        if (!strcmp("-b", argv[i])) {
            should_open_browser = atoi(argv[i + 1]);
            LOGD(LOG_TAG, "should open brwoser: %d", should_open_browser);
        }

        if (!strcmp("-P", argv[i])) {
            http_post_params = argv[i + 1];
            LOGD(LOG_TAG, "url post params: %s", http_post_params);
        }

        if (!strcmp("-H", argv[i])) {
            header_list = curl_slist_append(header_list, argv[i + 1]);
            LOGD(LOG_TAG, "url header : %s", argv[i + 1]);
        }
    }

    /* get the directory for watcher */
    char *app_dir = str_stitching("/data/data/", package_name);
    char *lib_dir = str_stitching(app_dir, "/lib");
    char *watch_file_path = str_stitching(app_dir, "/uninstall.watch");

    /* the file path should not be null */
    if (watch_file_path == NULL) {
        LOGE(LOG_TAG, "watch file path is NULL");
        exit(EXIT_FAILURE);
    }

    /* avoid zombie process */
    signal(SIGCHLD, sig_child);

    /* find pid by name and kill them */
    int pid_list[100];
    int total_num = find_pid_by_name(argv[0], pid_list);
    for (i = 0; i < total_num; i++) {
        int retval = 0;
        int watcher_pid = pid_list[i];
        if (watcher_pid > 1 && watcher_pid != getpid()) {
            retval = kill(watcher_pid, SIGKILL);
            if (!retval) {
                LOGD(LOG_TAG, "kill watcher process success: %d", watcher_pid);
            }
            else {
                LOGD(LOG_TAG, "kill wathcer process %d fail: %s", watcher_pid, strerror(errno));
                exit(EXIT_SUCCESS);
            }
        }
    }

    /* get child process */
    pid_t pid = fork();
    if (pid < 0) {
        LOGE(LOG_TAG, "fork failed");
    }
    else if (pid > 0) {
        /* parent process, main process */
        if (waitpid(pid, NULL, 0) != pid)
        {
            LOGE(LOG_TAG, "waitepid failed");
            exit(EXIT_FAILURE);
        }
        LOGD(LOG_TAG, "waitepid success and exit parent[%d] process", pid);
        exit(EXIT_SUCCESS);
    }
    else { /* pid == 0 */
        pid_t second_pid = fork();
        if (second_pid < 0) {
            LOGE(LOG_TAG, "fork second failed");
        }
        else if (second_pid > 0) {
            exit(EXIT_SUCCESS);
        }
        else { /* pid == 0  second process*/
            sleep(2); /* wait for the first process exit*/
            LOGD(LOG_TAG, "I am the child process.pid: %d\t %d\t ppid:%d\n", getpid(), second_pid,
                 getppid());
            /* inotify init */
            int fd = inotify_init();
            if (fd < 0) {
                LOGE(LOG_TAG, "inotify_init init failed");
                exit(EXIT_FAILURE);
            }

            int w_fd = open(watch_file_path, O_RDWR | O_CREAT | O_TRUNC,
                            S_IRWXU | S_IRWXG | S_IRWXO);
            if (w_fd < 0) {
                LOGE(LOG_TAG, "open watch file error");
                exit(EXIT_FAILURE);
            }

            close(w_fd);

            /* add watch in inotify */
            int watch_fd = inotify_add_watch(fd, watch_file_path, IN_DELETE);
            if (watch_fd < 0) {
                LOGE(LOG_TAG, "inotify_add_watch failed");
                exit(EXIT_FAILURE);
            }

            void *p_buf = malloc(sizeof(struct inotify_event));
            if (p_buf == NULL) {
                LOGD(LOG_TAG, "malloc inotify event failed");
                exit(EXIT_FAILURE);
            }

            LOGD(LOG_TAG, "watcher process fork ok, start to watch");

            fd_set fds; // file desc set
            struct timeval timeout = {0,
                                      0}; // {seconds, milliseconds} wait for 0ms; set 0 for no-block

            while (1) {
                /* read will block process */
                LOGD(LOG_TAG, "before read");
                size_t read_bytes = read(fd, p_buf, sizeof(struct inotify_event));

                /* delay 200ms */
//			    usleep(200*1000);
                LOGD(LOG_TAG, "before select");

                long long start = getSystemTime();
                FD_ZERO(&fds); // clear fds for every time
                FD_SET(1, &fds); // add fake file desc
                int ret = 0;
                ret = select(1 + 1, &fds, NULL, NULL, &timeout);
                LOGD(LOG_TAG, "after select result %d", ret);

                long long end = getSystemTime();

                LOGD(LOG_TAG, "time: %lld ms\n", (end - start));

                if (ret < 0) {
                    LOGE(LOG_TAG, "sleep failed");
                    free(p_buf);
                    exit(EXIT_FAILURE);
                }

                /* to check if the app has uninstalled, indeed */
                FILE *lib_dir_file = fopen(lib_dir, "r");
                FILE *app_dir_file = fopen(app_dir, "r");
                if (lib_dir_file == NULL || app_dir_file == NULL) {
                    LOGD(LOG_TAG, "DELETE FILE");
                    break;
                }
                else {
                    /* close app dir file */
                    fclose(lib_dir_file);
                    fclose(app_dir_file);

                    /* add notify watch again */
                    int w_fd = open(watch_file_path, O_WRONLY | O_CREAT | O_TRUNC,
                                    S_IRWXU | S_IRWXG | S_IRWXO);
                    close(w_fd);

                    int watch_fd = inotify_add_watch(fd, watch_file_path, IN_DELETE);
                    if (watch_fd < 0) {
                        LOGE(LOG_TAG, "inotify_add_watch failed");
                        free(p_buf);
                        exit(EXIT_FAILURE);
                    }
                }
            }

            free(p_buf);
            inotify_rm_watch(fd, IN_DELETE);

            LOGRD(LOG_TAG, "the app has been uninstalled");

            /* if the url was saved in file, read out */
            if (url_file_path != NULL) {
                int url_fd = open(url_file_path, O_RDONLY);
                if (url_fd < 0) {
                    LOGE(LOG_TAG, "url file open error");
                    exit(EXIT_FAILURE);
                }

                char buf[300] = {0};
                if (read(url_fd, buf, 300) > 0) {
                    url = buf;
                    LOGD(LOG_TAG, "url from file: %s", url);
                }

                close(url_fd);
            }

            /* call url */
            if (http_post_params) {
                chttp_post(url, header_list, http_post_params);
            } else {
                chttp_get(url, header_list);
            }

            if (header_list) {
                curl_slist_free_all(header_list);
            }

            /* open browser if needed */
            if (should_open_browser) {
                LOGD(LOG_TAG, "the app has been uninstalled, open browser");
                open_browser(url);
            }

            exit(EXIT_SUCCESS);
        }

    }
}