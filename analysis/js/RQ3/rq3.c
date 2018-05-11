#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define BENCH_NUM 1952
int PERF_NUM = 10;

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

int getPerfGapRange(float value1, float value2, float value3, int min, int max){
  if(value1 < 0) value1 = min + 1; 
  if(value2 < 0) value2 = min + 1; 
  if(value3 < 0) value3 = min + 1; 

  if(value1 > min && value1 <= max && value2 > min && value2<= max && value3 > min && value3 <= max)
    return 1;
  else
    return 0;
}

void getPerfGap(float value1, float value2, float value3, float value4, float *perf1, float *perf2, float *perf3){
  *perf1 = (value2 > 0) ? (1 - value2/value1) * 100 : -1;
  *perf2 = (value3 > 0) ? (1 - value3/value1) * 100 : -1;
  *perf3 = (value4 > 0) ? (1 - value4/value1) * 100 : -1;
}

void getPerfByFormat(int format, float coo, float csr, float dia, float ell, float *perf1, float *perf2, float *perf3){
  switch(format){
    case 1:
      getPerfGap(coo, csr, dia, ell, perf1, perf2, perf3);
      break;
    case 2:
      getPerfGap(csr, coo, dia, ell, perf1, perf2, perf3);
      break;
    case 3:
      getPerfGap(dia, coo, csr, ell, perf1, perf2, perf3);
      break;
    case 4:
      getPerfGap(ell, coo, csr, dia, perf1, perf2, perf3);
      break;
  }
}

int siblings(struct info *optimal, int index){
  int flag = 0;
  float optimal1_min, optimal2_max;
  optimal1_min = optimal[index].mflops - optimal[index].sd;
  int i = index + 1;
  float optimal1_perfdiff = (1 - PERF_NUM/100.0) * optimal[index].mflops;
  while(i <= 3){ 
    if(optimal[i].mflops > 0){
      optimal2_max = optimal[i].mflops + optimal[i].sd;
      //if(optimal1_min > optimal2_max && optimal1_min > optimal1_perfdiff && optimal2_max <= optimal1_perfdiff);
      if(optimal1_min > optimal2_max && optimal2_max <= optimal1_perfdiff);
      else 
        flag++;
    }
    i++;
  }
  return flag;
}


/*int siblings(struct info *optimal, int *index){
  int i = 1;
  int flag = 0;
  while(i < 4){ 
    if(optimal[i].mflops > 0){
      float perf = (1 - optimal[i].mflops/optimal[0].mflops) * 100;
      if(perf < PERF_NUM){
        optimal[i].sibling = 1;
        *index = i;
        flag++;
      }
    }
    i++;
  }
  return flag;
}
*/

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
  PERF_NUM = 10;
  int i = BENCH_NUM;
  int diff = 0, same = 0, diff25no = 0, diff50no = 0, diff1010 = 0, diff2525 = 0, diff5050 = 0, same1010 = 0, same2525 = 0, same5050 = 0, coo_diff = 0, csr_diff = 0, dia_diff = 0, ell_diff = 0;
  int c_optimal, js_optimal;
  int all = 0, coocsr=0, coodia = 0, cooell=0, csrdia=0, csrell = 0, diaell=0, coocsrdia = 0, coodiaell = 0, csrdiaell = 0, coocsrell = 0;
  int coo_all = 0, coo_coocsr=0, coo_coodia = 0, coo_cooell=0, coo_csrdia=0, coo_csrell = 0, coo_diaell=0, coo_coocsrdia = 0, coo_coodiaell = 0, coo_csrdiaell = 0, coo_coocsrell = 0;
  int csr_all = 0, csr_coocsr=0, csr_coodia = 0, csr_cooell=0, csr_csrdia=0, csr_csrell = 0, csr_diaell=0, csr_coocsrdia = 0, csr_coodiaell = 0, csr_csrdiaell = 0, csr_coocsrell = 0;
  int dia_all = 0, dia_coocsr=0, dia_coodia = 0, dia_cooell=0, dia_csrdia=0, dia_csrell = 0, dia_diaell=0, dia_coocsrdia = 0, dia_coodiaell = 0, dia_csrdiaell = 0, dia_coocsrell = 0;
  int ell_all = 0, ell_coocsr=0, ell_coodia = 0, ell_cooell=0, ell_csrdia=0, ell_csrell = 0, ell_diaell=0, ell_coocsrdia = 0, ell_coodiaell = 0, ell_csrdiaell = 0, ell_coocsrell = 0;
  int coo = 0, csr = 0, dia = 0, ell = 0;
  int diff_coo_csr = 0, diff_coo_dia = 0, diff_coo_ell = 0, same_coo_coo = 0;
  int diff_csr_coo = 0, diff_csr_dia = 0, diff_csr_ell = 0, same_csr_csr = 0;
  int diff_dia_coo = 0, diff_dia_csr = 0, diff_dia_ell = 0, same_dia_dia = 0;
  int diff_ell_coo = 0, diff_ell_csr = 0, diff_ell_dia = 0, same_ell_ell = 0;
  float c_perf1, c_perf2, c_perf3, js_perf1, js_perf2, js_perf3;
  float dummy;
  struct info optimal1[4], optimal2[4];
  float c_coo_sd = 0, c_csr_sd = 0, c_dia_sd = 0, c_ell_sd = 0;
  float js_coo_sd = 0, js_csr_sd = 0, js_dia_sd = 0, js_ell_sd = 0;
  while(i){
    fscanf(fp1, "%[^,],%d,%d,%f,%f,%d,%f,%f,%d,%f,%f,%d,%f,%f,%d\n", c_name, &c_N, &c_nnz, &c_coo_sd, &c_coo_mflops, &c_coo_sum, &c_csr_sd, &c_csr_mflops, &c_csr_sum, &c_dia_sd, &c_dia_mflops, &c_dia_sum, &c_ell_sd, &c_ell_mflops, &c_ell_sum);
    fscanf(fp2, "%[^,],%d,%d,%f,%f,%d,%f,%f,%d,%f,%f,%d,%f,%f,%d,\n", js_name, &js_N, &js_nnz, &js_coo_sd, &js_coo_mflops, &js_coo_sum, &js_csr_sd, &js_csr_mflops, &js_csr_sum, &js_dia_sd, &js_dia_mflops, &js_dia_sum, &js_ell_sd, &js_ell_mflops, &js_ell_sum);
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
    if(isinf(c_coo_sd) || isinf(c_csr_sd) || isinf(c_dia_sd) || isinf(c_ell_sd)){
      i--; continue;
    }
    optimal1[0].sd = isinf(c_coo_sd) ? 0 : c_coo_sd, optimal1[1].sd = isinf(c_csr_sd) ? 0 : c_csr_sd, optimal1[2].sd = isinf(c_dia_sd) ? 0 : c_dia_sd, optimal1[3].sd = isinf(c_ell_sd) ? 0 : c_ell_sd;
    optimal2[0].sd = isinf(js_coo_sd) ? 0 : js_coo_sd, optimal2[1].sd = isinf(js_csr_sd) ? 0 : js_csr_sd, optimal2[2].sd = isinf(js_dia_sd) ? 0 : js_dia_sd, optimal2[3].sd = isinf(js_ell_sd) ? 0 : js_ell_sd;
    optimal1[0].format = 1, optimal1[1].format = 2, optimal1[2].format = 3, optimal1[3].format = 4;
    optimal1[0].sibling = 0, optimal1[1].sibling = 0, optimal1[2].sibling = 0, optimal1[3].sibling = 0;
    optimal2[0].sibling = 0, optimal2[1].sibling = 0, optimal2[2].sibling = 0; optimal2[3].sibling = 0;
    optimal2[0].format = 1, optimal2[1].format = 2, optimal2[2].format = 3, optimal2[3].format = 4;
    optimal1[0].mflops = c_coo_mflops, optimal1[1].mflops = c_csr_mflops, optimal1[2].mflops = c_dia_mflops, optimal1[3].mflops = c_ell_mflops; 
    optimal2[0].mflops = js_coo_mflops, optimal2[1].mflops = js_csr_mflops, optimal2[2].mflops = js_dia_mflops, optimal2[3].mflops = js_ell_mflops; 
    max(optimal1);
    c_optimal = optimal1[0].format;
    max(optimal2);
    js_optimal = optimal2[0].format;
    getPerfByFormat(c_optimal, c_coo_mflops, c_csr_mflops, c_dia_mflops, c_ell_mflops, &c_perf1, &c_perf2, &c_perf3);
    int sib = 0, sib2 = 0;
    if(siblings(optimal1, 0) == 0)
      sib = 0;
    else if(siblings(optimal1, 0) == 1 && siblings(optimal1,1) == 0)
      sib = 1;
    else if(siblings(optimal1, 0) <= 2 && siblings(optimal1,1) <= 1 && siblings(optimal1,2) == 0)
      sib = 2;
    else 
      sib = 3; 

    if(siblings(optimal2, 0) == 0)
      sib2 = 0;
    else if(siblings(optimal2, 0) == 1 && siblings(optimal2,1) == 0)
      sib2 = 1;
    else if(siblings(optimal2, 0) <= 2 && siblings(optimal2,1) <= 1 && siblings(optimal2,2) == 0)
      sib2 = 2;
    else 
      sib2 = 3; 

    switch(c_optimal){
      case 1:
        if(sib == 0){
          if(sib2 == 0){
          switch(js_optimal){
            case 1:
                same_coo_coo++;
              break;
            case 2:
                diff_coo_csr++;
              break;
            case 3:
                diff_coo_dia++;
              break;
            case 4:
                diff_coo_ell++;
              break;
          }
          }
          else if(sib2 == 1){
            if((optimal2[0].format == 1 && optimal2[1].format == 2) || (optimal2[0].format == 2 && optimal2[1].format == 1)){
              coo_coocsr++;
            }
            else if((optimal2[0].format == 1 && optimal2[1].format == 3) || (optimal2[0].format == 3 && optimal2[1].format == 1)){
              coo_coodia++;
            }
            else if((optimal2[0].format == 1 && optimal2[1].format == 4) || (optimal2[0].format == 4 && optimal2[1].format == 1)){
              coo_cooell++;
            }
            else if((optimal2[0].format == 2 && optimal2[1].format == 3) || (optimal2[0].format == 3 && optimal2[1].format == 2)){
              coo_csrdia++;
            }
            else if((optimal2[0].format == 2 && optimal2[1].format == 4) || (optimal2[0].format == 4 && optimal2[1].format == 2)){
              coo_csrell++;
            }
            else if((optimal2[0].format == 3 && optimal2[1].format == 4) || (optimal2[0].format == 4 && optimal2[1].format == 3)){
              coo_diaell++;
            }
          }
          else if(sib2 == 2){
            if((optimal2[0].format == 1 && optimal2[1].format == 2 && optimal2[2].format == 3) || (optimal2[0].format == 2 && optimal2[1].format == 1 && optimal2[2].format == 3) || (optimal2[0].format == 1 && optimal2[1].format == 3 && optimal2[2].format == 2) || (optimal2[0].format == 3 && optimal2[1].format == 1 && optimal2[2].format == 2) || (optimal2[0].format == 3 && optimal2[1].format == 2 && optimal2[2].format == 1) || (optimal2[0].format == 2 && optimal2[1].format == 3 && optimal2[2].format == 1)){
              coo_coocsrdia++;
            }
            else if((optimal2[0].format == 1 && optimal2[1].format == 4 && optimal2[2].format == 3) || (optimal2[0].format == 4 && optimal2[1].format == 1 && optimal2[2].format == 3) || (optimal2[0].format == 1 && optimal2[1].format == 3 && optimal2[2].format == 4) || (optimal2[0].format == 3 && optimal2[1].format == 1 && optimal2[2].format == 4) || (optimal2[0].format == 3 && optimal2[1].format == 4 && optimal2[2].format == 1) || (optimal2[0].format == 4 && optimal2[1].format == 3 && optimal2[2].format == 1)){
              coo_coodiaell++;
            }
            else if((optimal2[0].format == 1 && optimal2[1].format == 2 && optimal2[2].format == 4) || (optimal2[0].format == 2 && optimal2[1].format == 1 && optimal2[2].format == 4) || (optimal2[0].format == 1 && optimal2[1].format == 4 && optimal2[2].format == 2) || (optimal2[0].format == 4 && optimal2[1].format == 1 && optimal2[2].format == 2) || (optimal2[0].format == 4 && optimal2[1].format == 2 && optimal2[2].format == 1) || (optimal2[0].format == 2 && optimal2[1].format == 4 && optimal2[2].format == 1)){
              coo_coocsrell++;
            }
            else if((optimal2[0].format == 4 && optimal2[1].format == 2 && optimal2[2].format == 3) || (optimal2[0].format == 2 && optimal2[1].format == 4  && optimal2[2].format == 3) || (optimal2[0].format == 4 && optimal2[1].format == 3 && optimal2[2].format == 2) || (optimal2[0].format == 3 && optimal2[1].format == 4 && optimal2[2].format == 2) || (optimal2[0].format == 3 && optimal2[1].format == 2 && optimal2[2].format == 4) || (optimal2[0].format == 2 && optimal2[1].format == 3 && optimal2[2].format == 4)){
              coo_csrdiaell++;
            }
          }
          else{
//            printf("%s %f %f %f %f\n", js_name,optimal2[0].mflops, optimal2[1].mflops, optimal2[2].mflops, optimal2[3].mflops); 
            coo_all++;
          }
          coo++;
        }
        else if(sib == 1){
          switch(optimal1[1].format){
            case 2:
              coocsr++;
              break;
            case 3:
              coodia++;
              break;
            case 4:
              cooell++;
              break;
          }
        }
        else if(sib == 2){
          switch(optimal1[1].format){
            case 2:
              switch(optimal1[2].format){
                case 3:
                  coocsrdia++;
                  break;
                case 4:
                  coocsrell++;
                  break;
              }
              break;
            case 3:
              switch(optimal1[2].format){
                case 2:
                  coocsrdia++;
                  break;
                case 4:
                  coodiaell++;
                  break;
              }
              break;
            case 4:
              switch(optimal1[2].format){
                case 2:
                  coocsrell++;
                  break;
                case 3:
                  coodiaell++;
                  break;
              }
              break;
          }
        }
        else if(sib == 3){
          all++;
        }
        break;
      case 2:
        if(sib == 0){
          if(sib2 == 0){
          switch(js_optimal){
            case 1:
                diff_csr_coo++;
              break;
            case 2:
                same_csr_csr++;
              break;
            case 3:
                diff_csr_dia++;
              break;
            case 4:
                diff_csr_ell++;
              break;
          }
          }
          else if(sib2 == 1){
            if((optimal2[0].format == 1 && optimal2[1].format == 2) || (optimal2[0].format == 2 && optimal2[1].format == 1)){
              csr_coocsr++;
            }
            else if((optimal2[0].format == 1 && optimal2[1].format == 3) || (optimal2[0].format == 3 && optimal2[1].format == 1)){
              csr_coodia++;
            }
            else if((optimal2[0].format == 1 && optimal2[1].format == 4) || (optimal2[0].format == 4 && optimal2[1].format == 1)){
              csr_cooell++;
            }
            else if((optimal2[0].format == 2 && optimal2[1].format == 3) || (optimal2[0].format == 3 && optimal2[1].format == 2)){
              csr_csrdia++;
            }
            else if((optimal2[0].format == 2 && optimal2[1].format == 4) || (optimal2[0].format == 4 && optimal2[1].format == 2)){
              csr_csrell++;
            }
            else if((optimal2[0].format == 3 && optimal2[1].format == 4) || (optimal2[0].format == 4 && optimal2[1].format == 3)){
              csr_diaell++;
            }
          }
          else if(sib2 == 2){
            if((optimal2[0].format == 1 && optimal2[1].format == 2 && optimal2[2].format == 3) || (optimal2[0].format == 2 && optimal2[1].format == 1 && optimal2[2].format == 3) || (optimal2[0].format == 1 && optimal2[1].format == 3 && optimal2[2].format == 2) || (optimal2[0].format == 3 && optimal2[1].format == 1 && optimal2[2].format == 2) || (optimal2[0].format == 3 && optimal2[1].format == 2 && optimal2[2].format == 1) || (optimal2[0].format == 2 && optimal2[1].format == 3 && optimal2[2].format == 1)){
              csr_coocsrdia++;
            }
            else if((optimal2[0].format == 1 && optimal2[1].format == 4 && optimal2[2].format == 3) || (optimal2[0].format == 4 && optimal2[1].format == 1 && optimal2[2].format == 3) || (optimal2[0].format == 1 && optimal2[1].format == 3 && optimal2[2].format == 4) || (optimal2[0].format == 3 && optimal2[1].format == 1 && optimal2[2].format == 4) || (optimal2[0].format == 3 && optimal2[1].format == 4 && optimal2[2].format == 1) || (optimal2[0].format == 4 && optimal2[1].format == 3 && optimal2[2].format == 1)){
              csr_coodiaell++;
            }
            else if((optimal2[0].format == 1 && optimal2[1].format == 2 && optimal2[2].format == 4) || (optimal2[0].format == 2 && optimal2[1].format == 1 && optimal2[2].format == 4) || (optimal2[0].format == 1 && optimal2[1].format == 4 && optimal2[2].format == 2) || (optimal2[0].format == 4 && optimal2[1].format == 1 && optimal2[2].format == 2) || (optimal2[0].format == 4 && optimal2[1].format == 2 && optimal2[2].format == 1) || (optimal2[0].format == 2 && optimal2[1].format == 4 && optimal2[2].format == 1)){
              csr_coocsrell++;
            }
            else if((optimal2[0].format == 4 && optimal2[1].format == 2 && optimal2[2].format == 3) || (optimal2[0].format == 2 && optimal2[1].format == 4  && optimal2[2].format == 3) || (optimal2[0].format == 4 && optimal2[1].format == 3 && optimal2[2].format == 2) || (optimal2[0].format == 3 && optimal2[1].format == 4 && optimal2[2].format == 2) || (optimal2[0].format == 3 && optimal2[1].format == 2 && optimal2[2].format == 4) || (optimal2[0].format == 2 && optimal2[1].format == 3 && optimal2[2].format == 4)){
              csr_csrdiaell++;
            }
          }
          else{
            //printf("%s %f %f %f %f\n", js_name,optimal2[0].mflops, optimal2[1].mflops, optimal2[2].mflops, optimal2[3].mflops); 
            csr_all++;
          }
          csr++;
        }
        else if(sib == 1){
          switch(optimal1[1].format){
            case 1:
              coocsr++;
              break;
            case 3:
              csrdia++;
              break;
            case 4:
              csrell++;
              break;
          }
        }
        else if(sib == 2){
          switch(optimal1[1].format){
            case 1:
              switch(optimal1[2].format){
                case 3:
                  coocsrdia++;
                  break;
                case 4:
                  coocsrell++;
                  break;
              }
              break;
            case 3:
              switch(optimal1[2].format){
                case 1:
                  coocsrdia++;
                  break;
                case 4:
                  csrdiaell++;
                  break;
              }
              break;
            case 4:
              switch(optimal1[2].format){
                case 1:
                  coocsrell++;
                  break;
                case 3:
                  csrdiaell++;
                  break;
              }
              break;
          }
        }
        else if(sib == 3){
          all++;
        }
        break;
      case 3:
        if(sib == 0){
          if(sib2 == 0){
          switch(js_optimal){
            case 1:
                diff_dia_coo++;
              break;
            case 2:
                diff_dia_csr++;
              break;
            case 3:
                same_dia_dia++;
              break;
            case 4:
                diff_dia_ell++;
              break;
          }
          }
          else if(sib2 == 1){
            if((optimal2[0].format == 1 && optimal2[1].format == 2) || (optimal2[0].format == 2 && optimal2[1].format == 1)){
              dia_coocsr++;
            }
            else if((optimal2[0].format == 1 && optimal2[1].format == 3) || (optimal2[0].format == 3 && optimal2[1].format == 1)){
              dia_coodia++;
            }
            else if((optimal2[0].format == 1 && optimal2[1].format == 4) || (optimal2[0].format == 4 && optimal2[1].format == 1)){
              dia_cooell++;
            }
            else if((optimal2[0].format == 2 && optimal2[1].format == 3) || (optimal2[0].format == 3 && optimal2[1].format == 2)){
              dia_csrdia++;
            }
            else if((optimal2[0].format == 2 && optimal2[1].format == 4) || (optimal2[0].format == 4 && optimal2[1].format == 2)){
              dia_csrell++;
            }
            else if((optimal2[0].format == 3 && optimal2[1].format == 4) || (optimal2[0].format == 4 && optimal2[1].format == 3)){
              dia_diaell++;
            }
          }
          else if(sib2 == 2){
            if((optimal2[0].format == 1 && optimal2[1].format == 2 && optimal2[2].format == 3) || (optimal2[0].format == 2 && optimal2[1].format == 1 && optimal2[2].format == 3) || (optimal2[0].format == 1 && optimal2[1].format == 3 && optimal2[2].format == 2) || (optimal2[0].format == 3 && optimal2[1].format == 1 && optimal2[2].format == 2) || (optimal2[0].format == 3 && optimal2[1].format == 2 && optimal2[2].format == 1) || (optimal2[0].format == 2 && optimal2[1].format == 3 && optimal2[2].format == 1)){
              dia_coocsrdia++;
            }
            else if((optimal2[0].format == 1 && optimal2[1].format == 4 && optimal2[2].format == 3) || (optimal2[0].format == 4 && optimal2[1].format == 1 && optimal2[2].format == 3) || (optimal2[0].format == 1 && optimal2[1].format == 3 && optimal2[2].format == 4) || (optimal2[0].format == 3 && optimal2[1].format == 1 && optimal2[2].format == 4) || (optimal2[0].format == 3 && optimal2[1].format == 4 && optimal2[2].format == 1) || (optimal2[0].format == 4 && optimal2[1].format == 3 && optimal2[2].format == 1)){
              dia_coodiaell++;
            }
            else if((optimal2[0].format == 1 && optimal2[1].format == 2 && optimal2[2].format == 4) || (optimal2[0].format == 2 && optimal2[1].format == 1 && optimal2[2].format == 4) || (optimal2[0].format == 1 && optimal2[1].format == 4 && optimal2[2].format == 2) || (optimal2[0].format == 4 && optimal2[1].format == 1 && optimal2[2].format == 2) || (optimal2[0].format == 4 && optimal2[1].format == 2 && optimal2[2].format == 1) || (optimal2[0].format == 2 && optimal2[1].format == 4 && optimal2[2].format == 1)){
              dia_coocsrell++;
            }
            else if((optimal2[0].format == 4 && optimal2[1].format == 2 && optimal2[2].format == 3) || (optimal2[0].format == 2 && optimal2[1].format == 4  && optimal2[2].format == 3) || (optimal2[0].format == 4 && optimal2[1].format == 3 && optimal2[2].format == 2) || (optimal2[0].format == 3 && optimal2[1].format == 4 && optimal2[2].format == 2) || (optimal2[0].format == 3 && optimal2[1].format == 2 && optimal2[2].format == 4) || (optimal2[0].format == 2 && optimal2[1].format == 3 && optimal2[2].format == 4)){
              dia_csrdiaell++;
            }
          }
          else{
            //printf("%s %f %f %f %f\n", js_name,optimal2[0].mflops, optimal2[1].mflops, optimal2[2].mflops, optimal2[3].mflops); 
            dia_all++;
          }
          dia++;
        }
        else if(sib == 1){
          switch(optimal1[1].format){
            case 1:
              coodia++;
              break;
            case 2:
              csrdia++;
              break;
            case 4:
              diaell++;
              break;
          }
        }
        else if(sib == 2){
          switch(optimal1[1].format){
            case 1:
              switch(optimal1[2].format){
                case 2:
                  coocsrdia++;
                  break;
                case 4:
                  coodiaell++;
                  break;
              }
              break;
            case 2:
              switch(optimal1[2].format){
                case 1:
                  coocsrdia++;
                  break;
                case 4:
                  csrdiaell++;
                  break;
              }
              break;
            case 4:
              switch(optimal1[2].format){
                case 1:
                  coodiaell++;
                  break;
                case 2:
                  csrdiaell++;
                  break;
              }
              break;
          }
        }
        else if(sib == 3){
          all++;
        }
        break;
      case 4:
        if(sib == 0){
          if(sib2 == 0){
          switch(js_optimal){
            case 1:
                diff_ell_coo++;
              break;
            case 2:
                diff_ell_csr++;
              break;
            case 3:
                diff_ell_dia++;
              break;
            case 4:
                same_ell_ell++;
              break;
          }
          }
          else if(sib2 == 1){
            if((optimal2[0].format == 1 && optimal2[1].format == 2) || (optimal2[0].format == 2 && optimal2[1].format == 1)){
              ell_coocsr++;
            }
            else if((optimal2[0].format == 1 && optimal2[1].format == 3) || (optimal2[0].format == 3 && optimal2[1].format == 1)){
              ell_coodia++;
            }
            else if((optimal2[0].format == 1 && optimal2[1].format == 4) || (optimal2[0].format == 4 && optimal2[1].format == 1)){
              ell_cooell++;
            }
            else if((optimal2[0].format == 2 && optimal2[1].format == 3) || (optimal2[0].format == 3 && optimal2[1].format == 2)){
              ell_csrdia++;
            }
            else if((optimal2[0].format == 2 && optimal2[1].format == 4) || (optimal2[0].format == 4 && optimal2[1].format == 2)){
              ell_csrell++;
            }
            else if((optimal2[0].format == 3 && optimal2[1].format == 4) || (optimal2[0].format == 4 && optimal2[1].format == 3)){
              ell_diaell++;
            }
          }
          else if(sib2 == 2){
            if((optimal2[0].format == 1 && optimal2[1].format == 2 && optimal2[2].format == 3) || (optimal2[0].format == 2 && optimal2[1].format == 1 && optimal2[2].format == 3) || (optimal2[0].format == 1 && optimal2[1].format == 3 && optimal2[2].format == 2) || (optimal2[0].format == 3 && optimal2[1].format == 1 && optimal2[2].format == 2) || (optimal2[0].format == 3 && optimal2[1].format == 2 && optimal2[2].format == 1) || (optimal2[0].format == 2 && optimal2[1].format == 3 && optimal2[2].format == 1)){
              ell_coocsrdia++;
            }
            else if((optimal2[0].format == 1 && optimal2[1].format == 4 && optimal2[2].format == 3) || (optimal2[0].format == 4 && optimal2[1].format == 1 && optimal2[2].format == 3) || (optimal2[0].format == 1 && optimal2[1].format == 3 && optimal2[2].format == 4) || (optimal2[0].format == 3 && optimal2[1].format == 1 && optimal2[2].format == 4) || (optimal2[0].format == 3 && optimal2[1].format == 4 && optimal2[2].format == 1) || (optimal2[0].format == 4 && optimal2[1].format == 3 && optimal2[2].format == 1)){
              ell_coodiaell++;
            }
            else if((optimal2[0].format == 1 && optimal2[1].format == 2 && optimal2[2].format == 4) || (optimal2[0].format == 2 && optimal2[1].format == 1 && optimal2[2].format == 4) || (optimal2[0].format == 1 && optimal2[1].format == 4 && optimal2[2].format == 2) || (optimal2[0].format == 4 && optimal2[1].format == 1 && optimal2[2].format == 2) || (optimal2[0].format == 4 && optimal2[1].format == 2 && optimal2[2].format == 1) || (optimal2[0].format == 2 && optimal2[1].format == 4 && optimal2[2].format == 1)){
              ell_coocsrell++;
            }
            else if((optimal2[0].format == 4 && optimal2[1].format == 2 && optimal2[2].format == 3) || (optimal2[0].format == 2 && optimal2[1].format == 4  && optimal2[2].format == 3) || (optimal2[0].format == 4 && optimal2[1].format == 3 && optimal2[2].format == 2) || (optimal2[0].format == 3 && optimal2[1].format == 4 && optimal2[2].format == 2) || (optimal2[0].format == 3 && optimal2[1].format == 2 && optimal2[2].format == 4) || (optimal2[0].format == 2 && optimal2[1].format == 3 && optimal2[2].format == 4)){
              ell_csrdiaell++;
            }
          }
          else{
            //printf("%s %f %f %f %f\n", js_name,optimal2[0].mflops, optimal2[1].mflops, optimal2[2].mflops, optimal2[3].mflops); 
            ell_all++;
          }
          ell++;
        }
        else if(sib == 1){
          switch(optimal1[1].format){
            case 1:
              cooell++;
              break;
            case 2:
              csrell++;
              break;
            case 3:
              diaell++;
              break;
          }
        }
        else if(sib == 2){
          switch(optimal1[1].format){
            case 1:
              switch(optimal1[2].format){
                case 2:
                  coocsrell++;
                  break;
                case 3:
                  coodiaell++;
                  break;
              }
              break;
            case 2:
              switch(optimal1[2].format){
                case 1:
                  coocsrell++;
                  break;
                case 3:
                  csrdiaell++;
                  break;
              }
              break;
            case 3:
              switch(optimal1[2].format){
                case 1:
                  coodiaell++;
                  break;
                case 2:
                  csrdiaell++;
                  break;
              }
              break;
          }
        }
        else if(sib == 3){
          all++;
        }
        break;
    }
    i--;
  }
  printf("COO,%d,COO,%d,CSR,%d,DIA,%d,ELL,%d,COO-CSR,%d,COO-CSR-ELL,%d,all,%d,Others,%d\n", coo, same_coo_coo, diff_coo_csr, diff_coo_dia, diff_coo_ell, coo_coocsr, coo_coocsrell, coo_all, coo_coodia + coo_cooell + coo_csrdia + coo_csrell + coo_diaell + coo_coocsrdia + coo_coodiaell + coo_csrdiaell);
  printf("CSR,%d,COO,%d,CSR,%d,DIA,%d,ELL,%d,COO-CSR,%d,COO-CSR-ELL,%d,all,%d,Others,%d\n", csr, diff_csr_coo, same_csr_csr, diff_csr_dia, diff_csr_ell, csr_coocsr, csr_coocsrell, csr_all, csr_coodia + csr_cooell + csr_csrell + csr_csrdia + csr_diaell + csr_coocsrdia + csr_coodiaell + csr_csrdiaell);
  printf("DIA,%d,COO,%d,CSR,%d,DIA,%d,ELL,%d,COO-CSR,%d,COO-CSR-ELL,%d,all,%d,Others,%d\n", dia, diff_dia_coo, diff_dia_csr, same_dia_dia, diff_dia_ell, dia_coocsr, dia_coocsrell, dia_all, dia_coodia + dia_cooell + dia_csrdia + dia_csrell + dia_diaell + dia_coocsrdia + dia_coodiaell + dia_csrdiaell);
  printf("ELL,%d,COO,%d,CSR,%d,DIA,%d,ELL,%d,COO-CSR,%d,COO-CSR-ELL,%d,all,%d,Others,%d\n", ell, diff_ell_coo, diff_ell_csr, diff_ell_dia, same_ell_ell, ell_coocsr, ell_coocsrell, ell_all, ell_coodia + ell_cooell + ell_csrdia + ell_csrell + ell_diaell + ell_coocsrdia + ell_coodiaell + ell_csrdiaell);
  fclose(fp1); 
  fclose(fp2); 
  return 0;
}
