use calc_graph::{add_calc, Calc1, Calc2, Calc3};

fn main() {
    let empty = Calc1 {};
    let calc2 = Calc2 {};
    let with_calc2 = add_calc(empty, calc2);
    let calc3 = Calc3 {};
    let with_calc3 = add_calc(with_calc2, calc3);
}
