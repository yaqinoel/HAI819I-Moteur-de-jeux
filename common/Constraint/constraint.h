#pragma once

class Constraint
{
public:
    Constraint(){}
    virtual void init()=0;
    virtual void solve(float dt)=0;
};

