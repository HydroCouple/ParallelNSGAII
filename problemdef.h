#ifndef PROBLEMDEFINITIONS_H
#define PROBLEMDEFINITIONS_H

#include <list>
#include <string>
#include <vector>
#include "global.h"

#ifdef USE_MPI
#include <mpi.h>
#endif

typedef void (*problemDef)(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);

extern problemDef problemDefinition;

extern std::list<std::string> problemOptions;

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
void test_problem_sch1 (double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//1
void test_problem_sch2 (double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//2
void test_problem_fon(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//3
void test_problem_kur(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//4
void test_problem_pol(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//5
void test_problem_vnt(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//6
void test_problem_zdt1(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//7
void test_problem_zdt2(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//8
void test_problem_zdt3(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//9
void test_problem_zdt4(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//10
void test_problem_zdt5(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//11
void test_problem_zdt6(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//12
void test_problem_bnh(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//13
void test_problem_osy(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//14
void test_problem_srn(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//15
void test_problem_tnk(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//16
void test_problem_ctp1(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//17
void test_problem_ctp2(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//18
void test_problem_ctp3(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//19
void test_problem_ctp4(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//20
void test_problem_ctp5(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//21
void test_problem_ctp6(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//22
void test_problem_ctp7(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);
//23
void test_problem_ctp8(double *xreal, double *xbin, int **gene, double *obj, double *constr, const std::list<std::string>& optionalArgs);

#endif // PROBLEMDEFINITIONS_H
