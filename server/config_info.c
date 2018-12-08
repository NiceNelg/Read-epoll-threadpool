/*
 * config_info.c
 *
 *  Created on: 2016.11
 *      Author: Leo
 */

#include "file_operations.h"
#include "config_info.h"
//Below variable should correspond with structure CONFIG_INFO_KEY_WORD_ID and CONFIG_INFO.
static char *config_file_key_word[MAX_NO_OF_CONFIG_KEY_WORD] =
{
		"Version",
		"LogLevel",
		"MySqlBranchServerAddr",
		"MySqlServerPort",
		"MySqlDbName",
		"MySqlUserName",
		"MySqlPassword",
};

/*
 * 检测配置项的值是否合法
 */
static BOOL 
parse_config_key_word(char *data, char *key_word)
{
	char *p;
	int i;
	BOOL ret = FALSE;

	p = data;
	if (strchr(p, ';') != NULL || strchr(p, '\n') != NULL) {
		i = 0;
		while (i < CONFIG_KEY_WORD_BUF_SIZE && p[i] != ';' && p[i] != '\n')
		{
			key_word[i] = p[i];
			i++;
		}
		key_word[MIN(i, CONFIG_KEY_WORD_BUF_SIZE-1)] = '\0';
		ret = TRUE;
	}

	return ret;
}

/*
 * 读取配置信息
 */
int 
read_config_info(CONFIG_INFO *config_info)
{
	char *config_file_buff = NULL;
	char *p = NULL;
	int index = 0;
	int ret = -1;
	char key_word_buf[CONFIG_KEY_WORD_BUF_SIZE] = "";

    /*读取配置文件，文件大小最大10K*/
	config_file_buff = malloc(INI_FILE_BUF_SIZE);
	if (config_file_buff == NULL) {
        /*文件读取错误，打印文件名、函数名、函数行数*/
		printf("[%s %s %d] No enough memory!\n", __FILE__, __FUNCTION__, 
                    __LINE__);
		return -1;
	}

    /*将内存块置空*/
	bzero(config_file_buff, INI_FILE_BUF_SIZE);

    /*读取配置文件失败*/
	if (read_file_to_buff(SERVER_INI_FILE, INI_FILE_BUF_SIZE - 1, 
                    config_file_buff) != 0) 
    {
		goto EXIT;
	}

    /*置空配置信息内存块*/
	bzero(config_info, sizeof(CONFIG_INFO));

    /*读取配置信息*/
	for (index = 0; index < MAX_NO_OF_CONFIG_KEY_WORD; index++) {
        
        /*判断有没有对应配置项*/
		p = strstr(config_file_buff, config_file_key_word[index]);

		if (p == NULL) {
			goto EXIT;
		}

		p = strchr(p, '=');
		if (p == NULL) {
			goto EXIT;
		}
		p++;

		/*排除空格或tab*/
		while (p[0] == 9 || p[0] == 32) {
			p++;
        }

        /*查看配置项的值是否合法，并获取配置项值的长度*/
		if (!parse_config_key_word(p, key_word_buf)) {
			continue;
		}

        /*筛选,配置*/
		switch (index) {

			case CON_VERSION:
				snprintf(config_info->version, sizeof(config_info->version), 
                            "%s", key_word_buf);
				break;
			case CON_LOG_LEVEL:
				config_info->log_level = atoi(key_word_buf);
				break;
			case CON_MYSQL_BRANCH_SERVER_ADDR:
				snprintf(config_info->mysql_branch_server_addr, 
                            MIN(PATH_BUFF_SIZE, CONFIG_KEY_WORD_BUF_SIZE), 
                            "%s", key_word_buf);
				break;
			case CON_MYSQL_SERVER_PORT:
				config_info->mysql_server_port = atol(key_word_buf);
				break;
			case CON_MYSQL_DB_NAME:
				snprintf(config_info->mysql_db_name, 
                            MIN(sizeof(config_info->mysql_db_name), 
                                CONFIG_KEY_WORD_BUF_SIZE), "%s", key_word_buf);
				break;
			case CON_MYSQL_USER_NAME:
				snprintf(config_info->mysql_user_name, 
                            MIN(sizeof(config_info->mysql_user_name), 
                                CONFIG_KEY_WORD_BUF_SIZE), "%s", key_word_buf);
				break;
			case CON_MYSQL_PASSWORD:
				snprintf(config_info->mysql_password, 
                            MIN(sizeof(config_info->mysql_password), 
                                CONFIG_KEY_WORD_BUF_SIZE), "%s", key_word_buf);
				break;
		}
	}
	ret = 0;

EXIT:
    /*清空缓存到内存的文件内容*/
	free(config_file_buff);
	return ret;
}


/*
 * 打印配置信息到屏幕
 */
void 
print_config_info(CONFIG_INFO config_info)
{
	printf("[%s %s %d] config_info.version: %s\n", __FILE__, __FUNCTION__, 
                __LINE__, config_info.version);
	
    printf("[%s %s %d] config_info.LogLevel: %d\n", __FILE__, __FUNCTION__, 
                __LINE__, config_info.log_level);
	
    printf("[%s %s %d] config_info.mysql_branch_server_addr: %s\n", __FILE__, 
                __FUNCTION__, __LINE__, config_info.mysql_branch_server_addr);
	
    printf("[%s %s %d] config_info.mysql_server_port: %d\n", __FILE__, 
                __FUNCTION__, __LINE__, config_info.mysql_server_port);
	
    printf("[%s %s %d] config_info.mysql_db_name: %s\n", __FILE__, 
                __FUNCTION__, __LINE__, config_info.mysql_db_name);
	
    printf("[%s %s %d] config_info.mysql_user_name: %s\n", __FILE__, 
                __FUNCTION__, __LINE__, config_info.mysql_user_name);
	
    printf("[%s %s %d] config_info.mysql_password: %s\n", __FILE__, 
                __FUNCTION__, __LINE__, config_info.mysql_password);
}

