#include <iostream>
#include <typeinfo>
#include <typeindex>
#include <vector>

#include "set.cpp"
#include "naturals.cpp"
#include "initializator.cpp"

using namespace std;

void testFiniteContains(){
    Initial init;
    init.AddCustom(100);
    init.AddCustom(200);
    init.Add(Natural{100});
    init.Add(Natural{200});
    init.Add(Natural{-1});
    init.AddCustom('c');
    // init.AddCustom(init);
    const FiniteSet set{init};

    cout << set.Contains(Natural{1}) << endl;
    cout << set.Contains(Natural{100}) << endl;
    cout << set.Contains(Natural{2}) << endl;
    cout << set.Contains(Natural{333}) << endl;
}

void testFiniteIncludes(){
    Initial i1;
    i1.AddCustom(3);
    i1.AddCustom(6);
    i1.Add(Natural{3});
    i1.Add(Natural{4});
    i1.AddCustom('a');
    const FiniteSet set1{i1};

    Initial i2;
    i2.AddCustom(3);
    i2.Add(Natural{4});
    // i2.AddCustom(4);
    const FiniteSet set2{i2};

    cout << (set1.Includes(set2)) << endl;
    cout << (set2.Includes(set1)) << endl;
    cout << (set1 == set2) << endl; //double includes
}

void testPrinting(){
    Initial i1;
    i1.AddCustom(3);
    i1.AddCustom(6);
    i1.Add(Natural{3});
    i1.Add(Natural{4});
    i1.AddCustom('a');
    const FiniteSet set1{i1};

    Initial i2;
    i2.AddCustom(3);
    i2.Add(Natural{4});
    // i2.AddCustom(4);
    i2.Add(set1);
    const FiniteSet set2{i2};
    
    cout << set2 << endl;
}

void testFiniteSum(){
    Initial i1;
    // i1.AddCustom(3);
    // i1.AddCustom(6);
    i1.Add(Natural{3});
    i1.Add(Natural{4});
    // i1.AddCustom('a');
    const FiniteSet set1{i1};

    Initial i2;
    // i2.AddCustom(3);
    i2.Add(Natural{4});
    i2.Add(Natural{7});
    // i2.AddCustom(4);
    i2.Add(set1);
    const FiniteSet set2{i2};
    
    cout << set1 << endl;
    cout << set2 << endl;
    cout << (FiniteSet&)(set1.Sum(set2)) << endl;
}

void testFiniteIntersect(){
    Initial i1;
    // i1.AddCustom(3);
    // i1.AddCustom(6);
    i1.Add(Natural{3});
    i1.Add(Natural{4});
    // i1.AddCustom('a');
    const FiniteSet set1{i1};

    Initial i2;
    // i2.AddCustom(3);
    i2.Add(Natural{4});
    i2.Add(Natural{7});
    // i2.AddCustom(4);
    i2.Add(set1);
    const FiniteSet set2{i2};
    
    cout << set1 << endl;
    cout << set2 << endl;
    cout << (FiniteSet&)(set1.Intersect(set2)) << endl;
}
void testFiniteInfiniteIntersect(){
    Initial i1;
    i1.AddCustom(3);
    i1.AddCustom(6);
    i1.Add(Natural{3});
    i1.Add(Natural{4});
    i1.AddCustom('a');
    const FiniteSet set1{i1};

    const Naturals set2{};
    
    cout << set1 << endl;
    // cout << set2 << endl;
    cout << (FiniteSet&)(set1.Intersect(set2)) << endl;
}

void testFiniteInfiniteSubstract(){
    Initial i1;
    i1.AddCustom(3);
    i1.AddCustom(6);
    i1.Add(Natural{3});
    i1.Add(Natural{4});
    i1.AddCustom('a');
    const FiniteSet set1{i1};

    const Naturals set2{};
    
    cout << set1 << endl;
    // cout << set2 << endl;
    cout << (FiniteSet&)(set1.Substract(set2)) << endl;
}

class AELEMENT : public virtual Element, public virtual IComparable<Natural>, public IPrintable {
public:
    AELEMENT(char c){}
    bool equal(const Natural& a, const Natural& b) const override {return false;} 
    void Print(ostream& os) const override { os << "a"; }
};
class ACLASS : public FunctionalSet {
public:
    bool Contains(const Element& e) const override { return IsTypeOf<AELEMENT>(&e); }
};
void testFiniteIntersect_InfiniteSumInfinite(){
    Initial i1;
    i1.AddCustom(3);
    i1.AddCustom(6);
    i1.Add(Natural{3});
    i1.Add(Natural{4});
    i1.Add(AELEMENT{'a'});
    const FiniteSet set1{i1};

    const Naturals set2{};
    const ACLASS set3{};
    const Set& sum = set2.Sum(set3);

    cout << set1 << endl;
    // cout << set2 << endl;
    cout << (FiniteSet&)(set1.Intersect(sum)) << endl;
}