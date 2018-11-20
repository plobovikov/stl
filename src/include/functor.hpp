#pragma once

#include <memory>

template<class R,class ...Args>
class FunctorImpl{
public:
    virtual R operator()(Args...) = 0;
    virtual ~FunctorImpl() { }
};
template<class R>
class FunctorImpl<R>{
public:
    virtual R operator()() = 0;
    virtual ~FunctorImpl() { }
};
template<class Functor,class Fun,class ...Args>
class FunctorHandler :
        public FunctorImpl<
        typename Functor::ResultType,
        Args...
        >
{
public:
    typedef typename Functor::ResultType ResultType;

    FunctorHandler(const Fun& fun) : fun(fun){
    }
    ResultType operator()(Args... args){
        return fun(args...);
    }

private:
    Fun fun;
};
template<class Functor,class Fun>
class FunctorHandler<Functor,Fun> :
        public FunctorImpl<
        typename Functor::ResultType
        >
{
public:
    typedef typename Functor::ResultType ResultType;

    FunctorHandler(const Fun& fun) : fun(fun){
    }
    ResultType operator()(){
        return fun();
    }

private:
    Fun fun;
};
template<class Functor,class ObjPointer,class FunPointer,class ...Args>
class MemHandler : public FunctorImpl<
        typename Functor::ResultType,
        Args...
        >
{
   public:
    typedef typename Functor::ResultType ResultType;

    MemHandler(const ObjPointer &obj,const FunPointer &fun) : _obj(obj),_fun(fun) { }

    ResultType operator()(Args...args){
        return (_obj->*_fun)(args...);
    }

private:
    ObjPointer _obj;
    FunPointer _fun;
};
template<class Functor,class ObjPointer,class FunPointer>
class MemHandler<Functor,ObjPointer,FunPointer> : public FunctorImpl<
        typename Functor::ResultType
        >
{
   public:
    typedef typename Functor::ResultType ResultType;

    MemHandler(const ObjPointer &obj,const FunPointer &fun) : _obj(obj),_fun(fun) { }

    ResultType operator()(){
        return (_obj->*_fun)();
    }

private:
    ObjPointer _obj;
    FunPointer _fun;
};
template<class R,class ...Args>
class functor;
template<class R,class ...Args>
class functor<R(Args...)>{

    typedef FunctorImpl<R,Args...> FImpl;
    std::shared_ptr<FImpl>Impl;

public:
    typedef R ResultType;

    functor() = default;
    R operator ()(Args... args){
        return Impl->operator ()(args...);
    }
    template<class F>
    functor(const F &fun) : Impl(new FunctorHandler<functor,F,Args...>(fun)){

    }
    template<class ObjPointer,class FunPointer>
    functor(const ObjPointer &obj,const FunPointer &fun)
        : Impl(new MemHandler<functor,ObjPointer,FunPointer,Args...>(obj,fun)){

    }
    functor(std::shared_ptr<FImpl>Impl) : Impl(Impl){

    }
};
template<class R>
class functor<R>{
    typedef FunctorImpl<R> FImpl;
    std::shared_ptr<FImpl>Impl;
public:
    typedef R ResultType;

    functor() = default;
    R operator ()(){
        return Impl->operator ()();
    }
    template<class F>
    functor(const F &fun) : Impl(new FunctorHandler<functor,F>(fun)){

    }
    template<class ObjPointer,class FunPointer>
    functor(const ObjPointer &obj,const FunPointer &fun)
        : Impl(new MemHandler<functor,ObjPointer,FunPointer>(obj,fun)){

    }
};
