#pragma once

#include "set.h"
#include "fset.h"
#include "orderedset.h"
#include "kartesianProduct.h"
#include "util.cpp"

class Relation : public virtual SubSet {
public:
    const KartesianProduct &kProduct;
public:
    Relation(const KartesianProduct &kProduct) : SubSet(kProduct), kProduct(kProduct) {}

    virtual bool Filter(const Element& e) const override {
        const IOrderedSet* o = ToType<const IOrderedSet*>(&e);
        
        if(o) {
            if(o->Size == kProduct.PairSize) { return Defines(*o); }
        }
        
        return false;
    }

    virtual bool Defines(const IOrderedSet& o) const = 0;
};

class TmpPair : public virtual FunctionalSet, public virtual IOrderedSet {
public:
    TmpPair() : x(nullptr), y(nullptr), ISizeable(2) {}

    mutable const Element* x;
    mutable const Element* y;

    const Element& Get(int index) const override {
        switch(index){
            case 0: return *x;
            case 1: return *y;
            default: throw "Index out of range 2 !";
        }
    }

    virtual bool Contains(const Element& e) const override { return *x == e || *y == e; }

    virtual bool equal(const Set &b) const override {return equal((const Element&)b);}
    bool equal(const Element &b) const {
        const IOrderedSet* o = ToType<const IOrderedSet*>(&b);

        if(o){
            if(o->Size != this->Size) {return false;}
            return o->Get(0) == *x && o->Get(1) == *y;
        }
        
        return false;
    }
    virtual bool operator == (const Element &e) const override { return equal(e); }
};

class PairRelation : public virtual Relation {
    static const ISetCollection& GetCollection(const KartesianProduct &kp) {
        const ISetCollection* re = ToType<const ISetCollection*>(&kp);
        if(re) 
        {
            if(kp.PairSize != *new Cardinality(2, false, false, 1)) { throw "Wrong PairSize! Expected=2 \n"; }
            return *re;
        }
        throw "Kartesian product is not of ISetCollection!";
    }

    PairRelation(const KartesianProduct &kProduct, const ISetCollection &scoll) 
        : Relation(kProduct), SubSet(kProduct),
        X(*scoll.list[0]), Y(*scoll.list[1]), 
        X_FA(ToType<const IForAll*> (scoll.list[0])), Y_FA(ToType<const IForAll*> (scoll.list[1])), 
        X_IE(ToType<const IExists*> (scoll.list[0])), Y_IE(ToType<const IExists*> (scoll.list[1])) 
        { }

    const IForAll * const X_FA, * const Y_FA;
    const IExists * const X_IE, * const Y_IE;
public:
    const Set &X, &Y;

    PairRelation(const KartesianProduct &kProduct) : PairRelation(kProduct, GetCollection(kProduct)) {}

    const Set& Domain() const // dziedzina
    {
        if(!Y_IE) { throw "Y does not support IExists -> Cannot compute the domain"; }

        TmpPair* pair = new TmpPair{};

        RulePtr rule { [this, pair](const Element &e) 
        {
            pair->x = &e;

            RulePtr rule2 { [this, pair](const Element& e2)
            {
                pair->y = &e2;
                return this->Contains(*pair);
            }};

            return Y_IE->Exists(rule2);
        }};

        return *new RSubSet(X, rule);
    }
    const Set& Range() const // przeciwdziedzina
    {
        if(!X_IE) { throw "X does not support IExists -> Cannot compute the codomain"; }
        
        TmpPair* pair = new TmpPair{};

        RulePtr rule { [this, pair](const Element &e) 
        {
            pair->y = &e;

            RulePtr rule2 { [this, pair](const Element& e2)
            {
                pair->x = &e2;
                return this->Contains(*pair);
            }};

            return X_IE->Exists(rule2);
        }};

        return *new RSubSet(Y, rule);
    }

    bool IsReflexive() const // zwrotna 
    {
        if(!X_FA) { throw "X does not support IForAll!"; }
        
        TmpPair* pair = new TmpPair{};

        RulePtr rule {
            [this, pair] (const Element &e) {
                pair->x = &e;
                pair->y = &e;
                return this->Contains(*pair);
            }};

        return X_FA->ForAll(rule);
    }
    bool IsSymmetric() const // symetryczna
    {
        if(!X_FA) { throw "X does not support IForAll!"; }
        if(!Y_FA) { throw "Y does not support IForAll!"; }
        
        TmpPair* p1 = new TmpPair{};
        TmpPair* p2 = new TmpPair{};

        RulePtr rule {
            [this, p1, p2] (const Element &e) {
                p1->x = &e;
                p2->y = &e;

                RulePtr rule2 {
                    [this, p1, p2] (const Element &e2) {
                        p1->y = &e2;
                        p2->x = &e2;

                        if(this->Contains(*p1)) { return this->Contains(*p2); }
                        else { return true; }
                }};

                return Y_FA->ForAll(rule2);
            }};

        return X_FA->ForAll(rule);
    }
    bool IsTransitive() const // przechodnia
    {
        if(!X_FA) { throw "X does not support IForAll!"; }
        if(!Y_FA) { throw "Y does not support IForAll!"; }
        
        TmpPair* p1 = new TmpPair{}; // x R y
        TmpPair* p2 = new TmpPair{}; // y R z
        TmpPair* p3 = new TmpPair{}; // x R z

        RulePtr ruleX {
            [this, p1, p2, p3] (const Element &ex) {
                p1->x = &ex;
                p3->x = &ex;

                RulePtr ruleY {
                    [this, p1, p2, p3] (const Element &ey) {
                        p1->y = &ey;
                        p2->x = &ey;

                        RulePtr ruleY2 {
                            [this, p1, p2, p3] (const Element &ez) {
                                p2->y = &ez;
                                p3->y = &ez;

                                if(this->Contains(*p1) && this->Contains(*p2)) { return this->Contains(*p3); }
                                else { return true; }
                        }};

                        return Y_FA->ForAll(ruleY2);
                }};

                return Y_FA->ForAll(ruleY);
            }};

        return X_FA->ForAll(ruleX);
    }

    bool IsTotal() const // spojna
    {
        if(!X_FA) { throw "X does not support IForAll!"; }
        if(!Y_FA) { throw "Y does not support IForAll!"; }
        
        TmpPair* p1 = new TmpPair{};
        TmpPair* p2 = new TmpPair{};

        RulePtr rule {
            [this, p1, p2] (const Element &e) {
                p1->x = &e;
                p2->y = &e;

                RulePtr rule2 {
                    [this, p1, p2] (const Element &e2) {
                        p1->y = &e2;
                        p2->x = &e2;

                        return this->Contains(*p1) || this->Contains(*p2) || *p1 == *p2;
                }};

                return Y_FA->ForAll(rule2);
            }};

        return X_FA->ForAll(rule);
    }

    bool IsAsymmetric() const // asymetryczna 
    {
        if(!X_FA) { throw "X does not support IForAll!"; }
        if(!Y_FA) { throw "Y does not support IForAll!"; }
        
        TmpPair* p1 = new TmpPair{};
        TmpPair* p2 = new TmpPair{};

        RulePtr rule {
            [this, p1, p2] (const Element &e) {
                p1->x = &e;
                p2->y = &e;

                RulePtr rule2 {
                    [this, p1, p2] (const Element &e2) {
                        p1->y = &e2;
                        p2->x = &e2;

                        if(this->Contains(*p1) && this->Contains(*p2)) { return *p1 == *p2; }
                        else { return true; }
                }};

                return Y_FA->ForAll(rule2);
            }};

        return X_FA->ForAll(rule);
    }

    bool IsEquivalence() const // relacja rownowaznosci 
    {
        return IsReflexive() && IsSymmetric() && IsTransitive();
    }
    bool IsPartialOrder() const // relacja porzadku
    {
        return IsReflexive() && IsAsymmetric() && IsTransitive();
    }
};