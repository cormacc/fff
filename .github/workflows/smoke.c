#include "fff.h"
DEFINE_FFF_GLOBALS;
FAKE_VOID_FUNC(f0);
FAKE_VALUE_FUNC(int, f2, int, char);
FAKE_VOID_FUNC_VARARG(fv, const char*, ...);
int main(void){f0(); f2(1,2); fv("x",1); return !(f0_fake.call_count==1 && f2_fake.arg1_val==2 && fv_fake.call_count==1);}
