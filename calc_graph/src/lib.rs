pub trait GraphCalc {
    fn do_calc(&mut self);
}

pub trait Has<T> {
    fn get(&self) -> &T;
}

pub trait GraphNodeCalc<T> {
    fn do_calc(&mut self, graph: &T);
}

pub struct WithCalc<Before: GraphCalc, Calc: GraphNodeCalc<Before>> {
    before: Before,
    calc: Calc,
}

impl<Before: GraphCalc, Calc: GraphNodeCalc<Before>> Has<Calc> for WithCalc<Before, Calc> {
    fn get(&self) -> &Calc {
        &self.calc
    }
}
// // Below doesn't work
// impl<Before: GraphCalc + Has<T>, T, Calc> Has<T> for WithCalc<Before, Calc> {
//     fn get(&self) -> &Calc {
//         &self.before.do_calc()
//     }
// }

impl Has<Calc1> for WithCalc<Calc1, Calc2> {
    fn get(&self) -> &Calc1 {
        self.before.get()
    }
}

impl<Before: GraphCalc, Calc: GraphNodeCalc<Before>> GraphCalc for WithCalc<Before, Calc> {
    fn do_calc(&mut self) {
        self.before.do_calc();
        self.calc.do_calc(&self.before);
    }
}

pub struct Calc1;

impl Has<Calc1> for Calc1 {
    fn get(&self) -> &Calc1 {
        self
    }
}

impl<T> GraphNodeCalc<T> for Calc1 {
    fn do_calc(&mut self, graph: &T) {}
}

impl GraphCalc for Calc1 {
    fn do_calc(&mut self) {}
}

pub struct Calc2;

impl<T: Has<Calc1>> GraphNodeCalc<T> for Calc2 {
    fn do_calc(&mut self, graph: &T) {
        let _c1: &Calc1 = graph.get();
    }
}

pub struct Calc3;

impl<T: Has<Calc1> + Has<Calc2>> GraphNodeCalc<T> for Calc3 {
    fn do_calc(&mut self, graph: &T) {}
}

pub fn add_calc<Before: GraphCalc, Calc: GraphNodeCalc<Before>>(
    before: Before,
    calc: Calc,
) -> WithCalc<Before, Calc> {
    WithCalc { before, calc }
}
