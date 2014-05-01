#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmModule.h"

namespace BansheeEngine
{
	/************************************************************************/
	/* 							ABSTRACT DELEGATE	                      	*/
	/************************************************************************/

	// 1 parameter
	template <class P0, class RetType>
	class Delegate1Abstract
	{
	public:
		virtual RetType call(P0 p0) = 0;
		virtual bool equals(const Delegate1Abstract<P0, RetType>& other) = 0;
	};

	/************************************************************************/
	/* 						MEMBER FUNCTION DELEGATE                   		*/
	/************************************************************************/

	// 1 parameter
	template <class X, class Y, class P0, class RetType>
	class Delegate1Member : public Delegate1Abstract<P0, RetType>
	{
	public:
		Delegate1Member(Y* instance, RetType (X::*func)(P0))
			:mInstance(instance), mFunc(func)
		{ }

		RetType call(P0 p0)
		{
			(mInstance->*mFunc)(p0);
		}

		bool equals(const Delegate1Abstract<P0, RetType>& other)
		{
			const Delegate1Member<X, Y, P0, RetType>& otherMemberDelegate = 
				reinterpret_cast<const Delegate1Member<X, Y, P0, RetType>&>(other);

			return otherMemberDelegate.mInstance == mInstance && otherMemberDelegate.mFunc == mFunc;
		}

	private:
		X* mInstance;
		RetType (X::*mFunc)(P0);
	};

	template <class X, class Y, class P0, class RetType>
	std::shared_ptr<Delegate1Member<X, Y, P0, RetType>> delegate(Y* instance, RetType (X::*func)(P0))
	{
		return cm_shared_ptr<Delegate1Member<X, Y, P0, RetType>>(instance, func);
	}

	/************************************************************************/
	/* 							FUNCTION DELEGATE                      		*/
	/************************************************************************/

	// 1 parameter
	template <class P0, class RetType>
	class Delegate1Function : public Delegate1Abstract<P0, RetType>
	{
	public:
		Delegate1Function(RetType (*func)(P0))
			:mInstance(instance), mFunc(func)
		{ }

		RetType call(P0 p0)
		{
			(*mFunc)(p0);
		}

		bool equals(const Delegate1Abstract<P0, RetType>& other)
		{
			const Delegate1Function<P0, RetType>& otherFunctionDelegate = 
				reinterpret_cast<const Delegate1Function<P0, RetType>&>(other);

			return otherMemberDelegate.mFunc == mFunc;
		}

	private:
		RetType (*mFunc)(P0);
	};

	template <class P0, class RetType>
	std::shared_ptr<Delegate1Function<P0, RetType>> delegate(RetType (*func)(P0))
	{
		return cm_shared_ptr<Delegate1Function<P0, RetType>>(func);
	}	

	/************************************************************************/
	/* 								EVENTS									*/
	/************************************************************************/
	
	// 1 parameter
	template <class P0, class RetType>
	class Event1
	{
		typedef Delegate1Abstract<P0, RetType> TDelegate;
		typedef std::shared_ptr<TDelegate> TDelegatePtr;
	public:
		void operator+= (const TDelegatePtr& delegate)
		{
			// TODO - Lock

			mDelegates.push_back(delegate);
		}

		void operator-= (const TDelegatePtr& delegate)
		{
			// TODO - Lock

			for(auto& iter = mDelegates.begin(); iter != mDelegates.end(); ++iter)
			{
				if((*iter)->equals(*delegate))
				{
					mDelegates.erase(iter);
					return;
				}
			}
		}

		void operator() (P0 args)
		{
			// TODO - Lock each call separately using a spin lock

			for(auto& delegate : mDelegates)
				delegate->call(args);
		}

		void clear()
		{
			// TODO - Lock
			mDelegates.clear();
		}

		bool empty()
		{
			// TODO - Lock
			return mDelegates.size() == 0;
		}

	private:
		typename Vector<TDelegatePtr>::type mDelegates;
	};

	/************************************************************************/
	/* 							SPECIALIZATIONS                      		*/
	/* 	SO YOU MAY USE FUNCTION LIKE SYNTAX FOR DECLARING EVENT SIGNATURE   */
	/************************************************************************/
	
	template <typename Signature>
	class Event;

	// 1 parameter
	template <class P0, class RetType>
	class Event<RetType(P0) > : public Event1 <P0, RetType>
	{ };
}