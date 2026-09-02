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
