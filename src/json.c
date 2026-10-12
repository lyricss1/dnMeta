#include "dn.h"

void jstr(FILE *f,const char *s){
  fputc('"',f);
  for(;*s;s++){
    u8 c=(u8)*s;
    switch(c){
      case '"':fputs("\\\"",f);break;
      case '\\':fputs("\\\\",f);break;
      case '<':fputs("\\u003c",f);break;
      case '\n':fputs("\\n",f);break;
      case '\r':fputs("\\r",f);break;
      case '\t':fputs("\\t",f);break;
      default:
        if(c<32)fprintf(f,"\\u%04x",c);else fputc(c,f);
    }
  }
  fputc('"',f);
}

static int jok(u32 row){
  int d=0;
  u32 r=row;
  while(r&&d++<16){
    if(!G.gen&&isgen(S(cl(T_Def,r,1))))return 0;
    r=X.encl[r];
  }
  return 1;
}

static void jat(int tag,u32 row){
  FILE *h=G.h;
  u32 r;
  int first=1;
  if(!X.cah[tag])return;
  for(r=X.cah[tag][row];r;r=X.can[r]){
    const char *nm=anm(cl(T_CA,r,1));
    size_t l;
    Sb b;
    if(!nm||!*nm||skipat(nm))continue;
    l=strlen(nm);
    if(l>9&&!strcmp(nm+l-9,"Attribute"))l-=9;
    fputs(first?",\"at\":[":",",h);
    first=0;
    sbi(&b);sbn(&b,nm,l);jstr(h,b.s);free(b.s);
  }
  if(!first)fputc(']',h);
}

static void jfld(u32 f,u32 trow){
  FILE *h=G.h;
  u32 fl=cl(T_Fld,f,0),first;
  Cx c={trow,0};
  Sg s=bsig(cl(T_Fld,f,2));
  Sb tb;
  sbi(&tb);
  if(su(&s,&first))ptype(&s,&tb,&c,0);else sba(&tb,"?");
  fputs("{\"n\":",h);jstr(h,S(cl(T_Fld,f,1)));
  fputs(",\"t\":",h);jstr(h,tb.s);
  fprintf(h,",\"a\":%u",fl&7);
  if(fl&0x10)fputs(",\"s\":1",h);
  if(fl&0x20)fputs(",\"r\":1",h);
  if(fl&0x40){
    Sb v;sbi(&v);cval(f,&v);
    fputs(",\"c\":",h);jstr(h,v.s);free(v.s);
  }
  jat(1,f);
  fputc('}',h);
  free(tb.s);
}

static void jprp(u32 p,u32 trow){
  FILE *h=G.h;
  Cx c={trow,0};
  Sg s=bsig(cl(T_Prp,p,2));
  u32 first=0,pc=0,i,am=X.pget[p]?X.pget[p]:X.pset[p];
  Sb tb;
  sbi(&tb);
  if(su(&s,&first)&&su(&s,&pc))ptype(&s,&tb,&c,0);else sba(&tb,"?");
  fputs("{\"n\":",h);jstr(h,S(cl(T_Prp,p,1)));
  fputs(",\"t\":",h);jstr(h,tb.s);
  fprintf(h,",\"a\":%u",pacc(p));
  if(am&&(cl(T_Met,am,2)&0x10))fputs(",\"s\":1",h);
  if(X.pget[p])fprintf(h,",\"g\":%u",cl(T_Met,X.pget[p],2)&7);
  if(X.pset[p])fprintf(h,",\"w\":%u",cl(T_Met,X.pset[p],2)&7);
  if(pc&&pc<64){
    fputs(",\"ix\":[",h);
    for(i=0;i<pc;i++){
      Sb pt;sbi(&pt);
      ptype(&s,&pt,&c,0);
      if(i)fputc(',',h);
      jstr(h,pt.s);free(pt.s);
    }
    fputc(']',h);
  }
  jat(9,p);
  fputc('}',h);
  free(tb.s);
}

static void jevt(u32 e,u32 trow){
  FILE *h=G.h;
  Cx c={trow,0};
  u32 am=X.eadd[e];
  Sb tb;
  sbi(&tb);
  tdr(cl(T_Evt,e,2),&tb,&c);
  fputs("{\"n\":",h);jstr(h,S(cl(T_Evt,e,1)));
  fputs(",\"t\":",h);jstr(h,tb.s);
  fprintf(h,",\"a\":%u",am?(cl(T_Met,am,2)&7):6);
  if(am&&(cl(T_Met,am,2)&0x10))fputs(",\"s\":1",h);
  jat(10,e);
  fputc('}',h);
  free(tb.s);
}

static void jgp(u32 head){
  FILE *h=G.h;
  int first=1;
  u32 r;
  fputs(",\"g\":\"",h);
  for(r=head;r;r=X.gnx[r]){
    if(!first)fputc(',',h);
    first=0;
    fputs(S(cl(T_GP,r,3)),h);
  }
  fputc('"',h);
}

static void jmet(u32 m,u32 trow,int ifc){
  FILE *h=G.h;
  Cx c={trow,m};
  Sg s=bsig(cl(T_Met,m,4));
  Ms ms;
  u32 fl=cl(T_Met,m,2),i;
  Sb mods;

  fputs("{\"n\":",h);jstr(h,S(cl(T_Met,m,3)));
  fprintf(h,",\"a\":%u",fl&7);
  if(fl&0x10)fputs(",\"s\":1",h);
  sbi(&mods);
  mmod(fl,ifc,0,&mods);
  if(mods.n){
    while(mods.n&&mods.s[mods.n-1]==' ')mods.s[--mods.n]=0;
    fputs(",\"m\":",h);jstr(h,mods.s);
  }
  free(mods.s);
  if(X.gmh[m])jgp(X.gmh[m]);

  if(pms(&s,&c,&ms,0)){
    const char **nm=calloc(ms.np+1,sizeof(char *));
    u32 *pf=calloc(ms.np+1,sizeof(u32));
    fputs(",\"r\":",h);jstr(h,ms.ret);
    pnames(m,&ms,nm,pf);
    fputs(",\"p\":[",h);
    for(i=0;i<ms.np;i++){
      Sb tb;
      const char *t=ms.pt[i];
      char an[32];
      sbi(&tb);
      if((pf[i]&2)&&!strncmp(t,"ref ",4)){sba(&tb,"out ");sba(&tb,t+4);}
      else sba(&tb,t);
      if(!nm[i]||!*nm[i])snprintf(an,sizeof an,"arg%u",i);
      if(i)fputc(',',h);
      fputc('[',h);jstr(h,tb.s);fputc(',',h);
      jstr(h,(nm[i]&&*nm[i])?nm[i]:an);
      fputc(']',h);
      free(tb.s);
    }
    fputc(']',h);
    free(nm);free(pf);
    fms(&ms);
  }else fputs(",\"r\":\"?\",\"p\":[]",h);
  jat(0,m);
  fputc('}',h);
}

static void jtyp(u32 row,int ai){
  FILE *h=G.h;
  u32 fl=cl(T_Def,row,0),ext=cl(T_Def,row,3),i,c,top=row;
  u32 fs=cl(T_Def,row,4),fe=lend(T_FPtr,T_Fld,row,4);
  u32 ms=cl(T_Def,row,5),me=lend(T_MPtr,T_Met,row,5);
  int kind=tkind(row),d=0,first;
  int save=G.sns;
  Cx cx={row,0};
  Sb full,disp,tmp;
  static const char *KN[]={"class","interface","enum","struct","delegate"};

  while(X.encl[top]&&d++<16)top=X.encl[top];
  G.sns=0;sbi(&full);tdef(row,&full,0);
  G.sns=1;sbi(&disp);tdef(row,&disp,0);

  if(!G.first)fputc(',',h);
  G.first=0;
  fprintf(h,"{\"a\":%d,\"ns\":",ai);
  jstr(h,S(cl(T_Def,top,2)));
  fputs(",\"n\":",h);jstr(h,disp.s);
  fputs(",\"f\":",h);jstr(h,full.s);
  fprintf(h,",\"k\":\"%s\",\"v\":%u",KN[kind],fl&7);
  if(kind==KD_CLASS){
    const char *x=((fl&0x80)&&(fl&0x100))?"static":(fl&0x80)?"abstract":(fl&0x100)?"sealed":NULL;
    if(x)fprintf(h,",\"x\":\"%s\"",x);
  }
  if(X.gth[row])jgp(X.gth[row]);

  G.sns=0;
  if(kind==KD_CLASS&&ext&&!bis(ext,"System.Object")){
    sbi(&tmp);tdr(ext,&tmp,&cx);
    fputs(",\"b\":",h);jstr(h,tmp.s);free(tmp.s);
  }
  if(X.ifh[row]){
    fputs(",\"if\":[",h);
    first=1;
    for(c=X.ifh[row];c;c=X.ifn[c]){
      sbi(&tmp);tdr(cl(T_Ifc,c,1),&tmp,&cx);
      if(!first)fputc(',',h);
      first=0;
      jstr(h,tmp.s);free(tmp.s);
    }
    fputc(']',h);
  }
  if(kind==KD_ENUM){
    for(i=fs;i<fe;i++){
      u32 f=rs(T_FPtr,i);
      if(!(cl(T_Fld,f,0)&0x10)){
        Sg s=bsig(cl(T_Fld,f,2));
        u32 f0;
        sbi(&tmp);
        if(su(&s,&f0))ptype(&s,&tmp,&cx,0);
        fputs(",\"u\":",h);jstr(h,tmp.s);free(tmp.s);
        break;
      }
    }
  }
  jat(3,row);

  fputs(",\"F\":[",h);first=1;
  for(i=fs;i<fe;i++){
    u32 f=rs(T_FPtr,i);
    if(!f||!fvis(f))continue;
    if(!first)fputc(',',h);
    first=0;
    jfld(f,row);
  }
  fputs("],\"P\":[",h);first=1;
  for(i=X.tps[row];i<X.tpe[row];i++){
    u32 p=rs(T_PrPtr,i);
    if(!p||p>M.rows[T_Prp])continue;
    if(!G.gen&&isgen(S(cl(T_Prp,p,1))))continue;
    if(!first)fputc(',',h);
    first=0;
    jprp(p,row);
  }
  fputs("],\"E\":[",h);first=1;
  for(i=X.tes[row];i<X.tee[row];i++){
    u32 e=rs(T_EPtr,i);
    if(!e||e>M.rows[T_Evt])continue;
    if(!G.gen&&isgen(S(cl(T_Evt,e,1))))continue;
    if(!first)fputc(',',h);
    first=0;
    jevt(e,row);
  }
  fputs("],\"M\":[",h);first=1;
  for(i=ms;i<me;i++){
    u32 m=rs(T_MPtr,i);
    const char *mn;
    if(!m||m>M.rows[T_Met]||X.mh[m])continue;
    mn=S(cl(T_Met,m,3));
    if(mn[0]=='.')continue;
    if(!G.gen&&isgen(mn))continue;
    if(!first)fputc(',',h);
    first=0;
    jmet(m,row,kind==KD_IFACE);
  }
  fputs("]}",h);
  free(full.s);free(disp.s);
  G.sns=save;
}

u32 jall(int ai){
  u32 r,n=0,nt=M.rows[T_Def];
  int save=G.sns,sp=G.pub;
  G.sns=0;G.pub=0;
  for(r=1;r<=nt;r++){
    if(!jok(r))continue;
    jtyp(r,ai);
    n++;
  }
  G.sns=save;G.pub=sp;
  return n;
}
