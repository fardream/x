#include <cstdio>
#include <tuple>
#include <type_traits>

// calc_graph contains a list of calculations.
// each of the calculation contains a template method in the form of
//   template<typename ...T>
//   do_calc_with_graph(const calc_graph<TContext, T...> &graph)
template <typename TContext, typename... TCalcs>
struct calc_graph {
 public:
  // context_type
  using context_type = TContext;

  TContext context;
  std::tuple<TCalcs...> calcs;

  void do_calc() {
    (std::get<TCalcs>(this->calcs).do_calc_with_graph(*this), ...);
  }
};

// general calc_list
template <typename TContext, typename... T>
struct calc_list {
  using type = calc_graph<TContext, T...>;
};

// specialized for calc_graph
template <typename TContext, typename... T>
struct calc_list<calc_graph<TContext, T...>> : calc_list<TContext, T...> {};

// Add U to the calc list only if U is not already in the calc list.
template <typename TContext, typename U, typename... Us, typename... Ts>
  requires requires(U c, const calc_graph<TContext, Us...>& d) {
    c.do_calc_with_graph(d);
  }
struct calc_list<calc_graph<TContext, Us...>, U, Ts...>
    : std::conditional_t<std::disjunction_v<std::is_same<U, Us>...>,
                         calc_list<calc_graph<TContext, Us...>, Ts...>,
                         calc_list<calc_graph<TContext, Us..., U>, Ts...>> {};

// short hand type to construct a calc_graph.
template <typename TContext, typename... T>
using new_calc_graph_t = typename calc_list<calc_graph<TContext>, T...>::type;

template <typename TContext>
using with_context_t = calc_graph<TContext>;

// add a calculation and its dependencies to the calc.
template <typename TGraph, typename TCalc, typename... TDeps>
  requires requires(
               TCalc c,
               const calc_graph<typename TGraph::context_type, TDeps...> v) {
    c.do_calc_with_graph(v);
  }
using add_calc_t = typename calc_list<TGraph, TDeps..., TCalc>::type;

// check if Ts contains the given dependency calc
template <typename TDep, typename... Ts>
inline constexpr bool contains_dep_v =
    std::disjunction_v<std::is_same<TDep, Ts>...>;

struct calc1 {
 public:
  double v;
  template <typename... T>
  void do_calc_with_graph(const calc_graph<double, T...>& d) {
    std::printf("in calc 1\n");
    this->do_calc(d.context);
  }

  void do_calc(double x) { this->v = x * 2; }
};

struct calc2 {
 public:
  double another_v;

  void another_calc(const double x, const calc1& c1) {
    this->another_v = x + c1.v;
  }

  template <typename... T>
    requires contains_dep_v<calc1, T...>
  void do_calc_with_graph(const calc_graph<double, T...>& d) {
    std::printf("in calc 2\n");
    this->another_calc(d.context, std::get<calc1>(d.calcs));
  }
};

struct calc3 {
 public:
  double v;

  void calc(const calc1& c1, const calc2& c2) { this->v = c1.v + c2.another_v; }

  template <typename... T>
    requires contains_dep_v<calc1, T...> && contains_dep_v<calc2, T...>
  void do_calc_with_graph(const calc_graph<double, T...>& d) {
    std::printf("in calc 3\n");
    this->calc(std::get<calc1>(d.calcs), std::get<calc2>(d.calcs));
  }
};

struct calc4 {
 public:
  double v;
  void calc(const calc1& c1, const calc2& c2) {
    this->v = c1.v + c2.another_v * 2;
  }
  template <typename... T>
    requires contains_dep_v<calc1, T...> && contains_dep_v<calc2, T...>
  void do_calc_with_graph(const calc_graph<double, T...>& d) {
    std::printf("in calc 4\n");
    this->calc(std::get<calc1>(d.calcs), std::get<calc2>(d.calcs));
  }
};

int main() {
  using empty = with_context_t<double>;
  using with_calc_1 = add_calc_t<empty, calc1>;
  using with_calc_2 = add_calc_t<with_calc_1, calc2, calc1>;
  using with_calc_3 = add_calc_t<with_calc_2, calc3, calc1, calc2>;
  using with_calc_4 = add_calc_t<with_calc_3, calc4, calc2, calc1>;

  with_calc_4 d;

  d.do_calc();

  return 0;
}
