#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *rd(const char *p,size_t *n){
  FILE *f=fopen(p,"rb");
  char *b;
  if(!f){fprintf(stderr,"cannot open %s\n",p);exit(1);}
  fseek(f,0,SEEK_END);*n=ftell(f);fseek(f,0,SEEK_SET);
  b=malloc(*n+1);
  if(fread(b,1,*n,f)!=*n){fprintf(stderr,"read error %s\n",p);exit(1);}
  b[*n]=0;
  fclose(f);
  return b;
}

static void arr(FILE *o,const char *name,const char *s,size_t n){
  size_t i;
  fprintf(o,"const char %s[]={",name);
  for(i=0;i<n;i++){
    if(i%24==0)fputs("\n",o);
    fprintf(o,"%u,",(unsigned char)s[i]);
  }
  fputs("0};\n\n",o);
}

static char *rep(const char *t,const char *k,const char *v){
  const char *p=strstr(t,k);
  char *r;
  size_t a;
  if(!p){fprintf(stderr,"marker %s not found\n",k);exit(1);}
  a=p-t;
  r=malloc(strlen(t)+strlen(v)+1);
  memcpy(r,t,a);
  strcpy(r+a,v);
  strcpy(r+a+strlen(v),p+strlen(k));
  return r;
}

int main(int argc,char **argv){
  const char *w=argc>1?argv[1]:"web",*out=argc>2?argv[2]:"src/ui.c";
  char p[512];
  size_t n,n2,n3;
  char *h,*css,*js,*t,*d;
  FILE *o;

  snprintf(p,sizeof p,"%s/ui.html",w);h=rd(p,&n);
  snprintf(p,sizeof p,"%s/ui.css",w);css=rd(p,&n2);
  snprintf(p,sizeof p,"%s/ui.js",w);js=rd(p,&n3);

  t=rep(h,"@CSS@",css);
  t=rep(t,"@JS@",js);
  d=strstr(t,"@DATA@");
  if(!d){fprintf(stderr,"marker @DATA@ not found\n");return 1;}

  o=fopen(out,"wb");
  if(!o){fprintf(stderr,"cannot write %s\n",out);return 1;}
  fputs("#include \"dn.h\"\n\n",o);
  arr(o,"UI_A",t,d-t);
  arr(o,"UI_B",d+6,strlen(d+6));
  fclose(o);
  return 0;
}
