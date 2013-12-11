
#ifndef INSTRLIST_IMP
#define INSTRLIST_IMP

#define INSTR_COMMENT 2349
#define INSTR_SW 91750

// sw uses src,src2,i; src is for the value to store, src2 is the address, i is immediate
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
