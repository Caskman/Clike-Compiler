
#ifndef INSTRLIST_IMP
#define INSTRLIST_IMP

#define INSTR_COMMENT 2349

// dummy uses src

typedef char* String;

// type is the instruction type
// i is for immediate values
// dest, src, and src2 are registers
typedef struct instr {
    int type;
    String dest,src,src2;
    int i;
} Instr;

void printInstr(Instr *data);
void freeInstr(Instr *data);
Instr* dupInstr(Instr *data);
int compareInstr(Instr *a,Instr *b);

#endif
