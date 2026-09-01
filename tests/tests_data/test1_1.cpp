class BaseA {
public:
    ~BaseA() {}
};

class BaseB {
public:
    ~BaseB() {}
};

class MultiDerived : public BaseA, public BaseB {};

class ProtectedBase {
protected:
    ~ProtectedBase() {}
};

class ProtectedDerived : public ProtectedBase {};

class RootBase {
public:
    ~RootBase() {}
};

class MiddleClass : public RootBase {
public:
    ~MiddleClass() {}
};

class LeafClass : public MiddleClass {};

class VirtualRoot {
public:
    virtual ~VirtualRoot() {}
};

class NonVirtualMiddle : public VirtualRoot {
public:
    ~NonVirtualMiddle() {}
};

class LeafFromVirtual : public NonVirtualMiddle {};

class OrphanClass {
public:
    ~OrphanClass() {}
};

class AbstractBase {
public:
    virtual void pure() = 0;
    ~AbstractBase() {}
};

class ConcreteDerived : public AbstractBase {
public:
    void pure() override {}
};

class DefaultDtorBase {
public:
    ~DefaultDtorBase() = default;
};

class DefaultDtorDerived : public DefaultDtorBase {};
