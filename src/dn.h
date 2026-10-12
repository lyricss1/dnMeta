#ifndef DN_H
#define DN_H
#define _CRT_SECURE_NO_WARNINGS
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

enum{
  T_Mod,T_Ref,T_Def,T_FPtr,T_Fld,T_MPtr,T_Met,T_PPtr,T_Par,T_Ifc,T_MRef,T_Con,T_CA,T_FMar,
  T_DSec,T_CLay,T_FLay,T_SSig,T_EMap,T_EPtr,T_Evt,T_PMap,T_PrPtr,T_Prp,T_Sem,T_MImp,T_MoRef,
  T_Spec,T_IMap,T_FRva,T_EncL,T_EncM,T_Asm,T_AsmP,T_AsmO,T_ARef,T_ARefP,T_ARefO,T_File,T_Exp,
  T_Man,T_Nest,T_GP,T_MSpec,T_GPC,T_N
};

enum{CI_TDR,CI_HC,CI_HCA,CI_HFM,CI_HDS,CI_MRP,CI_HS,CI_MDR,CI_MF,CI_Imp,CI_CAT,CI_RS,CI_TMD,CI_N};
enum{KD_CLASS,KD_IFACE,KD_ENUM,KD_STRUCT,KD_DELEG};

#define MAXC 10

typedef struct{char *s;size_t n,cap;}Sb;
typedef struct{const u8 *p,*e;}Sg;
typedef struct{u32 cc,ng,np;char *ret,**pt;}Ms;
typedef struct{u32 t,m;}Cx;
typedef struct{u32 va,vs,raw,rsz;}Sc;

typedef struct{
  const u8 *f;size_t fs;
  const u8 *md;size_t mds;
  const u8 *str,*blob,*tbl;
  size_t strs,blobs,tbls;
  int ws,wg,wb;
  u32 rows[64];
  size_t toff[T_N];
  u32 rsz[T_N];
  u8 cw[T_N][MAXC],co[T_N][MAXC];
  char ver[64];
}Mt;

typedef struct{
  u32 *encl,*ifh,*ifn,*gth,*gmh,*gnx;
  u32 *cah[11],*can;
  u32 *cof;
  u8 *mh,*show;
  u32 *pget,*pset,*eadd,*erem;
  u32 *tps,*tpe,*tes,*tee;
  u32 *mown,*fch,*nsb;
}Ix;

typedef struct{
  FILE *log,*h;
  int quiet,ascii,pub,gen,tok,attr,tattr,sns,tonly,tree,all,first;
  const char *flt,*inc;
}Op;

typedef struct{char name[128];char ver[40];u32 types;}Ai;

extern Op G;
extern Mt M;
extern Ix X;
extern const char *BL,*BM,*BB,*BS;
extern const char UI_A[],UI_B[];

void pr(const char *f,...);
void sbi(Sb *b);
void sbn(Sb *b,const char *t,size_t l);
void sba(Sb *b,const char *t);
void sbc(Sb *b,char c);
void sbf(Sb *b,const char *f,...);
char *sdup(const char *s);
int has(const char *h,const char *n);
int sta(const char *s,const char *p);
int fin(const char *s,const char *suf);
u16 r16(const u8 *p);
u32 r32(const u8 *p);
u64 r64(const u8 *p);
u8 *rd(const char *path,size_t *n);
int isd(const char *p);
void mkd(const char *p);
const char *bn(const char *p);
char *pj(const char *a,const char *b);
char **ls(const char *dir,int *cnt);
void fls(char **v,int n);

char *findm(const char *dir,int *il);
int want(const char *b);
void gname(const char *in,char *out,size_t cap);

const char *load(const u8 *f,size_t n);
u32 cl(int t,u32 row,int col);
const char *S(u32 off);
void cdec(int ci,u32 v,int *t,u32 *row);
int su(Sg *s,u32 *v);
Sg bsig(u32 off);
u32 rs(int pt,u32 i);
u32 lend(int pt,int tb,u32 trow,int col);
void bidx(void);
void fidx(void);

void tdef(u32 row,Sb *b,int d);
void tref(u32 row,Sb *b,int d);
void tdr(u32 coded,Sb *b,const Cx *c);
int pms(Sg *s,const Cx *c,Ms *m,int d);
void fms(Ms *m);
void ptype(Sg *s,Sb *b,const Cx *c,int d);
void addn(Sb *b,const char *n);
const char *acc(u32 a);
int apub(u32 a);
void mmod(u32 fl,int ifc,int vis,Sb *b);
void cval(u32 frow,Sb *b);
const char *anm(u32 v);
int skipat(const char *n);
void attrs(int tag,u32 row,Sb *b);
void gps(u32 head,Sb *b);
int isgen(const char *n);
int bis(u32 coded,const char *full);
int tkind(u32 row);
int tvis(u32 row);
int cshow(u32 r,int d);
int fvis(u32 f);
int mvis(u32 m);
u32 pacc(u32 p);
int pvis(u32 p);
int evis(u32 e);
void pnames(u32 m,Ms *ms,const char **nm,u32 *fl);

void tree(const char *path);

void jstr(FILE *f,const char *s);
u32 jall(int ai);

#endif
