/*
 *  neon_memcpy_impl.h.h
 *  iPhoneApp
 *
 *  Created by Lars Schneider on 24.11.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */

extern int memcpy_armneon(void *dest, const void *src, size_t n ) asm("memcpy_armneon");
