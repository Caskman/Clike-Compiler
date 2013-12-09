
#ifndef QUADLIST_IMP
#define QUADLIST_IMP

#define QUAD_INIT_INT 1094
#define QUAD_INIT_DOUBLE 9374
#define QUAD_ASSG 923
#define QUAD_GOTO 3983
#define QUAD_BRANCH 286
#define QUAD_LABEL 85322
#define QUAD_INDX_L 13253
#define QUAD_INDX_R 8763
#define QUAD_ENTER 5174
#define QUAD_PARAM 58890
#define QUAD_CALL 24311
#define QUAD_RETVAL 3476
#define QUAD_RETURN 2346
#define QUAD_RETRIEVE 7653
#define QUAD_DUMMY 3947

// initializations use intcon or doublecon and dest
// assignments use dest,src,src2, and op
// goto uses string
// branch uses src,src2,op,string
// labels use string
// indexed assignments use dest,src,src2
// enter uses intcon
// param uses src,intcon
// call uses string
// retval uses src
// retrieve uses dest
typedef struct quad {
    int type,op,intcon;
    double doublecon;
    Sym *dest,*src,*src2;
    String string;
} Quad;

void printQuad(Quad *data);
void freeQuad(Quad *data);
Quad* dupQuad(Quad *data);
int compareQuad(Quad *a,Quad *b);

#endif
