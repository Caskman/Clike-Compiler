
#ifndef INSTRLIST_IMP
#define INSTRLIST_IMP

#define INSTR_COMMENT 2349
#define INSTR_SW 91750
#define INSTR_LW 24132
#define INSTR_DATA 96744
#define INSTR_TEXT 18365
#define INSTR_DOUBLE 2845
#define INSTR_INT 7884
#define INSTR_ADDU 183275
#define INSTR_ADD 5825
#define INSTR_SUB 9562
#define INSTR_MUL 8552
#define INSTR_DIV 7980
#define INSTR_SUBU 79886
#define INSTR_JR 67452
#define INSTR_JAL 7854
#define INSTR_LABEL 48563
#define INSTR_LI 2659
#define INSTR_LA 9573
#define INSTR_SLL 18367
#define INSTR_J 2846
#define INSTR_SEQ 1836
#define INSTR_SNE 9652
#define INSTR_SLE 1659
#define INSTR_SGE 5452
#define INSTR_SLT 8765
#define INSTR_SGT 1548
#define INSTR_BEQ 2957
#define INSTR_NEG 129476
#define INSTR_XOR 6636

// sw uses src,src2,i; src is for the value to store, src2 is the address, i is immediate
// dummy uses src
// double and int use src and i

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
