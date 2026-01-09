#include <stdbool.h>
#include <float.h>

#include "hmath.h"

#define ACCESS_INDEX(A, m, n, selM, selN)  (A)[(selM)*(n) + (selN)]
#define ABS(x)                             (((x) < 0) ? -(x) : (x))

static int get_row_for_var(const MatrixType* A, size_t var, size_t* row);
static bool is_free_variable(const MatrixType* A, size_t col);
static CoeffType get_roundoff_tolerance(const MatrixType* A);
static void swap_rows(MatrixType* A,
                      MatrixType* B,
                      size_t      m1,
                      size_t      m2);
static int find_col_max(const MatrixType* A,
                              size_t      col,
                              size_t      startRowIdx,
                              CoeffType*  max,
                              size_t*     maxIdx);
static int normalise_rowA(MatrixType* A,
                          MatrixType* B,
                          size_t      row,
                          size_t      col2use);
static int create_zeroes_below(MatrixType* A,
                               MatrixType* B,
                               size_t      startRow,
                               size_t      col,
                               CoeffType   relRoffTol);
static int create_zeroes_above(MatrixType* A,
                               MatrixType* B,
                               size_t      startRow,
                               size_t      col,
                               CoeffType   relRoffTol);
static int get_rref_lineq_solution(const MatrixType* A,
                                   const MatrixType* b,
                                         MatrixType* x,
                                         MatrixType* isFree,
                                         CoeffType   relRoffTol);

static int get_row_for_var(const MatrixType* A, size_t var, size_t* row)
{
    int rv = 0;
    bool foundPivot = false;
    for (size_t i=0; i<(A->m); i++)
    {
        if ((A->a)[i*(A->n) + var] != 0.0)
        {
            if (foundPivot == false)
            {
                if (ABS((A->a)[i*(A->n) + var] - 1.0) > DBL_EPSILON)
                {
                    rv = -1;
                    break;
                }
                else
                {
                    foundPivot = true;
                    (*row) = i;
                }
            }
            else
            {
                rv = -1;
                break;
            }
                
        }
    }

    return rv;
}

static bool is_free_variable(const MatrixType* A, size_t col)
{
    bool rv = false;
    bool foundPivot = false;
    for (size_t i=0; i<(A->m); i++)
    {
        if ((A->a)[i*(A->n) + col] != 0.0)
        {
            if (foundPivot == false)
            {
                if (ABS((A->a)[i*(A->n) + col] - 1.0) > DBL_EPSILON)
                {
                    rv = true;
                    break;
                }
                else
                {
                    foundPivot = true;
                }
            }
            else
            {
                rv = true;
                break;
            }
                
        }
    }

    return rv;
}

static CoeffType get_roundoff_tolerance(const MatrixType* A)
{
    register CoeffType rv = 0;
    for (size_t i=0; i<(A->m); i++)
    {
        register CoeffType tempRv = 0;
        for (size_t k=0; k<(A->n); k++)
        {
            tempRv += ABS((A->a)[i*(A->n)+k]);
        }
        rv = (tempRv > rv) ? tempRv : rv;
    }
    rv *= DBL_EPSILON;
    rv *= ((A->m) > (A->n)) ? (A->m) : (A->n);
    return rv;
}

static void swap_rows(MatrixType* A,
                      MatrixType* B,
                      size_t      m1,
                      size_t      m2)
{
    for (size_t i=0; i<(A->n); i++)
    {
        register CoeffType temp = (A->a)[m1*(A->n)+i];
        (A->a)[m1*(A->n)+i] = (A->a)[m2*(A->n)+i];
        (A->a)[m2*(A->n)+i] = temp;
    }
    /* swap the rows in the result vector */
    for (size_t i=0; i<(B->n); i++)
    {
        register CoeffType temp = (B->a)[m1*(B->n)+i];
        (B->a)[m1*(B->n)+i] = (B->a)[m2*(B->n)+i];
        (B->a)[m2*(B->n)+i] = temp;
    }
}

static int find_col_max(const MatrixType* A,
                              size_t      col,
                              size_t      startRowIdx,
                              CoeffType*  max,
                              size_t*     maxIdx)
{
    (*max) = (A->a)[startRowIdx*(A->n)+col];
    (*maxIdx) = startRowIdx;
    for (size_t i=startRowIdx; i<(A->m); i++)
    {
        if (ABS(*max) < ABS((A->a)[i*(A->n)+col]))
        {
            (*max) = (A->a)[i*(A->n)+col];
            (*maxIdx) = i;
        }
    }
    return 0;
}

static int normalise_rowA(MatrixType* A,
                          MatrixType* B,
                          size_t      row,
                          size_t      col2use)
{
    register CoeffType div = (A->a)[row*(A->n)+col2use];
    if (div == 0.0)
    {
        return -1;
    }

    /* normalise the row of matrix A */
    for (size_t i=0; i<(A->n); i++)
    {
        (A->a)[row*(A->n)+i] /= div;
    }
    /* normalise the row of matrix B */
    for (size_t i=0; i<(B->n); i++)
    {
        (B->a)[row*(B->n)+i] /= div;
    }

    return 0;
}

static int create_zeroes_below(MatrixType* A,
                               MatrixType* B,
                               size_t      startRow,
                               size_t      col,
                               CoeffType   relRoffTol)
{
    for (size_t i=startRow+1; i<(A->m); i++)
    {
        register CoeffType rowMul = (A->a)[i*(A->n)+col];
        for (size_t k=0;k<(A->n); k++)
        {
            (A->a)[i*(A->n)+k] -= rowMul*(A->a)[startRow*(A->n)+k];
            if ((ABS((A->a)[i*(A->n)+k]) - relRoffTol*get_roundoff_tolerance(A)) < 0)
            {
                (A->a)[i*(A->n)+k] = 0.0;
            }
        }
        for (size_t k=0;k<(B->n); k++)
        {
            (B->a)[i*(B->n)+k] -= rowMul*(B->a)[startRow*(B->n)+k];
            if ((ABS((B->a)[i*(B->n)+k]) - relRoffTol*get_roundoff_tolerance(A)) < 0)
            {
                (B->a)[i*(B->n)+k] = 0.0;
            }
        }
    }
    return 0;
}

static int create_zeroes_above(MatrixType* A,
                               MatrixType* B,
                               size_t      startRow,
                               size_t      col,
                               CoeffType   relRoffTol)
{
    for (size_t i=startRow; i>0; i--)
    {
        register CoeffType rowMul = (A->a)[(i-1)*(A->n)+col];
        for (size_t k=0;k<(A->n); k++)
        {
            (A->a)[(i-1)*(A->n)+k] -= rowMul*(A->a)[startRow*(A->n)+k];
            if ((ABS((A->a)[(i-1)*(A->n)+k]) - relRoffTol*get_roundoff_tolerance(A)) < 0)
            {
                (A->a)[(i-1)*(A->n)+k] = 0.0;
            }
        }
        for (size_t k=0;k<(B->n); k++)
        {
            (B->a)[(i-1)*(B->n)+k] -= rowMul*(B->a)[startRow*(B->n)+k];
            if ((ABS((B->a)[(i-1)*(B->n)+k]) - relRoffTol*get_roundoff_tolerance(A)) < 0)
            {
                (B->a)[(i-1)*(B->n)+k] = 0.0;
            }
        }
    }
    return 0;
}

static int recursive_solver(MatrixType* A,
                            MatrixType* B,
                            size_t      startM,
                            size_t      startN,
                            CoeffType   relRoffTol)
{
    size_t noOfIterations = (((A->m)-startM) < ((A->n)-startN)) ? ((A->m)-startM) : ((A->n)-startN);

    CoeffType colMax = 0;
    size_t colMaxPos = 0;
    do 
    {
        find_col_max(A, startN, startM, &colMax, &colMaxPos);
        startN++;
    } while (colMax == 0.0);
    startN--;
    swap_rows(A, B, startM, colMaxPos);
    normalise_rowA(A, B, startM, startN);
    create_zeroes_below(A, B, startM, startN, relRoffTol);

    if (noOfIterations > 1)
    {
        recursive_solver(A, B, startM+1, startN+1, relRoffTol);
    }

    create_zeroes_above(A, B, startM, startN, relRoffTol);

    return 0;
}

static int get_rref_lineq_solution(const MatrixType* A,
                                   const MatrixType* b,
                                         MatrixType* x,
                                         MatrixType* isFree,
                                         CoeffType   relRoffTol)
{
    CoeffType roundoffTolerance = relRoffTol*get_roundoff_tolerance(A);
    for (size_t i=0; i<(x->m); i++)
    {
        if (is_free_variable(A, i))
        {
            isFree->a[i] = 1.0;
        }
        else
        {
            isFree->a[i] = 0.0;
            /* find row for this variable */
            size_t row = 0;
            get_row_for_var(A, i, &row);
            (x->a)[i] = (b->a)[row];
            for (size_t k=i+1; k<(A->n); k++)
            {
                (x->a)[i] -= ((A->a)[row*(A->n)+k])*((x->a)[k]);
            }
            
            if (ABS((x->a)[i]) < roundoffTolerance)
            {
                (x->a)[i] = 0.0;
            }
        }
    }

    return 0;
}

static bool restrict_eq(const MatrixType *restrict A,
                        const MatrixType *restrict B,
                              CoeffType            relRoffTol)
{
    /* make sure dimensions of A, B and C match */
    if (   (A->m != B->m)
        || (A->n != B->n))
    {
        return false;
    }

    register CoeffType rtolA = relRoffTol*get_roundoff_tolerance(A);
    register CoeffType rtolB = relRoffTol*get_roundoff_tolerance(B);
    register CoeffType rtol = (rtolA > rtolB) ? rtolA : rtolB;

    for (size_t i=0; i<(A->m); i++)
    {
        for (size_t k=0; k<(A->n); k++)
        {
            if (ABS((A->a)[i*(A->n) + k] - (B->a)[i*(A->n) + k] ) > rtol)
            {
                return false;
            }
        }
    }

    return true;
}

int hmath_rref(MatrixType* A,
               MatrixType* B,
               CoeffType relRoffTol)
{
    if (A->m != B->m)
    {
        return -1;
    }
    return recursive_solver(A, B, 0, 0, relRoffTol);
}

int hmath_get_lineq_solution(MatrixType* A,
                             MatrixType* b,
                             MatrixType* x,
                             MatrixType* isFree,
                             CoeffType   relRoffTol)
{
    /* make sure dimensions o A, b, x and isFree match */
    if (   (A->m != b->m)
        || (A->n != x->m)
        || (A->n != isFree->m)
        || (x->n != 1)
        || (isFree->n != 1))
    {
        return -1;
    }

    /* make sure (A|b) is in rref */
    size_t leadingZeroes = 0;
    
    for (size_t i=0; i<(A->m); i++)
    {
        size_t tempLeadingZeroes = 0;
        for (size_t k=0; k<(A->n); k++)
        {
            if ((A->a)[i*(A->n) + k] > 0.0)
            {
                break;
            }
            else
            {
                tempLeadingZeroes++;
            }
        }
        if (   (tempLeadingZeroes > leadingZeroes)
            || ((i==0) && (tempLeadingZeroes == 0)))
        {
            leadingZeroes = tempLeadingZeroes;
        }
        else
        {
            /* bring the matrix (A|b) to the reduced row echelon form */
            int rv = hmath_rref(A, b, relRoffTol);
            if (rv != 0)
            {
                return rv;
            }
            break;
        }
    }

    /* now we are sure that the matrix is in reduced row echelon form */
    return get_rref_lineq_solution(A, b, x, isFree, relRoffTol);
}

/* compares matrices A and B for equality */
bool hmath_eq(const MatrixType* A,
              const MatrixType* B,
                    CoeffType relRoffTol)
{
    if (A == B)
    {
        return true;
    }

    return restrict_eq(A, B, relRoffTol);
}

bool hmath_single_elem_gt(const MatrixType* A,
                                CoeffType b)
{
    for (size_t i=0; i<(A->m); i++)
    {
        for (size_t k=0; k<(A->n); k++)
        {
            if ((A->a)[i*(A->n) + k] > b)
            {
                return true;
            }
        }
    }

    return false;
}

/* calculates A*B = C */
int hmath_mtimes(const MatrixType* A,
                 const MatrixType* B,
                       MatrixType* C,
                       CoeffType   relRoffTol)
{
    /* make sure dimensions of A, B and C match */
    if (   (A->n != B->m)
        || (C->m != A->m)
        || (C->n != B->n))
    {
        return -1;
    }
    
    for (size_t i=0; i<(C->m); i++)
    {
        for (size_t k=0; k<(C->n); k++)
        {
            (C->a)[i*(C->n) + k] = 0;
            for (size_t g=0; g<(A->n); g++)
            {
                (C->a)[i*(C->n) + k] += (A->a)[i*(A->n) + g]* (B->a)[g*(A->n) + k];
            }
        }
    }

    for (size_t i=0; i<(C->m); i++)
    {
        for (size_t k=0; k<(C->n); k++)
        {
            if ((C->a)[i*(C->n) + k] < relRoffTol*get_roundoff_tolerance(C))
            {
                (C->a)[i*(C->n) + k] = 0.0;
            }
        }
    }

    return 0;
}

int hmath_transpose(const MatrixType* A, MatrixType* T)
{
    /* make sure the contents of A fit in the memory of T */
    if (((A->m)*(A->n)) != (T->m)*(T->n))
    {
        return -1;
    }

    T->m = A->n;
    T->n = A->m;
    
    for (size_t i=0; i<(A->m); i++)
    {
        for (size_t k=0; k<(A->n); k++)
        {
            (T->a)[k*(T->n) + i] = (A->a)[i*(A->n) + k];
        }
    }

    return 0;
}

int hmath_flipud(const MatrixType* A, MatrixType* T)
{
    /* make sure the contents of A fit in the memory of T */
    if (((A->m)*(A->n)) != (T->m)*(T->n))
    {
        return -1;
    }

    T->m = A->m;
    T->n = A->n;

    for (size_t i=0; i<(A->n); i++)
    {
        for (size_t k=0; k<(A->m); k++)
        {
            (T->a)[(((T->m) - i - 1)*(T->n) + k)] = (A->a)[i*(A->n) + k];
        }
    }

    return 0;
}

int hmath_fliplr(const MatrixType* A, MatrixType* T)
{
    /* make sure the contents of A fit in the memory of T */
    if (((A->m)*(A->n)) != (T->m)*(T->n))
    {
        return -1;
    }

    T->m = A->m;
    T->n = A->n;

    for (size_t i=0; i<(A->m); i++)
    {
        for (size_t k=0; k<(A->n); k++)
        {
            (T->a)[(i*(T->n) + ((T->n) - k - 1))] = (A->a)[i*(A->n) + k];
        }
    }

    return 0;
}

int hmath_add_elems(      MatrixType* A,
                    const MatrixType* B,
                          size_t      start_row,
                          size_t      start_col)
{
    if (   ((A->m) < (start_row + (B->m)))
        || ((A->n) < (start_col + (B->n))))
    {
        return -1;
    }

    for (size_t i=0; i<(B->m); i++)
    {
        for (size_t k=0; k<(B->n); k++)
        {
            (A->a)[(i+start_row)*(A->n) + (k+start_col)] += (B->a)[i*(B->n) + k];
        }
    }
}
