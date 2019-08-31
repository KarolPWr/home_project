/*
 * dbg.h
 *
 *  Created on: Aug 11, 2019
 *      Author: Karol
 */

#ifndef DBG_H_
#define DBG_H_

#include <errno.h>
#include <string.h>


#define debug(M, ...) debug_printf("DEBUG %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)


#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) debug_printf("[ERROR] (%s:%d:%s: errno: %s) " M "\n\r", __FILE__, __LINE__, __func__, clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) debug_printf("[WARN] (%s:%d:%s: errno: %s) " M "\n\r", __FILE__, __LINE__, __func__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) debug_printf("[INFO] (%s:%d:%s) " M "\n\r", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_mem(A) check((A), "Out of memory.")

#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }


#endif /* DBG_H_ */
