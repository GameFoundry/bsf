//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include <Testing/BsTestSuite.h>
#include "Utility/BsOctree.h"
#include "Utility/BsBitfield.h"
#include "Utility/BsDynArray.h"
#include "Math/BsComplex.h"
#include "Utility/BsMinHeap.h"
#include "Utility/BsQuadtree.h"
#include "Utility/BsBitstream.h"
#include "Utility/BsUSPtr.h"

namespace bs {

struct DebugOctreeElem {
  AABox box;
  mutable OctreeElementId octreeId;
};

struct DebugOctreeData {
  Vector<DebugOctreeElem> elements;
};

struct DebugOctreeOptions {
  enum { LoosePadding = 16 };
  enum { MinElementsPerNode = 8 };
  enum { MaxElementsPerNode = 16 };
  enum { MaxDepth = 12 };

  static simd::AABox getBounds(UINT32 elem, void* context) {
    DebugOctreeData* octreeData = (DebugOctreeData*)context;
    return simd::AABox(octreeData->elements[elem].box);
  }

  static void setElementId(UINT32 elem, const OctreeElementId& id,
                           void* context) {
    DebugOctreeData* octreeData = (DebugOctreeData*)context;
    octreeData->elements[elem].octreeId = id;
  }
};

typedef Octree<UINT32, DebugOctreeOptions> DebugOctree;

struct DebugQuadtreeElem {
  Rect2 box;
  mutable QuadtreeElementId quadtreeId;
};

struct DebugQuadtreeData {
  Vector<DebugQuadtreeElem> elements;
};

struct DebugQuadtreeOptions {
  enum { LoosePadding = 8 };
  enum { MinElementsPerNode = 4 };
  enum { MaxElementsPerNode = 8 };
  enum { MaxDepth = 6 };

  static simd::Rect2 getBounds(UINT32 elem, void* context) {
    DebugQuadtreeData* quadtreeData = (DebugQuadtreeData*)context;
    return simd::Rect2(quadtreeData->elements[elem].box);
  }

  static void setElementId(UINT32 elem, const QuadtreeElementId& id,
                           void* context) {
    DebugQuadtreeData* quadtreeData = (DebugQuadtreeData*)context;
    quadtreeData->elements[elem].quadtreeId = id;
  }
};

typedef Quadtree<UINT32, DebugQuadtreeOptions> DebugQuadtree;


class UtilityTestSuite : public bs::Test {};

TEST(UtilityTestSuite, testBitfield) {
  static constexpr UINT32 COUNT = 100;
  static constexpr UINT32 EXTRA_COUNT = 32;

  Bitfield bitfield(true, COUNT);

  // Basic iteration
  UINT32 i = 0;
  for (auto iter : bitfield) {
    ASSERT_EQ(iter, true);
    i++;
  }

  UINT32 curCount = COUNT;
  ASSERT_EQ(i, curCount);

  // Dynamic additon
  bitfield.add(false);
  bitfield.add(false);
  bitfield.add(true);
  bitfield.add(false);
  curCount += 4;

  // Realloc
  curCount += EXTRA_COUNT;
  for (uint32_t j = 0; j < 32; j++) bitfield.add(false);

  ASSERT_EQ(bitfield.size(), curCount);

  ASSERT_EQ(bitfield[COUNT + 0], false);
  ASSERT_EQ(bitfield[COUNT + 1], false);
  ASSERT_EQ(bitfield[COUNT + 2], true);
  ASSERT_EQ(bitfield[COUNT + 3], false);

  // Modify during iteration
  i = 0;
  for (auto iter : bitfield) {
    if (i >= 50 && i <= 70) iter = false;

    i++;
  }

  // Modify directly using []
  bitfield[5] = false;
  bitfield[6] = false;

  for (UINT32 j = 50; j < 70; j++) ASSERT_EQ(bitfield[j], false);

  ASSERT_EQ(bitfield[5], false);
  ASSERT_EQ(bitfield[6], false);

  // Removal
  bitfield.remove(10);
  bitfield.remove(10);
  curCount -= 2;

  for (UINT32 j = 48; j < 68; j++) ASSERT_EQ(bitfield[j], false);

  ASSERT_EQ(bitfield[5], false);
  ASSERT_EQ(bitfield[6], false);

  ASSERT_EQ(bitfield.size(), curCount);

  // Find
  ASSERT_EQ(bitfield.find(true), 0);
  ASSERT_EQ(bitfield.find(false), 5);
}

TEST(UtilityTestSuite, testOctree) {
  DebugOctreeData octreeData;
  DebugOctree octree(Vector3::ZERO, 800.0f, &octreeData);

  struct SizeAndCount {
    float sizeMin;
    float sizeMax;
    UINT32 count;
  };

  SizeAndCount types[]{
      {0.02f, 0.2f, 2000},   // Very small objects
      {0.2f, 1.0f, 2000},    // Small objects
      {1.0f, 5.0f, 5000},    // Medium sized objects
      {5.0f, 30.0f, 4000},   // Large objects
      {30.0f, 100.0f, 2000}  // Very large objects
  };

  float placementExtents = 750.0f;
  for (UINT32 i = 0; i < sizeof(types) / sizeof(types[0]); i++) {
    for (UINT32 j = 0; j < types[i].count; j++) {
      Vector3 position(
          ((rand() / (float)RAND_MAX) * 2.0f - 1.0f) * placementExtents,
          ((rand() / (float)RAND_MAX) * 2.0f - 1.0f) * placementExtents,
          ((rand() / (float)RAND_MAX) * 2.0f - 1.0f) * placementExtents);

      Vector3 extents(
          types[i].sizeMin + ((rand() / (float)RAND_MAX)) *
                                 (types[i].sizeMax - types[i].sizeMin) * 0.5f,
          types[i].sizeMin + ((rand() / (float)RAND_MAX)) *
                                 (types[i].sizeMax - types[i].sizeMin) * 0.5f,
          types[i].sizeMin + ((rand() / (float)RAND_MAX)) *
                                 (types[i].sizeMax - types[i].sizeMin) * 0.5f);

      DebugOctreeElem elem;
      elem.box = AABox(position - extents, position + extents);

      UINT32 elemIdx = (UINT32)octreeData.elements.size();
      octreeData.elements.push_back(elem);
      octree.addElement(elemIdx);
    }
  }

  DebugOctreeElem manualElems[3];
  manualElems[0].box =
      AABox(Vector3(100.0f, 100.0f, 100.f), Vector3(110.0f, 115.0f, 110.0f));
  manualElems[1].box =
      AABox(Vector3(200.0f, 100.0f, 100.f), Vector3(250.0f, 150.0f, 150.0f));
  manualElems[2].box =
      AABox(Vector3(90.0f, 90.0f, 90.f), Vector3(105.0f, 105.0f, 110.0f));

  for (UINT32 i = 0; i < 3; i++) {
    UINT32 elemIdx = (UINT32)octreeData.elements.size();
    octreeData.elements.push_back(manualElems[i]);
    octree.addElement(elemIdx);
  }

  AABox queryBounds = manualElems[0].box;
  DebugOctree::BoxIntersectIterator interIter(octree, queryBounds);

  Vector<UINT32> overlapElements;
  while (interIter.moveNext()) {
    UINT32 element = interIter.getElement();
    overlapElements.push_back(element);

    // Manually check for intersections
    ASSERT_TRUE(octreeData.elements[element].box.intersects(queryBounds));
  }

  // Ensure that all we have found all possible overlaps by manually testing all
  // elements
  UINT32 elemIdx = 0;
  for (auto& entry : octreeData.elements) {
    if (entry.box.intersects(queryBounds)) {
      auto iterFind =
          std::find(overlapElements.begin(), overlapElements.end(), elemIdx);
      ASSERT_TRUE(iterFind != overlapElements.end());
    }

    elemIdx++;
  }

  // Ensure nothing goes wrong during element removal
  for (auto& entry : octreeData.elements) octree.removeElement(entry.octreeId);
}

TEST(UtilityTestSuite, testSmallVector) {
  struct SomeElem {
    int a = 10;
    int b = 0;
  };

  // Make sure initial construction works
  SmallVector<SomeElem, 4> v(4);
  ASSERT_EQ(v.size(), 4);
  ASSERT_EQ(v.capacity(), 4);
  ASSERT_EQ(v[0].a, 10);
  ASSERT_EQ(v[3].a, 10);
  ASSERT_EQ(v[3].b, 0);

  // Making the vector dynamic
  v.add({3, 4});
  ASSERT_EQ(v.size(), 5);
  ASSERT_EQ(v[0].a, 10);
  ASSERT_EQ(v[3].a, 10);
  ASSERT_EQ(v[3].b, 0);
  ASSERT_EQ(v[4].a, 3);
  ASSERT_EQ(v[4].b, 4);

  // Make a copy
  SmallVector<SomeElem, 4> v2 = v;
  ASSERT_EQ(v2.size(), 5);
  ASSERT_EQ(v2[0].a, 10);
  ASSERT_EQ(v2[3].a, 10);
  ASSERT_EQ(v2[3].b, 0);
  ASSERT_EQ(v2[4].a, 3);
  ASSERT_EQ(v2[4].b, 4);

  // Pop an element
  v2.pop();
  ASSERT_EQ(v2.size(), 4);
  ASSERT_EQ(v2[0].a, 10);
  ASSERT_EQ(v2[3].a, 10);
  ASSERT_EQ(v2[3].b, 0);

  // Make a static only copy
  SmallVector<SomeElem, 4> v3 = v2;
  ASSERT_EQ(v3.size(), 4);
  ASSERT_EQ(v3.capacity(), 4);
  ASSERT_EQ(v3[0].a, 10);
  ASSERT_EQ(v3[3].a, 10);
  ASSERT_EQ(v3[3].b, 0);

  // Remove an element
  v.remove(2);
  ASSERT_EQ(v.size(), 4);
  ASSERT_EQ(v[0].a, 10);
  ASSERT_EQ(v[2].a, 10);
  ASSERT_EQ(v[3].a, 3);
  ASSERT_EQ(v[3].b, 4);

  // Move a static vector
  SmallVector<SomeElem, 4> v4 = std::move(v3);
  ASSERT_EQ(v3.size(), 0);
  ASSERT_EQ(v4.size(), 4);
  ASSERT_EQ(v4.capacity(), 4);
  ASSERT_EQ(v4[0].a, 10);
  ASSERT_EQ(v4[3].a, 10);
  ASSERT_EQ(v4[3].b, 0);

  // Move a dynamic vector
  SmallVector<SomeElem, 4> v5 = std::move(v2);
  ASSERT_EQ(v2.size(), 0);
  ASSERT_EQ(v5.size(), 4);
  ASSERT_EQ(v5[0].a, 10);
  ASSERT_EQ(v5[3].a, 10);
  ASSERT_EQ(v5[3].b, 0);

  // Move a dynamic vector into a dynamic vector
  v.add({33, 44});
  SmallVector<SomeElem, 4> v6 = std::move(v);
  ASSERT_EQ(v.size(), 0);
  ASSERT_EQ(v6.size(), 5);
  ASSERT_EQ(v6[0].a, 10);
  ASSERT_EQ(v6[3].a, 3);
  ASSERT_EQ(v6[3].b, 4);
  ASSERT_EQ(v6[4].a, 33);
  ASSERT_EQ(v6[4].b, 44);
}

TEST(UtilityTestSuite, testDynArray) {
  struct SomeElem {
    int a = 10;
    int b = 0;
  };

  // Make sure initial construction works
  DynArray<SomeElem> v(4);
  ASSERT_EQ(v.size(), 4);
  ASSERT_EQ(v.capacity(), 4);
  ASSERT_EQ(v[0].a, 10);
  ASSERT_EQ(v[3].a, 10);
  ASSERT_EQ(v[3].b, 0);

  // Add an element
  v.add({3, 4});
  ASSERT_EQ(v.size(), 5);
  ASSERT_EQ(v[0].a, 10);
  ASSERT_EQ(v[3].a, 10);
  ASSERT_EQ(v[3].b, 0);
  ASSERT_EQ(v[4].a, 3);
  ASSERT_EQ(v[4].b, 4);

  // Make a copy
  DynArray<SomeElem> v2 = v;
  ASSERT_EQ(v2.size(), 5);
  ASSERT_EQ(v2[0].a, 10);
  ASSERT_EQ(v2[3].a, 10);
  ASSERT_EQ(v2[3].b, 0);
  ASSERT_EQ(v2[4].a, 3);
  ASSERT_EQ(v2[4].b, 4);

  // Pop an element
  v2.pop();
  ASSERT_EQ(v2.size(), 4);
  ASSERT_EQ(v2[0].a, 10);
  ASSERT_EQ(v2[3].a, 10);
  ASSERT_EQ(v2[3].b, 0);

  // Remove an element
  v.remove(2);
  ASSERT_EQ(v.size(), 4);
  ASSERT_EQ(v[0].a, 10);
  ASSERT_EQ(v[2].a, 10);
  ASSERT_EQ(v[3].a, 3);
  ASSERT_EQ(v[3].b, 4);

  // Insert an element
  v.insert(v.begin() + 2, {99, 100});
  ASSERT_EQ(v.size(), 5);
  ASSERT_EQ(v[0].a, 10);
  ASSERT_EQ(v[2].a, 99);
  ASSERT_EQ(v[3].a, 10);
  ASSERT_EQ(v[4].a, 3);
  ASSERT_EQ(v[4].b, 4);

  // Insert a list
  v.insert(v.begin() + 1, {{55, 100}, {56, 100}, {57, 100}});
  ASSERT_EQ(v.size(), 8);
  ASSERT_EQ(v[0].a, 10);
  ASSERT_EQ(v[1].a, 55);
  ASSERT_EQ(v[2].a, 56);
  ASSERT_EQ(v[3].a, 57);
  ASSERT_EQ(v[4].a, 10);
  ASSERT_EQ(v[5].a, 99);
  ASSERT_EQ(v[6].a, 10);
  ASSERT_EQ(v[7].a, 3);
  ASSERT_EQ(v[7].b, 4);

  // Erase a range of elements
  v.erase(v.begin() + 2, v.begin() + 5);
  ASSERT_EQ(v.size(), 5);
  ASSERT_EQ(v[0].a, 10);
  ASSERT_EQ(v[1].a, 55);
  ASSERT_EQ(v[2].a, 99);
  ASSERT_EQ(v[3].a, 10);
  ASSERT_EQ(v[4].a, 3);
  ASSERT_EQ(v[4].b, 4);

  // Insert a range
  v.insert(v.begin() + 1, v2.begin() + 1, v2.begin() + 3);
  ASSERT_EQ(v.size(), 7);
  ASSERT_EQ(v[0].a, 10);
  ASSERT_EQ(v[1].a, 10);
  ASSERT_EQ(v[2].a, 10);
  ASSERT_EQ(v[3].a, 55);
  ASSERT_EQ(v[4].a, 99);
  ASSERT_EQ(v[5].a, 10);
  ASSERT_EQ(v[6].a, 3);
  ASSERT_EQ(v[6].b, 4);

  // Shrink capacity
  v.shrink();
  ASSERT_EQ(v.size(), v.capacity());
  ASSERT_EQ(v[0].a, 10);
  ASSERT_EQ(v[1].a, 10);
  ASSERT_EQ(v[2].a, 10);
  ASSERT_EQ(v[3].a, 55);
  ASSERT_EQ(v[4].a, 99);
  ASSERT_EQ(v[5].a, 10);
  ASSERT_EQ(v[6].a, 3);
  ASSERT_EQ(v[6].b, 4);

  // Move it
  DynArray<SomeElem> v3 = std::move(v2);
  ASSERT_EQ(v2.size(), 0);
  ASSERT_EQ(v3.size(), 4);
  ASSERT_EQ(v3[0].a, 10);
  ASSERT_EQ(v3[3].a, 10);
  ASSERT_EQ(v3[3].b, 0);
}

TEST(UtilityTestSuite, testComplex) {

  const float EPS = 0.001;

  Complex<float> c(10.0, 4.0);
  ASSERT_EQ(c.real(), 10.0);
  ASSERT_EQ(c.imag(), 4.0);

  Complex<float> c2(15.0, 5.0);
  ASSERT_EQ(c2.real(), 15.0);
  ASSERT_EQ(c2.imag(), 5.0);

  Complex<float> c3 = c + c2;
  ASSERT_EQ(c3.real(), 25.0);
  ASSERT_EQ(c3.imag(), 9.0);

  Complex<float> c4 = c - c2;
  ASSERT_EQ(c4.real(), -5.0);
  ASSERT_EQ(c4.imag(), -1.0);

  Complex<float> c5 = c * c2;
  ASSERT_EQ(c5.real(), 130.0);
  ASSERT_EQ(c5.imag(), 110.0);

  Complex<float> c6 = c / c2;
  ASSERT_EQ(c6.real(), 0.680000007f);
  ASSERT_EQ(c6.imag(), 0.0399999991f);

  ASSERT_EQ(Complex<float>::abs(c), 10.7703295f);
  ASSERT_NEAR(Complex<float>::arg(c), 0.380506366f, 0.00001);
  ASSERT_EQ(Complex<float>::norm(c), 116);

  Complex<float> c7 = Complex<float>::conj(c);
  ASSERT_EQ(c7.real(), 10);
  ASSERT_EQ(c7.imag(), -4);
  c7 = 0;

  c7 = Complex<float>::polar(2.0, 0.5);
  ASSERT_EQ(c7.real(), 1.75516510f);
  ASSERT_EQ(c7.imag(), 0.958851099f);
  c7 = 0;

  c7 = Complex<float>::cos(c);
  ASSERT_NEAR(c7.real(), -22.9135609f, EPS);
  ASSERT_NEAR(c7.imag(), 14.8462915f, EPS);
  c7 = 0;

  c7 = Complex<float>::cosh(c);
  ASSERT_NEAR(c7.real(), -7198.72900f, EPS);
  ASSERT_NEAR(c7.imag(), -8334.84180f, EPS);
  c7 = 0;

  c7 = Complex<float>::exp(c);
  ASSERT_NEAR(c7.real(), -14397.4580f, EPS);
  ASSERT_NEAR(c7.imag(), -16669.6836f, EPS);
  c7 = 0;

  c7 = Complex<float>::log(c);
  ASSERT_NEAR(c7.real(), 2.37679505f, EPS);
  ASSERT_NEAR(c7.imag(), 0.380506366f, EPS);
  c7 = 0;

  c7 = Complex<float>::pow(c, 2.0);
  ASSERT_NEAR(c7.real(), 84.0000000f, EPS);
  ASSERT_NEAR(c7.imag(), 79.9999924f, EPS);
  c7 = 0;

  c7 = Complex<float>::sin(c);
  ASSERT_NEAR(c7.real(), -14.8562555f, EPS);
  ASSERT_NEAR(c7.imag(), -22.8981915f, EPS);
  c7 = 0;

  c7 = Complex<float>::sinh(c);
  ASSERT_NEAR(c7.real(), -7198.72900f, EPS);
  ASSERT_NEAR(c7.imag(), -8334.84277f, EPS);
  c7 = 0;

  c7 = Complex<float>::sqrt(c);
  ASSERT_NEAR(c7.real(), 3.22260213f, EPS);
  ASSERT_NEAR(c7.imag(), 0.620616496f, EPS);
  c7 = 0;
}

TEST(UtilityTestSuite, testMinHeap) {
  struct SomeElem {
    int a;
    int b;
  };

  MinHeap<SomeElem, int> m;
  m.resize(8);
  ASSERT_EQ(m.valid(), true);

  SomeElem elements;
  elements.a = 4;
  elements.b = 5;

  m.insert(elements, 10);
  ASSERT_EQ(m[0].key.a, 4);
  ASSERT_EQ(m[0].key.b, 5);
  ASSERT_EQ(m[0].value, 10);
  ASSERT_EQ(m.size(), 1);

  int v = 11;
  m.insert(elements, v);
  ASSERT_EQ(m[1].key.a, 4);
  ASSERT_EQ(m[1].key.b, 5);
  ASSERT_EQ(m[1].value, 11);
  ASSERT_EQ(m.size(), 2);

  SomeElem minKey;
  int minValue;

  m.minimum(minKey, minValue);
  ASSERT_EQ(minKey.a, 4);
  ASSERT_EQ(minKey.b, 5);
  ASSERT_EQ(minValue, 10);

  m.erase(elements, v);
  ASSERT_EQ(m.size(), 1);
}

TEST(UtilityTestSuite, testQuadtree) {
  DebugQuadtreeData quadtreeData;
  DebugQuadtree quadtree(Vector2(0, 0), 800.0f, &quadtreeData);

  struct SizeAndCount {
    float sizeMin;
    float sizeMax;
    UINT32 count;
  };

  SizeAndCount types[]{
      {0.02f, 0.2f, 2000},   // Very small objects
      {0.2f, 1.0f, 2000},    // Small objects
      {1.0f, 5.0f, 5000},    // Medium sized objects
      {5.0f, 30.0f, 4000},   // Large objects
      {30.0f, 100.0f, 2000}  // Very large objects
  };

  float placementExtents = 750.0f;
  for (UINT32 i = 0; i < sizeof(types) / sizeof(types[0]); i++) {
    for (UINT32 j = 0; j < types[i].count; j++) {
      Vector2 position(
          ((rand() / (float)RAND_MAX) * 2.0f - 1.0f) * placementExtents,
          ((rand() / (float)RAND_MAX) * 2.0f - 1.0f) * placementExtents);

      Vector2 extents(
          types[i].sizeMin + ((rand() / (float)RAND_MAX)) *
                                 (types[i].sizeMax - types[i].sizeMin) * 0.5f,
          types[i].sizeMin + ((rand() / (float)RAND_MAX)) *
                                 (types[i].sizeMax - types[i].sizeMin) * 0.5f);

      DebugQuadtreeElem elem;
      elem.box = Rect2(position - extents, extents);

      UINT32 elemIdx = (UINT32)quadtreeData.elements.size();
      quadtreeData.elements.push_back(elem);
      quadtree.addElement(elemIdx);
    }
  }

  DebugQuadtreeElem manualElems[3];
  manualElems[0].box = Rect2(Vector2(100.0f, 100.0f), Vector2(110.0f, 115.0f));
  manualElems[1].box = Rect2(Vector2(200.0f, 100.0f), Vector2(250.0f, 150.0f));
  manualElems[2].box = Rect2(Vector2(90.0f, 90.0f), Vector2(105.0f, 105.0f));

  for (UINT32 i = 0; i < 3; i++) {
    UINT32 elemIdx = (UINT32)quadtreeData.elements.size();
    quadtreeData.elements.push_back(manualElems[i]);
    quadtree.addElement(elemIdx);
  }

  Rect2 queryBounds = manualElems[0].box;
  DebugQuadtree::BoxIntersectIterator interIter(quadtree, queryBounds);

  Vector<UINT32> overlapElements;
  while (interIter.moveNext()) {
    UINT32 element = interIter.getElement();
    overlapElements.push_back(element);

    // Manually check for intersections
    assert(quadtreeData.elements[element].box.overlaps(queryBounds));
  }

  // Ensure that all we have found all possible overlaps by manually testing all
  // elements
  UINT32 elemIdx = 0;
  for (auto& entry : quadtreeData.elements) {
    if (entry.box.overlaps(queryBounds)) {
      auto iterFind =
          std::find(overlapElements.begin(), overlapElements.end(), elemIdx);
      assert(iterFind != overlapElements.end());
    }

    elemIdx++;
  }

  // Ensure nothing goes wrong during element removal
  for (auto& entry : quadtreeData.elements)
    quadtree.removeElement(entry.quadtreeId);
}

TEST(UtilityTestSuite, testVarInt) {
  UINT32 u0 = 0;
  UINT32 u1 = 127;
  UINT32 u2 = 255;
  UINT32 u3 = 123456;

  INT32 i0 = 0;
  INT32 i1 = 127;
  INT32 i2 = -1;
  INT32 i3 = -123456;
  INT32 i4 = 123456;

  UINT8 output[50];

  UINT32 writeIdx = Bitwise::encodeVarInt(u0, output);
  ASSERT_EQ(writeIdx, 1);

  writeIdx += Bitwise::encodeVarInt(u1, output + writeIdx);
  ASSERT_EQ(writeIdx, 2);

  writeIdx += Bitwise::encodeVarInt(u2, output + writeIdx);
  ASSERT_EQ(writeIdx, 4);

  writeIdx += Bitwise::encodeVarInt(u3, output + writeIdx);

  writeIdx += Bitwise::encodeVarInt(i0, output + writeIdx);
  writeIdx += Bitwise::encodeVarInt(i1, output + writeIdx);
  writeIdx += Bitwise::encodeVarInt(i2, output + writeIdx);
  writeIdx += Bitwise::encodeVarInt(i3, output + writeIdx);
  writeIdx += Bitwise::encodeVarInt(i4, output + writeIdx);

  UINT32 readIdx = 0;
  UINT32 uv;
  readIdx += Bitwise::decodeVarInt(uv, output + readIdx, writeIdx - readIdx);
  ASSERT_EQ(uv, u0);
  ASSERT_TRUE(writeIdx > readIdx);

  readIdx += Bitwise::decodeVarInt(uv, output + readIdx, writeIdx - readIdx);
  ASSERT_EQ(uv, u1);
  ASSERT_TRUE(writeIdx > readIdx);

  readIdx += Bitwise::decodeVarInt(uv, output + readIdx, writeIdx - readIdx);
  ASSERT_EQ(uv, u2);
  ASSERT_TRUE(writeIdx > readIdx);

  readIdx += Bitwise::decodeVarInt(uv, output + readIdx, writeIdx - readIdx);
  ASSERT_EQ(uv, u3);
  ASSERT_TRUE(writeIdx > readIdx);

  INT32 iv;
  readIdx += Bitwise::decodeVarInt(iv, output + readIdx, writeIdx - readIdx);
  ASSERT_EQ(iv, i0);
  ASSERT_TRUE(writeIdx > readIdx);

  readIdx += Bitwise::decodeVarInt(iv, output + readIdx, writeIdx - readIdx);
  ASSERT_EQ(iv, i1);
  ASSERT_TRUE(writeIdx > readIdx);

  readIdx += Bitwise::decodeVarInt(iv, output + readIdx, writeIdx - readIdx);
  ASSERT_EQ(iv, i2);
  ASSERT_TRUE(writeIdx > readIdx);

  readIdx += Bitwise::decodeVarInt(iv, output + readIdx, writeIdx - readIdx);
  ASSERT_EQ(iv, i3);
  ASSERT_TRUE(writeIdx > readIdx);

  readIdx += Bitwise::decodeVarInt(iv, output + readIdx, writeIdx - readIdx);
  ASSERT_EQ(iv, i4);
  ASSERT_EQ(writeIdx, readIdx);
}

TEST(UtilityTestSuite, testBitStream) {
  uint32_t v0 = 12345;
  bool v1 = true;
  uint32_t v2 = 67890;
  bool v3 = true;
  bool v4 = false;
  uint32_t v5 = 987;
  String v6 = "Some test string";
  int32_t v7 = -777;
  uint64_t v8 = 1919191919191919ULL;
  float v9 = 0.3333f;
  float v10 = 10.54321f;

  uint64_t v11 = 5555555555ULL;

  Bitstream bs;

  bs.write(v0);  // 0  - 32
  bs.write(v1);  // 32 - 33
  bs.write(v2);  // 33 - 65
  bs.write(v3);  // 65 - 66
  bs.write(v4);  // 66 - 67

  bs.writeBits((uint8_t*)&v5, 10);  // 67 - 77
  bs.write(v6);                     // 77 - 213
  bs.writeVarInt(v7);               // 213 - 229
  bs.writeVarIntDelta(v7, 0);       // 229 - 246
  bs.writeVarInt(v8);               // 246 - 310
  bs.writeVarIntDelta(v8, v8);      // 310 - 311
  bs.writeNorm(v9);                 // 311 - 327
  bs.writeRange(v10, 5.0f, 15.0f);  // 327 - 343
  bs.writeRange(v5, 500U, 1000U);   // 343 - 352

  bs.align();     // 352
  bs.write(v11);  // 352 - 416

  ASSERT_EQ(bs.size(), 416);

  uint32_t uv;
  uint64_t ulv;
  int32_t iv;
  bool bv;
  float fv;
  String sv;

  bs.seek(0);
  bs.read(uv);
  ASSERT_EQ(uv, v0);

  bs.read(bv);
  ASSERT_EQ(bv, v1);

  bs.read(uv);
  ASSERT_EQ(uv, v2);

  bs.read(bv);
  ASSERT_EQ(bv, v3);

  bs.read(bv);
  ASSERT_EQ(bv, v4);

  uv = 0;
  bs.readBits((uint8_t*)&uv, 10);
  ASSERT_EQ(uv, v5);

  bs.read(sv);
  ASSERT_EQ(sv, v6);

  bs.readVarInt(iv);
  ASSERT_EQ(iv, v7);

  bs.readVarIntDelta(iv, 0);
  ASSERT_EQ(iv, v7);

  bs.readVarInt(ulv);
  ASSERT_EQ(ulv, v8);

  bs.readVarIntDelta(v8, v8);
  ASSERT_EQ(ulv, v8);

  bs.readNorm(fv);
  ASSERT_TRUE(Math::approxEquals(fv, v9, 0.01f));

  bs.readRange(fv, 5.0f, 15.0f);
  ASSERT_TRUE(Math::approxEquals(fv, v10, 0.01f));

  bs.readRange(uv, 500U, 1000U);
  ASSERT_EQ(uv, v5);

  bs.align();
  bs.read(ulv);
  ASSERT_EQ(ulv, v11);
}
}
