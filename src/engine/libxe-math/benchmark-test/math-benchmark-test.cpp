#include <benchmark/benchmark.h>



#include <iostream>
#include <functional>


namespace nonexpr {
    template<typename T, int N>
    struct TVector {
        using type = T;
        static constexpr int size = N;

        // contains the data
        T values[N];

        T operator[] (const int i) const {
            return values[i];
        }

        T& operator[] (const int i) {
            return values[i];
        }

        TVector<T, N> operator+ (const TVector<T, N> &rhs) const {
            TVector<T, N> result;

            for (int i = 0; i < N; i++) {
                result.values[i] =  values[i] + rhs.values[i];
            }

            return result;
        }
    };
}

namespace expr {
    template<typename T, int N, typename E>
    struct TVectorExpr {
        using type = T;
        static constexpr int size = N;
        static constexpr bool is_leaf = false;

        auto operator[] (const int i) const {
            return that()[i];
        }

        E const& that() const {
            return static_cast<E const&>(*this);
        }
    };


    template<typename T, int N>
    struct TVector : public TVectorExpr<T, N, TVector<T, N>> {
        using type = T;
        static constexpr int size = N;
        static constexpr bool is_leaf = true;

        template<typename E>
        using VectorExpr = TVectorExpr<T, N, E>;

        // evaluates the template expression
        template<typename E>
        TVector(VectorExpr<E> const& expr) {
            for (int i = 0; i < N; i++) {
                values[i] = expr[i];
            }
        }

        TVector(std::initializer_list<T> values) {
            int i = 0;

            for (T value : values) {
                this->values[i++] = value;
            }
        }

        // contains the data
        T values[N];

        T operator[] (const int i) const {
            return values[i];
        }

        T& operator[] (const int i) {
            return values[i];
        }
    };

    template<
        typename T,
        int N,
        typename E1,
        typename E2,
        template<typename> class BinaryOp>
    struct VectorBinOp : public TVectorExpr<T, N, VectorBinOp<T, N, E1, E2, BinaryOp>> {
    public:
        using type = T;
        static constexpr int size = N;

    public:
        VectorBinOp(E1 const &left, E2 const &right) : left(left), right(right) {}

        T operator[] (const int i) const {
            return op(left[i], right[i]);
        }

    private:
        std::conditional_t<E1::is_leaf, const E1&, const E1> left;
        std::conditional_t<E2::is_leaf, const E2&, const E2> right;
        const BinaryOp<T> op{};
    };

    template<typename T, int N, typename E1, typename E2>
    auto operator+ (const TVectorExpr<T, N, E1> &left, const TVectorExpr<T, N, E2> &right) {
        return VectorBinOp<T, N, E1, E2, std::plus> {
            *static_cast<const E1*>(&left),
            *static_cast<const E2*>(&right),
        };
    }
}


nonexpr::TVector<float, 3> test_nonexpr() {
    using namespace nonexpr;

    TVector<float, 3> v1 = {
        0.0f, 0.0f, 0.0f
    };

    TVector<float, 3> v2 = {
        0.0f, 0.0f, 0.0f
    };

    TVector<float, 3> v3 = {
        0.0f, 0.0f, 0.0f
    };

    return v1 + v2 + v3;
}


expr::TVector<float, 3> test_expr() {
    using namespace expr;

    TVector<float, 3> v1 = {
        0.0f, 0.0f, 0.0f
    };

    TVector<float, 3> v2 = {
        0.0f, 0.0f, 0.0f
    };

    TVector<float, 3> v3 = {
        0.0f, 0.0f, 0.0f
    };

    return v1 + v2 + v3;
}



void SomeFunction() {

}

static void BM_SomeFunction(benchmark::State& state) {
    // Perform setup here
    for (auto _ : state) {
        // This code gets timed
        SomeFunction();
    }
}
// Register the function as a benchmark
BENCHMARK(BM_SomeFunction);
// Run the benchmark
BENCHMARK_MAIN();
