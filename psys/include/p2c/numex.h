/* Header for module numex, generated by p2c */
#ifndef NUMEX_H
#define NUMEX_H


#ifndef NEWASM_H
#include <p2c/newasm.h>
#endif

#ifndef MATH_H
#include <p2c/math.h>
#endif


#ifdef NUMEX_G
# define vextern
#else
# define vextern extern
#endif


#define ne_maxargs      255

#define ne_badvalue     1.2345678e277   /*not likely to occur in nature*/

#define ne_badescape    (-50)



typedef enum {
  ne_notype, ne_integer, ne_real, ne_string, ne_boolean
} ne_datatype;
typedef ne_datatype ne_datatypearray[ne_maxargs];

typedef enum {
  ne_error, ne_ic, ne_rc, ne_sc, ne_ip, ne_rp, ne_sp, ne_if, ne_rf, ne_sf,
  ne_iarg, ne_rarg, ne_sarg, ne_iadd, ne_isub, ne_imul, ne_idiv, ne_ineg,
  ne_radd, ne_rsub, ne_rmul, ne_rdiv, ne_rneg, ne_rpow, ne_sadd, ne_itor,
  ne_imod, ne_rbool, ne_sbool, ne_ieq, ne_ine, ne_ilt, ne_ile, ne_igt, ne_ige,
  ne_req, ne_rne, ne_rlt, ne_rle, ne_rgt, ne_rge, ne_seq, ne_sne, ne_slt,
  ne_sle, ne_sgt, ne_sge, ne_not, ne_and, ne_or, ne_xor, ne_lsh, ne_rsh,
  ne_lnot, ne_ior, ne_ror, ne_sor, ne_iand, ne_rand, ne_sand, ne_icond,
  ne_rcond, ne_scond, ne_round, ne_trunc, ne_ln, ne_exp, ne_iabs, ne_rabs,
  ne_sqrt, ne_sin, ne_cos, ne_tan, ne_arctan, ne_rxp, ne_rxor
/* p2c: numex.text, line 131: 
 * Note: Line breaker spent 3.0 seconds, 5000 tries on line 45 [251] */
} ne_opkind;


typedef enum {
  ne_noerror, ne_syntax, ne_overflow, ne_underflow, ne_divzero, ne_strlong,
  ne_badtypes, ne_undef, ne_badval
} ne_errorkind;




typedef struct ne_functionrec {
  uchar nargs, minargs, maxargs;
  unsigned static_ : 1, subnex : 1, dummy_2 : 6;
  union {
    na_quadword qw;
    struct ne_nexrec **nexp;
    struct {
      _PROCEDURE ipr;
      unsigned arg1 : 3, arg2 : 3, arg3 : 3, arg4 : 3;
    } U5;
/* p2c: numex.text, line 78: Note:
 * Field width for ARG1 assumes enum ne_datatype has 5 elements [105] */
    _PROCEDURE rpr;
    _PROCEDURE spr;
    struct {
      na_quadword qw2;
      ne_datatype ptypes[ne_maxargs];
    } U99;
  } UU;
} ne_functionrec;

typedef struct ne_nexrec *ne_nexptrarray[ne_maxargs];

typedef na_quadword ne_argvalarray[ne_maxargs];

typedef struct ne_nexrec {
  uchar nargs;
  unsigned op : 7;
  union {
/* p2c: numex.text, line 94:
 * Note: Field width for OP assumes enum ne_opkind has 76 elements [105] */
    na_quadword qw;
    unsigned err : 4;
/* p2c: numex.text, line 98: Note:
 * Field width for ERR assumes enum ne_errorkind has 9 elements [105] */
    long i;
    double r;
    Char *s;
    struct {
      long *ip;
      na_strlist *ips;
      struct ne_nexrec *p1, *p2, *p3, *p4;
    } U10;
    struct {
      double *rp;
      na_strlist *rps;
    } U11;
    struct {
      Char **sp;
      na_strlist *sps;
    } U12;
    struct {
      ne_functionrec *fp;
      na_strlist *fps;
    } U15;
    struct {
      struct ne_nexrec **ep;
      /*!*/
      na_strlist *eps;
    } U16;
    struct {
      na_quadword qw2;
      ne_nexptrarray pvals;
    } U99;
  } UU;
} ne_nexrec;


typedef struct ne_desc {
  na_strlist *symtab;
  unsigned casesens : 1, builtin : 1, scaled : 1, doubleeq : 1, isfunc : 1,
	   dummy0 : 7, error : 4;
/* p2c: numex.text, line 123: Note:
 * Field width for ERROR assumes enum ne_errorkind has 9 elements [105] */
  Char units[6];
  long startident[9], ident[9];   /*must not contain #0*/
  _PROCEDURE symproc;
  na_long dummy1, dummy2;
} ne_desc;


#define ne_kind_intconst  'A'   /*for symbol table entries*/
#define ne_kind_intvalue  'B'
#define ne_kind_intptr  'C'
#define ne_kind_intfunc  'D'
#define ne_kind_realconst  'E'
#define ne_kind_srealconst  'F'
#define ne_kind_realptr  'G'
#define ne_kind_realfunc  'H'
#define ne_kind_strconst  'I'
#define ne_kind_strvalue  'J'
#define ne_kind_strptr  'K'
#define ne_kind_strfunc  'L'
#define ne_kind_intarg  'M'
#define ne_kind_realarg  'N'
#define ne_kind_strarg  'O'
#define ne_kind_realxptr  'P'
#define ne_kind_nex     'Z'


vextern na_quadword *ne_argarray;


extern Void ne_init PP((ne_desc *desc));
extern Void ne_compile PP((Char *ex, ne_nexrec **nex, ne_desc *desc));
/*
   procedure ne_intcompile(anyvar ex : na_chararray; var endp : integer;
                           var nex : ne_nexptr; var desc : ne_desc);
*/
extern Void ne_intcompile PP((Char *ex, long *endp, ne_nexrec **nex,
			      ne_desc *desc));
extern Void ne_dump PP((ne_nexrec *nex));
extern Void ne_uncompile PP((ne_nexrec *nex, Char *ex, ne_desc *desc));
extern Void ne_copy PP((ne_nexrec *nex, ne_nexrec **newnex));
extern Void ne_dispose PP((ne_nexrec **nex));
extern Void ne_constant PP((ne_nexrec **nex, ne_desc *desc, Char *which));
extern Void ne_notconstant PP((ne_nexrec **nex, ne_desc *desc, Char *which));
extern Void ne_constantlist PP((ne_nexrec **nex, ne_desc *desc,
				na_strlist *which));
extern Void ne_notconstantlist PP((ne_nexrec **nex, ne_desc *desc,
				   na_strlist *which));
extern ne_nexrec *ne_makeerror PP((ne_errorkind err));
extern ne_datatype ne_exprtype PP((ne_nexrec *nex));
extern ne_nexrec *ne_typecast PP((ne_nexrec *nex, ne_datatype typ));
extern Void ne_evaluate PP((ne_nexrec *nex, ne_nexrec *res));
extern long ne_ievaluate PP((ne_nexrec *nex, ne_desc *desc));
extern double ne_revaluate PP((ne_nexrec *nex, ne_desc *desc));
extern Char *ne_sevaluate PP((Char *Result, ne_nexrec *nex, ne_desc *desc));
extern long ne_ievalexpr PP((Char *ex, ne_desc *desc));
extern double ne_revalexpr PP((Char *ex, ne_desc *desc));

extern Char *ne_sevalexpr PP((Char *Result, Char *ex, ne_desc *desc));
extern Void ne_makeintconst PP((na_strlist *sym, long i));
extern Void ne_makesrealconst PP((na_strlist *sym, double r));
extern Void ne_makerealconst PP((na_strlist *sym, double r));
extern Void ne_makestrconst PP((na_strlist *sym, Char *s));
extern Void ne_makeintvar PP((na_strlist *sym, long *i));
extern Void ne_makerealvar PP((na_strlist *sym, double *r));
extern Void ne_makerealxvar PP((na_strlist *sym, double *r));
extern Void ne_makestrvar PP((na_strlist *sym, Char *s));
extern Void ne_makestrptrvar PP((na_strlist *sym, Char **sp));
extern Void ne_makeintfunc PP((na_strlist *sym, ne_datatype t1,
			       ne_datatype t2, ne_datatype t3,
			       _PROCEDURE pr));
extern Void ne_makerealfunc PP((na_strlist *sym, ne_datatype t1,
				ne_datatype t2, ne_datatype t3,
				_PROCEDURE pr));
extern Void ne_makestrfunc PP((na_strlist *sym, ne_datatype t1,
			       ne_datatype t2, ne_datatype t3,
			       _PROCEDURE pr));
extern Void ne_makeintmfunc PP((na_strlist *sym, long nargs,
				ne_datatype *args, _PROCEDURE pr));
extern Void ne_makerealmfunc PP((na_strlist *sym, long nargs,
				 ne_datatype *args, _PROCEDURE pr));
extern Void ne_makestrmfunc PP((na_strlist *sym, long nargs,
				ne_datatype *args, _PROCEDURE pr));
extern Void ne_makeintsfunc PP((na_strlist *sym, long nargs,
				ne_datatype *args, ne_nexrec **nex));
extern Void ne_makerealsfunc PP((na_strlist *sym, long nargs,
				 ne_datatype *args, ne_nexrec **nex));
extern Void ne_makestrsfunc PP((na_strlist *sym, long nargs,
				ne_datatype *args, ne_nexrec **nex));
extern Void ne_makeintarg PP((na_strlist *sym, long i));
extern Void ne_makerealarg PP((na_strlist *sym, long i));
extern Void ne_makestrarg PP((na_strlist *sym, long i));
extern Void ne_setminargs PP((na_strlist *sym, long minargs));
extern Void ne_setmaxargs PP((na_strlist *sym, long maxargs));
extern Void ne_makestaticfunc PP((na_strlist *sym));
extern Void ne_disposesym PP((na_strlist *sym));

extern Void ne_disposesymtab PP((na_strlist **symtab));
extern boolean ne_condeval PP((ne_nexrec *nex));
extern long ne_ieval PP((ne_nexrec *nex));
extern double ne_reval PP((ne_nexrec *nex));

extern Char *ne_seval PP((Char *Result, ne_nexrec *nex));





extern boolean ne_readreal PP((Char *s, double *r, ne_desc *desc));


#undef vextern

#endif /*NUMEX_H*/

/* End. */