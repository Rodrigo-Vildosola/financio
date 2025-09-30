#include <stdio.h>

// ============================
// 1. Result Base Types
// ============================

typedef enum {
    RESULT_OK,
    RESULT_ERR
} ResultTag;

#define DEFINE_RESULT_TYPE(type_name, ok_type, err_type) \
    typedef struct { \
        ResultTag tag; \
        union { \
            ok_type ok; \
            err_type err; \
        } value; \
    } type_name;

// ============================
// 2. Specific Result Types
// ============================

DEFINE_RESULT_TYPE(IntResult, int, const char *)

// ============================
// 3. Constructors
// ============================

IntResult OkInt(int val) {
    return (IntResult){ .tag = RESULT_OK, .value.ok = val };
}

IntResult ErrInt(const char *err) {
    return (IntResult){ .tag = RESULT_ERR, .value.err = err };
}

// ============================
// 4. Example Function Using Result
// ============================

IntResult divide(int a, int b) {
    if (b == 0) {
        return ErrInt("division by zero");
    }
    return OkInt(a / b);
}

// ============================
// 5. Optional Pattern-Matching Macro
// ============================

#define MATCH_RESULT(res, ok_block, err_block) \
    if ((res).tag == RESULT_OK) { ok_block } else { err_block }

// ============================
// 6. Main Function
// ============================

int main() {
    IntResult res1 = divide(10, 2);
    IntResult res2 = divide(10, 0);

    printf("First division:\n");
    MATCH_RESULT(res1,
        {
            printf("Success: %d\n", res1.value.ok);
        },
        {
            printf("Error: %s\n", res1.value.err);
        }
    );

    printf("\nSecond division:\n");
    MATCH_RESULT(res2,
        {
            printf("Success: %d\n", res2.value.ok);
        },
        {
            printf("Error: %s\n", res2.value.err);
        }
    );

    return 0;
}
