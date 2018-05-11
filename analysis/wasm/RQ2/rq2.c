#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define PERF_NUM 10
#define BENCH_NUM 1952

float ar_mean(float *arr, int n){
  float sum = 0;
  int i;

  for(i = 0; i < n; i++)
    sum += arr[i]; 
  
  sum /= n;
  
  return sum/1000;
}

float geo_mean(float *arr, int n){
  float sum = 0;
  int i;

  for(i = 0; i < n; i++)
    sum += log(arr[i]); 
 
  sum /= n;
  
  return exp(sum);
}

struct info{
  float mflops;
  int format;
  int sibling;
  float sd;
};

void max(struct info *optimal){
  int i, j, temp_format;
  float temp_mflops;
  for(i=0; i < 4; i++){
    for(j = 1; j < 4-i; j++){
      if(optimal[j-1].mflops < optimal[j].mflops){
        temp_format = optimal[j-1].format;
        temp_mflops = optimal[j-1].mflops;
        optimal[j-1].format = optimal[j].format;
        optimal[j-1].mflops = optimal[j].mflops;
        optimal[j].format = temp_format;
        optimal[j].mflops = temp_mflops; 
      }
    }
  }
}


int siblings(struct info *optimal, int *index){
  int i = 1;
  int flag = 0;
  float optimal1_min, optimal2_max;
  optimal1_min = optimal[0].mflops - optimal[0].sd;
  float optimal1_perfdiff = (1 - PERF_NUM/100.0) * optimal[0].mflops;
  while(i <= 3){ 
    if(optimal[i].mflops > 0){
      optimal2_max = optimal[i].mflops + optimal[i].sd;
      if(optimal1_min > optimal2_max && optimal2_max <= optimal1_perfdiff);
      //if(optimal1_min > optimal2_max && optimal1_min > optimal1_perfdiff && optimal2_max <= optimal1_perfdiff);
      else{
        optimal[i].sibling = 1;
        *index = i;
        flag++;
      }
    }
    i++;
  }
  return flag;
}

char* getFormat(int i){
  switch(i){
    case 1:
      return "COO";
    case 2:
      return "CSR";
    case 3:
      return "DIA";
    case 4:
      return "ELL";
  }
}

int main(int argc, char **argv){
  int c_nnz, js_nnz, c_N, js_N, c_coo_sum, js_coo_sum, c_csr_sum, js_csr_sum, c_dia_sum, js_dia_sum, c_ell_sum, js_ell_sum;
  char c_name[100], js_name[100];
  float c_coo_mflops, js_coo_mflops, c_csr_mflops, js_csr_mflops, c_dia_mflops, js_dia_mflops, c_ell_mflops, js_ell_mflops;
  FILE *fp1 = fopen(argv[1], "r");
  if(fp1 == NULL){
    printf("file pointer null");
    exit(1);
  }
  FILE *fp2 = fopen(argv[2], "r");
  if(fp2 == NULL){
    printf("file pointer null");
    exit(1);
  }
  int i = 0, all = 0, coo=0, csr=0, dia=0, ell=0;
  float dummy;
  int c_optimal, js_optimal;
  struct info optimal1[4], optimal2[4];
  int sib1 = 0, sib2 = 0, index = 0;
  float c_coo_sd = 0, c_csr_sd = 0, c_dia_sd = 0, c_ell_sd = 0;
  float js_coo_sd = 0, js_csr_sd = 0, js_dia_sd = 0, js_ell_sd = 0;
  float arr_sp[BENCH_NUM], arr_sp_coo[BENCH_NUM], arr_sp_csr[BENCH_NUM], arr_sp_dia[BENCH_NUM], arr_sp_ell[BENCH_NUM];
  float arr_dp[BENCH_NUM], arr_dp_coo[BENCH_NUM], arr_dp_csr[BENCH_NUM], arr_dp_dia[BENCH_NUM], arr_dp_ell[BENCH_NUM];
  while(i < BENCH_NUM){
    fscanf(fp1, "%[^,],%d,%d,%f,%f,%d,%f,%f,%d,%f,%f,%d,%f,%f,%d\n", c_name, &c_N, &c_nnz, &c_coo_sd, &c_coo_mflops, &c_coo_sum, &c_csr_sd, &c_csr_mflops, &c_csr_sum, &c_dia_sd, &c_dia_mflops, &c_dia_sum, &c_ell_sd, &c_ell_mflops, &c_ell_sum);
    fscanf(fp2, "%[^,],%d,%d,%f,%f,%d,%f,%f,%d,%f,%f,%d,%f,%f,%d\n", js_name, &js_N, &js_nnz, &js_coo_sd, &js_coo_mflops, &js_coo_sum, &js_csr_sd, &js_csr_mflops, &js_csr_sum, &js_dia_sd, &js_dia_mflops, &js_dia_sum, &js_ell_sd, &js_ell_mflops, &js_ell_sum);
    if(strcmp(c_name, js_name))
      printf("Names not same %s %s\n", c_name, js_name);
    /*if((c_coo_sum > 0 && c_csr_sum > 0 && c_coo_sum != c_csr_sum) || (c_coo_sum > 0 && c_dia_sum > 0 && c_coo_sum != c_dia_sum) || (c_coo_sum > 0 && c_ell_sum > 0 && c_coo_sum != c_ell_sum))
      printf("First SUM Not equal : %s\n", c_name);     
    if((js_coo_sum > 0 && js_csr_sum > 0 && js_coo_sum != js_csr_sum) || (js_coo_sum > 0 && js_dia_sum > 0 && js_coo_sum != js_dia_sum) || (js_coo_sum > 0 && js_ell_sum > 0 && js_coo_sum != js_ell_sum))
      printf("Second SUM Not equal : %s\n", js_name);     
    //if(js_coo_sum > 0 && c_coo_sum > 0 && js_coo_sum != c_coo_sum)
      //printf("Not equal : %s\n", js_name);   */  
    if(c_N != js_N || c_nnz != js_nnz)
      printf("N and nnz Not equal : %s %d %d %d %d\n", js_name, c_N, js_N, c_nnz, js_nnz);    
    optimal1[0].sd = isinf(c_coo_sd) ? 0 : c_coo_sd, optimal1[1].sd = isinf(c_csr_sd) ? 0 : c_csr_sd, optimal1[2].sd = isinf(c_dia_sd) ? 0 : c_dia_sd, optimal1[3].sd = isinf(c_ell_sd) ? 0 : c_ell_sd;
    optimal2[0].sd = isinf(js_coo_sd) ? 0 : js_coo_sd, optimal2[1].sd = isinf(js_csr_sd) ? 0 : js_csr_sd, optimal2[2].sd = isinf(js_dia_sd) ? 0 : js_dia_sd, optimal2[3].sd = isinf(js_ell_sd) ? 0 : js_ell_sd;
    optimal1[0].format = 1, optimal1[1].format = 2, optimal1[2].format = 3, optimal1[3].format = 4;
    optimal2[0].format = 1, optimal2[1].format = 2, optimal2[2].format = 3, optimal2[3].format = 4;
    optimal1[0].sibling = 0, optimal1[1].sibling = 0, optimal1[2].sibling = 0, optimal1[3].sibling = 0;
    optimal2[0].sibling = 0, optimal2[1].sibling = 0, optimal2[2].sibling = 0, optimal2[3].sibling = 0;
    optimal1[0].mflops = c_coo_mflops, optimal1[1].mflops = c_csr_mflops, optimal1[2].mflops = c_dia_mflops, optimal1[3].mflops = c_ell_mflops; 
    optimal2[0].mflops = js_coo_mflops, optimal2[1].mflops = js_csr_mflops, optimal2[2].mflops = js_dia_mflops, optimal2[3].mflops = js_ell_mflops; 
    max(optimal1);
    c_optimal = optimal1[0].format;
    max(optimal2);
    js_optimal = optimal2[0].format;
    sib1 = siblings(optimal1, &index);
    sib2 = siblings(optimal2, &index);
    if(sib1 == 0 && sib2 == 0){
    if(c_optimal == js_optimal){
      arr_sp[all] = optimal1[0].mflops;
      arr_dp[all] = optimal2[0].mflops;
      all++;
      switch(c_optimal){
        case 1:
          arr_sp_coo[coo] = optimal1[0].mflops;
          arr_dp_coo[coo] = optimal2[0].mflops;
          coo++;
          break;
        case 2:
          arr_sp_csr[csr] = optimal1[0].mflops;
          arr_dp_csr[csr] = optimal2[0].mflops;
          csr++;
          break;
        case 3:
          arr_sp_dia[dia] = optimal1[0].mflops;
          arr_dp_dia[dia] = optimal2[0].mflops;
          dia++;
          break;
        case 4:
          arr_sp_ell[ell] = optimal1[0].mflops;
          arr_dp_ell[ell] = optimal2[0].mflops;
          ell++;
          break;
      }
    }
    }
    i++;
  }
  printf("COO,%f,%f,%d\n", ar_mean(arr_sp_coo, coo), ar_mean(arr_dp_coo, coo), coo);
  printf("CSR,%f,%f,%d\n", ar_mean(arr_sp_csr, csr), ar_mean(arr_dp_csr, csr), csr);
  printf("DIA,%f,%f,%d\n", ar_mean(arr_sp_dia, dia), ar_mean(arr_dp_dia, dia), dia);
  printf("ELL,%f,%f,%d\n", ar_mean(arr_sp_ell, ell), ar_mean(arr_dp_ell, ell), ell);
  fclose(fp1); 
  fclose(fp2); 
  return 0;
}
