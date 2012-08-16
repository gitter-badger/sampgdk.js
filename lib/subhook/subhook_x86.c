/* Copyright (c) 2012 Zeex
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "subhook.h"
#include "subhook-private.h"

SUBHOOK_EXPORT int SUBHOOK_API subhook_install(struct subhook *hook) {
	static const unsigned char jmp = 0xE9;
	void *src, *dst;
	size_t offset;

	if (subhook_is_installed(hook)) {
		return 0;
	}

	src = subhook_get_source(hook);
	dst = subhook_get_destination(hook);

	subhook_unprotect(src, SUBHOOK_JUMP_SIZE);
	memcpy(subhook_get_code(hook), src, SUBHOOK_JUMP_SIZE);

	/* E9 - jump near, relative */	
	memcpy(src, &jmp, sizeof(jmp));

	/* jump address is relative to next instruction */
	offset = (int32_t)dst - ((int32_t)src + SUBHOOK_JUMP_SIZE);
	memcpy((void*)((int32_t)src + 1), &offset, SUBHOOK_JUMP_SIZE - sizeof(jmp));

	subhook_set_flags(hook, subhook_get_flags(hook) | SUBHOOK_FLAG_INSTALLED);
	return 1;
}

SUBHOOK_EXPORT int SUBHOOK_API subhook_remove(struct subhook *hook) {
	if (!subhook_is_installed(hook)) {
		return 0;
	}

	memcpy(subhook_get_source(hook), subhook_get_code(hook), SUBHOOK_JUMP_SIZE);
	subhook_set_flags(hook, subhook_get_flags(hook) & ~(SUBHOOK_FLAG_INSTALLED));
	return 1;
}

SUBHOOK_EXPORT void *SUBHOOK_API subhook_read_destination(void *src) {
	if (*(unsigned char*)src == 0xE9) {
		return (void *)(*(int32_t *)((int32_t)src + 1) + (int32_t)src + SUBHOOK_JUMP_SIZE);
	}
	return NULL;
}
