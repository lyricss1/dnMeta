#include "dn.h"
#ifdef _WIN32
#include <windows.h>
#endif

static Ai ai[512];
static int nai;

static int proc(const char *path){
  size_t n=0;
  u8 *buf=rd(path,&n);
  const char *err;
  u32 tc;
  Ai *a;
  if(!buf){fprintf(stderr,"  cannot read: %s\n",path);return 1;}
  err=load(buf,n);
  if(err){fprintf(stderr,"  skip: %s\n",err);free(buf);return 1;}
  bidx();
  if(G.tree)tree(path);
  a=&ai[nai<512?nai:511];
  tc=jall(nai);
  a->types=tc;
  {
    const char *an=M.rows[T_Asm]?S(cl(T_Asm,1,7)):S(cl(T_Mod,1,1));
    snprintf(a->name,sizeof a->name,"%s",an);
    if(M.rows[T_Asm])
      snprintf(a->ver,sizeof a->ver,"%u.%u.%u.%u",cl(T_Asm,1,1),cl(T_Asm,1,2),cl(T_Asm,1,3),cl(T_Asm,1,4));
    else a->ver[0]=0;
  }
  nai++;
  fprintf(stderr,"  OK: %u types, %u methods, %u fields\n",tc,M.rows[T_Met],M.rows[T_Fld]);
  fidx();
  free(buf);
  return 0;
}

static void usage(const char *exe){
  fprintf(stderr,
    "dnmeta - browse the code of a Unity (Mono) game: what you can read, toggle and patch\n\n"
    "usage: %s [game folder | Managed folder | Assembly-CSharp.dll] [options]\n"
    "       (without arguments the program asks for the game folder)\n\n"
    "output folder (default ./dnmeta_report):\n"
    "  index.html   interactive viewer: search, toggles/numbers, copy-paste code\n"
    "  tree.txt     full text tree of all types\n\n"
    "options:\n"
    "  -o <dir>     output folder\n"
    "  --all        read every DLL of the Managed folder (default: Assembly-CSharp*.dll)\n"
    "  --inc a,b    also read DLLs whose name contains a or b (e.g. --inc Zorro,pworld)\n"
    "  --no-tree    do not write tree.txt\n"
    "  --tree       also print the text tree to the console\n"
    "  -g           include compiler-generated types/members (<>c, <Foo>d__3 ...)\n"
    "tree.txt formatting: -p public only, -a attributes, -t tokens/RVA, -s short names,\n"
    "  -T types only, -f <text> filter by type name, --ascii ASCII tree characters\n",exe);
}

int main(int argc,char **argv){
  const char *in=NULL,*od="dnmeta_report";
  char pb[1024],gn[200],db[64];
  char **files=NULL,**dlls;
  int i,nf=0,nd=0,rc=0,il=0;
  char *man=NULL,*p;
  time_t now;

  G.attr=1;
  G.quiet=1;
  for(i=1;i<argc;i++){
    const char *a=argv[i];
    if(!strcmp(a,"-o")&&i+1<argc)od=argv[++i];
    else if(!strcmp(a,"-f")&&i+1<argc)G.flt=argv[++i];
    else if(!strcmp(a,"--inc")&&i+1<argc)G.inc=argv[++i];
    else if(!strcmp(a,"--all"))G.all=1;
    else if(!strcmp(a,"--no-tree"))G.tree=0;
    else if(!strcmp(a,"--tree"))G.quiet=0;
    else if(!strcmp(a,"-p"))G.pub=1;
    else if(!strcmp(a,"-a"))G.tattr=1;
    else if(!strcmp(a,"-t"))G.tok=1;
    else if(!strcmp(a,"-s"))G.sns=1;
    else if(!strcmp(a,"-g"))G.gen=1;
    else if(!strcmp(a,"-T"))G.tonly=1;
    else if(!strcmp(a,"--ascii"))G.ascii=1;
    else if(!strcmp(a,"-h")||!strcmp(a,"--help")){usage(argv[0]);return 0;}
    else if(a[0]=='-'){fprintf(stderr,"unknown option: %s\n",a);usage(argv[0]);return 2;}
    else in=a;
  }
#ifdef _WIN32
  SetConsoleOutputCP(65001);
#endif
  if(!in){
    printf("Enter the game folder (or drag it into this window), then press Enter:\n> ");
    fflush(stdout);
    if(!fgets(pb,sizeof pb,stdin))return 2;
    p=pb;
    while(*p==' '||*p=='"'||*p=='\'')p++;
    i=(int)strlen(p);
    while(i>0&&(p[i-1]=='\n'||p[i-1]=='\r'||p[i-1]==' '||p[i-1]=='"'||p[i-1]=='\''))p[--i]=0;
    if(!*p)return 2;
    in=p;
  }
  if(G.ascii){BL="`-- ";BM="|-- ";BB="|   ";}

  gname(in,gn,sizeof gn);
  if(isd(in)){
    int n=0,k;
    man=findm(in,&il);
    if(!man){
      if(il)
        fprintf(stderr,"This looks like an IL2CPP game (no managed Assembly-CSharp.dll).\n"
                       "dnmeta reads Mono games only; IL2CPP needs a metadata dumper (global-metadata.dat).\n");
      else
        fprintf(stderr,"Could not find the Managed folder inside: %s\n"
                       "Pass the game folder (the one with <Game>_Data), the Managed folder itself,\n"
                       "or the path to Assembly-CSharp.dll.\n",in);
      return 1;
    }
    fprintf(stderr,"Managed folder: %s\n",man);
    dlls=ls(man,&nd);
    files=malloc((nd+1)*sizeof(char *));
    for(k=0;k<nd;k++)if(want(bn(dlls[k])))files[n++]=dlls[k];else free(dlls[k]);
    free(dlls);
    nf=n;
    if(!nf){
      fprintf(stderr,"No Assembly-CSharp*.dll found there. Use --all or --inc <name>.\n");
      return 1;
    }
  }else{
    files=malloc(sizeof(char *));
    files[0]=sdup(in);
    nf=1;
  }

  mkd(od);
  {
    char *hp=pj(od,"index.html");
    G.h=fopen(hp,"wb");
    free(hp);
    if(!G.h){fprintf(stderr,"cannot write into %s\n",od);return 1;}
    if(G.tree){
      char *tp=pj(od,"tree.txt");
      G.log=fopen(tp,"wb");
      free(tp);
    }
  }
  G.first=1;
  fputs(UI_A,G.h);
  fputs("{\"types\":[",G.h);

  for(i=0;i<nf;i++){
    fprintf(stderr,"[%d/%d] %s\n",i+1,nf,bn(files[i]));
    rc|=proc(files[i]);
  }

  fputs("],\"asms\":[",G.h);
  for(i=0;i<nai&&i<512;i++){
    if(i)fputc(',',G.h);
    fputs("{\"n\":",G.h);jstr(G.h,ai[i].name);
    fputs(",\"v\":",G.h);jstr(G.h,ai[i].ver);
    fprintf(G.h,",\"t\":%u}",ai[i].types);
  }
  fputs("],\"game\":",G.h);jstr(G.h,gn);
  now=time(NULL);
  strftime(db,sizeof db,"%Y-%m-%d %H:%M",localtime(&now));
  fputs(",\"gen\":",G.h);jstr(G.h,db);
  fputc('}',G.h);
  fputs(UI_B,G.h);
  fclose(G.h);
  if(G.log)fclose(G.log);

  fprintf(stderr,"\nDone. Open this file in your browser:\n  %s/index.html\n",od);
  if(G.tree)fprintf(stderr,"Text tree:\n  %s/tree.txt\n",od);
  for(i=0;i<nf;i++)free(files[i]);
  free(files);
  free(man);
  return nai?0:1;
}
