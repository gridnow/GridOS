/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   内核 针对用户层的接口句柄
 */

#ifndef KE_USER_HANDLE_H
#define KE_USER_HANDLE_H

#include <kernel/ke_srv.h>
struct ko_process;

bool ke_handle_init(struct ko_process *on);
void ke_handle_deinit(struct ko_process *on);
unsigned long ke_handle_loop(struct ko_process *on, void (*action)(struct ko_process *on, ke_handle handle));
void *ke_handle_translate_no_lock(struct ko_process *on, ke_handle handle);
int ke_handle_put(ke_handle handle, void *object);
void *ke_handle_translate(ke_handle handle);
bool ke_handle_delete(ke_handle handle);
ke_handle ke_handle_create(void *ko);
#endif