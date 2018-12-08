/*
 * log.c
 *
 *  Created on: 2013-5-7
 *      Author: Fred
 */

#include "g_net_global.h"
#include "log.h"

#define MAX_LOG_LINE_NUM 500000
#define TIME_STAMP_BUF_LEN 128

static FILE *log_file_handle = NULL;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
static int log_level = LOG_LEVEL_INDISPENSABLE;
static char last_log_str[LOG_STR_BUF_LEN];
static char cur_ts_str[TIME_STAMP_BUF_LEN];

static char log_file_name[128] = {0};

/*
 * 将文件名设置到私有变量中
 */
void 
set_log_file_name(char *file_name)
{
	sprintf(log_file_name, "%s", file_name);
}

/*
 * 获取当前时间
 */
static int 
log_create_current_time_stamp(void)
{
	struct tm *time_stamp;
	time_t cur_time;

	cur_time = time(NULL);
	time_stamp = localtime(&cur_time);

	snprintf(cur_ts_str, TIME_STAMP_BUF_LEN, "%02d/%02d/%02d %02d:%02d:%02d", 
                time_stamp->tm_mday, time_stamp->tm_mon + 1, 
                time_stamp->tm_year % 100, time_stamp->tm_hour, 
                time_stamp->tm_min, time_stamp->tm_sec);
	return 0;
}

/*
 * 初始化日志
 */
int 
log_init()
{
    /*打开日志文件句柄，设置为只写*/
	log_file_handle = fopen(log_file_name, "w");
	if (log_file_handle == NULL) {
		printf("[%s %s %d] Can't create log file.\n", 
                    __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}

    /*日志记录缓存，256个字节*/
	memset(last_log_str, 0, LOG_STR_BUF_LEN);

    /*日志初始化完毕，记录起始标志*/
	log_string(LOG_LEVEL_INDISPENSABLE, "LOG START\n");
	return 0;
}

/*
 * 设置日志等级到私有变量
 */
void 
log_set_level(int level)
{
	log_level = level;
}

/*
 * 写入日志
 */
int 
log_string(int level, const char *str)
{
	char log_str[LOG_STR_BUF_LEN];
	static int count = 0;
	int need_log_str = 0;

    /*线程上锁*/
	pthread_mutex_lock(&log_mutex);
	log_create_current_time_stamp();

    /*每写入500000行日志后就清空日志重写日志*/
	if (++count >= MAX_LOG_LINE_NUM) {
		count = 0;
		fclose(log_file_handle);
		log_file_handle = fopen(log_file_name, "w");
	}

	/*判断是否为空字符串*/
	if (strcmp(str, last_log_str)) {
		switch (level) {
			case LOG_LEVEL_INFO:
				if (log_level <= LOG_LEVEL_INFO) {
					need_log_str = 1;
					snprintf(log_str, LOG_STR_BUF_LEN, "[INFO: %s] %s", 
                                cur_ts_str, str);
				}
				break;
			case LOG_LEVEL_WARNING:
				if (log_level <= LOG_LEVEL_WARNING) {
					need_log_str = 1;
					snprintf(log_str, LOG_STR_BUF_LEN, "[WARNING: %s] %s", 
                                cur_ts_str, str);
				}
				break;
			case LOG_LEVEL_ERROR:
				if (log_level <= LOG_LEVEL_ERROR) {
					need_log_str = 1;
					snprintf(log_str, LOG_STR_BUF_LEN, "[ERROR: %s] %s", 
                                cur_ts_str, str);
				}
				break;
			case LOG_LEVEL_FATAL:
				if (log_level <= LOG_LEVEL_FATAL) {
					need_log_str = 1;
					snprintf(log_str, LOG_STR_BUF_LEN, "[FATAL: %s] %s", 
                                cur_ts_str, str);
				}
				break;
			case LOG_LEVEL_INDISPENSABLE:
				need_log_str = 1;
				snprintf(log_str, LOG_STR_BUF_LEN, "[%s] %s", 
                            cur_ts_str, str);
				break;
		}

		if (need_log_str)
		{
			fprintf(log_file_handle, "%s", log_str);
			memset(last_log_str, 0, LOG_STR_BUF_LEN);
			strcpy(last_log_str, str);
		}
	}
    /*将缓冲的日志内容立刻写入到文件中*/
	fflush(log_file_handle);
    /*线程解锁*/
	pthread_mutex_unlock(&log_mutex);
	return 0;
}

/*
 * 关闭日志
 */
int 
log_close(void)
{
	if (log_file_handle != NULL) {
		log_string(LOG_LEVEL_INDISPENSABLE, "LOG END\n");
		fclose(log_file_handle);
	}
	return 0;
}

