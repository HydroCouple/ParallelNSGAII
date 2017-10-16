#ifndef PROBLEMDEFINITIONS_H
#define PROBLEMDEFINITIONS_H

#include <string>
#include <vector>
#include "global.h"

#ifdef USE_MPI
#include <mpi.h>
#endif

typedef void (*problemDef)(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);

typedef void (*problemDefInitialize)(int processorRank, std::vector<std::string>& optionalArgs);

extern problemDef problemDefinition;

extern problemDefInitialize problemDefinitionInitialize;

extern std::vector<std::string> problemOptions;

#ifdef USE_MPI

void mpi_send_inds_to_worker(population *pop, int start, int length, int mpiProcessor);

void mpi_serialize_ind_from_master(individual *ind, std::vector<double> &data);

void mpi_recieve_inds_from_master(double *values, int size);

void mpi_desrialize_ind_from_master(individual *ind, double *values, int &currentPos);

void mpi_serialize_ind_from_worker(individual *ind, std::vector<double>& data);

void mpi_recieve_inds_from_worker(population *pop, int mpiProcessor);

void mpi_desrialize_ind_from_worker(individual *ind, double *values, int &currentPos);


#endif

//0
void test_problem_sch1 (int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//1
void test_problem_sch2 (int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//2
void test_problem_fon(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//3
void test_problem_kur(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//4
void test_problem_pol(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//5
void test_problem_vnt(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//6
void test_problem_zdt1(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//7
void test_problem_zdt2(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//8z
void test_problem_zdt3(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//9
void test_problem_zdt4(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//10
void test_problem_zdt5(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//11
void test_problem_zdt6(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//12
void test_problem_bnh(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//13
void test_problem_osy(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//14
void test_problem_srn(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//15
void test_problem_tnk(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//16
void test_problem_ctp1(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//17
void test_problem_ctp2(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//18
void test_problem_ctp3(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//19
void test_problem_ctp4(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//20
void test_problem_ctp5(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//21
void test_problem_ctp6(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//22
void test_problem_ctp7(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);
//23
void test_problem_ctp8(int gen, int indIndex, int nreal, double *xreal, int nbin, double *xbin, int *nbits, int **gene, int nobj, double *obj, int ncon, double *constr, const std::vector<std::string>& optionalArgs);

#endif // PROBLEMDEFINITIONS_H
