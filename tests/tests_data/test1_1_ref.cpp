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
