//
// Created by rose on 2025/8/9.
//

#ifndef LSPOSED_TOOLS_COMPILER_H
#define LSPOSED_TOOLS_COMPILER_H

#define likely(x) __builtin_expect((x!= 0), true)
#define unlikely(x) __builtin_expect((x !=0), false)

#endif //LSPOSED_TOOLS_COMPILER_H
