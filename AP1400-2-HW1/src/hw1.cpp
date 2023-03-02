#include "hw1.h"

Matrix algebra::zeros(size_t n, size_t m)
{
    return Matrix(n, std::vector<double>(m, 0));
}

Matrix algebra::ones(size_t n, size_t m)
{
    return Matrix(n, std::vector<double>(m, 1));
}

Matrix algebra::random(size_t n, size_t m, double min, double max)
{
    assert(max >= min);
    // auto generate_random_vector = [min, max, m]() -> std::vector<double>
    // {
    //     std::vector<double> v(m);
    //     auto generate_random_number = [min, max]() -> double
    //     {
    //         return (max - min) * ((double)rand() / (double)RAND_MAX) + min;
    //     };
    //     std::generate(v.begin(), v.end(), generate_random_number);
    //     return v;
    // };

    // Matrix matrix(m);
    // std::generate(matrix.begin(), matrix.end(), generate_random_vector);
    Matrix matrix(n, std::vector<double>(m));
    for (size_t i = 0; i < matrix.size(); i++)
    {
        for (size_t j = 0; j < matrix[i].size(); j++)
        {
            matrix[i][j] = (max - min) * ((double)rand() / (double)RAND_MAX) + min;
        }
    }

    return matrix;
}

void algebra::show(const Matrix &matrix)
{
    for (std::vector<double> v : matrix)
    {
        for (double i : v)
        {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }
}

Matrix algebra::multiply(const Matrix &matrix, double c)
{
    Matrix _matrix(matrix.size(), std::vector<double>(matrix[0].size()));
    for (size_t i = 0; i < matrix.size(); i++)
    {
        for (size_t j = 0; j < matrix[i].size(); j++)
        {
            _matrix[i][j] = matrix[i][j] * c;
        }
    }
    return _matrix;
}

Matrix algebra::multiply(const Matrix &matrix1, const Matrix &matrix2)
{
    if (matrix1.empty() && matrix2.empty())
    {
        return Matrix{};
    }

    int matrix1_row = matrix1.size();
    int matrix2_row = matrix2.size();

    assert(!matrix1.empty() || !matrix2.empty());

    int matrix1_column = matrix1[0].size();
    int matrix2_column = matrix2[0].size();

    assert(matrix1_column == matrix2_row);

    Matrix _matrix(matrix1_row, std::vector<double>(matrix2_column));

    auto dot_prod = [matrix1, matrix2, matrix1_row](int i, int j) -> double
    {
        double result = 0;
        for (size_t k = 0; k < matrix1_row; k++)
        {
            result += matrix1[i][k] * matrix2[k][j];
        }
        return result;
    };

    for (size_t i = 0; i < matrix1_row; i++)
    {
        for (size_t j = 0; j < matrix2_column; j++)
        {
            _matrix[i][j] = dot_prod(i, j);
        }
    }

    return _matrix;
}

Matrix algebra::sum(const Matrix &matrix, double c)
{
    if (matrix.empty())
    {
        return Matrix{};
    }

    Matrix _matrix(matrix.size(), std::vector<double>(matrix[0].size()));
    for (size_t i = 0; i < matrix.size(); i++)
    {
        for (size_t j = 0; j < matrix[i].size(); j++)
        {
            _matrix[i][j] = matrix[i][j] + c;
        }
    }
    return _matrix;
}

Matrix algebra::sum(const Matrix &matrix1, const Matrix &matrix2)
{
    if (matrix1.empty() && matrix2.empty())
    {
        return Matrix{};
    }

    int matrix1_row = matrix1.size();
    int matrix2_row = matrix2.size();
    assert(matrix1_row == matrix2_row);

    int matrix1_column = matrix1[0].size();
    int matrix2_column = matrix2[0].size();
    assert(matrix1_column == matrix2_column);

    Matrix _matrix(matrix1_row, std::vector<double>(matrix1_column));

    for (size_t i = 0; i < matrix1_row; i++)
    {
        for (size_t j = 0; j < matrix1_column; j++)
        {
            _matrix[i][j] = matrix1[i][j] + matrix2[i][j];
        }
    }
    return _matrix;
}

Matrix algebra::transpose(const Matrix &matrix)
{
    if (matrix.empty())
    {
        return Matrix{};
    }

    int n = matrix.size();
    int m = matrix[0].size();

    Matrix _matrix(m, std::vector<double>(n));

    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < m; j++)
        {
            _matrix[j][i] = matrix[i][j];
        }
    }
    return _matrix;
}

Matrix algebra::minor(const Matrix &matrix, size_t n, size_t m)
{
    Matrix _matrix(matrix);

    _matrix.erase(_matrix.begin() + n);

    for (size_t i = 0; i < _matrix.size(); i++)
    {
        _matrix[i].erase(_matrix[i].begin() + m);
    }
    return _matrix;
}

double algebra::determinant(const Matrix &matrix)
{
    if (matrix.empty())
    {
        return 1;
    }

    if (matrix.size() != matrix[0].size())
        throw std::logic_error("non-square matrices have no determinant");

    if (matrix.size() == 2)
    {
        return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
    }
    else if (matrix.size() == 1)
    {
        return matrix[0][0];
    }
    else
    {
        double result = 0;
        for (size_t i = 0; i < matrix.size(); i++)
        {
            result += sign_factor(0, i) * matrix[0][i] * algebra::determinant(algebra::minor(matrix, 0, i));
        }
        return result;
    }
}

double algebra::sign_factor(size_t i, size_t j)
{
    return std::pow(-1.0, i + j);
}

Matrix algebra::inverse(const Matrix &matrix)
{
    if (matrix.empty())
        return Matrix{};

    if (matrix.size() != matrix[0].size())
        throw std::logic_error("non-square matrices have no inverse");

    double det = algebra::determinant(matrix);

    if (det == 0)
        throw std::logic_error("singular matrices have no inverse");

    Matrix adj{matrix.size(), std::vector<double>(matrix.size())};

    for (size_t i = 0; i < matrix.size(); i++)
    {
        for (size_t j = 0; j < matrix.size(); j++)
        {
            Matrix minor{algebra::minor(matrix, i, j)};
            double minor_det = algebra::determinant(minor);
            adj[i][j] = sign_factor(i, j) * minor_det;
        }
    }
    Matrix adj_transpose{algebra::transpose(adj)};
    return Matrix{algebra::multiply(adj_transpose, 1.0 / det)};
}

Matrix algebra::concatenate(const Matrix &matrix1, const Matrix &matrix2, int axis = 0)
{
    if (axis == 0)
    {
        if (matrix1[0].size() != matrix2[0].size())
            throw std::logic_error("matrices with wrong dimensions cannot be concatenated");
        Matrix _matrix{matrix1.size() + matrix2.size(), std::vector<double>(matrix1[0].size())};
        for (size_t i = 0; i < matrix1.size(); i++)
        {
            for (size_t j = 0; j < matrix2[0].size(); j++)
            {
                _matrix[i][j] = matrix1[i][j];
            }
        }
        for (size_t i = matrix1.size(), k = 0; k < matrix2.size(); i++, k++)
        {
            for (size_t j = 0; j < matrix2[0].size(); j++)
            {
                _matrix[i][j] = matrix2[k][j];
            }
        }
        return _matrix;
    }
    else if (axis == 1)
    {
        if (matrix1.size() != matrix2.size())
            throw std::logic_error("matrices with wrong dimensions cannot be concatenated");
        Matrix _matrix{matrix1.size(), std::vector<double>(matrix1[0].size() + matrix2[0].size())};
        for (size_t i = 0; i < matrix1.size(); i++)
        {
            for (size_t j = 0; j < matrix1[0].size(); j++)
            {
                _matrix[i][j] = matrix1[i][j];
            }
            for (size_t j = matrix1[0].size(); (j % matrix1[0].size()) < matrix2[0].size(); j++)
            {
                _matrix[i][j] = matrix2[i][j % matrix1[0].size()];
            }
        }
        return _matrix;
    }
    return Matrix{};
}

Matrix algebra::ero_swap(const Matrix &matrix, size_t r1, size_t r2)
{
    if (r1 >= matrix.size() || r2 >= matrix.size() || r1 < 0 || r2 < 0)
        throw std::logic_error("r1 or r2 inputs are out of range");
    Matrix _matrix = matrix;
    std::swap(_matrix[r1], _matrix[r2]);
    return _matrix;
}

Matrix algebra::ero_multiply(const Matrix &matrix, size_t r, double c)
{
    if (r >= matrix.size() || r <= 0)
        throw std::logic_error("row inputs are out of range");
    Matrix _matrix = matrix;
    for (size_t i = 0; i < matrix[0].size(); i++)
    {
        _matrix[r][i] *= c;
    }
    return _matrix;
}

Matrix algebra::ero_sum(const Matrix &matrix, size_t r1, double c, size_t r2)
{
    if (r1 >= matrix.size() || r2 >= matrix.size() || r1 < 0 || r2 < 0)
        throw std::logic_error("r1 or r2 inputs are out of range");
    Matrix _matrix = matrix;
    for (size_t i = 0; i < matrix[0].size(); i++)
    {
        _matrix[r2][i] += _matrix[r1][i] * c;
    }
    return _matrix;
}

Matrix algebra::upper_triangular(const Matrix &matrix)
{
    if (matrix.empty())
        return Matrix{};
    if (matrix.size() != matrix[0].size())
        throw std::logic_error("non-square matrices have no upper triangular form");

    Matrix _matrix{matrix};
    for (size_t i = 0; i < (_matrix.size() - 1); i++)
    {
        for (size_t j = i + 1; j < matrix.size(); j++)
        {   
            _matrix = algebra::ero_sum(_matrix, i, -(_matrix[j][i] / _matrix[i][i]), j);
        }
    }
    return _matrix;
}