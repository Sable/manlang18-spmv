#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define INPUT_NUM  1952

int PERF_NUM = 10;

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
  
  return exp(sum)/1000;
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

int siblings(struct info *optimal, int index){
  int flag = 0;
  float optimal1_min, optimal2_max;
  optimal1_min = optimal[index].mflops - optimal[index].sd;
  int i = index + 1;
  float optimal1_perfdiff = (1 - PERF_NUM/100.0) * optimal[index].mflops;
  while(i <= 3){ 
    if(optimal[i].mflops > 0){
      optimal2_max = optimal[i].mflops + optimal[i].sd;
      if(optimal1_min > optimal2_max && optimal2_max <= optimal1_perfdiff);
      //if(optimal1_min > optimal2_max && optimal1_min > optimal1_perfdiff && optimal2_max <= optimal1_perfdiff);
      else 
        flag++;
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

void print(FILE *fp, char *c_name, int c_N, int c_nnz, int c_optimal, float c_perf1, float c_perf2, float c_perf3, int js_optimal, float js_perf1, float js_perf2, float js_perf3){
  fprintf(fp, "%s,%d,%d,%s,%f,%f,%f,%s,%f,%f,%f\n", c_name, c_N, c_nnz, getFormat(c_optimal), c_perf1, c_perf2, c_perf3, getFormat(js_optimal),js_perf1, js_perf2, js_perf3);
}


int main(int argc, char **argv){
  int c_nnz, js_nnz, c_N, js_N, c_coo_sum, js_coo_sum, c_csr_sum, js_csr_sum, c_dia_sum, js_dia_sum, c_ell_sum, js_ell_sum;
  char c_name[100], js_name[100];
  float c_coo_mflops, js_coo_mflops, c_csr_mflops, js_csr_mflops, c_dia_mflops, js_dia_mflops, c_ell_mflops, js_ell_mflops;
  PERF_NUM = atoi(argv[2]);
  FILE *fp1 = fopen(argv[1], "r");
  if(fp1 == NULL){
    printf("file pointer null");
    exit(1);
  }
  int i = 0;
  int c_optimal, js_optimal;
  float c_coo_sd, c_csr_sd, c_dia_sd, c_ell_sd;
  int coocsr=0, coodia = 0, cooell=0, csrdia=0, csrell = 0, diaell=0, coocsrdia = 0, coodiaell = 0, csrdiaell = 0, coocsrell = 0;
  int coo = 0, csr = 0, dia = 0, ell = 0, all = 0;
  float c_perf1, c_perf2, c_perf3, js_perf1, js_perf2, js_perf3;
  struct info optimal1[4], optimal2[4];
  while(i < INPUT_NUM){
    fscanf(fp1, "%[^,],%d,%d,%f,%f,%d,%f,%f,%d,%f,%f,%d,%f,%f,%d,\n", c_name, &c_N, &c_nnz, &c_coo_sd, &c_coo_mflops, &c_coo_sum, &c_csr_sd, &c_csr_mflops, &c_csr_sum, &c_dia_sd, &c_dia_mflops, &c_dia_sum, &c_ell_sd, &c_ell_mflops, &c_ell_sum);
    //optimal1[0].sd = isinf(c_coo_sd) ? 0 : c_coo_sd, optimal1[1].sd = isinf(c_csr_sd) ? 0 : c_csr_sd, optimal1[2].sd = isinf(c_dia_sd) ? 0 : c_dia_sd, optimal1[3].sd = isinf(c_ell_sd) ? 0 : c_ell_sd;
    optimal1[0].sd = c_coo_sd, optimal1[1].sd = c_csr_sd, optimal1[2].sd = c_dia_sd, optimal1[3].sd = c_ell_sd;
    optimal1[0].format = 1, optimal1[1].format = 2, optimal1[2].format = 3, optimal1[3].format = 4;
    optimal1[0].sibling = 0, optimal1[1].sibling = 0, optimal1[2].sibling = 0, optimal1[3].sibling = 0;
    optimal1[0].mflops = c_coo_mflops, optimal1[1].mflops = c_csr_mflops, optimal1[2].mflops = c_dia_mflops, optimal1[3].mflops = c_ell_mflops; 
    /*if(isinf(c_coo_sd) || isinf(c_csr_sd) || isinf(c_dia_sd) || isinf(c_ell_sd)){
      i++; continue;
    }*/
    max(optimal1);
    c_optimal = optimal1[0].format;
    getPerfByFormat(c_optimal, c_coo_mflops, c_csr_mflops, c_dia_mflops, c_ell_mflops, &c_perf1, &c_perf2, &c_perf3);
    int sib = 0;
    if(siblings(optimal1, 0) == 0)
      sib = 0;
    else if(siblings(optimal1, 0) == 1 && siblings(optimal1,1) == 0)
      sib = 1;
    else if(siblings(optimal1, 0) <= 2 && siblings(optimal1,1) <= 1 && siblings(optimal1,2) == 0)
      sib = 2;
    else
      sib = 3; 
    switch(c_optimal){
      case 1:
        if(sib == 0){
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
//          printf("%s\n", c_name);
          all++;
        }
        break;
      case 2:
        if(sib == 0){
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
          //printf("%s\n", c_name);
          all++;
        }
        break;
      case 3:
        if(sib == 0){
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
          //printf("%s\n", c_name);
          all++;
        }
        break;
      case 4:
        if(sib == 0){
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
          //printf("%s\n", c_name);
          all++;
        }
        break;
    }
    i++;
  }
  printf("COO,%d\n" , coo);
  printf("CSR,%d\n" , csr);
  printf("DIA,%d\n" , dia);
  printf("ELL,%d\n" , ell);
  printf("COO-CSR,%d\n" , coocsr);
  //fprintf(sib_fp, "c,COO-DIA,%d,%.3f\n" , coodia, geo_mean(arr_c_coodia, coodia));
  //fprintf(sib_fp, "c,COO-ELL,%d,%.3f\n" , cooell, geo_mean(arr_c_cooell, cooell));
  //fprintf(sib_fp, "c,CSR-DIA,%d,%.3f\n" , csrdia, geo_mean(arr_c_csrdia, csrdia));
  //fprintf(sib_fp, "c,DIA-ELL,%d,%.3f\n" , diaell, geo_mean(arr_c_diaell, diaell));
  //fprintf(sib_fp, "c,COO-CSR-DIA,%d,%.3f\n" , coocsrdia, geo_mean(arr_c_coocsrdia, coocsrdia));
  //fprintf(sib_fp, "c,COO-DIA-ELL,%d,%.3f\n" , coodiaell, geo_mean(arr_c_coodiaell, coodiaell));
  //fprintf(sib_fp, "c,CSR-DIA-ELL,%d,%.3f\n" , csrdiaell, geo_mean(arr_c_csrdiaell, csrdiaell));
  printf("COO-CSR-ELL,%d\n" , coocsrell);
  //printf("COO geo mean %f\n", geo_mean(arr_c_coo, coo));
  //printf("CSR geo mean %f\n", geo_mean(arr_c_csr, csr));
  //printf("DIA geo mean %f\n", geo_mean(arr_c_dia, dia));
  printf("all,%d\n" , all);
  printf("Others,%d\n", coodia + cooell + csrdia + csrell + diaell + coocsrdia + coodiaell + csrdiaell); 
  fclose(fp1); 
  return 0;
}
