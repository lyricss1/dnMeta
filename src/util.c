#include "dn.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

Op G={.tree=1};

void pr(const char *f,...){
  va_list a;
  if(G.log){va_start(a,f);vfprintf(G.log,f,a);va_end(a);}
  if(!G.quiet){va_start(a,f);vfprintf(stdout,f,a);va_end(a);}
}

void sbi(Sb *b){b->cap=128;b->n=0;b->s=malloc(b->cap);b->s[0]=0;}

void sbn(Sb *b,const char *t,size_t l){
  if(b->n+l+1>b->cap){
    while(b->n+l+1>b->cap)b->cap*=2;
    b->s=realloc(b->s,b->cap);
  }
  memcpy(b->s+b->n,t,l);
  b->n+=l;
  b->s[b->n]=0;
}

void sba(Sb *b,const char *t){sbn(b,t,strlen(t));}
void sbc(Sb *b,char c){sbn(b,&c,1);}

void sbf(Sb *b,const char *f,...){
  char t[512];
  va_list a;
  va_start(a,f);
  vsnprintf(t,sizeof t,f,a);
  va_end(a);
  sba(b,t);
}

char *sdup(const char *s){
  size_t l=strlen(s)+1;
  char *r=malloc(l);
  memcpy(r,s,l);
  return r;
}

int has(const char *h,const char *n){
  size_t hl=strlen(h),nl=strlen(n),i,j;
  if(!nl)return 1;
  if(nl>hl)return 0;
  for(i=0;i+nl<=hl;i++){
    for(j=0;j<nl;j++)
      if(tolower((u8)h[i+j])!=tolower((u8)n[j]))break;
    if(j==nl)return 1;
  }
  return 0;
}

int sta(const char *s,const char *p){
  while(*p){
    if(tolower((u8)*s)!=tolower((u8)*p))return 0;
    s++;p++;
  }
  return 1;
}

int fin(const char *s,const char *suf){
  size_t l=strlen(s),k=strlen(suf);
  return l>=k&&sta(s+l-k,suf);
}

u16 r16(const u8 *p){return (u16)(p[0]|(p[1]<<8));}
u32 r32(const u8 *p){return (u32)p[0]|((u32)p[1]<<8)|((u32)p[2]<<16)|((u32)p[3]<<24);}
u64 r64(const u8 *p){return (u64)r32(p)|((u64)r32(p+4)<<32);}

u8 *rd(const char *path,size_t *n){
  FILE *f=fopen(path,"rb");
  u8 *buf;
  long sz;
  if(!f)return NULL;
  fseek(f,0,SEEK_END);
  sz=ftell(f);
  fseek(f,0,SEEK_SET);
  if(sz<=0){fclose(f);return NULL;}
  buf=malloc((size_t)sz);
  if(!buf||fread(buf,1,(size_t)sz,f)!=(size_t)sz){fclose(f);free(buf);return NULL;}
  fclose(f);
  *n=(size_t)sz;
  return buf;
}

int isd(const char *p){
#ifdef _WIN32
  DWORD a=GetFileAttributesA(p);
  return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_DIRECTORY);
#else
  struct stat st;
  return stat(p,&st)==0&&S_ISDIR(st.st_mode);
#endif
}

void mkd(const char *p){
#ifdef _WIN32
  CreateDirectoryA(p,NULL);
#else
  mkdir(p,0777);
#endif
}

const char *bn(const char *p){
  const char *a=strrchr(p,'/'),*b=strrchr(p,'\\');
  if(b>a)a=b;
  return a?a+1:p;
}

char *pj(const char *a,const char *b){
  size_t la=strlen(a);
  char *r=malloc(la+strlen(b)+2);
  strcpy(r,a);
  if(la&&a[la-1]!='/'&&a[la-1]!='\\')strcat(r,"/");
  strcat(r,b);
  return r;
}

static int cmps(const void *a,const void *b){return strcmp(*(char *const *)a,*(char *const *)b);}

char **ls(const char *dir,int *cnt){
  char **v=NULL;
  int n=0,cap=0;
#ifdef _WIN32
  WIN32_FIND_DATAA fd;
  HANDLE h;
  char *pat=pj(dir,"*");
  h=FindFirstFileA(pat,&fd);
  free(pat);
  if(h==INVALID_HANDLE_VALUE){*cnt=0;return NULL;}
  do{
    if(!strcmp(fd.cFileName,".")||!strcmp(fd.cFileName,".."))continue;
    if(n==cap){cap=cap?cap*2:64;v=realloc(v,cap*sizeof(char *));}
    v[n++]=pj(dir,fd.cFileName);
  }while(FindNextFileA(h,&fd));
  FindClose(h);
#else
  DIR *d=opendir(dir);
  struct dirent *de;
  if(!d){*cnt=0;return NULL;}
  while((de=readdir(d))){
    if(!strcmp(de->d_name,".")||!strcmp(de->d_name,".."))continue;
    if(n==cap){cap=cap?cap*2:64;v=realloc(v,cap*sizeof(char *));}
    v[n++]=pj(dir,de->d_name);
  }
  closedir(d);
#endif
  if(n)qsort(v,n,sizeof(char *),cmps);
  *cnt=n;
  return v;
}

void fls(char **v,int n){
  int i;
  for(i=0;i<n;i++)free(v[i]);
  free(v);
}
