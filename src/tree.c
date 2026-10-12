#include "dn.h"

const char *BL="\xE2\x94\x94\xE2\x94\x80\xE2\x94\x80 ";
const char *BM="\xE2\x94\x9C\xE2\x94\x80\xE2\x94\x80 ";
const char *BB="\xE2\x94\x82   ";
const char *BS="    ";

static char *pfx(const char *p,int last){
  const char *a=last?BS:BB;
  char *r=malloc(strlen(p)+strlen(a)+1);
  strcpy(r,p);strcat(r,a);
  return r;
}

static void ln(const char *p,int last,const char *t){
  pr("%s%s%s\n",p,last?BL:BM,t);
}

static void cv(Sb *b,u32 f){
  Sb v;sbi(&v);
  cval(f,&v);
  if(v.n){sba(b," = ");sba(b,v.s);}
  free(v.s);
}

static void pfld(u32 f,u32 trow,int en,const char *p,int last){
  Sb b;
  u32 fl=cl(T_Fld,f,0);
  Cx c={trow,0};
  Sg s;
  sbi(&b);
  attrs(1,f,&b);
  if(en){
    sba(&b,S(cl(T_Fld,f,1)));
    if(fl&0x40)cv(&b,f);
  }else{
    u32 first;
    sba(&b,acc(fl&7));sbc(&b,' ');
    if(fl&0x40)sba(&b,"const ");
    else{
      if(fl&0x10)sba(&b,"static ");
      if(fl&0x20)sba(&b,"readonly ");
    }
    s=bsig(cl(T_Fld,f,2));
    if(su(&s,&first))ptype(&s,&b,&c,0);else sba(&b,"?");
    sbc(&b,' ');
    sba(&b,S(cl(T_Fld,f,1)));
    if(fl&0x40)cv(&b,f);
  }
  if(G.tok)sbf(&b,"  // 0x%08X",0x04000000u|f);
  ln(p,last,b.s);
  free(b.s);
}

static void pprop(u32 p,u32 trow,const char *pf,int last){
  Sb b,tb;
  Cx c={trow,0};
  Sg s=bsig(cl(T_Prp,p,2));
  u32 first=0,pc=0,am=X.pget[p]?X.pget[p]:X.pset[p];
  u32 pa=pacc(p);
  sbi(&b);sbi(&tb);
  attrs(9,p,&b);
  sba(&b,acc(pa));sbc(&b,' ');
  if(am)mmod(cl(T_Met,am,2),0,0,&b);
  if(su(&s,&first)&&su(&s,&pc))ptype(&s,&tb,&c,0);else sba(&tb,"?");
  sba(&b,tb.s);sbc(&b,' ');
  if(pc){
    u32 i;
    sba(&b,"this[");
    for(i=0;i<pc;i++){
      Sb pt;sbi(&pt);
      ptype(&s,&pt,&c,0);
      if(i)sba(&b,", ");
      sba(&b,pt.s);
      free(pt.s);
    }
    sbc(&b,']');
  }else sba(&b,S(cl(T_Prp,p,1)));
  sba(&b," { ");
  if(X.pget[p]){
    u32 a=cl(T_Met,X.pget[p],2)&7;
    if(a!=pa){sba(&b,acc(a));sbc(&b,' ');}
    sba(&b,"get; ");
  }
  if(X.pset[p]){
    u32 a=cl(T_Met,X.pset[p],2)&7;
    if(a!=pa){sba(&b,acc(a));sbc(&b,' ');}
    sba(&b,"set; ");
  }
  sbc(&b,'}');
  ln(pf,last,b.s);
  free(b.s);free(tb.s);
}

static void pevt(u32 e,u32 trow,const char *p,int last){
  Sb b;
  Cx c={trow,0};
  u32 am=X.eadd[e];
  sbi(&b);
  attrs(10,e,&b);
  if(am)mmod(cl(T_Met,am,2),0,1,&b);else sba(&b,"public ");
  sba(&b,"event ");
  tdr(cl(T_Evt,e,2),&b,&c);
  sbc(&b,' ');
  sba(&b,S(cl(T_Evt,e,1)));
  ln(p,last,b.s);
  free(b.s);
}

static void pmet(u32 m,u32 trow,int ifc,const char *p,int last){
  Sb b;
  Cx c={trow,m};
  Sg s=bsig(cl(T_Met,m,4));
  Ms ms;
  u32 fl=cl(T_Met,m,2),i;
  const char **nm;
  u32 *pf;

  sbi(&b);
  attrs(0,m,&b);
  mmod(fl,ifc,1,&b);
  if(!pms(&s,&c,&ms,0)){
    sba(&b,"? ");
    sba(&b,S(cl(T_Met,m,3)));
    sba(&b,"(?)");
  }else{
    sba(&b,ms.ret);sbc(&b,' ');
    sba(&b,S(cl(T_Met,m,3)));
    gps(X.gmh[m],&b);
    sbc(&b,'(');
    nm=calloc(ms.np+1,sizeof(char *));
    pf=calloc(ms.np+1,sizeof(u32));
    pnames(m,&ms,nm,pf);
    for(i=0;i<ms.np;i++){
      const char *t=ms.pt[i];
      if(i)sba(&b,", ");
      if((pf[i]&2)&&!strncmp(t,"ref ",4)){sba(&b,"out ");sba(&b,t+4);}
      else sba(&b,t);
      sbc(&b,' ');
      if(nm[i]&&*nm[i])sba(&b,nm[i]);else sbf(&b,"arg%u",i);
    }
    sbc(&b,')');
    free(nm);free(pf);
    fms(&ms);
  }
  if(G.tok)sbf(&b,"  // 0x%08X RVA=0x%X",0x06000000u|m,cl(T_Met,m,0));
  ln(p,last,b.s);
  free(b.s);
}

static void ptyp(u32 row,const char *p,int last){
  Sb b;
  u32 fl=cl(T_Def,row,0),ext=cl(T_Def,row,3);
  u32 v=fl&7,i,c;
  int kind=tkind(row),nest=X.encl[row]!=0,ifc;
  Cx cx={row,0};
  int ng=0,gi=0,en=kind==KD_ENUM,bc=0;
  u32 nF=0,nP=0,nE=0,nM=0,nN=0;
  u32 fs,fe,ms,me;
  char *cp;

  sbi(&b);
  attrs(3,row,&b);
  if(!nest)sba(&b,v==1?"public ":"internal ");
  else{
    static const char *nv[]={"internal ","public ","public ","private ","protected ",
                             "internal ","private protected ","protected internal "};
    sba(&b,nv[v]);
  }
  switch(kind){
    case KD_IFACE:sba(&b,"interface ");break;
    case KD_ENUM:sba(&b,"enum ");break;
    case KD_STRUCT:sba(&b,"struct ");break;
    case KD_DELEG:sba(&b,"delegate ");break;
    default:
      if((fl&0x80)&&(fl&0x100))sba(&b,"static class ");
      else if(fl&0x80)sba(&b,"abstract class ");
      else if(fl&0x100)sba(&b,"sealed class ");
      else sba(&b,"class ");
  }
  addn(&b,S(cl(T_Def,row,1)));
  gps(X.gth[row],&b);

  fs=cl(T_Def,row,4);
  fe=lend(T_FPtr,T_Fld,row,4);
  if(en){
    for(i=fs;i<fe;i++){
      u32 f=rs(T_FPtr,i);
      if(!(cl(T_Fld,f,0)&0x10)){
        Sg s=bsig(cl(T_Fld,f,2));
        u32 first;
        sba(&b," : ");
        if(su(&s,&first))ptype(&s,&b,&cx,0);
        break;
      }
    }
  }else{
    int skip=kind==KD_STRUCT||kind==KD_DELEG||kind==KD_IFACE||bis(ext,"System.Object")||ext==0;
    if(!skip){sba(&b," : ");tdr(ext,&b,&cx);bc++;}
    for(c=X.ifh[row];c;c=X.ifn[c]){
      sba(&b,bc?", ":" : ");
      tdr(cl(T_Ifc,c,1),&b,&cx);
      bc++;
    }
  }
  if(G.tok)sbf(&b,"  // 0x%08X",0x02000000u|row);
  ln(p,last,b.s);
  free(b.s);

  if(G.tonly){
    cp=pfx(p,last);
    for(c=X.fch[row],nN=0;c;c=X.nsb[c])if(X.show[c])nN++;
    for(c=X.fch[row],i=0;c;c=X.nsb[c])
      if(X.show[c])ptyp(c,cp,++i==nN);
    free(cp);
    return;
  }

  for(i=fs;i<fe;i++)if(fvis(rs(T_FPtr,i)))nF++;
  for(i=X.tps[row];i<X.tpe[row];i++){u32 q=rs(T_PrPtr,i);if(q&&pvis(q))nP++;}
  for(i=X.tes[row];i<X.tee[row];i++){u32 q=rs(T_EPtr,i);if(q&&evis(q))nE++;}
  ms=cl(T_Def,row,5);
  me=lend(T_MPtr,T_Met,row,5);
  for(i=ms;i<me;i++)if(mvis(rs(T_MPtr,i)))nM++;
  for(c=X.fch[row];c;c=X.nsb[c])if(X.show[c])nN++;
  ng=(nF>0)+(nP>0)+(nE>0)+(nM>0)+(nN>0);
  if(!ng)return;
  cp=pfx(p,last);
  ifc=kind==KD_IFACE;

#define GB(label,cnt) {char hd[64];char *gp;u32 j=0;(void)j;\
  snprintf(hd,sizeof hd,"%s (%u)",label,(unsigned)(cnt));\
  ln(cp,++gi==ng,hd);\
  gp=pfx(cp,gi==ng);
#define GE free(gp);}

  if(nF){
    GB(en?"Values":"Fields",nF)
    for(i=fs;i<fe;i++){
      u32 f=rs(T_FPtr,i);
      if(fvis(f))pfld(f,row,en,gp,++j==nF);
    }
    GE
  }
  if(nP){
    GB("Properties",nP)
    for(i=X.tps[row];i<X.tpe[row];i++){
      u32 q=rs(T_PrPtr,i);
      if(q&&pvis(q))pprop(q,row,gp,++j==nP);
    }
    GE
  }
  if(nE){
    GB("Events",nE)
    for(i=X.tes[row];i<X.tee[row];i++){
      u32 q=rs(T_EPtr,i);
      if(q&&evis(q))pevt(q,row,gp,++j==nE);
    }
    GE
  }
  if(nM){
    GB("Methods",nM)
    for(i=ms;i<me;i++){
      u32 m=rs(T_MPtr,i);
      if(mvis(m))pmet(m,row,ifc,gp,++j==nM);
    }
    GE
  }
  if(nN){
    GB("Nested types",nN)
    for(c=X.fch[row];c;c=X.nsb[c])
      if(X.show[c])ptyp(c,gp,++j==nN);
    GE
  }
  free(cp);
}

static int cmpt(const void *a,const void *b){
  u32 x=*(const u32 *)a,y=*(const u32 *)b;
  int c=strcmp(S(cl(T_Def,x,2)),S(cl(T_Def,y,2)));
  if(c)return c;
  c=strcmp(S(cl(T_Def,x,1)),S(cl(T_Def,y,1)));
  if(c)return c;
  return x<y?-1:x>y;
}

void tree(const char *path){
  u32 nt=M.rows[T_Def],i,r,nto=0;
  u32 *ord=malloc((nt+1)*sizeof(u32));
  u32 *top=malloc((nt+1)*sizeof(u32));
  const char *an=M.rows[T_Asm]?S(cl(T_Asm,1,7)):S(cl(T_Mod,1,1));

  pr("# File:     %s\n",path);
  if(M.rows[T_Asm])
    pr("# Assembly: %s, Version=%u.%u.%u.%u\n",an,cl(T_Asm,1,1),cl(T_Asm,1,2),cl(T_Asm,1,3),cl(T_Asm,1,4));
  pr("# Runtime:  %s\n",M.ver);
  pr("# Counts:   types=%u fields=%u methods=%u properties=%u events=%u\n",nt,M.rows[T_Fld],
     M.rows[T_Met],M.rows[T_Prp],M.rows[T_Evt]);
  pr("# References:");
  for(r=1;r<=M.rows[T_ARef];r++)
    pr(" %s(%u.%u)",S(cl(T_ARef,r,6)),cl(T_ARef,r,0),cl(T_ARef,r,1));
  pr("\n\n");

  for(i=0;i<nt;i++)ord[i]=i+1;
  qsort(ord,nt,sizeof(u32),cmpt);
  for(i=nt;i-->0;){
    u32 rr=ord[i],e=X.encl[rr];
    if(e){X.nsb[rr]=X.fch[e];X.fch[e]=rr;}
  }
  for(i=0;i<nt;i++){
    u32 rr=ord[i];
    if(!X.encl[rr]&&cshow(rr,0))top[nto++]=rr;
  }

  pr("%s\n",an);
  {
    u32 a=0,ngr=0;
    for(i=0;i<nto;i++)
      if(i==0||strcmp(S(cl(T_Def,top[i],2)),S(cl(T_Def,top[i-1],2))))ngr++;
    i=0;
    while(i<nto){
      const char *ns=S(cl(T_Def,top[i],2));
      u32 j=i,cnt;
      char hd[512];
      char *cp;
      int last;
      while(j<nto&&!strcmp(S(cl(T_Def,top[j],2)),ns))j++;
      cnt=j-i;
      a++;
      last=a==ngr;
      snprintf(hd,sizeof hd,"%s%s (%u types)",*ns?"namespace ":"",*ns?ns:"<global namespace>",cnt);
      ln("",last,hd);
      cp=pfx("",last);
      for(r=i;r<j;r++)ptyp(top[r],cp,r+1==j);
      free(cp);
      i=j;
    }
  }
  pr("\n");
  free(ord);free(top);
}
