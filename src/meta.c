#include "dn.h"

Mt M;
Ix X;

static Sc sec[96];
static int nsec;

enum{K1=1,K2,K4,KS,KG,KB};
#define KI(t) (0x100+(t))
#define KC(c) (0x200+(c))

static const struct{int bits,n,tb[24];}CODED[CI_N]={
  {2,3,{T_Def,T_Ref,T_Spec}},
  {2,3,{T_Fld,T_Par,T_Prp}},
  {5,22,{T_Met,T_Fld,T_Ref,T_Def,T_Par,T_Ifc,T_MRef,T_Mod,T_DSec,T_Prp,T_Evt,T_SSig,T_MoRef,
         T_Spec,T_Asm,T_ARef,T_File,T_Exp,T_Man,T_GP,T_GPC,T_MSpec}},
  {1,2,{T_Fld,T_Par}},
  {2,3,{T_Def,T_Met,T_Asm}},
  {3,5,{T_Def,T_Ref,T_MoRef,T_Met,T_Spec}},
  {1,2,{T_Evt,T_Prp}},
  {1,2,{T_Met,T_MRef}},
  {1,2,{T_Fld,T_Met}},
  {2,3,{T_File,T_ARef,T_Exp}},
  {3,5,{-1,-1,T_Met,T_MRef,-1}},
  {2,4,{T_Mod,T_MoRef,T_ARef,T_Ref}},
  {1,2,{T_Def,T_Met}},
};

static const int SCH[T_N][MAXC]={
  {K2,KS,KG,KG,KG,0},
  {KC(CI_RS),KS,KS,0},
  {K4,KS,KS,KC(CI_TDR),KI(T_Fld),KI(T_Met),0},
  {KI(T_Fld),0},
  {K2,KS,KB,0},
  {KI(T_Met),0},
  {K4,K2,K2,KS,KB,KI(T_Par),0},
  {KI(T_Par),0},
  {K2,K2,KS,0},
  {KI(T_Def),KC(CI_TDR),0},
  {KC(CI_MRP),KS,KB,0},
  {K1,K1,KC(CI_HC),KB,0},
  {KC(CI_HCA),KC(CI_CAT),KB,0},
  {KC(CI_HFM),KB,0},
  {K2,KC(CI_HDS),KB,0},
  {K2,K4,KI(T_Def),0},
  {K4,KI(T_Fld),0},
  {KB,0},
  {KI(T_Def),KI(T_Evt),0},
  {KI(T_Evt),0},
  {K2,KS,KC(CI_TDR),0},
  {KI(T_Def),KI(T_Prp),0},
  {KI(T_Prp),0},
  {K2,KS,KB,0},
  {K2,KI(T_Met),KC(CI_HS),0},
  {KI(T_Def),KC(CI_MDR),KC(CI_MDR),0},
  {KS,0},
  {KB,0},
  {K2,KC(CI_MF),KS,KI(T_MoRef),0},
  {K4,KI(T_Fld),0},
  {K4,K4,0},
  {K4,0},
  {K4,K2,K2,K2,K2,K4,KB,KS,KS,0},
  {K4,0},
  {K4,K4,K4,0},
  {K2,K2,K2,K2,K4,KB,KS,KS,KB,0},
  {K4,KI(T_ARef),0},
  {K4,K4,K4,KI(T_ARef),0},
  {K4,KS,KB,0},
  {K4,K4,KS,KS,KC(CI_Imp),0},
  {K4,K4,KS,KC(CI_Imp),0},
  {KI(T_Def),KI(T_Def),0},
  {K2,K2,KC(CI_TMD),KS,0},
  {KC(CI_MDR),KB,0},
  {KI(T_GP),KC(CI_TDR),0},
};

static int r2o(u32 rva,size_t *off){
  int i;
  for(i=0;i<nsec;i++){
    u32 sz=sec[i].vs>sec[i].rsz?sec[i].vs:sec[i].rsz;
    if(rva>=sec[i].va&&rva-sec[i].va<sz){
      *off=(size_t)sec[i].raw+(rva-sec[i].va);
      return *off<M.fs;
    }
  }
  return 0;
}

static int csz(int ci){
  u32 max=0;
  int i;
  for(i=0;i<CODED[ci].n;i++){
    int t=CODED[ci].tb[i];
    if(t>=0&&M.rows[t]>max)max=M.rows[t];
  }
  return max<(1u<<(16-CODED[ci].bits))?2:4;
}

static int colsz(int k){
  if(k>=0x200)return csz(k-0x200);
  if(k>=0x100)return M.rows[k-0x100]<65536?2:4;
  switch(k){
    case K1:return 1;
    case K2:return 2;
    case K4:return 4;
    case KS:return M.ws;
    case KG:return M.wg;
    case KB:return M.wb;
  }
  return 4;
}

const char *load(const u8 *f,size_t n){
  size_t pe,opt,dd,sc,off,p;
  u16 magic,optsz,nstr;
  u32 cli,mdr,mds,vl,hs;
  u64 valid;
  const u8 *ts=NULL;
  size_t tss=0;
  int i,ri;

  memset(&M,0,sizeof M);
  M.f=f;M.fs=n;
  if(n<0x100||f[0]!='M'||f[1]!='Z')return "not a PE file";
  pe=r32(f+0x3C);
  if(pe+24>n||memcmp(f+pe,"PE\0\0",4))return "bad PE signature";
  nsec=r16(f+pe+4+2);
  optsz=r16(f+pe+4+16);
  opt=pe+4+20;
  if(opt+optsz>n)return "truncated optional header";
  magic=r16(f+opt);
  if(magic!=0x10b&&magic!=0x20b)return "bad optional header magic";
  dd=opt+(magic==0x20b?112:96);
  if(dd+15*8>opt+optsz)return "no data directories";
  cli=r32(f+dd+14*8);
  if(!cli)return "no CLR header (native DLL, not a managed assembly)";
  sc=opt+optsz;
  if(nsec>96||sc+(size_t)nsec*40>n)return "bad section table";
  for(i=0;i<nsec;i++){
    const u8 *s=f+sc+(size_t)i*40;
    sec[i].vs=r32(s+8);
    sec[i].va=r32(s+12);
    sec[i].rsz=r32(s+16);
    sec[i].raw=r32(s+20);
  }
  if(!r2o(cli,&off)||off+72>n)return "bad CLI header";
  mdr=r32(f+off+8);
  mds=r32(f+off+12);
  if(!r2o(mdr,&off))return "bad metadata RVA";
  if(off+mds>n)mds=(u32)(n-off);
  M.md=f+off;M.mds=mds;
  if(mds<24||r32(M.md)!=0x424A5342u)return "bad metadata signature (BSJB)";
  vl=r32(M.md+12);
  if(16+(size_t)vl+4>mds)return "bad metadata version string";
  for(i=0;i<63&&i<(int)vl&&M.md[16+i];i++)M.ver[i]=(char)M.md[16+i];
  nstr=r16(M.md+16+vl+2);
  p=20+(size_t)vl;

  for(i=0;i<nstr;i++){
    u32 so,ss;
    const char *nm;
    size_t nl=0;
    if(p+8>mds)return "bad stream header";
    so=r32(M.md+p);ss=r32(M.md+p+4);
    nm=(const char *)M.md+p+8;
    while(p+8+nl<mds&&nm[nl])nl++;
    p+=8+((nl+1+3)&~(size_t)3);
    if((size_t)so+ss>mds)return "stream out of range";
    if(!strcmp(nm,"#~")||!strcmp(nm,"#-")){ts=M.md+so;tss=ss;}
    else if(!strcmp(nm,"#Strings")){M.str=M.md+so;M.strs=ss;}
    else if(!strcmp(nm,"#Blob")){M.blob=M.md+so;M.blobs=ss;}
  }
  if(!ts||!M.str||!M.blob)return "missing metadata streams";
  if(tss<32)return "table stream too small";

  hs=ts[6];
  M.ws=(hs&1)?4:2;
  M.wg=(hs&2)?4:2;
  M.wb=(hs&4)?4:2;
  valid=r64(ts+8);
  p=24;
  for(i=0;i<64;i++){
    if(!((valid>>i)&1))continue;
    if(p+4>tss)return "table row counts truncated";
    M.rows[i]=r32(ts+p);
    p+=4;
    if(i>=T_N)return "unsupported metadata table (portable PDB?)";
  }
  if(hs&0x40)p+=4;
  M.tbl=ts+p;
  M.tbls=tss-p;

  {
    size_t acc=0;
    for(ri=0;ri<T_N;ri++){
      int c,o=0;
      M.toff[ri]=acc;
      for(c=0;c<MAXC&&SCH[ri][c];c++){
        int w=colsz(SCH[ri][c]);
        M.cw[ri][c]=(u8)w;
        M.co[ri][c]=(u8)o;
        o+=w;
      }
      M.rsz[ri]=(u32)o;
      acc+=(size_t)o*M.rows[ri];
      if(acc>M.tbls)return "tables exceed stream size";
    }
  }
  return NULL;
}

u32 cl(int t,u32 row,int col){
  const u8 *p;
  if(row==0||row>M.rows[t])return 0;
  p=M.tbl+M.toff[t]+(size_t)(row-1)*M.rsz[t]+M.co[t][col];
  switch(M.cw[t][col]){
    case 1:return p[0];
    case 2:return r16(p);
    default:return r32(p);
  }
}

const char *S(u32 off){
  if(off>=M.strs)return "";
  if(!memchr(M.str+off,0,M.strs-off))return "";
  return (const char *)M.str+off;
}

void cdec(int ci,u32 v,int *t,u32 *row){
  u32 tag=v&((1u<<CODED[ci].bits)-1);
  *row=v>>CODED[ci].bits;
  *t=tag<(u32)CODED[ci].n?CODED[ci].tb[tag]:-1;
}

int su(Sg *s,u32 *v){
  u8 b;
  if(s->p>=s->e)return 0;
  b=*s->p;
  if(!(b&0x80)){*v=b;s->p+=1;return 1;}
  if((b&0xC0)==0x80){
    if(s->e-s->p<2)return 0;
    *v=((u32)(b&0x3F)<<8)|s->p[1];
    s->p+=2;
    return 1;
  }
  if(s->e-s->p<4)return 0;
  *v=((u32)(b&0x1F)<<24)|((u32)s->p[1]<<16)|((u32)s->p[2]<<8)|s->p[3];
  s->p+=4;
  return 1;
}

Sg bsig(u32 off){
  Sg s={NULL,NULL},h;
  u32 len;
  if(off>=M.blobs)return s;
  h.p=M.blob+off;h.e=M.blob+M.blobs;
  if(!su(&h,&len))return s;
  if(len>(u32)(h.e-h.p))len=(u32)(h.e-h.p);
  s.p=h.p;s.e=h.p+len;
  return s;
}

u32 rs(int pt,u32 i){return M.rows[pt]?cl(pt,i,0):i;}

u32 lend(int pt,int tb,u32 trow,int col){
  if(trow<M.rows[T_Def])return cl(T_Def,trow+1,col);
  return (M.rows[pt]?M.rows[pt]:M.rows[tb])+1;
}

#define CA(n) ((u32 *)calloc((size_t)(n)+2,sizeof(u32)))

void fidx(void){
  u32 **pp[]={&X.encl,&X.ifh,&X.ifn,&X.gth,&X.gmh,&X.gnx,&X.can,&X.cof,&X.pget,&X.pset,
              &X.eadd,&X.erem,&X.tps,&X.tpe,&X.tes,&X.tee,&X.mown,&X.fch,&X.nsb};
  size_t i;
  for(i=0;i<sizeof pp/sizeof pp[0];i++){free(*pp[i]);*pp[i]=NULL;}
  for(i=0;i<11;i++){free(X.cah[i]);X.cah[i]=NULL;}
  free(X.mh);X.mh=NULL;
  free(X.show);X.show=NULL;
}

void bidx(void){
  u32 nt=M.rows[T_Def],nf=M.rows[T_Fld],nm=M.rows[T_Met];
  u32 np=M.rows[T_Prp],ne=M.rows[T_Evt],r,i;
  static const int tags[]={0,1,3,9,10};
  u32 lim[11]={0};
  size_t k;

  X.encl=CA(nt);X.ifh=CA(nt);X.ifn=CA(M.rows[T_Ifc]);
  X.gth=CA(nt);X.gmh=CA(nm);X.gnx=CA(M.rows[T_GP]);
  X.cof=CA(nf);
  X.mh=calloc(nm+2,1);
  X.show=calloc(nt+2,1);
  X.pget=CA(np);X.pset=CA(np);X.eadd=CA(ne);X.erem=CA(ne);
  X.tps=CA(nt);X.tpe=CA(nt);X.tes=CA(nt);X.tee=CA(nt);
  X.mown=CA(nm);X.fch=CA(nt);X.nsb=CA(nt);
  X.can=CA(M.rows[T_CA]);

  for(r=1;r<=M.rows[T_Nest];r++){
    u32 n=cl(T_Nest,r,0),e=cl(T_Nest,r,1);
    if(n&&n<=nt&&e<=nt)X.encl[n]=e;
  }
  for(r=M.rows[T_Ifc];r>=1;r--){
    u32 c=cl(T_Ifc,r,0);
    if(c&&c<=nt){X.ifn[r]=X.ifh[c];X.ifh[c]=r;}
  }
  for(r=M.rows[T_GP];r>=1;r--){
    u32 o=cl(T_GP,r,2),row=o>>1;
    if(!(o&1)&&row&&row<=nt){X.gnx[r]=X.gth[row];X.gth[row]=r;}
    else if((o&1)&&row&&row<=nm){X.gnx[r]=X.gmh[row];X.gmh[row]=r;}
  }
  for(r=1;r<=nt;r++){
    u32 s=cl(T_Def,r,5),e=lend(T_MPtr,T_Met,r,5);
    for(i=s;i<e;i++){
      u32 m=rs(T_MPtr,i);
      if(m&&m<=nm)X.mown[m]=r;
    }
  }
  for(r=1;r<=M.rows[T_Con];r++){
    int t;u32 row;
    cdec(CI_HC,cl(T_Con,r,2),&t,&row);
    if(t==T_Fld&&row&&row<=nf)X.cof[row]=r;
  }
  for(r=1;r<=M.rows[T_Sem];r++){
    u32 sem=cl(T_Sem,r,0),m=cl(T_Sem,r,1);
    int t;u32 row;
    cdec(CI_HS,cl(T_Sem,r,2),&t,&row);
    if(m&&m<=nm)X.mh[m]=1;
    if(t==T_Prp&&row&&row<=np){
      if(sem&2)X.pget[row]=m;
      if(sem&1)X.pset[row]=m;
    }else if(t==T_Evt&&row&&row<=ne){
      if(sem&8)X.eadd[row]=m;
      if(sem&16)X.erem[row]=m;
    }
  }
  for(r=1;r<=M.rows[T_PMap];r++){
    u32 par=cl(T_PMap,r,0),s=cl(T_PMap,r,1);
    u32 e=r<M.rows[T_PMap]?cl(T_PMap,r+1,1):(M.rows[T_PrPtr]?M.rows[T_PrPtr]:np)+1;
    if(par&&par<=nt){X.tps[par]=s;X.tpe[par]=e;}
  }
  for(r=1;r<=M.rows[T_EMap];r++){
    u32 par=cl(T_EMap,r,0),s=cl(T_EMap,r,1);
    u32 e=r<M.rows[T_EMap]?cl(T_EMap,r+1,1):(M.rows[T_EPtr]?M.rows[T_EPtr]:ne)+1;
    if(par&&par<=nt){X.tes[par]=s;X.tee[par]=e;}
  }

  lim[0]=nm;lim[1]=nf;lim[3]=nt;lim[9]=np;lim[10]=ne;
  if(G.attr){
    for(k=0;k<5;k++)X.cah[tags[k]]=CA(lim[tags[k]]);
    for(r=M.rows[T_CA];r>=1;r--){
      u32 par=cl(T_CA,r,0),tag=par&31,row=par>>5;
      if(tag<11&&X.cah[tag]&&row&&row<=lim[tag]){
        X.can[r]=X.cah[tag][row];
        X.cah[tag][row]=r;
      }
    }
  }
}
