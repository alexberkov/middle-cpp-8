class GrandBase {
public:
    virtual void action() {}
};

class MidClass : public GrandBase {
public:
    void action() {}
};

class FinalClass : public MidClass {
public:
    void action() {}
};

class BaseNewVirtual {
public:
    virtual void existing() {}
};

class DerivedNewVirtual : public BaseNewVirtual {
public:
    void existing() {}
    virtual void newMethod() {}  // Новый виртуальный метод, не override
};

class BaseWithDefaults {
public:
    virtual void call(int a = 0) {}
};

class DerivedWithDefaults : public BaseWithDefaults {
public:
    void call(int a = 0) {}
};

class BaseWithOverride {
public:
    virtual void foo() {}
};

class ChildWithOverride : public BaseWithOverride {
public:
    void foo() override {}  // Уже с override, не меняется
};

class BaseMixed {
public:
    virtual void first() {}
    virtual void second(int n) = 0;
    virtual ~BaseMixed() {}
};

class DerivedMixed : public BaseMixed {
public:
    void first() {}
    void second(int n) {}
    ~DerivedMixed() {}
};
