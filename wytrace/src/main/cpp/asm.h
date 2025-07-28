//
// Created by rose on 2025/7/28.
//

#pragma once

#if defined(__arm__)
#define asm_mode          .arm
#define asm_align         0
#define asm_align_bound   4
#define asm_function_type #function
#define asm_custom_entry  .fnstart
#define asm_custom_end    .fnend
#elif defined(__aarch64__)
#define asm_mode
#define asm_align         4
#define asm_align_bound   16
#define asm_function_type % function
#define asm_custom_entry
#define asm_custom_end
#endif

#define ENTRY(f)              \
  .text;                      \
  .type f, asm_function_type; \
  f:                          \
  asm_custom_entry;           \
  .cfi_startproc

#define ENTRY_GLOBAL_THUMB(f) \
  .globl f;                   \
  .thumb;                     \
  .balign asm_align;          \
  ENTRY(f)

#define ENTRY_GLOBAL_ARM(f) \
  .globl f;                 \
  asm_mode;                 \
  .balign asm_align;        \
  ENTRY(f)

#define ENTRY_HIDDEN_THUMB(f) \
  .hidden f;                  \
  .thumb;                     \
  .balign asm_align;          \
  ENTRY(f)

#define ENTRY_HIDDEN_ARM(f) \
  .hidden f;                \
  asm_mode;                 \
  .balign asm_align;        \
  ENTRY(f)

#define ENTRY_GLOBAL_THUMB_BOUND(f) \
  .globl f;                         \
  .thumb;                           \
  .balign asm_align_bound;          \
  ENTRY(f)

#define ENTRY_GLOBAL_ARM_BOUND(f) \
  .globl f;                       \
  asm_mode;                       \
  .balign asm_align_bound;        \
  ENTRY(f)

#define ENTRY_HIDDEN_THUMB_BOUND(f) \
  .hidden f;                        \
  .thumb;                           \
  .balign asm_align_bound;          \
  ENTRY(f)

#define ENTRY_HIDDEN_ARM_BOUND(f) \
  .hidden f;                      \
  asm_mode;                       \
  .balign asm_align_bound;        \
  ENTRY(f)

#define END(f)   \
  .cfi_endproc;  \
  .size f, .- f; \
  asm_custom_end\
