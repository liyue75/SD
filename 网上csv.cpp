//处理CSV格式文件，需要读取一个CSV文件，文件按照逗号进行分割。
//但是有些值中存在逗号，比如
//State Corporation Executives,"Vice President, Nordic Investment Bank",,,2006……
//每行60列
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#define MAXF 60    //每行最多列数
#define MAXC 80    //每列最多字符数+1
#define MAXW 4862  //每行最多字符数
#define MAXL 10000 //文件中最多行数
char D[MAXL][MAXF][MAXC];
char ln[MAXW];
int s,i,n,L;
char c,*p;
FILE *f;
int main() {
    setlocale(LC_ALL,"chs");
    f=fopen("d.csv","r");
    if (NULL==f) {
        fprintf(stderr,"无法打开文件d.csv!\n");
        return 1;
    }
    L=0;
    while (1) {
        if (NULL==fgets(ln,MAXW,f)) break;
        if ('\n'!=ln[strlen(ln)-1]) {
            fprintf(stderr,"第%d行太长(>%d个字符)!",L+1,MAXW);
            fclose(f);
            return 1;
        }
        ln[strlen(ln)-1]=0;
        s=0;
        i=0;
        n=0;
        p=ln;
        while (1) {
            c=ln[i];
            switch (s) {
            case 0://开始
                if (c==0) goto SHOW;
                if (c=='\"') {
                    s=1;
                    p=ln+i;
                } else if (c==',') {
                    strncpy(D[L][n],p,__min((ln+i)-p,MAXC-1));D[L][n][MAXC-1]=0;
                    n++;
                    if (n>=MAXF) {
                        fprintf(stderr,"警告:第%d行多于%d的字段被忽略。\n",L+1,MAXF);
                        goto SHOW;
                    }
                    p=ln+i+1;
                    s=3;
                } else {
                    s=2;
                    p=ln+i;
                }
            break;
            case 1://双引号中
                if (c==0) {
                    fprintf(stderr,"第%d行第%d个字符处格式错误！期望双引号。\n",L+1,i);
                    fclose(f);
                    return 1;
                }
                if (c=='\"' && (ln[i+1]==',' || ln[i+1]==0)) {
                    strncpy(D[L][n],p,__min((ln+i)-p+1,MAXC-1));D[L][n][MAXC-1]=0;
                    n++;
                    if (n>=MAXF) {
                        fprintf(stderr,"警告:第%d行多于%d的字段被忽略。\n",L+1,MAXF);
                        goto SHOW;
                    }
                    if (ln[i+1]==0) goto SHOW;
                    if (ln[i+1]==',') {
                        i++;
                        p=ln+i+1;
                        s=3;
                    }
                }
                if (c=='\"' && !(ln[i+1]==',' || ln[i+1]==0)) {
                    fprintf(stderr,"第%d行第%d个字符处格式错误！期望逗号或行结束。\n",L+1,i+1);
                    fclose(f);
                    return 1;
                }
            break;
            case 2://非双引号中
                if (c==0) {
                    strncpy(D[L][n],p,__min((ln+i)-p,MAXC-1));D[L][n][MAXC-1]=0;
                    n++;
                    goto SHOW;
                }
                if (c=='\"') {
                    fprintf(stderr,"第%d行第%d个字符处格式错误！此处不应出现双引号。\n",L+1,i);
                    fclose(f);
                    return 1;
                }
                if (c==',') {
                    strncpy(D[L][n],p,__min((ln+i)-p,MAXC-1));D[L][n][MAXC-1]=0;
                    n++;
                    if (n>=MAXF) {
                        fprintf(stderr,"警告:第%d行多于%d的字段被忽略。\n",L+1,MAXF);
                        goto SHOW;
                    }
                    p=ln+i+1;
                    s=3;
                }
            break;
            case 3://逗号后
                if (c==0) {
                    strncpy(D[L][n],p,__min((ln+i)-p,MAXC-1));D[L][n][MAXC-1]=0;
                    n++;
                    goto SHOW;
                }
                if (c=='\"') {
                    s=1;
                    p=ln+i;
                } else if (c==',') {
                    strncpy(D[L][n],p,__min((ln+i)-p,MAXC-1));D[L][n][MAXC-1]=0;
                    n++;
                    if (n>=MAXF) {
                        fprintf(stderr,"警告:第%d行多于%d的字段被忽略。\n",L+1,MAXF);
                        goto SHOW;
                    }
                    p=ln+i+1;
                } else {
                    s=2;
                    p=ln+i;
                }
            break;
            }
            i++;
        }
    SHOW:
        if (n!=MAXF) fprintf(stderr,"警告:第%d行数据列数%d不等于%d!\n",L+1,n,MAXF);
        printf("第%d行%d列:",L+1,n);
        for (i=0;i<n;i++) {
            if (i<n-1) printf("[%s]," ,D[L][i]);
            else       printf("[%s]\n",D[L][i]);
        }
        L++;
        if (L>=MAXL) {
            fprintf(stderr,"警告:多于%d行的数据被忽略。\n",MAXL);
            break;
        }
    }
    fclose(f);
    return 0;
}
//警告:第1行数据列数5不等于60!
//第1行5列:[State Corporation Executives],["Vice President, Nordic Investment Bank"],[],[],[2006……]