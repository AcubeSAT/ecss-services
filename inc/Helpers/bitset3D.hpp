//
// Created by chrisp on 13/06/23.
//

#ifndef ECSS_SERVICES_BITSET3D_HPP
#define ECSS_SERVICES_BITSET3D_HPP

#include "etl/bitset.h"

template <size_t M, size_t N, size_t O>
class bitset3D
{
public:
	//typedefs
//	typedef typename etl::bitset<M*N*O>::reference reference;

	//bitset::bitset
	bitset3D() : m_bits(){}

	//Bit access
	bool operator()(size_t m, size_t n, size_t o) const {return m_bits[m*N*O + n*O + o];}
//	reference operator()(size_t m, size_t n, size_t o) {return m_bits[m*N*O + n*O + o];}

	//Bit operations:
	bitset3D<M, N, O>& reset() {m_bits.reset(); return *this;}
	bitset3D<M, N, O>& reset(size_t m, size_t n, size_t o) {m_bits.reset(m*N*O + n*O + o); return *this;}
	bitset3D<M, N, O>& flip() {m_bits.flip(); return *this;}
	bitset3D<M, N, O>& flip(size_t m, size_t n, size_t o) {m_bits.flip(m*N*O + n*O + o); return *this;}
	bitset3D<M, N, O>& set() {m_bits.set(); return *this;}
	bitset3D<M, N, O>& set(size_t m, size_t n, size_t o, bool val = true) {m_bits.set(m*N*O + n*O + o, val); return *this;}

	//Bitset operations:
	unsigned long count() const {return m_bits.count();}
	unsigned long size() const {return m_bits.size();}
	unsigned long test(size_t m, size_t n, size_t o) const {m_bits.test(m*N*O + n*O + o); return *this;}
	unsigned long any() const {return m_bits.any();}
	unsigned long none() const {return m_bits.none();}

private:
	etl::bitset<M*N*O> m_bits;
};
#endif //ECSS_SERVICES_BITSET3D_HPP
