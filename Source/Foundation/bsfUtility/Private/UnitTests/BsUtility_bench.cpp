//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//

#include <benchmark/benchmark.h>
#include "Utility/BsOctree.h"
#include "Utility/BsBitfield.h"
// #include "Utility/BsDynArray.h"
#include "Math/BsComplex.h"
#include "Utility/BsMinHeap.h"
#include "Utility/BsQuadtree.h"
#include "Utility/BsBitstream.h"
#include "Utility/BsUSPtr.h"

namespace bs {

struct SomeElem {
	int a{10};
	int b{0};
};



void BenchSmallVector(benchmark::State& state) {
	// Make sure initial construction works

	while (state.KeepRunning()) {
		SmallVector<int, 4> v(4);
		v.add(0);
		v.add(1);
		v.add(2);
		v.add(3);	
	}
}

void BenchStdVector(benchmark::State& state) {
	struct SomeElem {
		int a = 10;
		int b = 0;
	};

	// Make sure initial construction works

	while (state.KeepRunning()) {
		std::vector<int> v;
		v.reserve(4);
		v.push_back(0);
		v.push_back(1);
		v.push_back(2);
		v.push_back(3);		
	}
}

void BenchStdVectorNoReserve(benchmark::State& state) {
	struct SomeElem {
		int a = 10;
		int b = 0;
	};

	// Make sure initial construction works

	while (state.KeepRunning()) {
		std::vector<int> v;
		v.push_back(0);
		v.push_back(1);
		v.push_back(2);
		v.push_back(3);		
	}
}


BENCHMARK(BenchSmallVector)->Unit(benchmark::kMicrosecond);
BENCHMARK(BenchStdVector)->Unit(benchmark::kMicrosecond);
BENCHMARK(BenchStdVectorNoReserve)->Unit(benchmark::kMicrosecond);

}  // namespace bs