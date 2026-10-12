#include "dn.h"

static int lm(const char *dir){
  int n=0,i,r=0;
  char **v;
  if(!isd(dir))return 0;
  v=ls(dir,&n);
  for(i=0;i<n&&!r;i++){
    const char *b=bn(v[i]);
    if(fin(b,".dll")&&(sta(b,"Assembly-CSharp")||sta(b,"mscorlib")))r=1;
  }
  fls(v,n);
  return r;
}

char *findm(const char *dir,int *il){
  int n=0,i;
  char **v;
  char *c,*r=NULL;
  *il=0;
  v=ls(dir,&n);
  for(i=0;i<n&&!r;i++){
    const char *b=bn(v[i]);
    if(!isd(v[i]))continue;
    if(fin(b,"_Data")){
      char *x=pj(v[i],"il2cpp_data");
      if(isd(x))*il=1;
      free(x);
      c=pj(v[i],"Managed");
      if(lm(c))r=c;else free(c);
    }else if(fin(b,".app")){
      char *t=pj(v[i],"Contents/Resources/Data/Managed");
      if(lm(t))r=t;else free(t);
    }
  }
  fls(v,n);
  if(r){*il=0;return r;}

  c=pj(dir,"Managed");
  if(lm(c))return c;
  free(c);
  c=pj(dir,"Contents/Resources/Data/Managed");
  if(lm(c))return c;
  free(c);
  if(lm(dir))return sdup(dir);
  return NULL;
}

int want(const char *b){
  char t[512];
  char *k;
  if(!fin(b,".dll"))return 0;
  if(G.all)return 1;
  if(sta(b,"Assembly-CSharp"))return 1;
  if(G.inc){
    snprintf(t,sizeof t,"%s",G.inc);
    for(k=strtok(t,",");k;k=strtok(NULL,","))
      if(*k&&has(b,k))return 1;
  }
  return 0;
}

static void norm(char *buf){
  char *p;
  size_t l;
  for(p=buf;*p;p++)if(*p=='\\')*p='/';
  l=strlen(buf);
  while(l>1&&buf[l-1]=='/')buf[--l]=0;
}

void gname(const char *in,char *out,size_t cap){
  char buf[1024],*p,*q;
  snprintf(buf,sizeof buf,"%s",in);
  norm(buf);
  for(;;){
    q=strrchr(buf,'/');
    p=q?q+1:buf;
    if(fin(p,"_Data")){
      size_t k=strlen(p)-5;
      snprintf(out,cap,"%.*s",(int)k,p);
      return;
    }
    if(!q)break;
    *q=0;
  }

  snprintf(buf,sizeof buf,"%s",in);
  norm(buf);
  p=(char *)bn(buf);
  if(fin(p,".dll")){
    q=strrchr(buf,'/');
    if(q){*q=0;p=(char *)bn(buf);}
  }
  snprintf(out,cap,"%s",p);
}
