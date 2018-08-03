# Sparse matrices on the web
- [Table of Contents](#table-of-contents)
  * [Input Matrices](#input-matrices)
  * [Research Questions](#research-questions)
    + [RQ1 : Performance comparison between C and both JavaScript and WebAssembly](#rq1--performance-comparison-between-c-and-both-javascript-and-webassembly)
    + [RQ2 : Performance comparison and format difference between single- and double-precision for C, 
    JavaScript and WebAssembly](#rq2--performance-comparison-and-format-difference-between-single--and-double-precision-for-c-javascript-and-webassembly)
    + [RQ3 : Format difference between C and both JavaScript and WebAssembly](#rq3--format-difference-between-c-and-both-javascript-and-webassembly)
  * [Additional results based on size N](#additional-results-based-on-size-n)
## Input Matrices
We conducted our SpMV experiments for C, JavaScript and WebAssembly with 1,952 real-life examples of sparse matrices obtained from The SuiteSparse Matrix Collection (formerly the University of Florida Sparse Matrix Collection) at (https://sparse.tamu.edu). 
Please find the list of selected [input matrices](./input-matrices.txt) from the collection.

## Research Questions

   ### RQ1 : Performance comparison between C and both JavaScript and WebAssembly
 
   | Precision | JavaScript | WebAssembly |
   | :---: | :---: | :---: |
   | Single-precision | <img src="./analysis/js/RQ1/rq1_single.png" width = 400/> | <img src="./analysis/wasm/RQ1/rq1_single.png" width = 400/> |
   | Double-precision | <img src="./analysis/js/RQ1/rq1_double.png" width = 400/> | <img src="./analysis/wasm/RQ1/rq1_double.png" width = 400/> |
   
   ### RQ2 : Performance comparison and format difference between single- and double-precision for C, JavaScript and WebAssembly
    
   | x%-affinity | Single-Precision | Double-Precision |
   | --- | :---: | :---: |
   | 10% | <img src="./analysis/c/single_10_pie.png" width = 400/> | <img src="./analysis/c/double_10_pie.png" width = 400/> |
   | 25% | <img src="./analysis/c/single_25_pie.png" width = 400/> | <img src="./analysis/c/double_25_pie.png" width = 400/> |
   | 50% | <img src="./analysis/c/single_50_pie.png" width = 400/> |  <img src="./analysis/c/double_50_pie.png" width = 400/>|
   
  
  ### RQ3 : Format difference between C and both JavaScript and WebAssembly
    
## Additional results based on size N
    ARQ1 : Performance comparison between C and both JavaScript and WebAssembly based on N
    Small matrices: N less than 512
    Large matrices: N more than 100,000
   ### JavaScript vs C
   | Precision | Small | Large |
   | :---: | :---: | :---: |
   | Single-precision | <img src="./analysis/js/extras/size_plots/rq1_js_small_single.png" width = 400/> | <img src="./analysis/js/extras/size_plots/rq1_js_large_single.png" width = 400/> |
   | Double-precision | <img src="./analysis/js/extras/size_plots/rq1_js_small_double.png" width = 400/> | <img src="./analysis/js/extras/size_plots/rq1_js_large_double.png" width = 400/> |
   
   ### WebAssembly vs C
   | Precision | Small | Large |
   | :---: | :---: | :---: |
   | Single-precision | <img src="./analysis/wasm/extras/size_plots/rq1_wasm_small_single.png" width = 400/> | <img src="./analysis/wasm/extras/size_plots/rq1_wasm_large_single.png" width = 400/> |
   | Double-precision | <img src="./analysis/wasm/extras/size_plots/rq1_wasm_small_double.png" width = 400/> | <img src="./analysis/wasm/extras/size_plots/rq1_wasm_large_double.png" width = 400/> |
