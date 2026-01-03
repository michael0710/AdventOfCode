#ifndef HMATH_H_INCLUDED
#define HMATH_H_INCLUDED

#include <stdint.h>

/** Datatype to solve the system of linear equations with */
typedef double CoeffType;

/**
 * \param a: Pointer to the memory location of the array's data
 * \param m: Number of rows of the given matrix
 * \param n: Number of columns of the given matrix  
 */
typedef struct
{
    CoeffType* a;
    size_t     m;
    size_t     n;
} MatrixType;

/**
 * Calculates the reduced row echelon form of (A|B).
 * 
 * \param[inout] A: Matrix coefficients of the left matrix. The matrix is
 *                  modified by the algorithm to:
 *                      the identidy matrix if the SLE has exactly one solution
 *                      undefined if the SLE has no solution
 *                      the identidy matrix plus the coefficients of free
 *                          variables if the SLE has infinite solutions
 *                  The matrix is of size m x n1. Where the data of a single row
 *                  shall be aligned next to each other in the memory.
 * \param[inout] B: Matrix coefficients of the right matrix. The matrix is of
 *                  size m x n2
 * 
 * \returns \arg 0: if the function calculated the rref of the matrix successfully
 *          \arg -1: if the function failed
 */
int hmath_rref(MatrixType* A,
               MatrixType* B);


/**
 * Calculates a specific solution to the system of linear equations (SLE).
 * 
 * $$\mathbf{A} \cdot \vec{x} = \vec{b}$$
 * 
 * \param[inout] A: Coefficients of the matrix A of size m x n. If the matrix
 *                  is not in reduced row echelon form, the function
 *                  hmath_rref(...) is called.
 * \param[inout] b: Coefficients of the result vector b. The vector must be of
 *                  size m x 1.
 * \param[inout] x: Coefficients of the result vector x. If a variable is a
 *                  free variable, its given value is used to calculate the
 *                  solution of the SLE. If a variable is non-free, its given
 *                  value is ignored and the actual result is written to it.
 *                  The vector must be of size n x 1.
 * \param[out] isFree: Vector to return wheather a variable is free or not.
 *                     The vector contains a non-zero value if the variable is
 *                     free. Otherwise the value is 0.0.
 * 
 * \returns \arg 0: if the function solved the equation successfully
 *          \arg -1: if the function failed
 * 
 */
int hmath_get_lineq_solution(MatrixType* A,
                             MatrixType* b,
                             MatrixType* x,
                             MatrixType* isFree);

#endif /* HMATH_H_INCLUDED */