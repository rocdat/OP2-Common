//
// auto-generated by op2.py on 2013-10-30 16:07
//

//user function
#include "adt_calc.h"
#ifdef VECTORIZE
inline void adt_calc_vec(floatv *x1,floatv *x2,floatv *x3,floatv *x4,floatv *q,floatv *adt){
  floatv dx,dy, ri,u,v,c;

  ri =  1.0f/q[0];
  u  =   ri*q[1];
  v  =   ri*q[2];
  c  = sqrt(gam*gm1*(ri*q[3]-0.5f*(u*u+v*v)));

  dx = x2[0] - x1[0];
  dy = x2[1] - x1[1];
  *adt  = fabs(u*dy-v*dx) + c*sqrt(dx*dx+dy*dy);

  dx = x3[0] - x2[0];
  dy = x3[1] - x2[1];
  *adt += fabs(u*dy-v*dx) + c*sqrt(dx*dx+dy*dy);

  dx = x4[0] - x3[0];
  dy = x4[1] - x3[1];
  *adt += fabs(u*dy-v*dx) + c*sqrt(dx*dx+dy*dy);

  dx = x1[0] - x4[0];
  dy = x1[1] - x4[1];
  *adt += fabs(u*dy-v*dx) + c*sqrt(dx*dx+dy*dy);

  *adt = (*adt) / cfl;
}

#endif

// host stub function
void op_par_loop_adt_calc(char const *name, op_set set,
  op_arg arg0,
  op_arg arg1,
  op_arg arg2,
  op_arg arg3,
  op_arg arg4,
  op_arg arg5){

  int nargs = 6;
  op_arg args[6];

  args[0] = arg0;
  args[1] = arg1;
  args[2] = arg2;
  args[3] = arg3;
  args[4] = arg4;
  args[5] = arg5;

  // initialise timers
  double cpu_t1, cpu_t2, wall_t1, wall_t2;
  op_timing_realloc(1);
  op_timers_core(&cpu_t1, &wall_t1);

  if (OP_diags>2) {
    printf(" kernel routine with indirection: adt_calc\n");
  }

  int exec_size = op_mpi_halo_exchanges(set, nargs, args);
  int set_size = ((set->size+set->exec_size-1)/16+1)*16; //align to 512 bits

  if (exec_size >0) {

#ifdef VECTORIZE
    for ( int n=0; n<exec_size/VECSIZE; n++ ){
      if (n==set->core_size/VECSIZE) {
        op_mpi_wait_all(nargs, args);
      }
      intv map0idx(&arg0.map_data_d[VECSIZE*n + set_size * 0]);
      intv map1idx(&arg0.map_data_d[VECSIZE*n + set_size * 1]);
      intv map2idx(&arg0.map_data_d[VECSIZE*n + set_size * 2]);
      intv map3idx(&arg0.map_data_d[VECSIZE*n + set_size * 3]);

      intv mapidx;
      mapidx = 2*map0idx;
      floatv arg0_p[2] = {
        floatv((float*)arg0.data+0, mapidx),
        floatv((float*)arg0.data+1, mapidx)};
      mapidx = 2*map1idx;
      floatv arg1_p[2] = {
        floatv((float*)arg1.data+0, mapidx),
        floatv((float*)arg1.data+1, mapidx)};
      mapidx = 2*map2idx;
      floatv arg2_p[2] = {
        floatv((float*)arg2.data+0, mapidx),
        floatv((float*)arg2.data+1, mapidx)};
      mapidx = 2*map3idx;
      floatv arg3_p[2] = {
        floatv((float*)arg3.data+0, mapidx),
        floatv((float*)arg3.data+1, mapidx)};
      floatv arg4_p[4] = {
        floatv(&((float*)arg4.data)[VECSIZE*4 * n +0], 4),
        floatv(&((float*)arg4.data)[VECSIZE*4 * n +1], 4),
        floatv(&((float*)arg4.data)[VECSIZE*4 * n +2], 4),
        floatv(&((float*)arg4.data)[VECSIZE*4 * n +3], 4)};
      floatv arg5_p[1];
      adt_calc_vec(
        arg0_p,
        arg1_p,
        arg2_p,
        arg3_p,
        arg4_p,
        arg5_p);
      store_a(arg5_p[0],&((float*)arg5.data)[VECSIZE* n]);
    }
    for ( int n=(exec_size/VECSIZE)*VECSIZE; n<exec_size; n++ ){
#else
    for ( int n=0; n<exec_size; n++ ){
#endif
      if (n==set->core_size) {
        op_mpi_wait_all(nargs, args);
      }
      int map0idx = arg0.map_data[n * arg0.map->dim + 0];
      int map1idx = arg0.map_data[n * arg0.map->dim + 1];
      int map2idx = arg0.map_data[n * arg0.map->dim + 2];
      int map3idx = arg0.map_data[n * arg0.map->dim + 3];

      adt_calc(
        &((float*)arg0.data)[2 * map0idx],
        &((float*)arg0.data)[2 * map1idx],
        &((float*)arg0.data)[2 * map2idx],
        &((float*)arg0.data)[2 * map3idx],
        &((float*)arg4.data)[4 * n],
        &((float*)arg5.data)[1 * n]);
    }
  }

  if (exec_size == 0 || exec_size == set->core_size) {
    op_mpi_wait_all(nargs, args);
  }
  // combine reduction data
  op_mpi_set_dirtybit(nargs, args);

  // update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[1].name      = name;
  OP_kernels[1].count    += 1;
  OP_kernels[1].time     += wall_t2 - wall_t1;
}
