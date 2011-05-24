
#ifndef _RIPOSTE_INTERNAL_H
#define _RIPOSTE_INTERNAL_H

#include "value.h"
#include "exceptions.h"
#include <math.h>

inline void _error(std::string message) {
	throw RiposteError(message);
}

inline void _warning(State& state, std::string const& message) {
	state.warnings.push_back(message);
}

void addMathOps(State& state);

inline Vector Clone(Vector const& in, uint64_t length) {
	Vector out(in.type, length);
	memcpy(out.data(), in.data(), std::min(length, in.length())*in.width());
	out.attributes = in.attributes;
	return out;
}

inline Vector Clone(Vector const& in) {
	return Clone(in, in.length());
}

template<class T>
T Clone(T const& in) {
	T out(in.length());
	memcpy(out.data(), in.data(), in.length()*sizeof(typename T::Element));
	out.attributes = in.attributes;
	return out;	
}

inline Value force(State& state, Value v) { 
	while(v.type == Type::I_promise) {
		eval(state, Closure(v)); 
		v = state.registers[0]; 
	} 
	return v; 
}
inline Value expression(Value const& v) { 
	if(v.type == Type::I_promise)
		return Closure(v).expression();
	else return v; 
}

inline double asReal1(Value const& v) { if(v.type != Type::R_double && v.type != Type::R_integer) _error("Can't cast argument to number"); if(v.type == Type::R_integer) return Integer(v)[0]; else return Double(v)[0]; }

// Casting functions (default is to attempt a C coercion)
template<class I, class O> struct Cast {
	typedef I A;
	typedef I TA;
	typedef O R;
	static typename O::Element eval(typename I::Element const& i) { return (typename O::Element)i; }
};

// More involved casting functions
template<> struct Cast<Logical, Double> {
	typedef Logical A;
	typedef Double R;
	static Double::Element eval(Logical::Element const& i) { return i ? 1.0 : 0.0; }
};
template<> struct Cast<Logical, Integer> {
	typedef Logical A;
	typedef Integer R;
	static Integer::Element eval(Logical::Element const& i) { return i ? 1 : 0; }
};
template<> struct Cast<Double, Logical> {
	typedef Double A;
	typedef Logical R;
	static Logical::Element eval(Double::Element const& i) { return i != 0.0 ? 1 : 0; }
};
template<> struct Cast<Integer, Logical> {
	typedef Integer A;
	typedef Logical R;
	static Logical::Element eval(Integer::Element const& i) { return i != 0 ? 1 : 0; }
};
template<> struct Cast<Character, Logical> {
	typedef Character A;
	typedef Logical R;
	static Logical::Element eval(Character::Element const& i) { return i.i != 0 ? 1 : 0; }
};
template<> struct Cast<Character, Integer> {
	typedef Character A;
	typedef Integer R;
	static Integer::Element eval(Character::Element const& i) { return i.i != 0 ? 1 : 0; }
};
template<> struct Cast<Character, Double> {
	typedef Character A;
	typedef Double R;
	static Double::Element eval(Character::Element const& i) { return i.i != 0 ? 1 : 0; }
};

template<class X, class Y, class Z>
struct UnaryOp {
	typedef X A;
	typedef Y TA;
	typedef Z R;
	
};

template<typename A, typename TA, typename R>
struct PosOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		return Cast<A, TA>::eval(a);
	}
};

template<typename A, typename TA, typename R>
struct NegOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		return -Cast<A, TA>::eval(a);
	}
};

template<typename A, typename TA, typename R>
struct LNotOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		return !Cast<A, TA>::eval(a);
	}
};

template<typename A, typename TA, typename R>
struct AbsOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		return fabs(Cast<A, TA>::eval(a));
	}
};

template<typename A, typename TA, typename R>
struct SignOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		typename TA::Element ta = Cast<A, TA>::eval(a);
		return ta > 0 ? 1 : (ta < 0 ? -1 : 0);
	}
};

template<typename A, typename TA, typename R>
struct SqrtOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		return sqrt(Cast<A, TA>::eval(a));
	}
};

template<typename A, typename TA, typename R>
struct FloorOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		return floor(Cast<A, TA>::eval(a));
	}
};

template<typename A, typename TA, typename R>
struct CeilingOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		return ceil(Cast<A, TA>::eval(a));
	}
};

template<typename A, typename TA, typename R>
struct TruncOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		typename TA::Element ta = Cast<A, TA>::eval(a);
		return ta >= 0 ? floor(ta) : ceil(ta);
	}
};

template<typename A, typename TA, typename R>
struct RoundOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		return round(Cast<A, TA>::eval(a));
	}
};

template<typename A, typename TA, typename R>
struct SignifOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		// NYI
		return round(Cast<A, TA>::eval(a));
	}
};

template<typename A, typename TA, typename R>
struct ExpOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		return exp(Cast<A, TA>::eval(a));
	}
};

template<typename A, typename TA, typename R>
struct LogOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		return log(Cast<A, TA>::eval(a));
	}
};

template<typename A, typename TA, typename R>
struct CosOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		return cos(Cast<A, TA>::eval(a));
	}
};

template<typename A, typename TA, typename R>
struct SinOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		return sin(Cast<A, TA>::eval(a));
	}
};

template<typename A, typename TA, typename R>
struct TanOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		return tan(Cast<A, TA>::eval(a));
	}
};

template<typename A, typename TA, typename R>
struct ACosOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		return acos(Cast<A, TA>::eval(a));
	}
};

template<typename A, typename TA, typename R>
struct ASinOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		return asin(Cast<A, TA>::eval(a));
	}
};

template<typename A, typename TA, typename R>
struct ATanOp : UnaryOp<A, TA, R> {
	static typename R::Element eval(typename A::Element const& a) {
		return atan(Cast<A, TA>::eval(a));
	}
};

template<class Op>
struct NA1 : UnaryOp<typename Op::A, typename Op::TA, typename Op::R> {
	static typename Op::R::Element eval(typename Op::A::Element const& a) {
		if(!Op::A::CheckNA || !Op::A::isNA(a))
			return Op::eval(a);
		else
			return Op::R::NAelement;
	}
};


// Binary operators

template<typename X, typename TX, typename Y, typename TY, typename Z> struct BinaryOp {
	typedef X A;
	typedef TX TA;
	typedef Y B;
	typedef TY TB;
	typedef Z R;
};

template<typename A, typename TA, typename B, typename TB, typename R>
struct AddOp : BinaryOp<A, TA, B, TB, R> {
	static typename R::Element eval(typename A::Element const& a, typename B::Element const& b) {
		return Cast<A, TA>::eval(a) + Cast<B, TB>::eval(b);
	}
};

template<typename A, typename TA, typename B, typename TB, typename R>
struct SubOp : BinaryOp<A, TA, B, TB, R> {
	static typename R::Element eval(typename A::Element const& a, typename B::Element const& b) {
		return Cast<A, TA>::eval(a) - Cast<B, TB>::eval(b);
	}
};

template<typename A, typename TA, typename B, typename TB, typename R>
struct MulOp : BinaryOp<A, TA, B, TB, R> {
	static typename R::Element eval(typename A::Element const& a, typename B::Element const& b) {
		return Cast<A, TA>::eval(a) * Cast<B, TB>::eval(b);
	}
};

template<typename A, typename TA, typename B, typename TB, typename R>
struct DivOp : BinaryOp<A, TA, B, TB, R> {
	static typename R::Element eval(typename A::Element const& a, typename B::Element const& b) {
		return Cast<A, TA>::eval(a) / Cast<B, TB>::eval(b);
	}
};

inline double IDiv(double a, double b) { return floor(a/b); /* TODO: Replace with ugly R version */ }
inline int64_t IDiv(int64_t a, int64_t b) { return a/b; }

template<typename A, typename TA, typename B, typename TB, typename R>
struct IDivOp : BinaryOp<A, TA, B, TB, R> {
	static typename R::Element eval(typename A::Element const& a, typename B::Element const& b) {
		return IDiv( Cast<A, TA>::eval(a), Cast<B, TB>::eval(b));
	}
};

template<typename A, typename TA, typename B, typename TB, typename R>
struct PowOp : BinaryOp<A, TA, B, TB, R> {
	static typename R::Element eval(typename A::Element const& a, typename B::Element const& b) {
		return pow( Cast<A, TA>::eval(a), Cast<B, TB>::eval(b));
	}
};

inline double Mod(double a, double b) { return a - IDiv(a,b) * b; /* TODO: Replace with ugly R version */ }
inline int64_t Mod(int64_t a, int64_t b) { return a % b; }

template<typename A, typename TA, typename B, typename TB, typename R>
struct ModOp : BinaryOp<A, TA, B, TB, R> {
	static typename R::Element eval(typename A::Element const& a, typename B::Element const& b) {
		return Mod( Cast<A, TA>::eval(a), Cast<B, TB>::eval(b));
	}
};

template<typename A, typename TA, typename B, typename TB, typename R>
struct LTOp : BinaryOp<A, TA, B, TB, R> {
	static typename R::Element eval(typename A::Element const& a, typename B::Element const& b) {
		return Cast<A, TA>::eval(a) < Cast<B, TB>::eval(b);
	}
};

template<typename A, typename TA, typename B, typename TB, typename R>
struct GTOp : BinaryOp<A, TA, B, TB, R> {
	static typename R::Element eval(typename A::Element const& a, typename B::Element const& b) {
		return Cast<A, TA>::eval(a) > Cast<B, TB>::eval(b);
	}
};

template<typename A, typename TA, typename B, typename TB, typename R>
struct EqOp : BinaryOp<A, TA, B, TB, R> {
	static typename R::Element eval(typename A::Element const& a, typename B::Element const& b) {
		return Cast<A, TA>::eval(a) == Cast<B, TB>::eval(b);
	}
};

template<typename A, typename TA, typename B, typename TB, typename R>
struct NeqOp : BinaryOp<A, TA, B, TB, R> {
	static typename R::Element eval(typename A::Element const& a, typename B::Element const& b) {
		return Cast<A, TA>::eval(a) != Cast<B, TB>::eval(b);
	}
};

template<typename A, typename TA, typename B, typename TB, typename R>
struct GEOp : BinaryOp<A, TA, B, TB, R> {
	static typename R::Element eval(typename A::Element const& a, typename B::Element const& b) {
		return Cast<A, TA>::eval(a) >= Cast<B, TB>::eval(b);
	}
};

template<typename A, typename TA, typename B, typename TB, typename R>
struct LEOp : BinaryOp<A, TA, B, TB, R> {
	static typename R::Element eval(typename A::Element const& a, typename B::Element const& b) {
		return Cast<A, TA>::eval(a) <= Cast<B, TB>::eval(b);
	}
};

template<typename A, typename TA, typename B, typename TB, typename R>
struct AndOp : BinaryOp<A, TA, B, TB, R> {
	static typename R::Element eval(typename A::Element const& a, typename B::Element const& b) {
		if(A::isNA(a)) return Cast<B, TB>::eval(b) ? R::NAelement : false;
		else if(B::isNA(b)) return Cast<A, TA>::eval(a) ? R::NAelement : false;
		else return Cast<A, TA>::eval(a) && Cast<B, TB>::eval(b);
	}
};

template<typename A, typename TA, typename B, typename TB, typename R>
struct OrOp : BinaryOp<A, TA, B, TB, R> {
	static typename R::Element eval(typename A::Element const& a, typename B::Element const& b) {
		if(A::isNA(a)) return Cast<B, TB>::eval(b) ? true : R::NAelement;
		else if(B::isNA(b)) return Cast<A, TA>::eval(a) ? true : R::NAelement;
		return Cast<A, TA>::eval(a) || Cast<B, TB>::eval(b);
	}
};

template<class Op>
struct NA2 : BinaryOp<typename Op::A, typename Op::TA, typename Op::B, typename Op::TB, typename Op::R> {
	static typename Op::R::Element eval(typename Op::A::Element const& a, typename Op::B::Element const& b) {
		if((!Op::A::CheckNA || !Op::A::isNA(a)) && (!Op::B::CheckNA || !Op::B::isNA(b)))
			return Op::eval(a, b);
		else
			return Op::R::NAelement;
	}
};


template< class Op >
struct Zip1 {
	static typename Op::R eval(typename Op::A const& a)
	{
		typename Op::R r = typename Op::R(a.length());
		for(uint64_t i = 0; i < a.length(); ++i) {
			r[i] = Op::eval(a[i]);
		}
		return r;
	}
};

/*
template<UnaryOp func>
struct ZipInto {
	static void eval(void* in, uint64_t in_index, void* out, uint64_t out_index, uint64_t length)
	{
		for(uint64_t i = 0; i < length; ++i) {
			func(in, in_index+i, out, out_index+i);
		}
	}
};
*/

template< class Op >
struct Zip2 {
	static typename Op::R eval(typename Op::A const& a, typename Op::B const& b)
	{
		if(a.length() == b.length()) {
			typename Op::R r(a.length());
			for(uint64_t i = 0; i < a.length(); ++i) {
				r[i] = Op::eval(a[i], b[i]);
			}
			return r;
		}
		else if(a.length() == 0 || b.length() == 0) {
			return typename Op::R(0);
		}
		else if(b.length() == 1) {
			typename Op::R r(a.length());
			for(uint64_t i = 0; i < a.length(); ++i) {
				r[i] = Op::eval(a[i], b[0]);
			}
			return r;
		}
		else if(a.length() == 1) {
			typename Op::R r(b.length());
			for(uint64_t i = 0; i < b.length(); ++i) {
				r[i] = Op::eval(a[0], b[i]);
			}
			return r;
		}
		else if(a.length() > b.length()) {
			typename Op::R r(a.length());
			uint64_t j = 0;
			for(uint64_t i = 0; i < a.length(); ++i) {
				r[i] = Op::eval(a[i], b[j]);
				++j;
				if(j >= b.length()) j = 0;
			}
			return r;
		}
		else {
			typename Op::R r(b.length());
			uint64_t j = 0;
			for(uint64_t i = 0; i < b.length(); ++i) {
				r[i] = Op::eval(a[j], b[i]);
				++j;
				if(j >= a.length()) j = 0;
			}
			return r;
		}
	}
};

template< class A, class Index >
struct SubsetIndex {
	static A eval(A const& a, Index const& d)
	{
		// compute length without 0s
		uint64_t outlength = 0;
		for(uint64_t i = 0; i < d.length(); i++)
			if( Cast<Index, Integer>::eval(d[i]) != 0)
				outlength++;
	
		A r(outlength);	
		uint64_t j = 0;
		for(uint64_t i = 0; i < d.length(); i++) {	
			int64_t idx = Cast<Index, Integer>::eval(d[i]);
			if(idx != 0)
				r[j++] = a[idx-1];
		}
		return r;
	}
};


template< class A, class Index, class B >
struct SubsetAssign {
	static A eval(A const& a, Index const& d, B const& b)
	{
		// compute max index 
		int64_t outlength = 0;
		for(uint64_t i = 0; i < d.length(); i++) {
			int64_t idx = Cast<Index, Integer>::eval(d[i]);
			outlength = std::max((int64_t)outlength, idx);
		}

		// should use max index here to extend vector if necessary	
		A r = a;//Clone(a);	
		for(uint64_t i = 0; i < d.length(); i++) {	
			int64_t idx = Cast<Index, Integer>::eval(d[i]);
			if(idx != 0)
				r[idx-1] = Cast<B, A>::eval(b[i]);
		}
		return r;
	}
};

template< 
	template<class Op> class Lift,
	template<typename A, typename TA, typename R> class Op > 
void unaryArith(State& state, Value const& a, Value& c) {

	if(a.type == Type::R_double) {
		c = Lift< NA1< Op<Double, Double, Double> > >::eval(a);
	}
	else if(a.type == Type::R_integer) {
		c = Lift< NA1< Op<Integer, Integer, Integer> > >::eval(a);
	}
	else if(a.type == Type::R_logical) {
		c = Lift< NA1< Op<Logical, Integer, Integer> > >::eval(a);
	}
	else {
		_error("non-numeric argument to unary numeric operator");
	}
};

template< 
	template<class Op> class Lift,
	template<typename A, typename TA, typename R> class Op > 
void unaryLogical(State& state, Value const& a, Value& c) {
	
	if(a.type == Type::R_double) {
		c = Lift< Op<Double, Logical, Logical> >::eval(a);
	}
	else if(a.type == Type::R_integer) {
		c = Lift< Op<Integer, Logical, Logical> >::eval(a);
	}
	else if(a.type == Type::R_logical) {
		c = Lift< Op<Logical, Logical, Logical> >::eval(a);
	}
	else {
		_error("non-numeric argument to unary logical operator");
	}
};

template< 
	template<class Op> class Lift,
	template<typename A, typename TA, typename B, typename TB, typename R> class Op > 
void binaryArith(State& state, Value const& a, Value const& b, Value& c) {

	if(a.type == Type::R_double && b.type == Type::R_double) {
		c = Lift< NA2< Op<Double, Double, Double, Double, Double> > >::eval(a, b);
	}
	else if(a.type == Type::R_integer && b.type == Type::R_double) {
		c = Lift< NA2< Op<Integer, Double, Double, Double, Double> > >::eval(a, b);
	}
	else if(a.type == Type::R_double && b.type == Type::R_integer) {
		c = Lift< NA2< Op<Double, Double, Integer, Double, Double> > >::eval(a, b);
	}
	else if(a.type == Type::R_integer && b.type == Type::R_integer) {
		c = Lift< NA2< Op<Integer, Integer, Integer, Integer, Integer> > >::eval(a, b);
	}
	else if(a.type == Type::R_double && b.type == Type::R_logical) {
		c = Lift< NA2< Op<Double, Double, Logical, Double, Double> > >::eval(a, b);
	}
	else if(a.type == Type::R_logical && b.type == Type::R_double) {
		c = Lift< NA2< Op<Logical, Double, Double, Double, Double> > >::eval(a, b);
	}
	else if(a.type == Type::R_integer && b.type == Type::R_logical) {
		c = Lift< NA2< Op<Integer, Integer, Logical, Integer, Integer> > >::eval(a, b);
	}
	else if(a.type == Type::R_logical && b.type == Type::R_integer) {
		c = Lift< NA2< Op<Logical, Integer, Integer, Integer, Integer> > >::eval(a, b);
	}
	else if(a.type == Type::R_logical && b.type == Type::R_logical) {
		c = Lift< NA2< Op<Logical, Integer, Logical, Integer, Integer> > >::eval(a, b);
	}
	else {
		_error("non-numeric argument to binary numeric operator");
	}
}

template< 
	template<class Op> class Lift,
	template<typename A, typename TA, typename B, typename TB, typename R> class Op > 
void binaryDoubleArith(State& state, Value const& a, Value const& b, Value& c) {

	if(a.type == Type::R_double && b.type == Type::R_double) {
		c = Lift< NA2< Op<Double, Double, Double, Double, Double> > >::eval(a, b);
	}
	else if(a.type == Type::R_integer && b.type == Type::R_double) {
		c = Lift< NA2< Op<Integer, Double, Double, Double, Double> > >::eval(a, b);
	}
	else if(a.type == Type::R_double && b.type == Type::R_integer) {
		c = Lift< NA2< Op<Double, Double, Integer, Double, Double> > >::eval(a, b);
	}
	else if(a.type == Type::R_integer && b.type == Type::R_integer) {
		c = Lift< NA2< Op<Integer, Double, Integer, Double, Double> > >::eval(a, b);
	}
	else if(a.type == Type::R_double && b.type == Type::R_logical) {
		c = Lift< NA2< Op<Double, Double, Logical, Double, Double> > >::eval(a, b);
	}
	else if(a.type == Type::R_logical && b.type == Type::R_double) {
		c = Lift< NA2< Op<Logical, Double, Double, Double, Double> > >::eval(a, b);
	}
	else if(a.type == Type::R_integer && b.type == Type::R_logical) {
		c = Lift< NA2< Op<Integer, Double, Logical, Double, Double> > >::eval(a, b);
	}
	else if(a.type == Type::R_logical && b.type == Type::R_integer) {
		c = Lift< NA2< Op<Logical, Double, Integer, Double, Double> > >::eval(a, b);
	}
	else if(a.type == Type::R_logical && b.type == Type::R_logical) {
		c = Lift< NA2< Op<Logical, Double, Logical, Double, Double> > >::eval(a, b);
	}
	else {
		_error("non-numeric argument to numeric operator");
	}
}

template< 
	template<class Op> class Lift,
	template<typename A, typename TA, typename B, typename TB, typename R> class Op > 
void binaryLogical(State& state, Value const& a, Value const& b, Value& c) {

	if(a.type == Type::R_double && b.type == Type::R_double) {
		c = Lift< Op<Double, Logical, Double, Logical, Logical> >::eval(a, b);
	}
	else if(a.type == Type::R_integer && b.type == Type::R_double) {
		c = Lift< Op<Integer, Logical, Double, Logical, Logical> >::eval(a, b);
	}
	else if(a.type == Type::R_double && b.type == Type::R_integer) {
		c = Lift< Op<Double, Logical, Integer, Logical, Logical> >::eval(a, b);
	}
	else if(a.type == Type::R_integer && b.type == Type::R_integer) {
		c = Lift< Op<Integer, Logical, Integer, Logical, Logical> >::eval(a, b);
	}
	else if(a.type == Type::R_double && b.type == Type::R_logical) {
		c = Lift< Op<Double, Logical, Logical, Logical, Logical> >::eval(a, b);
	}
	else if(a.type == Type::R_logical && b.type == Type::R_double) {
		c = Lift< Op<Logical, Logical, Double, Logical, Logical> >::eval(a, b);
	}
	else if(a.type == Type::R_integer && b.type == Type::R_logical) {
		c = Lift< Op<Integer, Logical, Logical, Logical, Logical> >::eval(a, b);
	}
	else if(a.type == Type::R_logical && b.type == Type::R_integer) {
		c = Lift< Op<Logical, Logical, Integer, Logical, Logical> >::eval(a, b);
	}
	else if(a.type == Type::R_logical && b.type == Type::R_logical) {
		c = Lift< Op<Logical, Logical, Logical, Logical, Logical> >::eval(a, b);
	}
	else {
		_error("non-logical argument to binary logical operator");
	}
}

template< 
	template<class Op> class Lift,
	template<typename A, typename TA, typename B, typename TB, typename R> class Op > 
void binaryOrdinal(State& state, Value const& a, Value const& b, Value& c) {
	
	if(a.type == Type::R_double && b.type == Type::R_double) {
		c = Lift< NA2< Op<Double, Double, Double, Double, Logical> > >::eval(a, b);
	}
	else if(a.type == Type::R_integer && b.type == Type::R_double) {
		c = Lift< NA2< Op<Integer, Double, Double, Double, Logical> > >::eval(a, b);
	}
	else if(a.type == Type::R_double && b.type == Type::R_integer) {
		c = Lift< NA2< Op<Double, Double, Integer, Double, Logical> > >::eval(a, b);
	}
	else if(a.type == Type::R_integer && b.type == Type::R_integer) {
		c = Lift< NA2< Op<Integer, Integer, Integer, Integer, Logical> > >::eval(a, b);
	}
	else if(a.type == Type::R_double && b.type == Type::R_logical) {
		c = Lift< NA2< Op<Double, Double, Logical, Double, Logical> > >::eval(a, b);
	}
	else if(a.type == Type::R_logical && b.type == Type::R_double) {
		c = Lift< NA2< Op<Logical, Double, Double, Double, Logical> > >::eval(a, b);
	}
	else if(a.type == Type::R_integer && b.type == Type::R_logical) {
		c = Lift< NA2< Op<Integer, Integer, Logical, Integer, Logical> > >::eval(a, b);
	}
	else if(a.type == Type::R_logical && b.type == Type::R_integer) {
		c = Lift< NA2< Op<Logical, Integer, Integer, Integer, Logical> > >::eval(a, b);
	}
	else if(a.type == Type::R_logical && b.type == Type::R_logical) {
		c = Lift< NA2< Op<Logical, Logical, Logical, Logical, Logical> > >::eval(a, b);
	}
	else {
		_error("non-ordinal argument to ordinal operator");
	}
}

inline Vector As(Vector a, Type type) {
	Vector r;
        if(a.type == Type::R_double && type == Type::R_double) {
                r = a;
        }
        else if(a.type == Type::R_integer && type == Type::R_double) {
                r = Zip1< NA1< Cast<Integer, Double> > >::eval(a);
        }
        else if(a.type == Type::R_double && type == Type::R_integer) {
                r = Zip1< Cast<Double, Integer> >::eval(a);
        }
        else if(a.type == Type::R_integer && type == Type::R_integer) {
                r = a;
        }
        else if(a.type == Type::R_logical && type == Type::R_double) {
                r = Zip1< Cast<Logical, Double> >::eval(a);
        }
        else if(a.type == Type::R_logical && type == Type::R_integer) {
                r = Zip1< Cast<Logical, Integer> >::eval(a);
        }
        else if(a.type == Type::R_character && type == Type::R_double) {
                r = Zip1< Cast<Character, Double> >::eval(a);
        }
        else if(a.type == Type::R_character && type == Type::R_integer) {
                r = Zip1< Cast<Character, Integer> >::eval(a);
        }
        else if(a.type == Type::R_double && type == Type::R_logical) {
                r = Zip1< Cast<Double, Logical> >::eval(a);
        }
        else if(a.type == Type::R_integer && type == Type::R_logical) {
                r = Zip1< Cast<Integer, Logical> >::eval(a);
        }
        else if(a.type == Type::R_logical && type == Type::R_logical) {
        	r = a;
	}
        else if(a.type == Type::R_character && type == Type::R_character) {
                r = a;
        }
        else if(a.type == Type::R_list && type == Type::R_list) {
                r = a;
        }
        else if(a.type == Type::R_call && type == Type::R_call) {
                r = a;
        }
        else if(a.type == Type::R_call && type == Type::R_list) {
                r = List(a);
        }
        else if(a.type == Type::R_list && type == Type::R_call) {
                r = Call(a);
        }
        else {
                _error("Invalid cast");
        }
	return r;
}

inline void subAssign(State& state, Value const& a, Value const& i, Value const& b, Value& c) {

	Vector idx(i);
	idx = As(i, Type::R_integer);

        if(a.type == Type::R_double && b.type == Type::R_double) {
                c = SubsetAssign< Double, Integer, Double >::eval(a, idx, b);
        }
        else if(a.type == Type::R_integer && b.type == Type::R_double) {
                c = SubsetAssign< Integer, Integer, Double >::eval(a, idx, b);
        }
        else if(a.type == Type::R_double && b.type == Type::R_integer) {
                c = SubsetAssign< Integer, Integer, Double >::eval(a, idx, b);
        }
        else if(a.type == Type::R_integer && b.type == Type::R_integer) {
                c = SubsetAssign< Integer, Integer, Integer >::eval(a, idx, b);
        }
        else if(a.type == Type::R_logical && b.type == Type::R_double) {
                c = SubsetAssign< Logical, Integer, Double >::eval(a, idx, b);
        }
        else if(a.type == Type::R_logical && b.type == Type::R_integer) {
                c = SubsetAssign< Logical, Integer, Integer >::eval(a, idx, b);
        }
        else if(a.type == Type::R_character && b.type == Type::R_double) {
                c = SubsetAssign< Character, Integer, Double >::eval(a, idx, b);
        }
        else if(a.type == Type::R_character && b.type == Type::R_integer) {
                c = SubsetAssign< Character, Integer, Integer >::eval(a, idx, b);
        }
        else if(a.type == Type::R_double && b.type == Type::R_logical) {
                c = SubsetAssign< Double, Integer, Logical >::eval(a, idx, b);
        }
        else if(a.type == Type::R_integer && b.type == Type::R_logical) {
                c = SubsetAssign< Integer, Integer, Logical >::eval(a, idx, b);
        }
        else if(a.type == Type::R_logical && b.type == Type::R_logical) {
                c = SubsetAssign< Logical, Integer, Logical >::eval(a, idx, b);
        }
        else {
                _error("Invalid index");
        }
}

inline void Insert(Vector const& src, uint64_t srcIndex, Vector& dst, uint64_t dstIndex, uint64_t length) {
	// First cast to destination type. This operation should be fused eventually to avoid a copy.
        if(dst.type == Type::R_double) {
		Double d(dst); Double s = As(src, Type::R_double);
		for(uint64_t i = 0; i < length; i++) d[dstIndex+i] = s[srcIndex+i];
	}
        else if(dst.type == Type::R_integer) {
		Integer d(dst); Integer s = As(src, Type::R_integer);
		for(uint64_t i = 0; i < length; i++) d[dstIndex+i] = s[srcIndex+i];
	}
        else if(dst.type == Type::R_logical) {
		Logical d(dst); Logical s = As(src, Type::R_logical);
		for(uint64_t i = 0; i < length; i++) d[dstIndex+i] = s[srcIndex+i];
	}
        else if(dst.type == Type::R_character) {
		Character d(dst); Character s = As(src, Type::R_character);
		for(uint64_t i = 0; i < length; i++) d[dstIndex+i] = s[srcIndex+i];
	}
        else if(dst.type == Type::R_list) {
		List d(dst); List s = As(src, Type::R_list);
		for(uint64_t i = 0; i < length; i++) d[dstIndex+i] = s[srcIndex+i];
	}
        else if(dst.type == Type::R_call) {
		Call d(dst); Call s = As(src, Type::R_call);
		for(uint64_t i = 0; i < length; i++) d[dstIndex+i] = s[srcIndex+i];
	}
        else {
                _error("Invalid insertion");
        }
}

inline Vector Subset(Vector const& src, uint64_t start, uint64_t length) {
	Vector v(src.type, length);
	memcpy(v.data(), (char*)src.data()+start*src.width(), length*src.width());
	return v;
}

inline Double Sequence(double from, double by, double len) {
	Double r(len);
	double j = 0;
	for(uint64_t i = 0; i < len; i++) {
		r[i] = from+j;
		j = j + by;
	}
	return r;
}

inline Vector Element(Vector const& a, uint64_t index)
{
	if(a.type == Type::R_double) return Double::c(Double(a)[index]);
	else if(a.type == Type::R_integer) return Integer::c(Integer(a)[index]);
	else if(a.type == Type::R_logical) return Logical::c(Logical(a)[index]);
	else if(a.type == Type::R_character) return Character::c(Character(a)[index]);
	else if(a.type == Type::R_complex) return Complex::c(Complex(a)[index]);
	else if(a.type == Type::R_list) return List::c(List(a)[index]);
	else {
		printf("Invalid element\n");
		return Null::singleton;
	};
}

inline Value Element2(Vector const& a, uint64_t index)
{
	if(a.type == Type::R_double) return Double::c(Double(a)[index]);
	else if(a.type == Type::R_integer) return Integer::c(Integer(a)[index]);
	else if(a.type == Type::R_logical) return Logical::c(Logical(a)[index]);
	else if(a.type == Type::R_character) return Character::c(Character(a)[index]);
	else if(a.type == Type::R_complex) return Complex::c(Complex(a)[index]);
	else if(a.type == Type::R_list) return List(a)[index];
	else {
		_error("Invalid element");
		return Null::singleton;
	};
}

#endif

