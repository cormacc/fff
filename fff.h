/*
LICENSE

The MIT License (MIT)

Copyright (c) 2010 Michael Long

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef FAKE_FUNCTIONS
#define FAKE_FUNCTIONS

#include <stdarg.h>
#include <string.h> /* For memset and memcpy */

#define FFF_MAX_ARGS (20u)
#ifndef FFF_ARG_HISTORY_LEN
    #define FFF_ARG_HISTORY_LEN (50u)
#endif
#ifndef FFF_CALL_HISTORY_LEN
    #define FFF_CALL_HISTORY_LEN (50u)
#endif
#ifndef FFF_GCC_FUNCTION_ATTRIBUTES
    #define FFF_GCC_FUNCTION_ATTRIBUTES
#endif
#ifndef CUSTOM_FFF_FUNCTION_TEMPLATE
#define CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN, FUNCNAME, ...) \
    RETURN(*FUNCNAME)(__VA_ARGS__)
#endif /* CUSTOM_FFF_FUNCTION_TEMPLATE */
/* -- INTERNAL HELPER MACROS -- */
#define SET_RETURN_SEQ(FUNCNAME, ARRAY_POINTER, ARRAY_LEN) \
    FUNCNAME##_fake.return_val_seq = ARRAY_POINTER; \
    FUNCNAME##_fake.return_val_seq_len = ARRAY_LEN;
#define SET_CUSTOM_FAKE_SEQ(FUNCNAME, ARRAY_POINTER, ARRAY_LEN) \
    FUNCNAME##_fake.custom_fake_seq = ARRAY_POINTER; \
    FUNCNAME##_fake.custom_fake_seq_len = ARRAY_LEN;

/* Defining a function to reset a fake function */
#define RESET_FAKE(FUNCNAME) { \
    FUNCNAME##_reset(); \
} \


#define DECLARE_ARG(type, n, FUNCNAME) \
    type arg##n##_val; \
    type arg##n##_history[FFF_ARG_HISTORY_LEN];

#define DECLARE_ALL_FUNC_COMMON \
    unsigned int call_count; \
    unsigned int arg_history_len; \
    unsigned int arg_histories_dropped; \

#define DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
    RETURN_TYPE return_val_history[FFF_ARG_HISTORY_LEN];

#define SAVE_ARG(FUNCNAME, n) \
    memcpy((void*)&FUNCNAME##_fake.arg##n##_val, (void*)&arg##n, sizeof(arg##n));

#define ROOM_FOR_MORE_HISTORY(FUNCNAME) \
    FUNCNAME##_fake.call_count < FFF_ARG_HISTORY_LEN

#define SAVE_RET_HISTORY(FUNCNAME, RETVAL) \
    if ((FUNCNAME##_fake.call_count - 1) < FFF_ARG_HISTORY_LEN) \
        memcpy((void *)&FUNCNAME##_fake.return_val_history[FUNCNAME##_fake.call_count - 1], (const void *) &RETVAL, sizeof(RETVAL)); \

#define SAVE_ARG_HISTORY(FUNCNAME, ARGN) \
    memcpy((void*)&FUNCNAME##_fake.arg##ARGN##_history[FUNCNAME##_fake.call_count], (void*)&arg##ARGN, sizeof(arg##ARGN));

#define HISTORY_DROPPED(FUNCNAME) \
    FUNCNAME##_fake.arg_histories_dropped++

#define DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
    RETURN_TYPE return_val; \
    int return_val_seq_len; \
    int return_val_seq_idx; \
    RETURN_TYPE * return_val_seq; \

#define DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
    int custom_fake_seq_len; \
    int custom_fake_seq_idx; \

#define INCREMENT_CALL_COUNT(FUNCNAME) \
    FUNCNAME##_fake.call_count++

#define RETURN_FAKE_RESULT(FUNCNAME) \
    if (FUNCNAME##_fake.return_val_seq_len){ /* then its a sequence */ \
        if(FUNCNAME##_fake.return_val_seq_idx < FUNCNAME##_fake.return_val_seq_len) { \
            SAVE_RET_HISTORY(FUNCNAME, FUNCNAME##_fake.return_val_seq[FUNCNAME##_fake.return_val_seq_idx]) \
            return FUNCNAME##_fake.return_val_seq[FUNCNAME##_fake.return_val_seq_idx++]; \
        } \
        SAVE_RET_HISTORY(FUNCNAME, FUNCNAME##_fake.return_val_seq[FUNCNAME##_fake.return_val_seq_len-1]) \
        return FUNCNAME##_fake.return_val_seq[FUNCNAME##_fake.return_val_seq_len-1]; /* return last element */ \
    } \
    SAVE_RET_HISTORY(FUNCNAME, FUNCNAME##_fake.return_val) \
    return FUNCNAME##_fake.return_val; \

#ifdef __cplusplus
    #define FFF_EXTERN_C extern "C"{
    #define FFF_END_EXTERN_C } 
#else  /* ansi c */
    #define FFF_EXTERN_C 
    #define FFF_END_EXTERN_C 
#endif  /* cpp/ansi c */

#define DEFINE_RESET_FUNCTION(FUNCNAME) \
    void FUNCNAME##_reset(void){ \
        memset((void*)&FUNCNAME##_fake, 0, sizeof(FUNCNAME##_fake) - sizeof(FUNCNAME##_fake.custom_fake) - sizeof(FUNCNAME##_fake.custom_fake_seq)); \
        FUNCNAME##_fake.custom_fake = NULL; \
        FUNCNAME##_fake.custom_fake_seq = NULL; \
        FUNCNAME##_fake.arg_history_len = FFF_ARG_HISTORY_LEN; \
    }
/* -- END INTERNAL HELPER MACROS -- */

typedef void (*fff_function_t)(void);
typedef struct { 
    fff_function_t call_history[FFF_CALL_HISTORY_LEN];
    unsigned int call_history_idx;
} fff_globals_t;

FFF_EXTERN_C
extern fff_globals_t fff;
FFF_END_EXTERN_C

#define DEFINE_FFF_GLOBALS \
    FFF_EXTERN_C \
        fff_globals_t fff; \
    FFF_END_EXTERN_C

#define FFF_RESET_HISTORY() \
    fff.call_history_idx = 0; \
    memset(fff.call_history, 0, sizeof(fff.call_history));

#define REGISTER_CALL(function) \
    if(fff.call_history_idx < FFF_CALL_HISTORY_LEN) \
        fff.call_history[fff.call_history_idx++] = (fff_function_t)function;

#define DECLARE_FAKE_VOID_FUNC0(FUNCNAME) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, void); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, void); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(void); \

#define DEFINE_FAKE_VOID_FUNC0(FUNCNAME) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(void){ \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC0(FUNCNAME) \
    DECLARE_FAKE_VOID_FUNC0(FUNCNAME) \
    DEFINE_FAKE_VOID_FUNC0(FUNCNAME) \


#define DECLARE_FAKE_VOID_FUNC1(FUNCNAME, ARG0_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0); \

#define DEFINE_FAKE_VOID_FUNC1(FUNCNAME, ARG0_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0){ \
        SAVE_ARG(FUNCNAME, 0); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC1(FUNCNAME, ARG0_TYPE) \
    DECLARE_FAKE_VOID_FUNC1(FUNCNAME, ARG0_TYPE) \
    DEFINE_FAKE_VOID_FUNC1(FUNCNAME, ARG0_TYPE) \


#define DECLARE_FAKE_VOID_FUNC2(FUNCNAME, ARG0_TYPE, ARG1_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1); \

#define DEFINE_FAKE_VOID_FUNC2(FUNCNAME, ARG0_TYPE, ARG1_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC2(FUNCNAME, ARG0_TYPE, ARG1_TYPE) \
    DECLARE_FAKE_VOID_FUNC2(FUNCNAME, ARG0_TYPE, ARG1_TYPE) \
    DEFINE_FAKE_VOID_FUNC2(FUNCNAME, ARG0_TYPE, ARG1_TYPE) \


#define DECLARE_FAKE_VOID_FUNC3(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2); \

#define DEFINE_FAKE_VOID_FUNC3(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC3(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE) \
    DECLARE_FAKE_VOID_FUNC3(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE) \
    DEFINE_FAKE_VOID_FUNC3(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE) \


#define DECLARE_FAKE_VOID_FUNC4(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3); \

#define DEFINE_FAKE_VOID_FUNC4(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC4(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE) \
    DECLARE_FAKE_VOID_FUNC4(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE) \
    DEFINE_FAKE_VOID_FUNC4(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE) \


#define DECLARE_FAKE_VOID_FUNC5(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4); \

#define DEFINE_FAKE_VOID_FUNC5(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC5(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE) \
    DECLARE_FAKE_VOID_FUNC5(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE) \
    DEFINE_FAKE_VOID_FUNC5(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE) \


#define DECLARE_FAKE_VOID_FUNC6(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5); \

#define DEFINE_FAKE_VOID_FUNC6(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC6(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE) \
    DECLARE_FAKE_VOID_FUNC6(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE) \
    DEFINE_FAKE_VOID_FUNC6(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE) \


#define DECLARE_FAKE_VOID_FUNC7(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6); \

#define DEFINE_FAKE_VOID_FUNC7(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC7(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE) \
    DECLARE_FAKE_VOID_FUNC7(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE) \
    DEFINE_FAKE_VOID_FUNC7(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE) \


#define DECLARE_FAKE_VOID_FUNC8(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7); \

#define DEFINE_FAKE_VOID_FUNC8(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC8(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE) \
    DECLARE_FAKE_VOID_FUNC8(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE) \
    DEFINE_FAKE_VOID_FUNC8(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE) \


#define DECLARE_FAKE_VOID_FUNC9(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8); \

#define DEFINE_FAKE_VOID_FUNC9(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC9(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE) \
    DECLARE_FAKE_VOID_FUNC9(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE) \
    DEFINE_FAKE_VOID_FUNC9(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE) \


#define DECLARE_FAKE_VOID_FUNC10(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9); \

#define DEFINE_FAKE_VOID_FUNC10(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC10(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE) \
    DECLARE_FAKE_VOID_FUNC10(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE) \
    DEFINE_FAKE_VOID_FUNC10(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE) \


#define DECLARE_FAKE_VOID_FUNC11(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10); \

#define DEFINE_FAKE_VOID_FUNC11(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC11(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE) \
    DECLARE_FAKE_VOID_FUNC11(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE) \
    DEFINE_FAKE_VOID_FUNC11(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE) \


#define DECLARE_FAKE_VOID_FUNC12(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11); \

#define DEFINE_FAKE_VOID_FUNC12(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC12(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE) \
    DECLARE_FAKE_VOID_FUNC12(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE) \
    DEFINE_FAKE_VOID_FUNC12(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE) \


#define DECLARE_FAKE_VOID_FUNC13(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12); \

#define DEFINE_FAKE_VOID_FUNC13(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC13(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE) \
    DECLARE_FAKE_VOID_FUNC13(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE) \
    DEFINE_FAKE_VOID_FUNC13(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE) \


#define DECLARE_FAKE_VOID_FUNC14(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13); \

#define DEFINE_FAKE_VOID_FUNC14(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC14(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE) \
    DECLARE_FAKE_VOID_FUNC14(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE) \
    DEFINE_FAKE_VOID_FUNC14(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE) \


#define DECLARE_FAKE_VOID_FUNC15(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14); \

#define DEFINE_FAKE_VOID_FUNC15(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC15(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE) \
    DECLARE_FAKE_VOID_FUNC15(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE) \
    DEFINE_FAKE_VOID_FUNC15(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE) \


#define DECLARE_FAKE_VOID_FUNC16(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ARG(ARG15_TYPE, 15, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15); \

#define DEFINE_FAKE_VOID_FUNC16(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        SAVE_ARG(FUNCNAME, 15); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
            SAVE_ARG_HISTORY(FUNCNAME, 15); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC16(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE) \
    DECLARE_FAKE_VOID_FUNC16(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE) \
    DEFINE_FAKE_VOID_FUNC16(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE) \


#define DECLARE_FAKE_VOID_FUNC17(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ARG(ARG15_TYPE, 15, FUNCNAME) \
        DECLARE_ARG(ARG16_TYPE, 16, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16); \

#define DEFINE_FAKE_VOID_FUNC17(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        SAVE_ARG(FUNCNAME, 15); \
        SAVE_ARG(FUNCNAME, 16); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
            SAVE_ARG_HISTORY(FUNCNAME, 15); \
            SAVE_ARG_HISTORY(FUNCNAME, 16); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC17(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE) \
    DECLARE_FAKE_VOID_FUNC17(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE) \
    DEFINE_FAKE_VOID_FUNC17(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE) \


#define DECLARE_FAKE_VOID_FUNC18(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ARG(ARG15_TYPE, 15, FUNCNAME) \
        DECLARE_ARG(ARG16_TYPE, 16, FUNCNAME) \
        DECLARE_ARG(ARG17_TYPE, 17, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17); \

#define DEFINE_FAKE_VOID_FUNC18(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        SAVE_ARG(FUNCNAME, 15); \
        SAVE_ARG(FUNCNAME, 16); \
        SAVE_ARG(FUNCNAME, 17); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
            SAVE_ARG_HISTORY(FUNCNAME, 15); \
            SAVE_ARG_HISTORY(FUNCNAME, 16); \
            SAVE_ARG_HISTORY(FUNCNAME, 17); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC18(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE) \
    DECLARE_FAKE_VOID_FUNC18(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE) \
    DEFINE_FAKE_VOID_FUNC18(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE) \


#define DECLARE_FAKE_VOID_FUNC19(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ARG(ARG15_TYPE, 15, FUNCNAME) \
        DECLARE_ARG(ARG16_TYPE, 16, FUNCNAME) \
        DECLARE_ARG(ARG17_TYPE, 17, FUNCNAME) \
        DECLARE_ARG(ARG18_TYPE, 18, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17, ARG18_TYPE arg18); \

#define DEFINE_FAKE_VOID_FUNC19(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17, ARG18_TYPE arg18){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        SAVE_ARG(FUNCNAME, 15); \
        SAVE_ARG(FUNCNAME, 16); \
        SAVE_ARG(FUNCNAME, 17); \
        SAVE_ARG(FUNCNAME, 18); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
            SAVE_ARG_HISTORY(FUNCNAME, 15); \
            SAVE_ARG_HISTORY(FUNCNAME, 16); \
            SAVE_ARG_HISTORY(FUNCNAME, 17); \
            SAVE_ARG_HISTORY(FUNCNAME, 18); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC19(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE) \
    DECLARE_FAKE_VOID_FUNC19(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE) \
    DEFINE_FAKE_VOID_FUNC19(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE) \


#define DECLARE_FAKE_VOID_FUNC20(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ARG19_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ARG(ARG15_TYPE, 15, FUNCNAME) \
        DECLARE_ARG(ARG16_TYPE, 16, FUNCNAME) \
        DECLARE_ARG(ARG17_TYPE, 17, FUNCNAME) \
        DECLARE_ARG(ARG18_TYPE, 18, FUNCNAME) \
        DECLARE_ARG(ARG19_TYPE, 19, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ARG19_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ARG19_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17, ARG18_TYPE arg18, ARG19_TYPE arg19); \

#define DEFINE_FAKE_VOID_FUNC20(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ARG19_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17, ARG18_TYPE arg18, ARG19_TYPE arg19){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        SAVE_ARG(FUNCNAME, 15); \
        SAVE_ARG(FUNCNAME, 16); \
        SAVE_ARG(FUNCNAME, 17); \
        SAVE_ARG(FUNCNAME, 18); \
        SAVE_ARG(FUNCNAME, 19); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
            SAVE_ARG_HISTORY(FUNCNAME, 15); \
            SAVE_ARG_HISTORY(FUNCNAME, 16); \
            SAVE_ARG_HISTORY(FUNCNAME, 17); \
            SAVE_ARG_HISTORY(FUNCNAME, 18); \
            SAVE_ARG_HISTORY(FUNCNAME, 19); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19); \
            } \
            else{ \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19); \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC20(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ARG19_TYPE) \
    DECLARE_FAKE_VOID_FUNC20(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ARG19_TYPE) \
    DEFINE_FAKE_VOID_FUNC20(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ARG19_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC0(RETURN_TYPE, FUNCNAME) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, void); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, void); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(void); \

#define DEFINE_FAKE_VALUE_FUNC0(RETURN_TYPE, FUNCNAME) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(void){ \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC0(RETURN_TYPE, FUNCNAME) \
    DECLARE_FAKE_VALUE_FUNC0(RETURN_TYPE, FUNCNAME) \
    DEFINE_FAKE_VALUE_FUNC0(RETURN_TYPE, FUNCNAME) \


#define DECLARE_FAKE_VALUE_FUNC1(RETURN_TYPE, FUNCNAME, ARG0_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0); \

#define DEFINE_FAKE_VALUE_FUNC1(RETURN_TYPE, FUNCNAME, ARG0_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0){ \
        SAVE_ARG(FUNCNAME, 0); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC1(RETURN_TYPE, FUNCNAME, ARG0_TYPE) \
    DECLARE_FAKE_VALUE_FUNC1(RETURN_TYPE, FUNCNAME, ARG0_TYPE) \
    DEFINE_FAKE_VALUE_FUNC1(RETURN_TYPE, FUNCNAME, ARG0_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC2(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1); \

#define DEFINE_FAKE_VALUE_FUNC2(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC2(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE) \
    DECLARE_FAKE_VALUE_FUNC2(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE) \
    DEFINE_FAKE_VALUE_FUNC2(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC3(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2); \

#define DEFINE_FAKE_VALUE_FUNC3(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC3(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE) \
    DECLARE_FAKE_VALUE_FUNC3(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE) \
    DEFINE_FAKE_VALUE_FUNC3(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC4(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3); \

#define DEFINE_FAKE_VALUE_FUNC4(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC4(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE) \
    DECLARE_FAKE_VALUE_FUNC4(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE) \
    DEFINE_FAKE_VALUE_FUNC4(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC5(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4); \

#define DEFINE_FAKE_VALUE_FUNC5(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC5(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE) \
    DECLARE_FAKE_VALUE_FUNC5(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE) \
    DEFINE_FAKE_VALUE_FUNC5(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC6(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5); \

#define DEFINE_FAKE_VALUE_FUNC6(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC6(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE) \
    DECLARE_FAKE_VALUE_FUNC6(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE) \
    DEFINE_FAKE_VALUE_FUNC6(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC7(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6); \

#define DEFINE_FAKE_VALUE_FUNC7(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC7(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE) \
    DECLARE_FAKE_VALUE_FUNC7(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE) \
    DEFINE_FAKE_VALUE_FUNC7(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC8(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7); \

#define DEFINE_FAKE_VALUE_FUNC8(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC8(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE) \
    DECLARE_FAKE_VALUE_FUNC8(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE) \
    DEFINE_FAKE_VALUE_FUNC8(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC9(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8); \

#define DEFINE_FAKE_VALUE_FUNC9(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC9(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE) \
    DECLARE_FAKE_VALUE_FUNC9(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE) \
    DEFINE_FAKE_VALUE_FUNC9(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC10(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9); \

#define DEFINE_FAKE_VALUE_FUNC10(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC10(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE) \
    DECLARE_FAKE_VALUE_FUNC10(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE) \
    DEFINE_FAKE_VALUE_FUNC10(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC11(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10); \

#define DEFINE_FAKE_VALUE_FUNC11(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC11(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE) \
    DECLARE_FAKE_VALUE_FUNC11(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE) \
    DEFINE_FAKE_VALUE_FUNC11(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC12(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11); \

#define DEFINE_FAKE_VALUE_FUNC12(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC12(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE) \
    DECLARE_FAKE_VALUE_FUNC12(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE) \
    DEFINE_FAKE_VALUE_FUNC12(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC13(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12); \

#define DEFINE_FAKE_VALUE_FUNC13(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC13(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE) \
    DECLARE_FAKE_VALUE_FUNC13(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE) \
    DEFINE_FAKE_VALUE_FUNC13(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC14(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13); \

#define DEFINE_FAKE_VALUE_FUNC14(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC14(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE) \
    DECLARE_FAKE_VALUE_FUNC14(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE) \
    DEFINE_FAKE_VALUE_FUNC14(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC15(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14); \

#define DEFINE_FAKE_VALUE_FUNC15(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC15(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE) \
    DECLARE_FAKE_VALUE_FUNC15(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE) \
    DEFINE_FAKE_VALUE_FUNC15(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC16(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ARG(ARG15_TYPE, 15, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15); \

#define DEFINE_FAKE_VALUE_FUNC16(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        SAVE_ARG(FUNCNAME, 15); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
            SAVE_ARG_HISTORY(FUNCNAME, 15); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC16(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE) \
    DECLARE_FAKE_VALUE_FUNC16(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE) \
    DEFINE_FAKE_VALUE_FUNC16(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC17(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ARG(ARG15_TYPE, 15, FUNCNAME) \
        DECLARE_ARG(ARG16_TYPE, 16, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16); \

#define DEFINE_FAKE_VALUE_FUNC17(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        SAVE_ARG(FUNCNAME, 15); \
        SAVE_ARG(FUNCNAME, 16); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
            SAVE_ARG_HISTORY(FUNCNAME, 15); \
            SAVE_ARG_HISTORY(FUNCNAME, 16); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC17(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE) \
    DECLARE_FAKE_VALUE_FUNC17(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE) \
    DEFINE_FAKE_VALUE_FUNC17(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC18(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ARG(ARG15_TYPE, 15, FUNCNAME) \
        DECLARE_ARG(ARG16_TYPE, 16, FUNCNAME) \
        DECLARE_ARG(ARG17_TYPE, 17, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17); \

#define DEFINE_FAKE_VALUE_FUNC18(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        SAVE_ARG(FUNCNAME, 15); \
        SAVE_ARG(FUNCNAME, 16); \
        SAVE_ARG(FUNCNAME, 17); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
            SAVE_ARG_HISTORY(FUNCNAME, 15); \
            SAVE_ARG_HISTORY(FUNCNAME, 16); \
            SAVE_ARG_HISTORY(FUNCNAME, 17); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC18(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE) \
    DECLARE_FAKE_VALUE_FUNC18(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE) \
    DEFINE_FAKE_VALUE_FUNC18(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC19(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ARG(ARG15_TYPE, 15, FUNCNAME) \
        DECLARE_ARG(ARG16_TYPE, 16, FUNCNAME) \
        DECLARE_ARG(ARG17_TYPE, 17, FUNCNAME) \
        DECLARE_ARG(ARG18_TYPE, 18, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17, ARG18_TYPE arg18); \

#define DEFINE_FAKE_VALUE_FUNC19(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17, ARG18_TYPE arg18){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        SAVE_ARG(FUNCNAME, 15); \
        SAVE_ARG(FUNCNAME, 16); \
        SAVE_ARG(FUNCNAME, 17); \
        SAVE_ARG(FUNCNAME, 18); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
            SAVE_ARG_HISTORY(FUNCNAME, 15); \
            SAVE_ARG_HISTORY(FUNCNAME, 16); \
            SAVE_ARG_HISTORY(FUNCNAME, 17); \
            SAVE_ARG_HISTORY(FUNCNAME, 18); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC19(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE) \
    DECLARE_FAKE_VALUE_FUNC19(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE) \
    DEFINE_FAKE_VALUE_FUNC19(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE) \


#define DECLARE_FAKE_VALUE_FUNC20(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ARG19_TYPE) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ARG(ARG15_TYPE, 15, FUNCNAME) \
        DECLARE_ARG(ARG16_TYPE, 16, FUNCNAME) \
        DECLARE_ARG(ARG17_TYPE, 17, FUNCNAME) \
        DECLARE_ARG(ARG18_TYPE, 18, FUNCNAME) \
        DECLARE_ARG(ARG19_TYPE, 19, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ARG19_TYPE); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ARG19_TYPE); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17, ARG18_TYPE arg18, ARG19_TYPE arg19); \

#define DEFINE_FAKE_VALUE_FUNC20(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ARG19_TYPE) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17, ARG18_TYPE arg18, ARG19_TYPE arg19){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        SAVE_ARG(FUNCNAME, 15); \
        SAVE_ARG(FUNCNAME, 16); \
        SAVE_ARG(FUNCNAME, 17); \
        SAVE_ARG(FUNCNAME, 18); \
        SAVE_ARG(FUNCNAME, 19); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
            SAVE_ARG_HISTORY(FUNCNAME, 15); \
            SAVE_ARG_HISTORY(FUNCNAME, 16); \
            SAVE_ARG_HISTORY(FUNCNAME, 17); \
            SAVE_ARG_HISTORY(FUNCNAME, 18); \
            SAVE_ARG_HISTORY(FUNCNAME, 19); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
            else{ \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                return ret; \
            } \
        } \
        if (FUNCNAME##_fake.custom_fake != NULL){  \
            RETURN_TYPE ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC20(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ARG19_TYPE) \
    DECLARE_FAKE_VALUE_FUNC20(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ARG19_TYPE) \
    DEFINE_FAKE_VALUE_FUNC20(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ARG19_TYPE) \


#define DECLARE_FAKE_VOID_FUNC2_VARARG(FUNCNAME, ARG0_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ...); \

#define DEFINE_FAKE_VOID_FUNC2_VARARG(FUNCNAME, ARG0_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg0); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg0); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg0); \
            FUNCNAME##_fake.custom_fake(arg0, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC2_VARARG(FUNCNAME, ARG0_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC2_VARARG(FUNCNAME, ARG0_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC2_VARARG(FUNCNAME, ARG0_TYPE, ...) \


#define DECLARE_FAKE_VOID_FUNC3_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ...); \

#define DEFINE_FAKE_VOID_FUNC3_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg1); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg1); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg1); \
            FUNCNAME##_fake.custom_fake(arg0, arg1, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC3_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC3_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC3_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ...) \


#define DECLARE_FAKE_VOID_FUNC4_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ...); \

#define DEFINE_FAKE_VOID_FUNC4_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg2); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg2); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg2); \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC4_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC4_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC4_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ...) \


#define DECLARE_FAKE_VOID_FUNC5_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ...); \

#define DEFINE_FAKE_VOID_FUNC5_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg3); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg3); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg3); \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC5_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC5_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC5_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ...) \


#define DECLARE_FAKE_VOID_FUNC6_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ...); \

#define DEFINE_FAKE_VOID_FUNC6_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg4); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg4); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg4); \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC6_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC6_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC6_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ...) \


#define DECLARE_FAKE_VOID_FUNC7_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ...); \

#define DEFINE_FAKE_VOID_FUNC7_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg5); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg5); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg5); \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC7_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC7_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC7_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ...) \


#define DECLARE_FAKE_VOID_FUNC8_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ...); \

#define DEFINE_FAKE_VOID_FUNC8_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg6); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg6); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg6); \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC8_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC8_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC8_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ...) \


#define DECLARE_FAKE_VOID_FUNC9_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ...); \

#define DEFINE_FAKE_VOID_FUNC9_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg7); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg7); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg7); \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC9_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC9_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC9_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ...) \


#define DECLARE_FAKE_VOID_FUNC10_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ...); \

#define DEFINE_FAKE_VOID_FUNC10_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg8); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg8); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg8); \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC10_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC10_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC10_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ...) \


#define DECLARE_FAKE_VOID_FUNC11_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ...); \

#define DEFINE_FAKE_VOID_FUNC11_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg9); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg9); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg9); \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC11_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC11_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC11_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ...) \


#define DECLARE_FAKE_VOID_FUNC12_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ...); \

#define DEFINE_FAKE_VOID_FUNC12_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg10); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg10); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg10); \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC12_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC12_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC12_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ...) \


#define DECLARE_FAKE_VOID_FUNC13_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ...); \

#define DEFINE_FAKE_VOID_FUNC13_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg11); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg11); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg11); \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC13_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC13_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC13_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ...) \


#define DECLARE_FAKE_VOID_FUNC14_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ...); \

#define DEFINE_FAKE_VOID_FUNC14_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg12); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg12); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg12); \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC14_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC14_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC14_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ...) \


#define DECLARE_FAKE_VOID_FUNC15_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ...); \

#define DEFINE_FAKE_VOID_FUNC15_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg13); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg13); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg13); \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC15_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC15_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC15_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ...) \


#define DECLARE_FAKE_VOID_FUNC16_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ...); \

#define DEFINE_FAKE_VOID_FUNC16_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg14); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg14); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg14); \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC16_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC16_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC16_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ...) \


#define DECLARE_FAKE_VOID_FUNC17_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ARG(ARG15_TYPE, 15, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ...); \

#define DEFINE_FAKE_VOID_FUNC17_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        SAVE_ARG(FUNCNAME, 15); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
            SAVE_ARG_HISTORY(FUNCNAME, 15); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg15); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg15); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg15); \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC17_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC17_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC17_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ...) \


#define DECLARE_FAKE_VOID_FUNC18_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ARG(ARG15_TYPE, 15, FUNCNAME) \
        DECLARE_ARG(ARG16_TYPE, 16, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ...); \

#define DEFINE_FAKE_VOID_FUNC18_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        SAVE_ARG(FUNCNAME, 15); \
        SAVE_ARG(FUNCNAME, 16); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
            SAVE_ARG_HISTORY(FUNCNAME, 15); \
            SAVE_ARG_HISTORY(FUNCNAME, 16); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg16); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg16); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg16); \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC18_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC18_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC18_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ...) \


#define DECLARE_FAKE_VOID_FUNC19_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ARG(ARG15_TYPE, 15, FUNCNAME) \
        DECLARE_ARG(ARG16_TYPE, 16, FUNCNAME) \
        DECLARE_ARG(ARG17_TYPE, 17, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17, ...); \

#define DEFINE_FAKE_VOID_FUNC19_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        SAVE_ARG(FUNCNAME, 15); \
        SAVE_ARG(FUNCNAME, 16); \
        SAVE_ARG(FUNCNAME, 17); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
            SAVE_ARG_HISTORY(FUNCNAME, 15); \
            SAVE_ARG_HISTORY(FUNCNAME, 16); \
            SAVE_ARG_HISTORY(FUNCNAME, 17); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg17); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg17); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg17); \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC19_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC19_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC19_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ...) \


#define DECLARE_FAKE_VOID_FUNC20_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ARG(ARG15_TYPE, 15, FUNCNAME) \
        DECLARE_ARG(ARG16_TYPE, 16, FUNCNAME) \
        DECLARE_ARG(ARG17_TYPE, 17, FUNCNAME) \
        DECLARE_ARG(ARG18_TYPE, 18, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(void, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17, ARG18_TYPE arg18, ...); \

#define DEFINE_FAKE_VOID_FUNC20_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    void FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17, ARG18_TYPE arg18, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        SAVE_ARG(FUNCNAME, 15); \
        SAVE_ARG(FUNCNAME, 16); \
        SAVE_ARG(FUNCNAME, 17); \
        SAVE_ARG(FUNCNAME, 18); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
            SAVE_ARG_HISTORY(FUNCNAME, 15); \
            SAVE_ARG_HISTORY(FUNCNAME, 16); \
            SAVE_ARG_HISTORY(FUNCNAME, 17); \
            SAVE_ARG_HISTORY(FUNCNAME, 18); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg18); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, ap); \
                va_end(ap); \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg18); \
                FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, ap); \
                va_end(ap); \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            va_list ap; \
            va_start(ap, arg18); \
            FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, ap); \
            va_end(ap); \
        } \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VOID_FUNC20_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ...) \
    DECLARE_FAKE_VOID_FUNC20_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ...) \
    DEFINE_FAKE_VOID_FUNC20_VARARG(FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC2_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ...); \

#define DEFINE_FAKE_VALUE_FUNC2_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg0); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg0); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg0); \
            ret = FUNCNAME##_fake.custom_fake(arg0, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC2_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC2_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC2_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC3_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ...); \

#define DEFINE_FAKE_VALUE_FUNC3_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg1); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg1); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg1); \
            ret = FUNCNAME##_fake.custom_fake(arg0, arg1, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC3_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC3_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC3_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC4_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ...); \

#define DEFINE_FAKE_VALUE_FUNC4_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg2); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg2); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg2); \
            ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC4_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC4_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC4_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC5_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ...); \

#define DEFINE_FAKE_VALUE_FUNC5_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg3); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg3); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg3); \
            ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC5_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC5_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC5_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC6_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ...); \

#define DEFINE_FAKE_VALUE_FUNC6_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg4); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg4); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg4); \
            ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC6_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC6_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC6_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC7_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ...); \

#define DEFINE_FAKE_VALUE_FUNC7_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg5); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg5); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg5); \
            ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC7_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC7_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC7_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC8_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ...); \

#define DEFINE_FAKE_VALUE_FUNC8_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg6); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg6); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg6); \
            ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC8_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC8_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC8_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC9_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ...); \

#define DEFINE_FAKE_VALUE_FUNC9_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg7); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg7); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg7); \
            ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC9_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC9_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC9_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC10_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ...); \

#define DEFINE_FAKE_VALUE_FUNC10_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg8); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg8); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg8); \
            ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC10_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC10_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC10_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC11_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ...); \

#define DEFINE_FAKE_VALUE_FUNC11_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg9); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg9); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg9); \
            ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC11_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC11_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC11_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC12_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ...); \

#define DEFINE_FAKE_VALUE_FUNC12_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg10); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg10); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg10); \
            ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC12_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC12_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC12_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC13_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ...); \

#define DEFINE_FAKE_VALUE_FUNC13_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg11); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg11); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg11); \
            ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC13_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC13_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC13_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC14_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ...); \

#define DEFINE_FAKE_VALUE_FUNC14_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg12); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg12); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg12); \
            ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC14_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC14_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC14_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC15_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ...); \

#define DEFINE_FAKE_VALUE_FUNC15_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg13); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg13); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg13); \
            ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC15_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC15_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC15_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC16_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ...); \

#define DEFINE_FAKE_VALUE_FUNC16_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg14); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg14); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg14); \
            ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC16_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC16_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC16_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC17_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ARG(ARG15_TYPE, 15, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ...); \

#define DEFINE_FAKE_VALUE_FUNC17_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        SAVE_ARG(FUNCNAME, 15); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
            SAVE_ARG_HISTORY(FUNCNAME, 15); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg15); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg15); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg15); \
            ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC17_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC17_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC17_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC18_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ARG(ARG15_TYPE, 15, FUNCNAME) \
        DECLARE_ARG(ARG16_TYPE, 16, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ...); \

#define DEFINE_FAKE_VALUE_FUNC18_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        SAVE_ARG(FUNCNAME, 15); \
        SAVE_ARG(FUNCNAME, 16); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
            SAVE_ARG_HISTORY(FUNCNAME, 15); \
            SAVE_ARG_HISTORY(FUNCNAME, 16); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg16); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg16); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg16); \
            ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC18_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC18_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC18_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC19_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ARG(ARG15_TYPE, 15, FUNCNAME) \
        DECLARE_ARG(ARG16_TYPE, 16, FUNCNAME) \
        DECLARE_ARG(ARG17_TYPE, 17, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17, ...); \

#define DEFINE_FAKE_VALUE_FUNC19_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        SAVE_ARG(FUNCNAME, 15); \
        SAVE_ARG(FUNCNAME, 16); \
        SAVE_ARG(FUNCNAME, 17); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
            SAVE_ARG_HISTORY(FUNCNAME, 15); \
            SAVE_ARG_HISTORY(FUNCNAME, 16); \
            SAVE_ARG_HISTORY(FUNCNAME, 17); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg17); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg17); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg17); \
            ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC19_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC19_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC19_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ...) \


#define DECLARE_FAKE_VALUE_FUNC20_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ...) \
    typedef struct FUNCNAME##_Fake { \
        DECLARE_ARG(ARG0_TYPE, 0, FUNCNAME) \
        DECLARE_ARG(ARG1_TYPE, 1, FUNCNAME) \
        DECLARE_ARG(ARG2_TYPE, 2, FUNCNAME) \
        DECLARE_ARG(ARG3_TYPE, 3, FUNCNAME) \
        DECLARE_ARG(ARG4_TYPE, 4, FUNCNAME) \
        DECLARE_ARG(ARG5_TYPE, 5, FUNCNAME) \
        DECLARE_ARG(ARG6_TYPE, 6, FUNCNAME) \
        DECLARE_ARG(ARG7_TYPE, 7, FUNCNAME) \
        DECLARE_ARG(ARG8_TYPE, 8, FUNCNAME) \
        DECLARE_ARG(ARG9_TYPE, 9, FUNCNAME) \
        DECLARE_ARG(ARG10_TYPE, 10, FUNCNAME) \
        DECLARE_ARG(ARG11_TYPE, 11, FUNCNAME) \
        DECLARE_ARG(ARG12_TYPE, 12, FUNCNAME) \
        DECLARE_ARG(ARG13_TYPE, 13, FUNCNAME) \
        DECLARE_ARG(ARG14_TYPE, 14, FUNCNAME) \
        DECLARE_ARG(ARG15_TYPE, 15, FUNCNAME) \
        DECLARE_ARG(ARG16_TYPE, 16, FUNCNAME) \
        DECLARE_ARG(ARG17_TYPE, 17, FUNCNAME) \
        DECLARE_ARG(ARG18_TYPE, 18, FUNCNAME) \
        DECLARE_ALL_FUNC_COMMON \
        DECLARE_VALUE_FUNCTION_VARIABLES(RETURN_TYPE) \
        DECLARE_RETURN_VALUE_HISTORY(RETURN_TYPE) \
        DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, custom_fake, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, va_list ap); \
        CUSTOM_FFF_FUNCTION_TEMPLATE(RETURN_TYPE, *custom_fake_seq, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, va_list ap); \
    } FUNCNAME##_Fake; \
    extern FUNCNAME##_Fake FUNCNAME##_fake; \
    void FUNCNAME##_reset(void); \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17, ARG18_TYPE arg18, ...); \

#define DEFINE_FAKE_VALUE_FUNC20_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ...) \
    FUNCNAME##_Fake FUNCNAME##_fake; \
    RETURN_TYPE FFF_GCC_FUNCTION_ATTRIBUTES FUNCNAME(ARG0_TYPE arg0, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6, ARG7_TYPE arg7, ARG8_TYPE arg8, ARG9_TYPE arg9, ARG10_TYPE arg10, ARG11_TYPE arg11, ARG12_TYPE arg12, ARG13_TYPE arg13, ARG14_TYPE arg14, ARG15_TYPE arg15, ARG16_TYPE arg16, ARG17_TYPE arg17, ARG18_TYPE arg18, ...){ \
        SAVE_ARG(FUNCNAME, 0); \
        SAVE_ARG(FUNCNAME, 1); \
        SAVE_ARG(FUNCNAME, 2); \
        SAVE_ARG(FUNCNAME, 3); \
        SAVE_ARG(FUNCNAME, 4); \
        SAVE_ARG(FUNCNAME, 5); \
        SAVE_ARG(FUNCNAME, 6); \
        SAVE_ARG(FUNCNAME, 7); \
        SAVE_ARG(FUNCNAME, 8); \
        SAVE_ARG(FUNCNAME, 9); \
        SAVE_ARG(FUNCNAME, 10); \
        SAVE_ARG(FUNCNAME, 11); \
        SAVE_ARG(FUNCNAME, 12); \
        SAVE_ARG(FUNCNAME, 13); \
        SAVE_ARG(FUNCNAME, 14); \
        SAVE_ARG(FUNCNAME, 15); \
        SAVE_ARG(FUNCNAME, 16); \
        SAVE_ARG(FUNCNAME, 17); \
        SAVE_ARG(FUNCNAME, 18); \
        if(ROOM_FOR_MORE_HISTORY(FUNCNAME)){ \
            SAVE_ARG_HISTORY(FUNCNAME, 0); \
            SAVE_ARG_HISTORY(FUNCNAME, 1); \
            SAVE_ARG_HISTORY(FUNCNAME, 2); \
            SAVE_ARG_HISTORY(FUNCNAME, 3); \
            SAVE_ARG_HISTORY(FUNCNAME, 4); \
            SAVE_ARG_HISTORY(FUNCNAME, 5); \
            SAVE_ARG_HISTORY(FUNCNAME, 6); \
            SAVE_ARG_HISTORY(FUNCNAME, 7); \
            SAVE_ARG_HISTORY(FUNCNAME, 8); \
            SAVE_ARG_HISTORY(FUNCNAME, 9); \
            SAVE_ARG_HISTORY(FUNCNAME, 10); \
            SAVE_ARG_HISTORY(FUNCNAME, 11); \
            SAVE_ARG_HISTORY(FUNCNAME, 12); \
            SAVE_ARG_HISTORY(FUNCNAME, 13); \
            SAVE_ARG_HISTORY(FUNCNAME, 14); \
            SAVE_ARG_HISTORY(FUNCNAME, 15); \
            SAVE_ARG_HISTORY(FUNCNAME, 16); \
            SAVE_ARG_HISTORY(FUNCNAME, 17); \
            SAVE_ARG_HISTORY(FUNCNAME, 18); \
        } \
        else{ \
            HISTORY_DROPPED(FUNCNAME); \
        } \
        INCREMENT_CALL_COUNT(FUNCNAME); \
        REGISTER_CALL(FUNCNAME); \
        if (FUNCNAME##_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
            if (FUNCNAME##_fake.custom_fake_seq_idx < FUNCNAME##_fake.custom_fake_seq_len){ \
                va_list ap; \
                va_start(ap, arg18); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_idx++](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
            else{ \
                va_list ap; \
                va_start(ap, arg18); \
                RETURN_TYPE ret = FUNCNAME##_fake.custom_fake_seq[FUNCNAME##_fake.custom_fake_seq_len-1](arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, ap); \
                SAVE_RET_HISTORY(FUNCNAME, ret); \
                va_end(ap); \
                return ret; \
            } \
        } \
        if(FUNCNAME##_fake.custom_fake){ \
            RETURN_TYPE ret; \
            va_list ap; \
            va_start(ap, arg18); \
            ret = FUNCNAME##_fake.custom_fake(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, ap); \
            va_end(ap); \
            SAVE_RET_HISTORY(FUNCNAME, ret); \
            return ret; \
        } \
        RETURN_FAKE_RESULT(FUNCNAME) \
    } \
    DEFINE_RESET_FUNCTION(FUNCNAME) \

#define FAKE_VALUE_FUNC20_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ...) \
    DECLARE_FAKE_VALUE_FUNC20_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ...) \
    DEFINE_FAKE_VALUE_FUNC20_VARARG(RETURN_TYPE, FUNCNAME, ARG0_TYPE, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE, ARG7_TYPE, ARG8_TYPE, ARG9_TYPE, ARG10_TYPE, ARG11_TYPE, ARG12_TYPE, ARG13_TYPE, ARG14_TYPE, ARG15_TYPE, ARG16_TYPE, ARG17_TYPE, ARG18_TYPE, ...) \


//Argument counting (first, so that a missing __VA_OPT__ is reported before anything else)
#ifndef PP_UTIL_H
  #define PP_UTIL_H
  #define PP_SEQ_N() 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63
  #define PP_RSEQ_N() 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
  #define PP_I_THIRD_ARG(a, b, c, ...) c
  #define PP_I_VA_OPT_PROBE(...) PP_I_THIRD_ARG(__VA_OPT__(,), 1, 0, )
  #define PP_HAS_VA_OPT PP_I_VA_OPT_PROBE(?)
#endif //PP_UTIL_H
#if !PP_HAS_VA_OPT
  #error "pp_iter.h requires a preprocessor with __VA_OPT__ support (for MSVC, use /Zc:preprocessor)"
#endif

#ifndef PP_NARG_H
  #define PP_NARG_H
  #define PP_ARG_N(...) PP_I_ARG_N(__VA_ARGS__)
  #define PP_NARG(...) PP_ARG_N(PP_I_PLACEHOLDER __VA_OPT__(,) __VA_ARGS__, PP_RSEQ_N())
  #define PP_I_ARG_N(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, N, ...) N
#endif //PP_NARG_H


//Defer / evaluate macros
#ifndef PP_DEFER_H
  #define PP_DEFER_H
  #define PP_NOP()
  #define PP_DEFER(...) __VA_ARGS__ PP_NOP()
  #define PP_DEFER2(...) __VA_ARGS__ PP_DEFER(PP_NOP) ()
#endif //PP_DEFER_H

#ifndef PP_EVAL_H
  #define PP_EVAL_H
  #define PP_EVAL(...) PP_I_EVAL_6(__VA_ARGS__)
  #define PP_I_EVAL_1(...) __VA_ARGS__
  #define PP_I_EVAL_2(...) PP_I_EVAL_1(PP_I_EVAL_1(__VA_ARGS__))
  #define PP_I_EVAL_3(...) PP_I_EVAL_2(PP_I_EVAL_2(__VA_ARGS__))
  #define PP_I_EVAL_4(...) PP_I_EVAL_3(PP_I_EVAL_3(__VA_ARGS__))
  #define PP_I_EVAL_5(...) PP_I_EVAL_4(PP_I_EVAL_4(__VA_ARGS__))
  #define PP_I_EVAL_6(...) PP_I_EVAL_5(PP_I_EVAL_5(__VA_ARGS__))
#endif //PP_EVAL_H


//Token concatenation (tuple-aware)
#ifndef PP_CAT_H
  #define PP_CAT_H
  // Defer the call to the PP_CAT so that we get the updated parameters first
  #define PP_CAT(a, b) PP_I_CAT_EVAL ( PP_I_CAT_EXPAND_PARAMETERS(a, b) )
  #define PP_I_CAT_EVAL(...) PP_I_CAT_HELPER __VA_ARGS__

  // Find the result of testing whether a macro is enclosed or not
  #define PP_I_CAT_EXPAND_PARAMETERS(a, b) (a, PP_I_CAT_PAREN_CHECK b, PP_I_DEFAULT, b )
  #define PP_I_CAT_PAREN_CHECK(...) PP_I_EXPANDED, PP_I_ENCLOSED, (__VA_ARGS__) ) PP_EAT (

  // Pattern match the result of testing if it is enclose or not
  #define PP_I_CAT_HELPER(a, _, f, b) PP_I_CAT_HELPER_ ## f (a, b)
  #define PP_I_CAT_HELPER_PP_I_ENCLOSED(a, b) a b
  #define PP_I_CAT_HELPER_PP_I_DEFAULT(a, b) a ## b
#endif //PP_CAT_H


//Logical operations
#ifndef PP_LOGIC_H
  #define PP_LOGIC_H
  //Usage: PP_IF(<condition>) (<then>, <else>)
  #define  PP_IF(value) PP_CAT(PP_I_IF_, value)
  #define PP_I_IF_1(true, ...) true
  #define PP_I_IF_0(true, ...) __VA_ARGS__

  #define PP_NOT(x) PP_MATCHER ( PP_CAT(PP_I_NOT_, x), 0 )
  #define PP_I_NOT_0 PP_I_EXISTS(1)

  //== Building blocks

  //PP_MATCHER
  #define PP_MATCHER(value, ...)   PP_IF ( PP_I_MATCH(PP_I_MATCH_FIND(value)) )       ( PP_I_MATCH_EXTRACT(value), __VA_ARGS__ )

  #define PP_I_MATCH(x) PP_CAT(PP_I_MATCH_, x)
  #define PP_I_MATCH_PP_I_EXISTS(...) 1
  #define PP_I_MATCH_PP_I_UNMATCHED 0

  #define PP_I_MATCH_FIND(x) PP_I_MATCH_RESULT_EXTRACT (  PP_I_MATCH_RESULT_FIND(x) )

  /**
   * Extract 2nd element of a match result in the format:
   * (IGNORED, PP_I_EXISTS(DEFINED_VALUE)) or (IGNORED, PP_I_UNMATCHED)
   * This is appended to PP_I_MATCH_ to convert result to a boolean,
   */
  #define PP_I_MATCH_RESULT_EXTRACT(x) PP_I_MATCH_RESULT_EXTRACT_HELPER  x
  #define PP_I_MATCH_RESULT_EXTRACT_HELPER(match, return_value) return_value

  /**
   * PP_I_MATCH_RESULT_FIND( PP_I_EXISTS(bla) ) returns ( PP_I_EXPANDED, PP_I_EXISTS(bla) )
   * PP_I_MATCH_RESULT_FIND( bla ) returns ( TEST_bla, PP_I_UNMATCHED)
   */
  #define PP_I_MATCH_RESULT_FIND(x) ( PP_CAT(PP_I_MATCH_RESULT_EXPAND_, x),  PP_I_UNMATCHED )
  #define PP_I_MATCH_RESULT_EXPAND_PP_I_EXISTS(...) PP_I_EXPANDED, PP_I_EXISTS(__VA_ARGS__) ) PP_EAT (
  #define PP_EAT(...)

  //PP_I_MATCH_EXTRACT / PP_I_MATCH_EXTRACT_PP_I_EXISTS
  #define PP_I_MATCH_EXTRACT(value) PP_CAT(PP_I_MATCH_EXTRACT_, value)
  #define PP_I_MATCH_EXTRACT_PP_I_EXISTS(...) __VA_ARGS__
#endif //PP_LOGIC_H


//Lists (PP_HEAD, PP_TAIL, PP_IS_EMPTY etc.)
#ifndef PP_LISTS_H
  #define PP_LISTS_H
  #define PP_HEAD(FIRST, ...) FIRST
  #define PP_TAIL(FIRST, ...) __VA_ARGS__

  #define PP_NOT_EMPTY(...) PP_NOT(PP_IS_EMPTY(__VA_ARGS__))
  #define PP_IS_EMPTY(...) PP_NOT(PP_NARG(__VA_ARGS__))
#endif //PP_LISTS_H


//Tuples
#ifndef PP_TUPLES_H
  #define PP_TUPLES_H
  #define PP_DEPAREN(...) PP_I_DEPAREN __VA_ARGS__
  #define PP_I_DEPAREN(...) __VA_ARGS__

  #define PP_IS_ENCLOSED(x, ...) PP_MATCHER ( PP_I_IS_ENCLOSED_TEST x, 0 )
  #define PP_I_IS_ENCLOSED_TEST(...) PP_I_EXISTS(1)

  #define PP_IF_ENCLOSED(...) PP_CAT(PP_I_IF_ENCLOSED_, PP_IS_ENCLOSED(__VA_ARGS__))
  #define PP_I_IF_ENCLOSED_0(true, ...) __VA_ARGS__
  #define PP_I_IF_ENCLOSED_1(true, ...) true
  // This function will optionally remove parentheses around its arguments
  // if there are any. Otherwise it will return normally
  #define PP_OPT_DEPAREN(...)   PP_IF_ENCLOSED (__VA_ARGS__) ( PP_DEPAREN(__VA_ARGS__), __VA_ARGS__ )
#endif //PP_TUPLES_H


//PP_EACH
#ifndef PP_EACH_H
  #define PP_EACH_H
  #define PP_EACH(TF, ...) \
    PP_EVAL(PP_I_EACH_DEFER(TF, __VA_ARGS__))

  #define PP_I_EACH_DEFER(TF, ...) \
    PP_IF ( PP_NOT_EMPTY( __VA_ARGS__ )  ) \
    ( \
      PP_DEFER(TF) (PP_OPT_DEPAREN(PP_HEAD(__VA_ARGS__))) \
      PP_DEFER2 ( PP_I_EACH_INDIRECT ) () (TF, PP_TAIL(__VA_ARGS__)) \
    )

  //This indirection along with the PP_DEFER2 and PP_EVAL macros allows the recursive implementation of PP_I_EACH_DEFER
  #define PP_I_EACH_INDIRECT() PP_I_EACH_DEFER
#endif //PP_EACH_H


//PP_EACH_IDX
#ifndef PP_EACH_IDX_H
  #define PP_EACH_IDX_H
  #define PP_EACH_IDX(TF, ...) PP_EVAL(PP_I_EACH_IDX_DEFER(TF, (PP_SEQ_N()), __VA_ARGS__))

  #define PP_I_EACH_IDX_DEFER(TF, VA_INDICES, ...) \
      PP_IF ( PP_NOT_EMPTY( __VA_ARGS__ )  ) \
      ( \
        PP_DEFER2(TF) (PP_OPT_DEPAREN(PP_HEAD(__VA_ARGS__)), PP_DEFER(PP_HEAD)(PP_DEPAREN(VA_INDICES))) \
        PP_DEFER2 ( PP_I_EACH_IDX_INDIRECT ) () (TF, (PP_TAIL VA_INDICES), PP_TAIL(__VA_ARGS__)) \
      )

    #define PP_I_EACH_IDX_INDIRECT() PP_I_EACH_IDX_DEFER
#endif //PP_EACH_IDX_H


//PP_PAR_EACH_IDX
#ifndef PP_PAR_EACH_IDX_H
  #define PP_PAR_EACH_IDX_H
  #define PP_PAR_EACH_IDX(TF, FARGS, ...) PP_EVAL(PP_I_PAR_EACH_IDX_DEFER(TF, FARGS, (PP_SEQ_N()), __VA_ARGS__))

  #define PP_I_PAR_EACH_IDX_DEFER(TF, FARGS, VA_INDICES, ...)                         \
    PP_IF ( PP_NOT_EMPTY( __VA_ARGS__ )  )                                      \
    (                                                                     \
      PP_DEFER2(TF) (PP_OPT_DEPAREN(FARGS), PP_OPT_DEPAREN(PP_HEAD(__VA_ARGS__)), PP_DEFER(PP_HEAD)(PP_DEPAREN(VA_INDICES))) \
      PP_DEFER2 ( PP_I_PAR_EACH_IDX_INDIRECT ) () (TF, FARGS, (PP_TAIL VA_INDICES), PP_TAIL(__VA_ARGS__)) \
    )

  #define PP_I_PAR_EACH_IDX_INDIRECT() PP_I_PAR_EACH_IDX_DEFER
#endif //PP_PAR_EACH_IDX_H


//PP_COMMA
#ifndef PP_COMMA_H
  #define PP_COMMA_H
  #define PP_COMMA() ,
#endif //PP_COMMA_H


//PP_COMMA_EACH
#ifndef PP_COMMA_EACH_H
  #define PP_COMMA_EACH_H
  #define PP_COMMA_EACH(TF, ...) \
    PP_EVAL(PP_I_COMMA_EACH_DEFER(TF, __VA_ARGS__))

  #define PP_I_COMMA_EACH_DEFER(TF, ...) \
    PP_IF ( PP_NOT_EMPTY( __VA_ARGS__ )  ) \
    ( \
      PP_DEFER(TF) (PP_OPT_DEPAREN(PP_HEAD(__VA_ARGS__))) \
      PP_IF ( PP_NOT_EMPTY(PP_TAIL(__VA_ARGS__))) (PP_COMMA()) \
      PP_DEFER2 ( PP_I_COMMA_EACH_INDIRECT ) () (TF, PP_TAIL(__VA_ARGS__)) \
    )

  //This indirection along with the PP_DEFER2 and PP_EVAL macros allows the recursive implementation of PP_I_COMMA_EACH_DEFER
  #define PP_I_COMMA_EACH_INDIRECT() PP_I_COMMA_EACH_DEFER
#endif //PP_COMMA_EACH_H



#ifndef PP_NARG_MINUS2_H
  #define PP_NARG_MINUS2_H
  #define PP_ARG_MINUS2_N(...) PP_I_ARG_MINUS2_N(__VA_ARGS__)
  #define PP_NARG_MINUS2(...) PP_ARG_MINUS2_N(PP_I_PLACEHOLDER __VA_OPT__(,) __VA_ARGS__, PP_RSEQ_N())
  #define PP_I_ARG_MINUS2_N(m1, m0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, N, ...) N
#endif //PP_NARG_MINUS2_H

#ifndef PP_NARG_MINUS1_H
  #define PP_NARG_MINUS1_H
  #define PP_ARG_MINUS1_N(...) PP_I_ARG_MINUS1_N(__VA_ARGS__)
  #define PP_NARG_MINUS1(...) PP_ARG_MINUS1_N(PP_I_PLACEHOLDER __VA_OPT__(,) __VA_ARGS__, PP_RSEQ_N())
  #define PP_I_ARG_MINUS1_N(m0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, N, ...) N
#endif //PP_NARG_MINUS1_H


/* DECLARE AND DEFINE FAKE FUNCTIONS - PLACE IN TEST FILES */

#define FAKE_VALUE_FUNC(...)     FUNC_VALUE_(PP_NARG_MINUS2(__VA_ARGS__), __VA_ARGS__)

#define FUNC_VALUE_(N,...)     FUNC_VALUE_N(N,__VA_ARGS__)

#define FUNC_VALUE_N(N,...)     FAKE_VALUE_FUNC ## N(__VA_ARGS__)


#define FAKE_VOID_FUNC(...)     FUNC_VOID_(PP_NARG_MINUS1(__VA_ARGS__), __VA_ARGS__)

#define FUNC_VOID_(N,...)     FUNC_VOID_N(N,__VA_ARGS__)

#define FUNC_VOID_N(N,...)     FAKE_VOID_FUNC ## N(__VA_ARGS__)


#define FAKE_VALUE_FUNC_VARARG(...)     FUNC_VALUE_VARARG_(PP_NARG_MINUS2(__VA_ARGS__), __VA_ARGS__)

#define FUNC_VALUE_VARARG_(N,...)     FUNC_VALUE_VARARG_N(N,__VA_ARGS__)

#define FUNC_VALUE_VARARG_N(N,...)     FAKE_VALUE_FUNC ## N ## _VARARG(__VA_ARGS__)


#define FAKE_VOID_FUNC_VARARG(...)     FUNC_VOID_VARARG_(PP_NARG_MINUS1(__VA_ARGS__), __VA_ARGS__)

#define FUNC_VOID_VARARG_(N,...)     FUNC_VOID_VARARG_N(N,__VA_ARGS__)

#define FUNC_VOID_VARARG_N(N,...)     FAKE_VOID_FUNC ## N ## _VARARG(__VA_ARGS__)



/* DECLARE FAKE FUNCTIONS - PLACE IN HEADER FILES */

#define DECLARE_FAKE_VALUE_FUNC(...)     DECLARE_FUNC_VALUE_(PP_NARG_MINUS2(__VA_ARGS__), __VA_ARGS__)

#define DECLARE_FUNC_VALUE_(N,...)     DECLARE_FUNC_VALUE_N(N,__VA_ARGS__)

#define DECLARE_FUNC_VALUE_N(N,...)     DECLARE_FAKE_VALUE_FUNC ## N(__VA_ARGS__)


#define DECLARE_FAKE_VOID_FUNC(...)     DECLARE_FUNC_VOID_(PP_NARG_MINUS1(__VA_ARGS__), __VA_ARGS__)

#define DECLARE_FUNC_VOID_(N,...)     DECLARE_FUNC_VOID_N(N,__VA_ARGS__)

#define DECLARE_FUNC_VOID_N(N,...)     DECLARE_FAKE_VOID_FUNC ## N(__VA_ARGS__)


#define DECLARE_FAKE_VALUE_FUNC_VARARG(...)     DECLARE_FUNC_VALUE_VARARG_(PP_NARG_MINUS2(__VA_ARGS__), __VA_ARGS__)

#define DECLARE_FUNC_VALUE_VARARG_(N,...)     DECLARE_FUNC_VALUE_VARARG_N(N,__VA_ARGS__)

#define DECLARE_FUNC_VALUE_VARARG_N(N,...)     DECLARE_FAKE_VALUE_FUNC ## N ## _VARARG(__VA_ARGS__)


#define DECLARE_FAKE_VOID_FUNC_VARARG(...)     DECLARE_FUNC_VOID_VARARG_(PP_NARG_MINUS1(__VA_ARGS__), __VA_ARGS__)

#define DECLARE_FUNC_VOID_VARARG_(N,...)     DECLARE_FUNC_VOID_VARARG_N(N,__VA_ARGS__)

#define DECLARE_FUNC_VOID_VARARG_N(N,...)     DECLARE_FAKE_VOID_FUNC ## N ## _VARARG(__VA_ARGS__)



/* DEFINE FAKE FUNCTIONS - PLACE IN SOURCE FILES */

#define DEFINE_FAKE_VALUE_FUNC(...)     DEFINE_FUNC_VALUE_(PP_NARG_MINUS2(__VA_ARGS__), __VA_ARGS__)

#define DEFINE_FUNC_VALUE_(N,...)     DEFINE_FUNC_VALUE_N(N,__VA_ARGS__)

#define DEFINE_FUNC_VALUE_N(N,...)     DEFINE_FAKE_VALUE_FUNC ## N(__VA_ARGS__)


#define DEFINE_FAKE_VOID_FUNC(...)     DEFINE_FUNC_VOID_(PP_NARG_MINUS1(__VA_ARGS__), __VA_ARGS__)

#define DEFINE_FUNC_VOID_(N,...)     DEFINE_FUNC_VOID_N(N,__VA_ARGS__)

#define DEFINE_FUNC_VOID_N(N,...)     DEFINE_FAKE_VOID_FUNC ## N(__VA_ARGS__)


#define DEFINE_FAKE_VALUE_FUNC_VARARG(...)     DEFINE_FUNC_VALUE_VARARG_(PP_NARG_MINUS2(__VA_ARGS__), __VA_ARGS__)

#define DEFINE_FUNC_VALUE_VARARG_(N,...)     DEFINE_FUNC_VALUE_VARARG_N(N,__VA_ARGS__)

#define DEFINE_FUNC_VALUE_VARARG_N(N,...)     DEFINE_FAKE_VALUE_FUNC ## N ## _VARARG(__VA_ARGS__)


#define DEFINE_FAKE_VOID_FUNC_VARARG(...)     DEFINE_FUNC_VOID_VARARG_(PP_NARG_MINUS1(__VA_ARGS__), __VA_ARGS__)

#define DEFINE_FUNC_VOID_VARARG_(N,...)     DEFINE_FUNC_VOID_VARARG_N(N,__VA_ARGS__)

#define DEFINE_FUNC_VOID_VARARG_N(N,...)     DEFINE_FAKE_VOID_FUNC ## N ## _VARARG(__VA_ARGS__)



/* -- VARIADIC EXTENSIONS -- */

/* Accessors (Public API) */
#define FFF(FN) FN##_fake
#define FFF_CALLS(FN) FFF(FN).call_count
#define FFF_LAST_ARG_VAL(FN, ARG_IDX) FFF(FN).arg##ARG_IDX##_val
#define FFF_NTH_ARG_VAL(FN, CALL_IDX, ARG_IDX) FFF(FN).arg##ARG_IDX##_history[CALL_IDX-1]
#define FFF_RET(FN) FFF(FN).return_val

/* Assertion hook: define _FFF_ASSERT_EQ_MSG before you include fff.h to report through your test framework */
#ifndef _FFF_ASSERT_EQ_MSG
#include <assert.h>
#include <stdio.h>
#define _FFF_ASSERT_EQ_MSG(expected, actual, message) do { \
        if((expected)!=(actual)) {printf("%s\n", message);} \
        assert((expected)==(actual)); \
} while(0)
#endif

#ifndef _FFF_ASSERT_EQ
#define _FFF_ASSERT_EQ(expected, actual) _FFF_ASSERT_EQ_MSG(expected, actual, "Expected " #expected " / Got " #actual)
#endif

/* The ternary gives int operands, so C needs no <stdbool.h> and MSVC gives no bool/int warning */
#define _FFF_ASSERT_TRUE_MSG(COND, MSG) _FFF_ASSERT_EQ_MSG(((COND) ? 1 : 0), 1, MSG)
#define _FFF_ASSERT_FALSE_MSG(COND, MSG) _FFF_ASSERT_EQ_MSG(((COND) ? 1 : 0), 0, MSG)

/* Call count assertions (Public API) */
#define FFF_ASSERT_CALLS(FN, COUNT) _FFF_ASSERT_EQ_MSG((unsigned int)(COUNT), FFF_CALLS(FN), #FN " called incorrect number of times")
#define FFF_ASSERT_CALLED(FN)  FFF_ASSERT_CALLS(FN, 1);
#define FFF_ASSERT_NOT_CALLED(FN)  FFF_ASSERT_CALLS(FN, 0);

/* FFF_RETURN (Public API). It needs typeof (GCC, Clang) or decltype (C++). */
#define FFF_RETURN(...)                                             \
  _FFF_SEQ_DEF(__VA_ARGS__); if(PP_NARG(__VA_ARGS__) == 2) {FFF_RETURN_VAL(__VA_ARGS__, 0);} else {_FFF_RETURN_SEQ(PP_HEAD(__VA_ARGS__));}
#ifdef _MSC_VER
#define _FFF_RET_TYPE(FN) decltype(FFF_RET(FN))
#else
#define _FFF_RET_TYPE(FN) __typeof__(FFF_RET(FN))
#endif
#define _FFF_SEQ(FN) PP_CAT(FN, __LINE__)
#define _FFF_SEQ_DEF(FN, ...) _FFF_RET_TYPE(FN) _FFF_SEQ(FN)[] = {__VA_ARGS__}
#define _FFF_SEQ_LEN(FN) (sizeof(_FFF_SEQ(FN))/sizeof(FFF_RET(FN)))
#define FFF_RETURN_VAL(FN, ...) FFF_RET(FN) = PP_HEAD(__VA_ARGS__)
#define _FFF_RETURN_SEQ(FN) SET_RETURN_SEQ(FN, _FFF_SEQ(FN), _FFF_SEQ_LEN(FN)); \
    _FFF_ASSERT_TRUE_MSG(FFF(FN).return_val_seq_len>0, "No return values specified")

/* Argument assertions (Public API) */
#define FFF_ASSERT(FN, ...) \
  FFF_ASSERT_CALLS(FN, 1); \
  PP_PAR_EACH_IDX(_FFF_ASSERT_ARG, (FN), __VA_ARGS__)

#define FFF_ASSERT_NTH(FN, CALL_IDX, ...) \
  _FFF_ASSERT_TRUE_MSG(CALL_IDX>0, "Invalid call index -- expected >0, got " #CALL_IDX); \
  _FFF_ASSERT_TRUE_MSG(FFF_CALLS(FN) >= (unsigned int)(CALL_IDX), #FN " not called " #CALL_IDX " times"); \
  PP_PAR_EACH_IDX(_FFF_ASSERT_HISTORICAL_ARG, (FN, CALL_IDX), __VA_ARGS__)

#define FFF_ASSERT_LAST(FN, ...) FFF_ASSERT_NTH(FN, FFF_CALLS(FN), __VA_ARGS__)

/* FFF_ASSERT_ANY and FFF_ASSERT_NONE use a GCC statement expression (GCC, Clang) */
#define FFF_ASSERT_ANY(FN, ...) _FFF_ASSERT_TRUE_MSG(_FFF_VERIFY_ANY_CALL(FN, __VA_ARGS__), "No calls made to '"#FN"("#__VA_ARGS__")'")
#define FFF_ASSERT_NONE(FN, ...)  _FFF_ASSERT_FALSE_MSG(_FFF_VERIFY_ANY_CALL(FN, __VA_ARGS__), "At least one call made to '"#FN"("#__VA_ARGS__")'")

/* Argument assertions (Internal) */
#define _FFF_ASSERT_ARG(FN, VAL, ARG_IDX) _FFF_ASSERT_EQ_MSG((VAL), FFF_LAST_ARG_VAL(FN, ARG_IDX), #FN" parameter value mismatch at ARG"#ARG_IDX);
#define _FFF_ASSERT_HISTORICAL_ARG(FN, CALL_IDX, VAL, ARG_IDX) _FFF_ASSERT_EQ_MSG((VAL), FFF_NTH_ARG_VAL(FN, CALL_IDX, ARG_IDX),  #FN", call "#CALL_IDX": value mismatch for arg"#ARG_IDX);
#define _FFF_VERIFY_HISTORICAL_ARG(FN, CALL_IDX, VAL, ARG_IDX) ((VAL) == FFF_NTH_ARG_VAL(FN, CALL_IDX, ARG_IDX))
#define _FFF_AND_VERIFY_NTH_CALL_ARG(FN, CALL_IDX, VAL, ARG_IDX) && _FFF_VERIFY_HISTORICAL_ARG(FN, CALL_IDX, VAL, ARG_IDX)
#define _FFF_VERIFY_NTH_CALL(FN, CALL_IDX, ...)                                \
  ((CALL_IDX > 0) && ((unsigned int)(CALL_IDX) <= FFF_CALLS(FN))               \
   PP_PAR_EACH_IDX(_FFF_AND_VERIFY_NTH_CALL_ARG, (FN, CALL_IDX), __VA_ARGS__))
#define _FFF_VERIFY_ANY_CALL(FN, ...)                                          \
  ({                                                                           \
    int verified = 0;                                                          \
    unsigned int call_idx = FFF_CALLS(FN);                                     \
    while (call_idx && !verified) {                                            \
      verified |= _FFF_VERIFY_NTH_CALL(FN, call_idx, __VA_ARGS__);             \
      call_idx--;                                                              \
    }                                                                          \
    verified;                                                                  \
  })

/* -- END VARIADIC EXTENSIONS -- */

#endif /* FAKE_FUNCTIONS */
