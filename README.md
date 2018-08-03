# Sparse matrices on the web

## Input Matrices
We conducted our SpMV experiments for C, JavaScript and WebAssembly with 1,952 real-life examples of sparse matrices obtained from The SuiteSparse Matrix Collection (formerly the University of Florida Sparse Matrix Collection) at (https://sparse.tamu.edu). 
Please find the list of selected [input matrices](./input-matrices.txt) from the collection.

## Research Questions

    RQ1 : Performance comparison between C and both JavaScript and WebAssembly
    
   __Figure 1__. Single-precision JavaScript slowdown compared to C
   ![Figure 1.](./analysis/js/RQ1/rq1_single.png)
   
   __Figure 2__. Single-precision WebAssembly slowdown compared to C
    ![Figure 2.](./analysis/wasm/RQ1/rq1_single.png)
    
   __Figure 3__. Double-precision JavaScript slowdown compared to C
   ![Figure 3.](./analysis/js/RQ1/rq1_double.png)
    
   __Figure 4__. Double-precision WebAssembly slowdown compared to C
   ![Figure 4.](./analysis/wasm/RQ1/rq1_double.png)
    
    RQ2 : Performance comparison and format difference between single- and double-precision for C, 
    JavaScript and WebAssembly
    
    RQ3 : Format difference between C and both JavaScript and WebAssembly
