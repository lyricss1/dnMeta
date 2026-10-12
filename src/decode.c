#include "dn.h"

void addn(Sb *b,const char *n){
  const char *t=strchr(n,'`');
  if(t)sbn(b,n,(size_t)(t-n));else sba(b,n);
}

void tdef(u32 row,Sb *b,int d){
  u32 e;
  if(!row||row>M.rows[T_Def]){sba(b,"?");return;}
  e=X.encl[row];
  if(e&&d<16){tdef(e,b,d+1);sbc(b,'.');}
  else{
    const char *ns=S(cl(T_Def,row,2));
    if(*ns&&!G.sns){sba(b,ns);sbc(b,'.');}
  }
  addn(b,S(cl(T_Def,row,1)));
}

void tref(u32 row,Sb *b,int d){
  int t;u32 r;
  if(!row||row>M.rows[T_Ref]){sba(b,"?");return;}
  cdec(CI_RS,cl(T_Ref,row,0),&t,&r);
  if(t==T_Ref&&d<16){tref(r,b,d+1);sbc(b,'.');}
  else{
    const char *ns=S(cl(T_Ref,row,2));
    if(*ns&&!G.sns){sba(b,ns);sbc(b,'.');}
  }
  addn(b,S(cl(T_Ref,row,1)));
}

static const char *gpn(int meth,u32 row,u32 num){
  u32 r;
  if(!row)return NULL;
  if(!meth){if(row>M.rows[T_Def])return NULL;r=X.gth[row];}
  else{if(row>M.rows[T_Met])return NULL;r=X.gmh[row];}
  for(;r;r=X.gnx[r])
    if(cl(T_GP,r,0)==num)return S(cl(T_GP,r,3));
  return NULL;
}

void tdr(u32 coded,Sb *b,const Cx *c){
  int t;u32 r;
  cdec(CI_TDR,coded,&t,&r);
  if(t==T_Def)tdef(r,b,0);
  else if(t==T_Ref)tref(r,b,0);
  else if(t==T_Spec){
    Sg s=bsig(cl(T_Spec,r,0));
    ptype(&s,b,c,1);
  }else sba(b,"?");
}

int pms(Sg *s,const Cx *c,Ms *m,int d){
  Sb b;
  u32 i;
  memset(m,0,sizeof *m);
  if(!su(s,&m->cc))return 0;
  if((m->cc&0x10)&&!su(s,&m->ng))return 0;
  if(!su(s,&m->np))return 0;
  if(m->np>(u32)(s->e-s->p))return 0;
  sbi(&b);
  ptype(s,&b,c,d+1);
  m->ret=b.s;
  m->pt=calloc(m->np+1,sizeof(char *));
  for(i=0;i<m->np;i++){
    if(s->p<s->e&&*s->p==0x41)s->p++;
    sbi(&b);
    ptype(s,&b,c,d+1);
    m->pt[i]=b.s;
  }
  return 1;
}

void fms(Ms *m){
  u32 i;
  free(m->ret);
  if(m->pt)for(i=0;i<m->np;i++)free(m->pt[i]);
  free(m->pt);
}

static const char *PRIM[0x1f]={
  NULL,"void","bool","char","sbyte","byte","short","ushort","int","uint","long",
  "ulong","float","double","string",NULL,NULL,NULL,NULL,NULL,NULL,NULL,"TypedReference",
  NULL,"IntPtr","UIntPtr",NULL,NULL,"object",NULL,NULL
};

void ptype(Sg *s,Sb *b,const Cx *c,int d){
  u32 et,v,i,n;
  if(d>24){sba(b,"...");return;}
  if(!su(s,&et)){sba(b,"?");return;}
  if(et<0x1f&&PRIM[et]){sba(b,PRIM[et]);return;}
  switch(et){
    case 0x0f:ptype(s,b,c,d+1);sbc(b,'*');break;
    case 0x10:sba(b,"ref ");ptype(s,b,c,d+1);break;
    case 0x11:case 0x12:
      if(su(s,&v))tdr(v,b,c);else sba(b,"?");
      break;
    case 0x13:case 0x1e:
      if(su(s,&v)){
        const char *nm=et==0x13?gpn(0,c?c->t:0,v):gpn(1,c?c->m:0,v);
        if(nm)sba(b,nm);else sbf(b,"%s%u",et==0x13?"!":"!!",v);
      }
      break;
    case 0x14:{
      u32 rank=0,ns=0,nl=0,tmp;
      ptype(s,b,c,d+1);
      su(s,&rank);
      if(su(s,&ns))for(i=0;i<ns;i++)su(s,&tmp);
      if(su(s,&nl))for(i=0;i<nl;i++)su(s,&tmp);
      sbc(b,'[');
      for(i=1;i<rank;i++)sbc(b,',');
      sbc(b,']');
      break;
    }
    case 0x15:
      su(s,&v);
      if(su(s,&v))tdr(v,b,c);
      sbc(b,'<');
      if(su(s,&n)){
        for(i=0;i<n;i++){
          if(i)sba(b,", ");
          ptype(s,b,c,d+1);
        }
      }
      sbc(b,'>');
      break;
    case 0x1d:ptype(s,b,c,d+1);sba(b,"[]");break;
    case 0x1b:{
      Ms ms;
      if(pms(s,c,&ms,d)){
        sba(b,"delegate*<");
        for(i=0;i<ms.np;i++){sba(b,ms.pt[i]);sba(b,", ");}
        sba(b,ms.ret);
        sbc(b,'>');
        fms(&ms);
      }else sba(b,"delegate*");
      break;
    }
    case 0x1f:case 0x20:su(s,&v);ptype(s,b,c,d+1);break;
    case 0x41:case 0x45:ptype(s,b,c,d+1);break;
    default:sbf(b,"?0x%x",et);
  }
}

const char *acc(u32 a){
  switch(a){
    case 6:return "public";
    case 4:return "protected";
    case 5:return "protected internal";
    case 3:return "internal";
    case 2:return "private protected";
    default:return "private";
  }
}

int apub(u32 a){return a==6||a==4||a==5;}

void mmod(u32 fl,int ifc,int vis,Sb *b){
  if(vis){sba(b,acc(fl&7));sbc(b,' ');}
  if(fl&0x10)sba(b,"static ");
  if(fl&0x400){if(!ifc)sba(b,"abstract ");}
  else if(fl&0x40){
    if(!(fl&0x100))sba(b,(fl&0x20)?"sealed override ":"override ");
    else if(!(fl&0x20)&&!ifc)sba(b,"virtual ");
  }
  if(fl&0x2000)sba(b,"extern ");
}

static void u16s(Sb *b,const u8 *p,size_t nb){
  size_t i,sh=0;
  sbc(b,'"');
  for(i=0;i+1<nb&&sh<80;i+=2,sh++){
    u16 ch=r16(p+i);
    if(ch=='"'||ch=='\\'){sbc(b,'\\');sbc(b,(char)ch);}
    else if(ch=='\n')sba(b,"\\n");
    else if(ch=='\r')sba(b,"\\r");
    else if(ch=='\t')sba(b,"\\t");
    else if(ch>=32&&ch<127)sbc(b,(char)ch);
    else if(ch<0x800&&ch>=0x80){
      sbc(b,(char)(0xC0|(ch>>6)));
      sbc(b,(char)(0x80|(ch&0x3F)));
    }else if(ch>=0x800){
      sbc(b,(char)(0xE0|(ch>>12)));
      sbc(b,(char)(0x80|((ch>>6)&0x3F)));
      sbc(b,(char)(0x80|(ch&0x3F)));
    }else sbf(b,"\\u%04x",ch);
  }
  if(i+1<nb)sba(b,"...");
  sbc(b,'"');
}

void cval(u32 frow,Sb *b){
  u32 r,t;
  Sg s;
  size_t n;
  const u8 *p;
  if(!frow||frow>M.rows[T_Fld])return;
  r=X.cof[frow];
  if(!r)return;
  t=cl(T_Con,r,0);
  s=bsig(cl(T_Con,r,3));
  n=(size_t)(s.e-s.p);
  p=s.p;
  switch(t){
    case 2:if(n>=1)sba(b,p[0]?"true":"false");break;
    case 3:if(n>=2)sbf(b,"'\\u%04x'",r16(p));break;
    case 4:if(n>=1)sbf(b,"%d",(int)(int8_t)p[0]);break;
    case 5:if(n>=1)sbf(b,"%u",p[0]);break;
    case 6:if(n>=2)sbf(b,"%d",(int)(int16_t)r16(p));break;
    case 7:if(n>=2)sbf(b,"%u",r16(p));break;
    case 8:if(n>=4)sbf(b,"%d",(int)(int32_t)r32(p));break;
    case 9:if(n>=4)sbf(b,"%u",r32(p));break;
    case 10:if(n>=8)sbf(b,"%lld",(long long)(int64_t)r64(p));break;
    case 11:if(n>=8)sbf(b,"%llu",(unsigned long long)r64(p));break;
    case 12:if(n>=4){float f;u32 u=r32(p);memcpy(&f,&u,4);sbf(b,"%gf",(double)f);}break;
    case 13:if(n>=8){double d;u64 u=r64(p);memcpy(&d,&u,8);sbf(b,"%g",d);}break;
    case 14:u16s(b,p,n);break;
    case 0x12:sba(b,"null");break;
  }
}

static const char *SKIP[]={
  "CompilerGeneratedAttribute","DebuggerHiddenAttribute","DebuggerStepThroughAttribute",
  "DebuggerBrowsableAttribute","DebuggerNonUserCodeAttribute","IteratorStateMachineAttribute",
  "AsyncStateMachineAttribute","NullableContextAttribute","NullableAttribute",
  "EmbeddedAttribute","ExtensionAttribute","DefaultMemberAttribute",NULL
};

int skipat(const char *n){
  int i;
  for(i=0;SKIP[i];i++)if(!strcmp(n,SKIP[i]))return 1;
  return 0;
}

const char *anm(u32 v){
  u32 tag=v&7,row=v>>3;
  if(tag==2){
    u32 t=row<=M.rows[T_Met]?X.mown[row]:0;
    return t?S(cl(T_Def,t,1)):NULL;
  }
  if(tag==3){
    int pt;u32 pr;
    cdec(CI_MRP,cl(T_MRef,row,0),&pt,&pr);
    if(pt==T_Def)return S(cl(T_Def,pr,1));
    if(pt==T_Ref)return S(cl(T_Ref,pr,1));
  }
  return NULL;
}

void attrs(int tag,u32 row,Sb *b){
  u32 r;
  if(!G.tattr||!X.cah[tag])return;
  for(r=X.cah[tag][row];r;r=X.can[r]){
    const char *nm=anm(cl(T_CA,r,1));
    size_t l;
    if(!nm||!*nm||skipat(nm))continue;
    l=strlen(nm);
    sbc(b,'[');
    if(l>9&&!strcmp(nm+l-9,"Attribute"))sbn(b,nm,l-9);else sba(b,nm);
    sba(b,"] ");
  }
}

void gps(u32 head,Sb *b){
  int first=1;
  u32 r;
  if(!head)return;
  sbc(b,'<');
  for(r=head;r;r=X.gnx[r]){
    if(!first)sba(b,", ");
    first=0;
    sba(b,S(cl(T_GP,r,3)));
  }
  sbc(b,'>');
}

int isgen(const char *n){return strchr(n,'<')!=NULL;}

int bis(u32 coded,const char *full){
  int t,res,save=G.sns;
  u32 r;
  Sb b;
  cdec(CI_TDR,coded,&t,&r);
  if(t!=T_Ref)return 0;
  G.sns=0;
  sbi(&b);
  tref(r,&b,0);
  res=!strcmp(b.s,full);
  free(b.s);
  G.sns=save;
  return res;
}

int tkind(u32 row){
  u32 fl=cl(T_Def,row,0),ext=cl(T_Def,row,3);
  if(fl&0x20)return KD_IFACE;
  if(bis(ext,"System.Enum"))return KD_ENUM;
  if(bis(ext,"System.ValueType"))return KD_STRUCT;
  if(bis(ext,"System.MulticastDelegate"))return KD_DELEG;
  return KD_CLASS;
}

int tvis(u32 row){
  u32 fl=cl(T_Def,row,0),v=fl&7;
  if(!G.gen&&isgen(S(cl(T_Def,row,1))))return 0;
  if(G.pub){
    if(!X.encl[row])return v==1;
    return v==2||v==4||v==7;
  }
  return 1;
}

int cshow(u32 r,int d){
  int self,any=0;
  u32 c;
  if(!tvis(r)){X.show[r]=0;return 0;}
  self=1;
  if(G.flt){
    Sb b;sbi(&b);
    tdef(r,&b,0);
    self=has(b.s,G.flt);
    free(b.s);
  }
  for(c=X.fch[r];c;c=X.nsb[c])
    if(d<16&&cshow(c,d+1))any=1;
  X.show[r]=(u8)(self||any);
  return X.show[r];
}

int fvis(u32 f){
  u32 fl=cl(T_Fld,f,0);
  if(fl&0x400)return 0;
  if(!G.gen&&isgen(S(cl(T_Fld,f,1))))return 0;
  if(G.pub&&!apub(fl&7))return 0;
  return 1;
}

int mvis(u32 m){
  u32 fl=cl(T_Met,m,2);
  if(X.mh[m])return 0;
  if(!G.gen&&isgen(S(cl(T_Met,m,3))))return 0;
  if(G.pub&&!apub(fl&7))return 0;
  return 1;
}

u32 pacc(u32 p){
  u32 a=0,b=0;
  if(X.pget[p])a=cl(T_Met,X.pget[p],2)&7;
  if(X.pset[p])b=cl(T_Met,X.pset[p],2)&7;
  return a>b?a:b;
}

int pvis(u32 p){
  if(!G.gen&&isgen(S(cl(T_Prp,p,1))))return 0;
  if(G.pub&&!apub(pacc(p)))return 0;
  return 1;
}

int evis(u32 e){
  u32 m=X.eadd[e];
  if(!G.gen&&isgen(S(cl(T_Evt,e,1))))return 0;
  if(G.pub&&m&&!apub(cl(T_Met,m,2)&7))return 0;
  return 1;
}

void pnames(u32 m,Ms *ms,const char **nm,u32 *fl){
  u32 ps=cl(T_Met,m,5),pe,i;
  pe=m<M.rows[T_Met]?cl(T_Met,m+1,5):(M.rows[T_PPtr]?M.rows[T_PPtr]:M.rows[T_Par])+1;
  for(i=ps;i<pe&&ps;i++){
    u32 p=rs(T_PPtr,i),seq=cl(T_Par,p,1);
    if(seq>=1&&seq<=ms->np){
      nm[seq-1]=S(cl(T_Par,p,2));
      fl[seq-1]=cl(T_Par,p,0);
    }
  }
}
