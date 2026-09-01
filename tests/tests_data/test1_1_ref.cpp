class BaseA {
public:
    virtual ~BaseA() {}
};

class BaseB {
public:
    virtual ~BaseB() {}
};

class MultiDerived : public BaseA, public BaseB {};

class ProtectedBase {
protected:
    virtual ~ProtectedBase() {}
};

class ProtectedDerived : public ProtectedBase {};

class RootBase {
public:
    virtual ~RootBase() {}
};

class MiddleClass : public RootBase {
public:
    virtual ~MiddleClass() {}
};

class LeafClass : public MiddleClass {};

class VirtualRoot {
public:
    virtual ~VirtualRoot() {}
};

class NonVirtualMiddle : public VirtualRoot {
public:
    virtual ~NonVirtualMiddle() {}
};

class LeafFromVirtual : public NonVirtualMiddle {};

class OrphanClass {
public:
    ~OrphanClass() {}
};

class AbstractBase {
public:
    virtual void pure() = 0;
    virtual ~AbstractBase() {}
};

class ConcreteDerived : public AbstractBase {
public:
    void pure() override {}
};

class DefaultDtorBase {
public:
    virtual ~DefaultDtorBase() = default;
};

class DefaultDtorDerived : public DefaultDtorBase {};
