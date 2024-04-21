#pragma once

class Matrix
{
private:
    vector<double>elements;
    int rows;
    int cols;
    Parallelizer* parallel_engine;



public:

    void setElement(double element, int col, int row)
    {
        elements[row * cols + col] = element;
    }

    double getElement(int col, int row) const
    {
        return elements[row * cols + col];
    }

    int getRows() const
    {
        return rows;
    }

    int getCols() const
    {
        return cols;
    }

    void setEngine(Parallelizer* engine)
    {
        parallel_engine = engine;
    }


    Matrix(int rows, int cols) :elements()
    {
        elements.resize(cols * rows);
        this->rows = rows;
        this->cols = cols;
    }

    Matrix(Matrix& old)
    {
        cols = old.getCols();
        rows = old.getRows();
        elements.resize(cols * rows);
        for (int i = 0; i < elements.size(); i++)
        {
            elements[i] = old.elements[i];
        }
    }

    Matrix(Matrix&& old)
    {
        cols = old.getCols();
        rows = old.getRows();
        elements.resize(cols * rows);
        for (int i = 0; i < elements.size(); i++)
        {
            elements[i] = old.elements[i];
        }
    }

    Matrix& operator=(const Matrix& right)
    {
        cols = right.getCols();
        rows = right.getRows();
        elements.resize(cols * rows);
        for (int i = 0; i < elements.size(); i++)
        {
            elements[i] = right.elements[i];
        }
        return *this;
    }

    friend Matrix operator*(Matrix& left, Matrix& right);


};

Matrix operator*(Matrix& left, Matrix& right)
{

    left.parallel_engine->flush();
    int new_rows = left.getRows();
    int new_cols = right.getCols();
    int subvector_number = right.getRows();
    double* first_step_vectors = (double*)malloc(sizeof(double) * (new_rows * subvector_number * new_cols));
    int offset = 0;

    for (int i = 0; i < new_cols; i++)
    {

        for (int j = 0; j < subvector_number; j++)
        {
            double scalar = right.getElement(i, j);
            left.parallel_engine->parallelize([=, &left]() {

                for (int k = 0; k < new_rows; k++)
                {
                    first_step_vectors[offset + k] = scalar * left.getElement(j, k);
                }

            });

            offset += new_rows;
        }
    }

    left.parallel_engine->wait_until_done();
    left.parallel_engine->flush();

    Matrix result(new_rows, new_cols);

    for (int i = 0; i < new_cols; i++)
    {

        auto lambda = [i, new_rows, subvector_number, first_step_vectors, &result]() {
            for (int j = 0; j < new_rows; j++)
            {
                double sum = 0;
                for (int k = 0; k < subvector_number; k++)
                {
                    sum += first_step_vectors[i * subvector_number * new_rows + new_rows * k + j];
                }
                result.setElement(sum, i, j);
            }
        };
        left.parallel_engine->parallelize(lambda);
    }

    left.parallel_engine->wait_until_done();
    left.parallel_engine->flush();
    free(first_step_vectors);
    return result;
}